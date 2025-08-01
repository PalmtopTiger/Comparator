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
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _defaultDirKeys(SHEET_COUNT),
    _viewStyles({"Blue", "DarkRed"}),
    _sheets(SHEET_COUNT)
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

    for (qsizetype i = 0; i < _defaultDirKeys.size(); ++i) {
        _defaultDirKeys[i] = QString("DefaultDir%1").arg(i + 1);
    }

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
    // Заливка фона
    ui->graphicsView->setScene(_sheets[0].scene());

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

void MainWindow::processCommandLine(const QStringList &args)
{
    if (!args.isEmpty()) {
        int firstPos = -1;
        for (qsizetype i = 0; i < args.size() && i < SHEET_COUNT; ++i) {
            const QString &fileName = args[i];
            if (loadImage(i, fileName) && firstPos < 0) {
                firstPos = i;
            }
        }

        if (firstPos >= 0) {
            centerView(firstPos);
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData &mimeData = *event->mimeData();
    if (mimeData.hasUrls()) {
        const QList<QUrl> &urls = mimeData.urls();
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
    const QMimeData &mimeData = *event->mimeData();
    if (mimeData.hasUrls()) {
        const QList<QUrl> &urls = mimeData.urls();
        if (urls.size() == 1) {
            static int nextPos = 0;
            const QString &fileName = urlToPath(urls.at(0));
            if (loadImage(nextPos, fileName)) {
                centerView(nextPos);
                nextPos = (nextPos + 1) % SHEET_COUNT;
            }
        } else {
            int firstPos = -1;
            for (qsizetype i = 0; i < urls.size() && i < SHEET_COUNT; ++i) {
                const QString &fileName = urlToPath(urls.at(i));
                if (loadImage(i, fileName) && firstPos < 0) {
                    firstPos = i;
                }
            }

            if (firstPos >= 0) {
                centerView(firstPos);
            }
        }
        event->acceptProposedAction();
    }
}

void MainWindow::on_btOpen1_clicked()
{
    const QString &fileName = getOpenFileName(_defaultDirKeys[0]);
    if (loadImage(0, fileName)) {
        centerView(0);
    }
}

void MainWindow::on_btOpen2_clicked()
{
    const QString &fileName = getOpenFileName(_defaultDirKeys[1]);
    if (loadImage(1, fileName)) {
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

bool MainWindow::loadImage(const int pos, const QString &fileName)
{
    Sheet &sheet = _sheets[pos],
          &other = _sheets[!pos];

    if (fileName.isEmpty()) {
        return false;
    }

    if (!sheet.load(fileName)) {
        QMessageBox::critical(this, "Ошибка", QString("Не удалось открыть файл \"%1\"").arg(fileName));
        return false;
    }

    if (!other.isEmpty()) {
        const QSize &maxSize = sheet.size().expandedTo(other.size());
        sheet.scale(maxSize);
        other.scale(maxSize);
    }

    _buttons[pos]->setText(QFileInfo(fileName).fileName());

    ui->graphicsView->setEnabled(true);
    ui->btSwitch->setEnabled(true);
    ui->slZoom->setEnabled(true);
    ui->spZoom->setEnabled(true);

    return true;
}

void MainWindow::switchImage(const int inputPos)
{
    static int pos = 1;
    if (inputPos < 0 || inputPos >= SHEET_COUNT) {
        pos = !pos;
    } else {
        pos = inputPos;
    }

    Sheet &sheet = _sheets[pos];
    if (!sheet.isEmpty()) {
        QPointF center = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect()).boundingRect().center();
        ui->graphicsView->setScene(sheet.scene());
        ui->graphicsView->centerOn(center);

        _buttons[pos]->setPalette(_activePalettes[pos]);
        _buttons[!pos]->setPalette(_passivePalette);

        ui->graphicsView->setStyleSheet(_viewStyles[pos]);
    }
}

void MainWindow::centerView(const int pos)
{
    if (pos < 0 || pos >= SHEET_COUNT) {
        return;
    }

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
        const QString &path = url.toLocalFile();
        if (_imageFormats.contains(QFileInfo(path).suffix(), Qt::CaseInsensitive)) {
            return path;
        }
    }
    return QString();
}
