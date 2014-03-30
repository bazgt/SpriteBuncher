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

//! Performs sprite packing - add new algorithms as required. Mostly static functions.
// [Dev note - the rbp::Rect data structure is used throughout so all results are compatible with MaxRects].
class Packer
{
public:

    //! MaxRect packing on the provided image list. List is modified. Items are packed in order.
    /*! \param allowrot - rotates sprites to pack better. However, be sure your chosen library/app supports rotation.
        \returns number of valid items that were packed.
    */
    static int MaxRects( const SheetProperties &sheetProp, QList<PackSprite> &packedsprites,
                         rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic = rbp::MaxRectsBinPack::RectBestAreaFit,
                         bool allowRotation = false, bool allowCrop = false );

    //! Simple packing by equal height rows ('shelves'). List is modified. Items are packed in order.
    /*! Returns: number of valid items that were packed.
    */
    static int Rows( const SheetProperties &sheetProp, QList<PackSprite> &packedsprites,  bool allowRotation = false, bool allowCrop =false );

};

#endif // PACKER_H
