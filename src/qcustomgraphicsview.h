#ifndef QCUSTOMGRAPHICSVIEW_H
#define QCUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>

class QCustomGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    QCustomGraphicsView(QWidget *parent = nullptr);
    QCustomGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);

signals:
    void rightMouseButtonClicked();
    void middleMouseButtonClicked();
    void wheelScrolledUp();
    void wheelScrolledDown();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif // QCUSTOMGRAPHICSVIEW_H
