/*
    This file is part of SpriteBuncher - a texture packing program.
    Copyright (C) 2014 Barry R Smith.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include <QGraphicsPixmapItem>

//! Class derived from QGraphicsPixmapItem to allow custom paint.
class PixmapItem : public QGraphicsPixmapItem
{

public:
    //! Constructor.
    explicit PixmapItem(const QPixmap & pixmap, QGraphicsItem * parent = 0);

    //! Reimplemented from base class.
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);

};

#endif // PIXMAPITEM_H
