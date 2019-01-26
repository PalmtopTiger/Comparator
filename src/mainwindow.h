#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sheet.h"
#include <QMainWindow>
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
    void on_slZoom_customContextMenuRequested(const QPoint &pos);
    void on_graphicsView_customContextMenuRequested(const QPoint &pos);
    void zoomIn();
    void zoomOut();
    void zoomReset();

private:
    Ui::MainWindow *ui;
    QStringList imageFormats;
    QString imageFormatsFilter;
    QPalette palettePassive;
    QPalette paletteActive;
    QVector<Sheet> sheet;

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void loadImage(const QString fileName, const int pos);
    void switchImage(const int pos = INT_MAX);
    QString urlToPath(const QUrl &url);
};

#endif // MAINWINDOW_H
