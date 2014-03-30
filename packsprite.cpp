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

#include <QGraphicsPixmapItem>
#include <QDebug>
#include "packsprite.h"

PackSprite::PackSprite( const QPixmap &pm, const QFileInfo &fi )
{
    m_pm = pm;
    m_fi = fi;
    m_rotated = false; // (currently unused).
}

QFileInfo PackSprite::fileInfo() const
{
    return m_fi;
}

QPixmap PackSprite::pixmap() const
{
    return m_pm;
}

QPixmap PackSprite::croppedPixmap()
{
    if ( !m_pm_cropped.isNull() ) return m_pm_cropped; // return cached data
     if ( m_pm.isNull() ) return m_pm_cropped;

    // [using  QGraphicsPixmapItem is not fast, but its easier than scanning lines ourselves]
    QGraphicsPixmapItem pm( m_pm );
    QRectF opaqueArea = pm.opaqueArea().boundingRect();
    if ( opaqueArea.isValid() && ( opaqueArea.width() < m_pm.width() || opaqueArea.height() < m_pm.height() ) ) {
        m_pm_cropped = m_pm.copy( opaqueArea.x(), opaqueArea.y(), opaqueArea.width(), opaqueArea.height() ); // (deep copy).
        qDebug() << "Pixmap was cropped to size " << m_pm_cropped.width() << m_pm_cropped.height();
    }
    else
        m_pm_cropped = m_pm; // ref back to original pixmap - prevents us trying to crop again.

    return m_pm_cropped;
}

bool PackSprite::hasSizeCropped() const
{
    if ( !m_pm_cropped.isNull() )
        return !(m_pm.size() == m_pm_cropped.size() );
    return false;
}

 void PackSprite::resetCropping()
 {
     m_pm_cropped = QPixmap();
 }

rbp::Rect PackSprite::packedRect() const
{
    return m_packedRect;
}

void PackSprite::setPackedRect( rbp::Rect rect )
{
    m_packedRect = rect;
}

bool PackSprite::isRotated() const
{
    return m_rotated;
}

void PackSprite::setIsRotated( bool state )
{
    m_rotated = state;
}
