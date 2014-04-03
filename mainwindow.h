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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QPixmap>
#include <QFileInfo>

#include "packsprite.h"
#include "./maxrects/MaxRectsBinPack.h"

//! Struct that stores basic sheet data.
struct SheetProperties {
    //! sheet width in pixles.
    int width;
    //! sheet height in pixles.
    int height;
    //! The gap added around each sprite, in pixels.
    int padding;
    //! The border added around entire sheet, in pixels.
    int border;
};

namespace Ui {
class MainWindow;
}

//! Main Window for the application.
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    //! Constructor.
    explicit MainWindow(QWidget *parent = 0);
    //! Destructor.
    ~MainWindow();

    //! Packing Methods. Indexes must match ui packComboBox -- be careful if adding new ones. Index is saved in json settings file.
    enum PackMethods { MAXRECTS_BESTAREA = 0, MAXRECTS_SHORTSIDE, MAXRECTS_LONGSIDE, MAXRECTS_BOTTOMLEFT, MAXRECTS_CONTACTPOINT,
                       ROWS_BY_NAME, ROWS_BY_AREA, ROWS_BY_HEIGHT, ROWS_BY_WIDTH };
    //! Image formats for output. These get converted to matching QImage formats.
    enum ImageFormats { FORMAT_ARGB32, FORMAT_ARGB32_PRE, FORMAT_ARGB4444_PREM, FORMAT_RGB888, FORMAT_RGB565, FORMAT_RGB565_PREM,
                        FORMAT_RGB555 };

protected slots:

    // UI form action slots:
    //! Slot called when item is selected in the list widget.
    void on_listWidget_clicked(const QModelIndex &index);
    //! Slot which shows the About box.
    void on_actionAbout_triggered();
    //! Slot to open pdf manual (external file).
    void on_actionView_manual_triggered();
    //! Slot to reload current folder.
    void on_action_Reload_triggered();
    //! Slot for 'open folder' menu item.
    void on_actionOpen_folder_triggered();
    //! Slot for 'Export' menu item.
    void on_actionExport_triggered();
    //! Zoom In action menu slot.
    void on_actionZoom_In_triggered();
    //! Zoom Out action menu slot.
    void on_actionZoom_Out_triggered();
    //! Zoom Reset action menu slot.
    void on_actionZoom_Reset_triggered();
    //! Menu item slot to toggle dark UI skin.
    void on_actionUse_Dark_UI_Theme_triggered();

    //! Slot called when selection changes on the graphicsview.
    void sceneSelectionChanged();

    //! Calls up a standard file dialog to choose the input folder.
    void openFileDialog();

    //! Slot is called when a main option is changed, ie requires full reload of image data and repacking. It calls reloadAndRepackAll().
    /*! \see sheetOptionChanged(int), which is faster if the image list doesn't need to be re-processed.
        \param None - don't rely on using the index param, the slot is connected to various signals.
     */
    void packingOptionChanged(int);

    //! Slot is called when a sheet option is changed. It calls repackAll(), so does not reload the images.
    /*! See also: packingOptionChanged(int), which will reload all images before packing.
        \param None - don't rely on using the index param, the slot is connected to various signals.
    */
    void sheetOptionChanged(int);
    //! Alternative overloaded form.
    void sheetOptionChanged(double);

    //! Exports the current sprite sheet and data file.
    void exportFiles();

    // Graphicsview zoom:
    //! Zoom in on the GraphicsView canvas.
    void zoomIn();
    //! Zoom out on the GraphicsView canvas.
    void zoomOut();
    //! Resets zoom to 1:1 on the GraphicsView canvas.
    void zoomReset();
    //! Approx best-fit zoom on the GraphicsView canvas, based on current window size.
    void zoomBestFit();

protected:

    //! Reads previous window geometry etc (via QSettings, in standard locations).
    void readAppSettings();

    //! Handles standard Qt drag events.
    void dragEnterEvent(QDragEnterEvent *event);
    //! Handles standard Qt drop events - we attempt to load a dropped folder.
    void dropEvent(QDropEvent *event);
    //! Standard event Qt calls when application is closed. We save app QSettings here.
    void closeEvent(QCloseEvent *event);

    //! Calls processFolder(), pack(), and updateViewWidgets(). i.e. reloads all images and re-packs everything.
    void reloadAndRepackAll();

    //! Calls pack(), and updateViewWidgets(), using the currently loaded image list. Used if the image list is up-to-date.
    void repackAll();

    //! Loads buncher json settings file from current folder. If none exists, returns false.
    bool loadJsonSettings();

    //! Saves buncher json settings file for the current folder. These store the sheet data, current packing method, and so on.
    void saveJsonSettings();

    //! Opens the requested image folder.
    /*! \param path - full path of folder to load.
     *  \param ignoreIfCurrent - set to false to force a reload of currently loaded folder (else, reqest is ignored)..
     *  \param loadSettings - set to false to retain current settings rather than reload from previous export.
    */
    void openFolder( const QString &path, bool ignoreIfCurrent = true, bool loadSettings = true );

    //! Loads images from the opened folder, ready for packing. Files are sorted based on packing method settings.
    /*! Note - UI has user option to load subfolders.
      * \see openFolder, to open the folder.
     */
    void processFolder();

    //! Starts the packing, based on current method and settings. Previous packing rect data will be lost.
    /*!
     * \return The number of sprites that failed to pack. Anything non-zero means we failed to pack all the images.
     */
    int pack();

    //! Returns the current image format as a QImage format id (be careful, they use different enum values).
    QImage::Format currentQImageFormat() const;

    //! Renders the current sheet to an image, based on current format and settings.
    /*!
     * \return The QImage, with current color-depth setting.
     */
    QImage renderSheet();

    //! Repopulates the listWidget and GraphicsView widgets to display the current packing sprites.
    /*!
     * \param nfails passes in the number of failed packing sprites, so widgets can display fail status.
     */
    void updateViewWidgets( int nfails = 0 );

    //! Contains current list of packing sprites. Packing rects will be valid only after pack() is called.
    /*!
     * \see PackSprite class for more details.
     */
    QList<PackSprite> packedsprites;

    //! Stores main sheet properties.
    SheetProperties sheetProp;

    //! Output file name (without extension).
    QString outFilen;
    //! Input folder name.
    QString inDirn;
    //! Output folder name.
    /*! Currently, this is always set internally to be the "buncher" dir inside the input folder.
    */
    QString outDirn;
    //! File name for this folders local settings, which gets written to outDirn.
    /*! Currently, this is hardcoded to be the "buncher.data" file.
     */
    QString jsonFilen;

private:

    //! UI form object.
    Ui::MainWindow *ui;

    //! graphicsview background item (gets hidden for export).
    QGraphicsRectItem *canvasBG;
    //! graphicsview full sheet image (gets hidden for export).
    QGraphicsPixmapItem *canvasSheet;

    //! Flag for custom ui skin.
    /*! \see mainStyleSheet
     */
    bool useCustomStyleSheet;
};

#endif // MAINWINDOW_H
