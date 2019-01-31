/*
 * This file is part of Comparator.
 * Copyright (C) 2011-2019  Andrey Efremov <duxus@yandex.ru>
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

#include "qcustomgraphicsview.h"
#include <QMouseEvent>
#include <QWheelEvent>

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
