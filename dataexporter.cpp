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

#include "dataexporter.h"

#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QCoreApplication>

int DataExporter::NumDataFormats = 8; // --> Keep this up-to-date when adding formats! UI uses this to populate format combobox.

QString DataExporter::displayName( const DataFormats type )
{
    switch( type ){
    case FORMAT_GENERIC_XML:
        return "Generic XML";
        break;
    case FORMAT_PLAINTEXT:
        return "Plain text";
        break;
    case FORMAT_LIBGDX:
        return "LibGDX";
        break;
    case FORMAT_SPARROW:
        return "Sparrow / Starling";
        break;
    case FORMAT_JSON:
        return "JSON";
        break;
    case FORMAT_UNITY:
        return "Unity3d (JSON)";
        break;
    case FORMAT_GIDEROS:
        return "Gideros";
        break;
    case FORMAT_COCOS2D:
        return "Cocos2d (PLIST)";
        break;
    default:
        return "Undefined"; // (shouldnt see this, check NumFormats etc.)
    }
}

bool DataExporter::Export( const SheetProperties &sheetProp, const DataExporter::DataFormats format, const QString &path, const QString &filen,
                           const QList<PackSprite> &packedsprites )
{
    bool ok = false;
    switch (format ){
        case FORMAT_GENERIC_XML:
        ok = ExportXML( sheetProp, path, filen, packedsprites, false );
        break;
        case FORMAT_PLAINTEXT:
        ok = ExportPlainText( sheetProp, path, filen, packedsprites );
        break;
        case FORMAT_LIBGDX:
        ok = ExportLibGDX( sheetProp, path, filen, packedsprites );
        break;
        case FORMAT_SPARROW:
        ok = ExportXML( sheetProp, path, filen, packedsprites, true ); // note sparrow flag
        break;
        case FORMAT_JSON:
        ok = ExportJSON( sheetProp, path, filen, packedsprites, false );
        break;
        case FORMAT_UNITY:
        ok = ExportJSON( sheetProp, path, filen, packedsprites, true ); // note unity flag
        break;
        case FORMAT_GIDEROS:
        ok = ExportPlainText( sheetProp, path, filen, packedsprites );
        break;
        case FORMAT_COCOS2D:
        ok = ExportPLIST( sheetProp, path, filen, packedsprites, true ); // note cocos2d flag
        break;
    default:
        qDebug() << "Unsupported format in Expoter::Export";
    }
    return ok;
}

// Note: trim/frame data not written. Starling doesnt support rotate.
// For Sparrow/Starling see http://doc.starling-framework.org/core/starling/textures/TextureAtlas.html
// [Dev note - could use Qt's xml classes?].
bool DataExporter::ExportXML( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites,
                              bool starlingStyle )
{
    QString str;
    QTextStream qs(&str);
    QString spriteTag = "sprite";
    QString nameTag = "n";
    QString widthTag = "w";
    QString heightTag = "h";
    if ( starlingStyle ){
        spriteTag = "SubTexture";
        nameTag = "name";
        widthTag = "width";
        heightTag = "height";
    }
    qs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    qs << "<!-- Exported from SpriteBuncher -->\n";
    qs << "<TextureAtlas imagePath=" << quoted( filen + ".png" ); // note - png sheet file format assumed!
    if ( !starlingStyle ) qs << " width=" << quoted( sheetProp.width ) << " height=" << quoted( sheetProp.height );
    qs << ">\n";
    for (int i = 0; i < packedsprites.size(); ++i){
        qs << "    <" << spriteTag << " " << nameTag << "=" << quoted( packedsprites[i].fileInfo().fileName() )
           << " x=" << quoted( packedsprites[i].packedRect().x + sheetProp.border + sheetProp.padding ) // note - must add border and padding to all posn
           << " y=" << quoted (packedsprites[i].packedRect().y + sheetProp.border + sheetProp.padding )
           << " " << widthTag << "=" << quoted( packedsprites[i].packedRect().width - sheetProp.padding ) // note - must subtr padding to get orig size
           << " " << heightTag << "=" << quoted( packedsprites[i].packedRect().height - sheetProp.padding );
        if ( !starlingStyle && packedsprites[i].isRotated() ) qs << " r=\"y\"";
        qs << "/>\n";
    }
    qs << "</TextureAtlas>\n";
    bool ok = DataExporter::WriteFile( path, filen, ".xml", str );
    return ok;
}

bool DataExporter::ExportJSON( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites,
                               bool unityStyle )
{
    QJsonObject jsMainObject;
    QJsonArray jsFramesArray;

    for (int i = 0; i < packedsprites.size(); ++i){
        QJsonObject spriteObject;
        if ( !unityStyle )
            spriteObject.insert( "filename", packedsprites[i].fileInfo().fileName() );
        QJsonObject frameObject;
        frameObject.insert( "x", packedsprites[i].packedRect().x + sheetProp.border + sheetProp.padding  );
        frameObject.insert( "y", packedsprites[i].packedRect().y + sheetProp.border + sheetProp.padding  );
        frameObject.insert( "w", packedsprites[i].packedRect().width - sheetProp.padding );
        frameObject.insert( "h", packedsprites[i].packedRect().height - sheetProp.padding );
        spriteObject.insert( "frame", frameObject );

        spriteObject.insert( "rotated", packedsprites[i].isRotated() );
        spriteObject.insert( "trimmed", false ); // (not same as cropped)

        QJsonObject srcObject;
        srcObject.insert( "x", packedsprites[i].packedRect().x + sheetProp.border + sheetProp.padding  );
        srcObject.insert( "y", packedsprites[i].packedRect().y + sheetProp.border + sheetProp.padding  );
        srcObject.insert( "w", packedsprites[i].packedRect().width - sheetProp.padding );
        srcObject.insert( "h", packedsprites[i].packedRect().height - sheetProp.padding );
        spriteObject.insert( "spriteSourceSize", srcObject );

        QJsonObject szObject;
        szObject.insert( "w", packedsprites[i].packedRect().width - sheetProp.padding );
        szObject.insert( "h", packedsprites[i].packedRect().height - sheetProp.padding );
        spriteObject.insert( "sourceSize", szObject );

        if ( unityStyle ){ // Unity format puts the sprite data inside a containing object, using the filename.
            QJsonObject unityObject;
            unityObject.insert( packedsprites[i].fileInfo().fileName(), spriteObject );
            jsFramesArray.append( unityObject );
        }
        else{
            jsFramesArray.append( spriteObject );
        }
    }
    jsMainObject.insert( "frames", jsFramesArray );

    QJsonObject metaObject;
    QString imgExtn = ".png";
    metaObject.insert( "image", filen + imgExtn );
    metaObject.insert( "app", QCoreApplication::applicationName() );
    metaObject.insert( "version", QCoreApplication::applicationVersion() );
    metaObject.insert( "scale", 1.0 );
    metaObject.insert( "format", QString("RGBA8888") ); // [TODO - need to set this!]
    QJsonObject mszObject;
    mszObject.insert( "w", sheetProp.width );
    mszObject.insert( "h", sheetProp.height );
    metaObject.insert( "size", mszObject );
    jsMainObject.insert( "meta", metaObject );

    QJsonDocument jsDoc(jsMainObject);
    QString str;
    QTextStream qs(&str);
    // qDebug() << jsDoc.toJson();
    qs << jsDoc.toJson();
    bool ok = DataExporter::WriteFile( path, filen, ".json", str );
    return ok;
}

// See for example https://github.com/libgdx/libgdx/blob/master/tests/gdx-tests-android/assets/data/uiskin.atlas
bool DataExporter::ExportLibGDX( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites )
{
    QString str;
    QTextStream qs(&str);
    qs << filen + ".png\n"; // Note - assumes png...
    qs <<   "format: RGBA8888\n"; // [TODO - need to set this!]
    qs <<  "filter: Linear,Linear\n";
    qs <<  "repeat: none\n";
    for (int i = 0; i < packedsprites.size(); ++i){
        qs << ( packedsprites[i].fileInfo().baseName() ) << "\n" // note - name without extn.
           << "  rotate: false\n"
           << "  xy: " << packedsprites[i].packedRect().x + sheetProp.border + sheetProp.padding << ", " << packedsprites[i].packedRect().y + sheetProp.border + sheetProp.padding << "\n"
           << "  size: " << packedsprites[i].packedRect().width - sheetProp.padding << ", " << packedsprites[i].packedRect().height - sheetProp.padding << "\n"
           << "  orig: " << packedsprites[i].packedRect().width - sheetProp.padding << ", " << packedsprites[i].packedRect().height - sheetProp.padding << "\n"
           <<  "  offset: 0, 0\n"
           <<  "  index: -1\n";
    }
    bool ok = DataExporter::WriteFile( path, filen, ".atlas", str );
    return ok;
}

// This is a simple all-purpose text format - you could use this as a base for any new format.
bool DataExporter::ExportPlainText( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites )
{
    QString str;
    QTextStream qs(&str);
    for (int i = 0; i < packedsprites.size(); ++i){
        qs << "image=" << quoted( packedsprites[i].fileInfo().fileName() ) << "\t"
           << " x=" << packedsprites[i].packedRect().x + sheetProp.border + sheetProp.padding << "\t y=" << packedsprites[i].packedRect().y + sheetProp.border + sheetProp.padding
           << "\t width=" << packedsprites[i].packedRect().width - sheetProp.padding << "\t height=" << packedsprites[i].packedRect().height - sheetProp.padding
           << "\t rotated=" << packedsprites[i].isRotated() << "\n";
    }
    bool ok = DataExporter::WriteFile( path, filen, ".txt", str );
    return ok;
}

// For Gideros see http://docs.giderosmobile.com/reference/gideros/TexturePack
// Frame/trim data currently not written.
bool DataExporter::ExportGideros( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites )
{
    QString str;
    QTextStream qs(&str);
    for (int i = 0; i < packedsprites.size(); ++i){
        qs << packedsprites[i].fileInfo().fileName() << ", "
           << packedsprites[i].packedRect().x + sheetProp.border + sheetProp.padding << ", " << packedsprites[i].packedRect().y + sheetProp.border + sheetProp.padding << ", "
           << packedsprites[i].packedRect().width - sheetProp.padding << ", " << packedsprites[i].packedRect().height - sheetProp.padding << ", "
           << "0, 0, 0, 0\n";
    }
    bool ok = DataExporter::WriteFile( path, filen, ".txt", str );
    return ok;
}

bool DataExporter::ExportPLIST( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites,
                                bool cocosStyle )
{
    QString str;
    QTextStream qs(&str);
    QString keytag = "<key>";
    QString keytag2 = "</key>";
    QString dicttag = "<dict>";
    QString dicttag2 = "</dict>";
    QString truetag = "<true/>";
    QString falsetag = "<false/>";
    QString strtag = "<string>";
    QString strtag2 = "</string>";
    QString tab = "    ";
    QString tab2x = tab + tab;
    QString tab4x = tab2x + tab2x;
    if ( !cocosStyle ){
        // (non-cocos style options could go here)
    }
    qs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    qs << "<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
    qs << "<plist version=\"1.0\">\n";
    qs << tab << dicttag << "\n";
    qs << tab2x << keytag << "frames" << keytag2 << "\n";
    qs << tab2x << dicttag << "\n";

    for (int i = 0; i < packedsprites.size(); ++i){

        QString posx, posy, szw, szh;
        posx.setNum( packedsprites[i].packedRect().x + sheetProp.border + sheetProp.padding );
        posy.setNum( packedsprites[i].packedRect().y + sheetProp.border + sheetProp.padding );
        szw.setNum( packedsprites[i].packedRect().width - sheetProp.padding );
        szh.setNum( packedsprites[i].packedRect().height - sheetProp.padding );
        QString posstr = "{" + posx + "," + posy + "}";
        QString szstr = "{" + szw + "," + szh + "}";

        qs << tab << tab2x << keytag << packedsprites[i].fileInfo().fileName() << keytag2 << "\n";          qs << tab4x << dicttag << "\n";
        qs << tab4x << keytag << "frame" << keytag2 << "\n";
        qs << tab4x << strtag << "{" << posstr << "," << szstr << "}" << strtag2 << "\n";
        qs << tab4x << keytag << "offset" << keytag2 << "\n";
        qs << tab4x << strtag << "{" << 0 << "," << 0 << "}" << strtag2 << "\n"; // Todo?
        qs << tab4x << keytag << "rotated" << keytag2 << "\n";
        if ( packedsprites[i].isRotated() )
            qs << tab4x << falsetag << "\n";
        else
            qs << tab4x << truetag << "\n";

        qs << tab4x << keytag << "sourceColorRect" << keytag2 << "\n";
        qs << tab4x << strtag << "{" << "{0,0}" << "," << szstr << "}" << strtag2 << "\n";
        qs << tab4x << keytag << "sourceSize" << keytag2 << "\n";
        qs << tab4x << strtag << "{" << szstr << "}" << strtag2 << "\n";
        qs << tab << tab2x << dicttag2 << "\n";
    }
    qs << tab2x << dicttag2 << "\n";
    qs << tab2x << keytag << "metadata" << keytag2 << "\n";
    qs << tab2x << dicttag << "\n";
    qs << tab << tab2x << keytag << "textureFileName" << keytag2 << "\n";
    qs << tab << tab2x << strtag << filen + ".png" << strtag2 << "\n"; // NB png assumed.
    qs << tab2x << dicttag2 << "\n";
    qs << tab << dicttag2 << "\n";
    qs << "</plist>\n";
    bool ok = DataExporter::WriteFile( path, filen, ".xml", str );
    return ok;
}

bool DataExporter::WriteFile( const QString &path, const QString &filen, const QString &extn, const QString &text )
{
    bool ok = false;
    QFile outf( path + "/" + filen + extn );
    if ( !outf.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) { // (truncate ensures we wipe over prev content)
        qDebug() << "WriteFile: Could not open output file for writing.";
    }
    else{ ok = true;
        QTextStream ts(&outf);
        ts << text;
        outf.close();
    }
    return ok;
}

QString DataExporter::quoted( const QString &str, const QString &quote )
{
    QString ret = str;
    ret.prepend( quote ).append( quote );
    return ret;
}

QString DataExporter::quoted( const int val, const QString &quote )
{
    QString str;
    str.setNum( val );
    return quoted( str, quote );
}

const QList<PackSprite> DataExporter::sortByFileName( const  QList<PackSprite> input )
{
    // uses QMap to sort the sprite list by filename key (see Qt's Doc page for qSort()).
    QMap<QString, PackSprite> map;
    foreach ( PackSprite spr, input)
        map.insert( spr.fileInfo().fileName(), spr );

    return( map.values() );
}
