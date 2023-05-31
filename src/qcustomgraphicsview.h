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

#ifndef QCUSTOMGRAPHICSVIEW_H
#define QCUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>

class QCustomGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit QCustomGraphicsView(QWidget *parent = nullptr);
    explicit QCustomGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);

signals:
    void rightMouseButtonClicked();
    void middleMouseButtonClicked();
    void wheelScrolledUp();
    void wheelScrolledDown();

protected:
    void init();
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif // QCUSTOMGRAPHICSVIEW_H
