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
     QPixmap pixmap() const; // TODO should this ret ref?

     //! returns auto-cropped version of pixmap, based on opaque bounding area. pixmap is cached for future use.
     QPixmap croppedPixmap();
    //! returns true if cropping has changed the pixmaps original size. returns false if cropping not performed on this pixmap.
     bool hasSizeCropped() const;

     //! removes knowledge of any cropping that has been applied.
     void resetCropping();

     //! returns the packedrect. will be a zero-sized rect if packing has not run yet, or if it has failed for this item.
     rbp::Rect packedRect() const;
     void setPackedRect( rbp::Rect rect );

     bool isRotated() const;
     void setIsRotated( bool state = true );

protected:

    QPixmap m_pm;
    QPixmap m_pm_cropped;
    QFileInfo m_fi;
    rbp::Rect m_packedRect;
    bool m_rotated;
};

#endif // PACKSPRITE_H
