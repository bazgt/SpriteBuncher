#ifndef EXPORTER_H
#define EXPORTER_H

#include <Qlist>
#include <QPixmap>
#include <QFileInfo>

#include "packsprite.h"
#include "mainwindow.h"
#include "./maxrects/MaxRectsBinPack.h"

//! Performs data file export - add new formats as required. Mostly static functions.
class DataExporter
{
public:

    //! Defines the supported data file formats. Be careful when updating this list, the indexing is used in the UI and saved data.
    enum DataFormats { FORMAT_GENERIC_XML = 0, FORMAT_PLAINTEXT, FORMAT_LIBGDX, FORMAT_SPARROW, FORMAT_JSON, FORMAT_UNITY, FORMAT_GIDEROS,
                       FORMAT_COCOS2D, FORMAT_CORONA, FORMAT_SPRITEKIT };
    static int NumDataFormats; // --> Must be set to the number of formats defined above.

    static bool Export( const SheetProperties &sheetProp, const DataFormats format, const QString &path, const QString &filen,
                        const QList<PackSprite> &packedsprites );

    //! Returns readable form of the format type (as shown to user in UI menus etc).
    static QString displayName( const DataFormats type );

protected:

    //! Exports 'generic' XML file, and any variations, e.g. Sparrow/Starling.
    static bool ExportXML( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites,
                           bool starlingStyle = false ); //, int sheetw = 1024, int sheeth = 1024, int padding = 0, int border = 0 );

    //! Exports JSON file, and any variations, e.g. Unity.
    static bool ExportJSON( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites,
                            bool unityStyle = false );

    //! Exports LibGDX file.
    static bool ExportLibGDX( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites );

     //! Exports a generic plain text file.
    static bool ExportPlainText( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites );

     //! Exports a Gideros file.
    static bool ExportGideros( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites );

    //! Exports a PLIST properties file, and any variations. Exports cocos2d format by default.
    static bool ExportPLIST( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites,
                             bool cocosStyle = true ); //, int sheetw = 1024, int sheeth = 1024, int padding = 0, int border = 0 );

    //! Writes text to the required file.
    /*! \param path - folder path to write to.
     *  \param filen - filename without extension.
     *  \param extn - file extension.
     *  \param text - the text to be written.
     */
    static bool WriteFile( const QString &path, const QString &filen, const QString &extn, const QString &text );

    //! Convenience function, returns a string with added quotes.
    static QString quoted( const QString &str, const QString quote = "\"" );

    //! Overload. Convenience function, returns a string with added quotes around an integer.
    static QString quoted( const int val, const QString quote = "\"" );

    //! Sorts a packsprite list by filename, returns a new list.
    static const QList<PackSprite> sortByFileName( const  QList<PackSprite> input );
};

#endif // EXPORTER_H
