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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sheet.h"
#include <QMainWindow>
#include <QSettings>
#include <QUrl>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btOpen1_clicked();
    void on_btOpen2_clicked();
    void on_btSwitch_clicked();
    void on_slZoom_valueChanged(const int value);
    void on_spZoom_valueChanged(const int value);
    void on_slZoom_customContextMenuRequested(const QPoint &pos);
    void zoomIn();
    void zoomOut();
    void zoomReset();

private:
    Ui::MainWindow *ui;
    QSettings _settings;
    QStringList _imageFormats;
    QString _imageFormatsFilter;
    QPalette _paletteActive1,
             _paletteActive2,
             _palettePassive;
    Sheet _sheet1, _sheet2;

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    QString getOpenFileName(const QString &defaultDirKey);
    void loadImage(const int pos, const QString &fileName);
    void switchImage(const int pos = -1);
    void centerView(const int pos);
    void setZoom(const int value);
    QString urlToPath(const QUrl &url);

    const QString DEFAULT_DIR1_KEY = "DefaultDir1",
                  DEFAULT_DIR2_KEY = "DefaultDir2";
    const qreal ZOOM_MULTIPLIER = 1.4;
};

#endif // MAINWINDOW_H
