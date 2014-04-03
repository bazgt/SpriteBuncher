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
#include <QPainter>
#include <QDebug>
#include "packsprite.h"

PackSprite::PackSprite( const QPixmap &pm, const QFileInfo &fi )
{
    setPixmap( pm );
    m_pm_original = m_pm; // ?.copy();
    m_fi = fi;
    m_rotated = false;
    m_isCropped = false;
    m_isExpanded = false;
}

void PackSprite::setPixmap ( const QPixmap& pm )
{
    m_pm = pm;
    //qDebug() << "setPixmap.  size is " << pixmap().width() << pixmap().height();
}

void PackSprite::resetForPacking()
{
    setPackedRect( rbp::Rect() );
    setIsRotated( false );
    m_isScaled = false;
    restoreOriginalPixmap();
}

QFileInfo PackSprite::fileInfo() const
{
    return m_fi;
}

const QPixmap& PackSprite::pixmap() const
{
    return m_pm;
}

const QPixmap& PackSprite::originalPixmap() const
{
    return m_pm_original;
}

void PackSprite::cropPixmap()
{
    if ( m_pm.isNull() ) return;
    // [using  QGraphicsPixmapItem is not fast, but its easier than scanning lines ourselves]
    QPixmap cropped;
    QGraphicsPixmapItem pm( m_pm );
    QRectF opaqueArea = pm.opaqueArea().boundingRect();
    if ( opaqueArea.isValid() && ( opaqueArea.width() < m_pm.width() || opaqueArea.height() < m_pm.height() ) ) {
        cropped = m_pm.copy( opaqueArea.x(), opaqueArea.y(), opaqueArea.width(), opaqueArea.height() ); // (deep copy).
        m_isCropped = true;
        m_pm = cropped;
        qDebug() << "Pixmap was cropped to size " << m_pm.width() << m_pm.height();
    }
}

void  PackSprite::scalePixmap( qreal scalef )
{
    qreal fnwid = scalef * pixmap().width();
    qreal fnhgt = scalef * pixmap().height();
    int nwid = int(fnwid);
    int nhgt = int(fnhgt);
    // usually ints get scaled down, but we prefer to scale up if we have any fractions of pixels.
    if ( fnwid - nwid > 0.1 )
        nwid += 1;
    if ( fnhgt - nhgt > 0.1 )
        nhgt += 1;
    if ( nwid == pixmap().width() && nhgt == pixmap().height() )
        return;
    // (could do with some smoothing/scaling options here really).
    QPixmap newpm = pixmap().scaled( nwid, nhgt, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ); // Todo need better option for pixel-art scaling.
    setPixmap( newpm );
    m_isScaled = true;
}

bool PackSprite::isCropped() const
{
    return m_isCropped;
}

bool PackSprite::isExpanded() const
{
    return m_isExpanded;
}

void PackSprite::restoreOriginalPixmap()
{
    setPixmap( m_pm_original );
    m_isCropped = false;
    m_isExpanded = false;
    //qDebug() << " - new pixmap() size is " << pixmap().width() << " " << pixmap().height();
}

void PackSprite::expandPixmap( int npixels )
{
    if ( npixels <= 0 ) return;
    QPixmap newpm( pixmap().width() + 2*npixels, pixmap().height() + 2*npixels );
    newpm.fill(Qt::transparent);
    QPainter painter( &newpm );
    painter.drawPixmap( npixels, npixels, pixmap() );
    qDebug() << "expandPixmap from " << pixmap().width() << " " << pixmap().height();
    m_isExpanded = true;
    setPixmap( newpm );
    qDebug() << "expandPixmap to " << pixmap().width() << " " << pixmap().height();
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
