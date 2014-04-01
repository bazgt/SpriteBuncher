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

#ifndef PACKSPRITE_H
#define PACKSPRITE_H

#include <QFileInfo>
#include <QPixmap>

#include "./maxrects/MaxRectsBinPack.h"

//! Simple class that groups together a pixmap with its originating fileinfo, and its packed rect on the sheet.
/*! The packsprite also has knowledge of its original size, versus an adjusted size based on cropping or extending.
 */
class PackSprite
{
public:
    PackSprite( const QPixmap &pm, const QFileInfo &fi );

    QFileInfo fileInfo() const;
    const QPixmap& pixmap() const;
    const QPixmap& originalPixmap() const;

    void setPixmap ( const QPixmap& pm );

    //! Resets the packing rect, rotation flag, cropping, and restores original pixmap (calls restoreOriginalPixmap).
    /* This is generally used before the item is to be re-packed - ie we dont want previous data in place.
       */
    void resetForPacking();

    //!  auto-crops the current pixmap based on opaque bounding area. Original can be restored via restoreOriginalPixmap.
    void cropPixmap();

    bool isCropped() const;
    bool isExpanded() const;

    //! Reverts to the (cached) original pixmap, removing any cropping or extending.
    void restoreOriginalPixmap();

    //! Expands the current pixmap on each side by the chosen number of pixels.
    void expandPixmap( int npixels = 0 );

    //! returns the packedrect. will be a zero-sized rect if packing has not run yet, or if it has failed for this item.
    rbp::Rect packedRect() const;
    void setPackedRect( rbp::Rect rect );

    bool isRotated() const;
    void setIsRotated( bool state = true );

protected:

    QPixmap m_pm;
    QPixmap m_pm_original; // before any cropping, expanding etc.

    QFileInfo m_fi;
    rbp::Rect m_packedRect;
    bool m_isCropped;
    bool m_rotated;
    bool m_isExpanded;
};

#endif // PACKSPRITE_H
