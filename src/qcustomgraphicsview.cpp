#include "qcustomgraphicsview.h"
#include <QMouseEvent>

QCustomGraphicsView::QCustomGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{}

QCustomGraphicsView::QCustomGraphicsView(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{}

void QCustomGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    switch (event->button()) {
    case Qt::RightButton:
        emit rightMouseButtonClicked();
        break;

    case Qt::MiddleButton:
        emit middleMouseButtonClicked();
        break;

    default:
        QGraphicsView::mouseDoubleClickEvent(event);
        break;
    }
}

void QCustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
    case Qt::RightButton:
        emit rightMouseButtonClicked();
        break;

    case Qt::MiddleButton:
        emit middleMouseButtonClicked();
        break;

    default:
        QGraphicsView::mousePressEvent(event);
        break;
    }
}

void QCustomGraphicsView::wheelEvent(QWheelEvent *event)
{
    // Только при зажатом Ctrl
    if (event->modifiers() & Qt::ControlModifier)
    {
        if (event->delta() > 0)
        {
            emit wheelScrolledUp();
        }
        else
        {
            emit wheelScrolledDown();
        }
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}
