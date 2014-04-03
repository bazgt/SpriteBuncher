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

#ifndef PACKER_H
#define PACKER_H

#include <QList>
#include <QPixmap>
#include "packsprite.h"
#include "./maxrects/MaxRectsBinPack.h"
#include "mainwindow.h"

//! Methods that performs sprite packing. Add new algorithms as required.
/*! Mostly static functions. Note, the rbp::Rect data structure is used throughout, so that
    all results are can be treated the same as the main MaxRects data.
 */
class Packer
{
public:

    //! MaxRect packing on the provided image list. List is modified. Items are packed in order.
    /*! \param sheetProp - the sheet properties.
     *  \param packedsprites - the list of packing sprites. Data for the items may be overwritten.
     *  \param heuristic - MaxRects packing order parameter - see MaxRectsBinPack.h or MaxRects docs for info.
     *  \param allowRotation - set this to allow rotated sprites for better packing.
     *  \param allowCrop - set this to crop all sprites based on their opaque bounding rects.
     *  \param expandSprites - expands sprites on all sides by the chosen number of pixels.
     *  \param extrude - should equal the extrusion size already applied (so it gets added to the padding).
        \param scaleSprites - scales sprites by this amount. Default 1.0.
        \returns number of valid items that were packed.
    */
    static int MaxRects( const SheetProperties &sheetProp, QList<PackSprite> &packedsprites,
                         rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic = rbp::MaxRectsBinPack::RectBestAreaFit,
                         bool allowRotation = false, bool allowCrop = false, int expandSprites = 0,
                         int extrude = 0, qreal scaleSprites = 1.0 );

    //! Simple packing method using equal height rows ('shelves'). List is modified. Items are packed in order.
    /*! \param sheetProp - the sheet properties.
     *  \param packedsprites - the list of packing sprites. Data for the items may be overwritten.
     *  \param allowRotation - *Currently not supported* for this packing method.
     *  \param allowCrop - set this to crop all sprites based on their opaque bounding rects.
     *  \param expandSprites - expands sprites on all sides by the chosen number of pixels.
     *  \param extrude - should equal the extrusion size already applied (so it gets added to the padding).
        \param scaleSprites - scales sprites by this amount. Default 1.0.
     * \returns number of valid items that were packed.
    */
    static int Rows( const SheetProperties &sheetProp, QList<PackSprite> &packedsprites,  bool allowRotation = false,
                     bool allowCrop = false, int expandSprites = 0,
                      int extrude = 0, qreal scaleSprites = 1.0 );

};

#endif // PACKER_H
