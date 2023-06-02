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

#include "sheet.h"
#include <QApplication>

Sheet::Sheet()
{}

Sheet::Sheet(const Sheet &sheet) :
    _pixmap(sheet._pixmap),
    _scaledPixmap(sheet._scaledPixmap)
{}

void Sheet::load(const QString &fileName)
{
    _scene.clear();
    _scaledPixmap = QPixmap();

    _pixmap.load(fileName);
    _pixmap.setDevicePixelRatio(qApp->devicePixelRatio());
}

void Sheet::scale(const QSize &size)
{
    if (_pixmap.isNull()) {
        return;
    }

    _scene.clear();

    if (size == _pixmap.size()) {
        _scaledPixmap = QPixmap();
        return;
    }

    _scaledPixmap = _pixmap.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

bool Sheet::isEmpty() const
{
    return _pixmap.isNull();
}

const QSize Sheet::size() const
{
    return _pixmap.size();
}

QGraphicsScene *Sheet::scene()
{
    if (!_pixmap.isNull() && _scene.items().isEmpty()) {
        if (_scaledPixmap.isNull()) {
            _scene.addPixmap(_pixmap);
        } else {
            _scene.addPixmap(_scaledPixmap);
        }
        _scene.setSceneRect(_scene.itemsBoundingRect());
    }

    return &_scene;
}
