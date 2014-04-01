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

#include <QDebug>
#include "packer.h"

int Packer::MaxRects( const SheetProperties &sheetProp, QList<PackSprite> &packedsprites,
                      rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic,
                      bool allowRotation, bool allowCrop )
{
    // Reset previous rect data, incl rotation and cropping.
    for (int i = 0; i < packedsprites.size(); ++i) {
        packedsprites[i].resetForPacking();
    }

    int validitems = 0;
    int ignoreditems = 0;
    int failitems = 0;
    rbp::MaxRectsBinPack bin;
    bin.Init( sheetProp.width - 2*sheetProp.border, sheetProp.height - 2*sheetProp.border, allowRotation ); // note - border area is removed for packing.
    for (int i = 0; i < packedsprites.size(); ++i) {
        // This is the last chance to modifiy (e.g. crop, extend) pixmaps before they get packed.
        QPixmap px;
        if ( allowCrop )
            packedsprites[i].cropPixmap();

        px = packedsprites[i].pixmap();

        if ( !px.isNull()  && px.width() > 0 && px.width() > 0 )
        {
            // MaxRects does the hard work:
            rbp::Rect packedRect = bin.Insert( px.width() + sheetProp.padding, px.height() + sheetProp.padding, heuristic); // note - packed rects must include the padding
            packedsprites[i].setPackedRect( packedRect ); // will be zero size rect if didnt pack.

            // need to check for rotated packed rect and set the pixmap to match.
            if ( ( px.width() > px.height() && packedRect.width < packedRect.height ) ||
                 ( px.width() < px.height() && packedRect.width > packedRect.height ) )
            {
                packedsprites[i].setIsRotated( true );
            }

            if (packedRect.height > 0) {
                qDebug() << "Packed to " <<  packedRect.x << " " << packedRect.y << " w:"
                         << packedRect.width << " h:" << packedRect.height << " free space=" << 100.f - bin.Occupancy()*100.f;
                validitems++;
            }
            else{
                qDebug() << "Could not pack this rect - skipping this one.\n";
                failitems++;
            }
        }
        else{
            ignoreditems++; // (wasn't a valid pixmap file)
        }
    }
    return failitems;
}

int Packer::Rows( const SheetProperties &sheetProp, QList<PackSprite> &packedsprites,  bool allowRotation, bool allowCrop )
{
    Q_UNUSED( allowRotation ) // rot currently not supported, but we could...

    // Reset previous rect data, incl rotation and cropping.
    for (int i = 0; i < packedsprites.size(); ++i) {
        packedsprites[i].resetForPacking();
    }
    int validitems = 0;
    int ignoreditems = 0;
    int failitems = 0;

    // (recall, we dont include the sheet border in packing data, its added back in later)
    int sheetx = 0;
    int sheety = 0;
    int rowhgt = 1;

    for (int i = 0; i < packedsprites.size(); ++i) {
        QPixmap px;
        if ( allowCrop )
            packedsprites[i].cropPixmap();

        // packedsprites[i].expandPixmap( 2 ); // TEST
        px = packedsprites[i].pixmap();

        if ( !px.isNull()  && px.width() > 0 && px.width() > 0 )
        {
            rbp::Rect packedRect;
            // see if it fits on current row.
            if ( sheetx + px.width() + sheetProp.padding < sheetProp.width - 2*sheetProp.border ){
                // ok, it fits in x, now check it doesnt flow beyond y space. If so, invalidate the rect size.
                if ( sheety + px.height() + sheetProp.padding > sheetProp.height - 2*sheetProp.border ){
                    packedRect.height = 0;
                    packedRect.width = 0;
                    qDebug() << "Doesnt fit in y - could not pack this rectangle.\n";
                }
                else{
                    // it fits - update the sheet data.
                    packedRect.x = sheetx;
                    packedRect.y = sheety;
                    packedRect.height = px.height() + sheetProp.padding;
                    packedRect.width = px.width() + sheetProp.padding;
                    sheetx += px.width() + sheetProp.padding;
                    if ( packedRect.height > rowhgt )
                        rowhgt = packedRect.height;
                }
            }
            else{ // try a new row. first check it would fit in new space. If not, invalidate the rect size.
                if ( px.height() + sheety + rowhgt + sheetProp.padding > sheetProp.height - 2*sheetProp.border || // ie cant fit in remaining y space
                     px.width()  + sheetProp.padding > sheetProp.width - 2*sheetProp.border ){ // ie wider than any x space avail
                    packedRect.height = 0;
                    packedRect.width = 0;
                    qDebug() << "Tried new row - but could not pack this rectangle.\n";
                }
                else{ // ok, add it on new row
                    qDebug() << "New row for img " << px.width() << " x " << px.height();
                    sheetx = 0;
                    sheety += rowhgt;
                    packedRect.x = sheetx;
                    packedRect.y = sheety;
                    packedRect.height = px.height() + sheetProp.padding;
                    packedRect.width = px.width() + sheetProp.padding;
                    sheetx += px.width() + sheetProp.padding;
                    rowhgt = packedRect.height;
                }
            }
            packedsprites[i].setPackedRect( packedRect ); // will be zero size rect if didnt pack.
            if (packedRect.height > 0 && packedRect.width > 0) {
                qDebug() << "Packed to " <<  packedRect.x << " " << packedRect.y << " w:" << packedRect.width << " h:" << packedRect.height;
                validitems++;
            }
            else{
                qDebug() << "Could not pack this rectangle - skipping this one.\n";
                failitems++;
            }
        }
        else{
            ignoreditems++; // (wasn't a valid pixmap file)
        }
    }

    return failitems;
}
