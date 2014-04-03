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

//! Class that defines a 'sprite' on the sprite sheet.
/*! The class groups together a pixmap with its associated fileinfo, and packed rect data on the sheet.
 *  The packsprite also has knowledge of its original pixmap, versus an adjusted copy based on cropping or extending.
 */
class PackSprite
{
public:
    //! Constructor.
    /*!
     * \param pm - the QPixmap for the sprite.
     * \param fi - the QFileInfo associated with the sprite.
     */
    PackSprite( const QPixmap &pm, const QFileInfo &fi );

    //! Access to the fileinfo for this sprite.
    QFileInfo fileInfo() const;
    //! Access to the current pixmap for this sprite (could be cropped, extended etc compared to original).
    const QPixmap& pixmap() const;
    //! Access to the original pixmap that was used to construct the item (prior to any subsequent cropping etc).
    const QPixmap& originalPixmap() const;

    //! Sets a new pixmap for this sprite (doesn't affect any 'original' pixmap that was set).
    void setPixmap ( const QPixmap& pm );

    //! Resets the packing rect, rotation flag, cropping, and restores original pixmap (calls restoreOriginalPixmap).
    /*! This is generally used before the item is to be re-packed - ie we dont want previous data in place.
    */
    void resetForPacking();

    //!  Auto-crops the current pixmap based on it's opaque bounding area. Original can be restored via restoreOriginalPixmap.
    void cropPixmap();

    //! Scales the current pixmap by the specified amount.
    void scalePixmap( qreal scalef );

    //! Returns true if the image has been cropped, and its size reduced.
    bool isCropped() const;
    //! Returns true if the image has been extended from its original size.
    bool isExpanded() const;

    //! Reverts to the (cached) original pixmap, removing any cropping or extending.
    void restoreOriginalPixmap();

    //! Expands the current pixmap on each side by the chosen number of pixels.
    void expandPixmap( int npixels = 0 );

    //! Returns the packedrect data.
    /*! Will return a zero-sized rect if packing has not been run, or if packing failed for this item.
    */
    rbp::Rect packedRect() const;

    //! Sets the packing rect data for this item.
    void setPackedRect( rbp::Rect rect );

    //! Returns true if the item was rotated by a packing algorithm, and hence should be displayed rotated on the sheet.
    bool isRotated() const;
    //! Sets the rotation status for this item. Packing algorithms use this, so we know the item must be drawn rotated later.
    void setIsRotated( bool state = true );

protected:

    //! Stores current pixmap.
    QPixmap m_pm;
     //! Stores original pixmap, before any cropping, expanding etc.
    QPixmap m_pm_original;

    //! Stores the fileinfo.
    QFileInfo m_fi;
    //! Stores the packing rect data.
    rbp::Rect m_packedRect;
    //! Cropping status.
    bool m_isCropped;
    //! Rotation status.
    bool m_rotated;
    //! Expanded status.
    bool m_isExpanded;
    //! Scaled status.
    bool m_isScaled;
};

#endif // PACKSPRITE_H
