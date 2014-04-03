#ifndef EXPORTER_H
#define EXPORTER_H

#include <QList>
#include <QPixmap>
#include <QFileInfo>

#include "packsprite.h"
#include "mainwindow.h"
#include "./maxrects/MaxRectsBinPack.h"

//! Performs data file export. Add new formats as required. Mostly static functions.
class DataExporter
{
public:

    //! Defines the supported data file formats. Be careful when updating this list, the indexing is used in the UI and saved data.
    /*!
     * \see NumDataFormats - the number of formats defined.
     */
    enum DataFormats { FORMAT_GENERIC_XML = 0, FORMAT_PLAINTEXT, FORMAT_LIBGDX, FORMAT_SPARROW, FORMAT_JSON, FORMAT_UNITY, FORMAT_GIDEROS,
                       FORMAT_COCOS2D, FORMAT_CORONA, FORMAT_SPRITEKIT };
    //! Number of supported data formats. Must match the total number of formats defined in DataFormats.
    /*!
     * \see DataFormats - enum where the formats are defined.
     */
    static int NumDataFormats;

    //! Public interface to the exporter functions. Choose your format, and the file will be written.
    /*! \param sheetProp - sheet properties struct.
     *  \param format - the format required (see DataFormats enum).
     *  \param path - folder path to write to.
     *  \param filen - filename (without extension).
     *  \param packedsprites - the list of packed sprites.
     *  \returns true - if the operation was successful.
     */
    static bool Export( const SheetProperties &sheetProp, const DataFormats format, const QString &path, const QString &filen,
                        const QList<PackSprite> &packedsprites );

    //! Returns readable form of the format type (as shown to user in UI menus etc).
    static QString displayName( const DataFormats type );

protected:

    //! Exports 'generic' XML file, and any variations, e.g. Sparrow/Starling.
    /*! \param sheetProp - sheet properties struct.
     *  \param path - folder path to write to.
     *  \param filen - filename (without extension).
     *  \param packedsprites - the list of packed sprites.
     *  \param starlingStyle - set to true for Starling specific version of XML (default false).
     *  \returns true - if the operation was successful.
     */
    static bool ExportXML( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites,
                           bool starlingStyle = false );

    //! Exports JSON file, and any variations, e.g. Unity.
    /*! \param sheetProp - sheet properties struct.
     *  \param path - folder path to write to.
     *  \param filen - filename (without extension).
     *  \param packedsprites - the list of packed sprites.
     *  \param unityStyle - set to true for Unity specific version of Json (default false).
     *  \returns true - if the operation was successful.
     */
    static bool ExportJSON( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites,
                            bool unityStyle = false );

    //! Exports LibGDX format file.
    /*! \param sheetProp - sheet properties struct.
     *  \param path - folder path to write to.
     *  \param filen - filename (without extension).
     *  \param packedsprites - the list of packed sprites.
     *  \returns true - if the operation was successful.
     */
    //! Exports LibGDX file.
    static bool ExportLibGDX( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites );

    //! Exports a generic plain text file.
    /*! \param sheetProp - sheet properties struct.
     *  \param path - folder path to write to.
     *  \param filen - filename (without extension).
     *  \param packedsprites - the list of packed sprites.
     *  \returns true - if the operation was successful.
     */
    static bool ExportPlainText( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites );

    //! Exports a Gideros format file.
    /*! \param sheetProp - sheet properties struct.
     *  \param path - folder path to write to.
     *  \param filen - filename (without extension).
     *  \param packedsprites - the list of packed sprites.
     *  \returns true - if the operation was successful.
     */
    static bool ExportGideros( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites );

    //! Exports a PLIST properties file, and any variations. Exports cocos2d format by default.
    /*! \param sheetProp - sheet properties struct.
     *  \param path - folder path to write to.
     *  \param filen - filename (without extension).
     *  \param packedsprites - the list of packed sprites.
     *  \param cocosStyle - set to true for Cocos2d style PLIST (default true).
     *  \returns true - if the operation was successful.
     */
    static bool ExportPLIST( const SheetProperties &sheetProp, const QString &path, const QString &filen, const QList<PackSprite> &packedsprites,
                             bool cocosStyle = true );

    //! Writes text to the output file. The the other format-specific functions use this.
    /*! \param path - folder path to write to.
     *  \param filen - filename (without extension).
     *  \param extn - file extension.
     *  \param text - the text to be written.
     *  \returns true - if the operation was successful.
     */
    static bool WriteFile( const QString &path, const QString &filen, const QString &extn, const QString &text );

    //! Convenience function, returns a string with added quotes.
    static QString quoted( const QString &str, const QString &quote = "\"" );

    //! Overload. Convenience function, returns a string with added quotes around an integer.
    static QString quoted( const int val, const QString &quote = "\"" );

    //! Sorts a packsprite list by filename [currently not used?].
    static const QList<PackSprite> sortByFileName( const  QList<PackSprite> input );
};

#endif // EXPORTER_H
