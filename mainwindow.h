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
     int width;
     int height;

     int padding;   // added pixel gap around each sprite
     int border;    // border around edges of sheet

     int extend;    // increases sprite size
     int extrude;   // duplicates edge pixels of sprites
     qreal scale;   // sprite scaling (doesnt affect sheet size)
     };

namespace Ui {
class MainWindow;
}

//! Main Window for the application.
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    //! Constr.
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void readAppSettings();

    //! Packing Methods. Indexes must match ui packComboBox -- be careful if adding new ones. Index is saved in json settings file.
     enum PackMethods { MAXRECTS_BESTAREA = 0, MAXRECTS_SHORTSIDE, MAXRECTS_LONGSIDE, MAXRECTS_BOTTOMLEFT, MAXRECTS_CONTACTPOINT,
                        ROWS_BY_NAME, ROWS_BY_AREA, ROWS_BY_HEIGHT, ROWS_BY_WIDTH };
     //! Image formats for output.
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

    //! Slot called when selection changes on the graphicsview.
    void sceneSelectionChanged();

    //! Calls up a standard file dialog to choose the input folder.
    void openFileDialog();

    //! Slot is called when a packing option is changed. It calls reloadAndRepackAll(), ie triggers reload and repack of all data.
    /* See also: sheetOptionChanged(int), which is faster if the image list doesn't need to be re-processed.
     */
    void packingOptionChanged(int);

    //! Slot is called when a sheet option is changed. It calls repackAll(), so does not reload the images.
    /* See also: packingOptionChanged(int), which will reload all images before packing.
    */
    void sheetOptionChanged(int);

    //! Exports the current sprite sheet and data file.
    void exportFiles();

    // Graphicsview zoom:
    //! Zoom in on the GraphicsView
    void zoomIn();
    //! Zoom out on the GraphicsView
    void zoomOut();
    //! Resets zoom to 1:1 on the GraphicsView
    void zoomReset();
    //! Approx best-fit zoom level based on current window size.
    void zoomBestFit();

protected:

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
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
    /*! Set ignoreIfCurrent to false to force a reloading, else duplicate request is ignored.
    */
     void openFolder( const QString &path, bool ignoreIfCurrent = true );

    //! Loads images from the current folder, ready for packing. Files are sorted based on packing method settings.
    void processFolder();

   //! Starts the packing, based on current method and settings. Previous packing rect data will be lost.
    /*!
     * \return The number of sprites that failed to pack. ie anything >0 means we failed to pack everything.
     */
    int pack();

    //! Returns the current image format (for sheet output).
    QImage::Format currentImgFormat() const;

    //! Renders the current sheet to an image, based on current format and settings.
    QImage renderSheet();

    //! Repopulates the listWidget and GraphicsView widgets to display the current packing sprites.
    /*!
     * \param nfails passes in the number of failed packing sprites.
     */
    void updateViewWidgets( int nfails = 0 );

    //! Contains current list of packing sprites. See PackSprite class for more details.
    QList<PackSprite> packedsprites;

    PackMethods packMethod;
    QImage::Format imgFormat;
    SheetProperties sheetProp;

    QString outFilen;
    QString inDirn;
    QString outDirn;
    //! File name for this folders local settings.
    QString jsonFilen;

private:

    //! UI form object.
    Ui::MainWindow *ui;

    //! graphicsview background (gets hidden for export).
    QGraphicsRectItem *canvasBG;
    //! graphicsview full sheet image (gets hidden for export).
    QGraphicsPixmapItem *canvasSheet;

    //! Flag for custom ui skin.
    bool useCustomStyleSheet;
};

#endif // MAINWINDOW_H
