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

#include "pixmapitem.h"
#include <QPainter>
#include <QPalette>
#include <QBrush>
#include <QPen>

PixmapItem::PixmapItem(const QPixmap & pixmap, QGraphicsItem * parent ) :
    QGraphicsPixmapItem(pixmap, parent)
{
}

// int PixmapItem::m_extrude = 0;

void PixmapItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    if ( isSelected() ){
        qreal opac = painter->opacity();
        painter->setOpacity( 1.0 );
        painter->setBrush( QBrush( QColor( 28, 120, 222 )));
        painter->setPen( Qt::NoPen );
        painter->setPen( QPen( Qt::white, 1, Qt::DotLine ) );
        painter->drawRect( boundingRect() );
        painter->setOpacity( opac );
    }
    QGraphicsPixmapItem::paint( painter, option, widget );

/*    // Extrusion. TODO (not here though!)
    if ( m_extrude > 0 ) {
        QRectF target(0.0, 0.0, this->pixmap().width(), m_extrude);
        QRectF source(0.0, m_extrude, this->pixmap().width(), 1.0 );
        painter->drawPixmap( target, this->pixmap(), source );
    }
    */
}
