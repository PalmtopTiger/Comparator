/*
 * This file is part of Comparator.
 * Copyright (C) 2011-2023  Andrey Efremov <duxus@yandex.ru>
 *
 * Comparator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Comparator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Comparator.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QShortcut>
#include <QFileDialog>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QImageReader>
#include <QMimeData>
#include <QStyleHints>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _viewStyles({"Blue", "DarkRed"}),
    _sheets(2)
{
    ui->setupUi(this);

    connect(ui->graphicsView, &QCustomGraphicsView::rightMouseButtonClicked, this, &MainWindow::on_btSwitch_clicked);
    connect(ui->graphicsView, &QCustomGraphicsView::middleMouseButtonClicked, this, &MainWindow::zoomReset);
    connect(ui->graphicsView, &QCustomGraphicsView::wheelScrolledUp, this, &MainWindow::zoomIn);
    connect(ui->graphicsView, &QCustomGraphicsView::wheelScrolledDown, this, &MainWindow::zoomOut);

    connect(new QShortcut(Qt::Key_Space,  this), &QShortcut::activated, this, &MainWindow::on_btSwitch_clicked);
    connect(new QShortcut(Qt::Key_Plus,   this), &QShortcut::activated, this, &MainWindow::zoomIn);
    connect(new QShortcut(Qt::Key_Minus,  this), &QShortcut::activated, this, &MainWindow::zoomOut);
    connect(new QShortcut(Qt::Key_0,      this), &QShortcut::activated, this, &MainWindow::zoomReset);
    connect(new QShortcut(Qt::Key_Insert, this), &QShortcut::activated, this, &MainWindow::zoomReset);

    _buttons = {ui->btOpen1, ui->btOpen2};

    _passivePalette = ui->btOpen1->palette();
    _activePalettes = {_passivePalette, _passivePalette};
    if (qApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
        _activePalettes[0].setColor(QPalette::Button, Qt::darkBlue);
        _activePalettes[1].setColor(QPalette::Button, Qt::darkRed);
        ui->graphicsView->setBackgroundBrush(QColor(0x191919));
    } else {
        _activePalettes[0].setColor(QPalette::Button, Qt::blue);
        _activePalettes[1].setColor(QPalette::Button, Qt::red);
    }

    for (QString &style : _viewStyles) {
        style = QString("QCustomGraphicsView { border: 2px solid %1; }").arg(style);
    }

    setWindowState(Qt::WindowMaximized);

    const QByteArrayList supportedImageFormats = QImageReader::supportedImageFormats();
    for (const QByteArray &format : supportedImageFormats) {
        _imageFormats.append(format);
    }
    _imageFormats.sort();
    _imageFormatsFilter = QString("Изображения (*.%1)").arg(_imageFormats.join(" *."));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        const QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl &url : urls) {
            if (!urlToPath(url).isEmpty()) {
                event->acceptProposedAction();
                return;
            }
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        for (int i = 0; i < event->mimeData()->urls().size() && i < 2; ++i) {
            QString path = urlToPath(event->mimeData()->urls().at(i));
            if (!path.isEmpty()) {
                loadImage(i, path);
            }
        }
        centerView(0);
        event->acceptProposedAction();
    }
}

void MainWindow::on_btOpen1_clicked()
{
    const QString &fileName = getOpenFileName(DEFAULT_DIR1_KEY);
    if (!fileName.isEmpty()) {
        loadImage(0, fileName);
        centerView(0);
    }
}

void MainWindow::on_btOpen2_clicked()
{
    const QString &fileName = getOpenFileName(DEFAULT_DIR2_KEY);
    if (!fileName.isEmpty()) {
        loadImage(1, fileName);
        centerView(1);
    }
}

void MainWindow::on_btSwitch_clicked()
{
    if (ui->btSwitch->isEnabled()) {
        switchImage();
    }
}

void MainWindow::on_slZoom_valueChanged(const int value)
{
    ui->spZoom->setValue(value);
    setZoom(value);
}

void MainWindow::on_spZoom_valueChanged(const int value)
{
    ui->slZoom->setValue(value);
}

void MainWindow::on_slZoom_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    zoomReset();
}

void MainWindow::zoomIn()
{
    if (ui->slZoom->isEnabled()) {
        ui->slZoom->setValue(qCeil(ui->slZoom->value() * ZOOM_MULTIPLIER));
    }
}

void MainWindow::zoomOut()
{
    if (ui->slZoom->isEnabled()) {
        ui->slZoom->setValue(qFloor(ui->slZoom->value() / ZOOM_MULTIPLIER));
    }
}

void MainWindow::zoomReset()
{
    if (ui->slZoom->isEnabled()) {
        ui->slZoom->setValue(100);
    }
}


QString MainWindow::getOpenFileName(const QString &defaultDirKey)
{
    const QString &fileName = QFileDialog::getOpenFileName(this,
                                                           "Выберите изображение",
                                                           _settings.value(defaultDirKey).toString(),
                                                           _imageFormatsFilter);

    if (!fileName.isEmpty()) {
        _settings.setValue(defaultDirKey, QFileInfo(fileName).absolutePath());
    }

    return fileName;
}

void MainWindow::loadImage(const int pos, const QString &fileName)
{
    Sheet &current = _sheets[pos],
          &other   = _sheets[!pos];

    current.load(fileName);

    if (!other.isEmpty()) {
        const QSize &maxSize = current.size().expandedTo(other.size());
        current.scale(maxSize);
        other.scale(maxSize);
    }

    _buttons[pos]->setText(QFileInfo(fileName).fileName());

    ui->graphicsView->setEnabled(true);
    ui->btSwitch->setEnabled(true);
    ui->slZoom->setEnabled(true);
    ui->spZoom->setEnabled(true);
}

void MainWindow::switchImage(const int inputPos)
{
    static int pos = 1;

    if (inputPos < 0 || inputPos > 1) {
        pos = !pos;
    } else {
        pos = inputPos;
    }

    Sheet &current = _sheets[pos];
    if (!current.isEmpty()) {
        QPointF center = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect()).boundingRect().center();
        ui->graphicsView->setScene(current.scene());
        ui->graphicsView->centerOn(center);

        _buttons[pos]->setPalette(_activePalettes[pos]);
        _buttons[!pos]->setPalette(_passivePalette);

        ui->graphicsView->setStyleSheet(_viewStyles[pos]);
    }
}

void MainWindow::centerView(const int pos)
{
    switchImage(pos);
    zoomReset();
    ui->graphicsView->centerOn(ui->graphicsView->sceneRect().center());
}

void MainWindow::setZoom(const int value)
{
    const qreal factor = value / 100.0;
    ui->graphicsView->resetTransform();
    ui->graphicsView->scale(factor, factor);
}

QString MainWindow::urlToPath(const QUrl &url)
{
    if (url.isLocalFile()) {
        const QString path = url.toLocalFile();
        if (_imageFormats.contains(QFileInfo(path).suffix(), Qt::CaseInsensitive)) {
            return path;
        }
    }
    return QString();
}
