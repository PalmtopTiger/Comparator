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
    void on_slZoom_customContextMenuRequested(const QPoint &pos);
    void zoomIn();
    void zoomOut();
    void zoomReset();

private:
    Ui::MainWindow *ui;
    QSettings _settings;
    QStringList _imageFormats;
    QString _imageFormatsFilter;
    QPalette _paletteDefault, _paletteMagenta, _paletteGreen;
    Sheet _sheet1, _sheet2;

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void loadImage(const int pos, QString fileName = QString::null);
    void switchImage(const int pos = INT_MAX);
    QString urlToPath(const QUrl &url);

protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // MAINWINDOW_H
