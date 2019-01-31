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

#ifndef SHEET_H
#define SHEET_H

#include <QPixmap>
#include <QGraphicsScene>

class Sheet
{
public:
    Sheet();
    ~Sheet();

    void clear();

    QPixmap* pixmap;
    QGraphicsScene* scene;

    QString name;
    bool scaled;
};

#endif // SHEET_H
