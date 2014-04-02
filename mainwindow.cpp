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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_aboutbox.h"

#include "./maxrects/MaxRectsBinPack.h"
#include "packer.h"
#include "dataexporter.h"
#include "pixmapitem.h"
#include "customstylesheet.h"

#include <QtDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QListWidget>
#include <QMessageBox>
#include <QPixmap>
#include <QMimeData>
#include <QDropEvent>
#include <QDir>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QSpinBox>
#include <QMap>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "started";
    ui->setupUi(this);
    setAcceptDrops( true );
    setWindowIcon( QIcon( ":/res1/images/icon-full.png" )); // (see also res.qrc and buncher.icns, set in .pro file).
    setWindowTitle( tr( "SpriteBuncher" ));

    sheetProp.padding = 2.0;
    sheetProp.border = 2.0;
    sheetProp.height = 512;
    sheetProp.width = 512;

    readAppSettings();
    outFilen =  ui->basenameLineEdit->text();
    inDirn = QString();
    outDirn = inDirn + "/buncher"; // (name hardcoded for now).
    jsonFilen = "/buncher.data"; // (name hardcoded for now).

    // note - graphicsview is set to 'interactive' in ui form, so we can select items.
    QGraphicsScene* scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // populate the data format combo box. Formats are defined in DataExporter.
     ui->formatComboBox->blockSignals( true ); // (dont want slot calls yet)
    for( int i = 0; i < DataExporter::NumDataFormats; i++ )
        ui->formatComboBox->addItem( DataExporter::displayName( DataExporter::DataFormats(i) ));
    ui->formatComboBox->blockSignals( false );

    // populate the image format combo box - must be in same order as ImageFormats enum [not quite as flexible as data formats].
    ui->imgFormatComboBox->blockSignals( true ); // (dont want slot calls yet)
    ui->imgFormatComboBox->addItem( "RGBA8888 (Best)" ); // aka 'ARGB32'
    ui->imgFormatComboBox->addItem( "RGBA8888 Premultiplied alpha" );
    ui->imgFormatComboBox->addItem( "RGBA4444 Premultiplied alpha" );
    ui->imgFormatComboBox->addItem( "RGB888 (no alpha)" );
    ui->imgFormatComboBox->addItem( "RGB565 (no alpha)" ); // aka 'RGB16'
    ui->imgFormatComboBox->addItem( "RGB565 Premultiplied alpha" );
    ui->imgFormatComboBox->addItem( "RGB555 (no alpha)" );
    ui->imgFormatComboBox->blockSignals( false );

    // Other signal/slot connections (most are already done via ui file).
    QObject::connect( ui->graphicsView->scene(), SIGNAL(selectionChanged()), this, SLOT( sceneSelectionChanged() ));
    ui->publishButton->setEnabled( false );

    // Start with the sample folder [Todo - remember recent folders etc].
    QDir dir( qApp->applicationDirPath() );
    #if defined(Q_OS_MAC)
    if (dir.dirName() == "MacOS") {  // Qt mentions the same trick, e.g. https://qt-project.org/doc/qt-5.0/qtwidgets/tools-plugandpaint.html
        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
    }
    #endif
    openFolder( dir.path() + "/sample" );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    qDebug() << "on_listWidget_clicked " << index.row() ;
    QListWidgetItem *item = ui->listWidget->item(index.row() );
    qDebug() << item->text();

    ui->graphicsView->scene()->clearSelection();
    QList<QGraphicsItem *> items = ui->graphicsView->scene()->items();
    for (int i = 0; i < items.size(); ++i) {
        static const int ObjectName = 0;
        QString name = items.at(i)->data(ObjectName).toString();
        if ( name == item->text() ){
            qDebug() << "Found item on scene";
            QPainterPath selectionArea;
            QGraphicsPixmapItem *item = (QGraphicsPixmapItem*)items.at(i);
            if ( item ) {
                selectionArea.addRect( item->pos().x()+1, item->pos().y()+1, item->pixmap().width()-2, item->pixmap().height()-2 ); // (want to avoid catching neigbouring sprites).
                // [dev note - theres a cyclic signal/slot effect here between graphicsview and listwidget. blockSignals doesnt fix it!].
                ui->graphicsView->scene()->setSelectionArea( selectionArea );
                ui->graphicsView->ensureVisible( item->pos().x(), item->pos().y(), item->pixmap().width()-1, item->pixmap().height()-1 ); // nice effect if zoomed
            }
        }
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QDialog *about = new QDialog();
    Ui::AboutBox uia;
    uia.setupUi(about);
    about->setStyleSheet( mainStyleSheet );
    uia.versionLabel->setText( QCoreApplication::applicationVersion() );
    about->exec();
}

void MainWindow::on_actionView_manual_triggered()
{
    QDir dir( qApp->applicationDirPath() );
    #if defined(Q_OS_MAC)
    if (dir.dirName() == "MacOS") {  // (same Mac trick again)
        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
    }
    #endif
    bool ok = QDesktopServices::openUrl( QUrl::fromLocalFile( dir.absoluteFilePath( "guide.pdf" ) ));
    if ( !ok )
        QMessageBox::warning(this, tr("SpriteBuncher"), QString( "Sorry, could not open manual file in\n" + dir.absoluteFilePath( "guide.pdf" ) ), QMessageBox::Ok );
}

void MainWindow::on_action_Reload_triggered()
{
    zoomReset(); // (zoom seems to mess-up without this?).
    openFolder( inDirn, false ); // ignoreIfCurrent=false
}

void MainWindow::on_actionOpen_folder_triggered()
{
    openFileDialog();
}

void MainWindow::on_actionExport_triggered()
{
    exportFiles();
}

void MainWindow::on_actionZoom_In_triggered()
{
    zoomIn();
}

void MainWindow::on_actionZoom_Out_triggered()
{
    zoomOut();
}

void MainWindow::on_actionZoom_Reset_triggered()
{
    zoomReset();
}

void MainWindow::on_actionUse_Dark_UI_Theme_triggered()
{
    useCustomStyleSheet = !useCustomStyleSheet;
    if ( useCustomStyleSheet )
         setStyleSheet( mainStyleSheet );
    else
        setStyleSheet( qApp->styleSheet() ); // reset to default stylesheet
    this->update();
}

void MainWindow::sceneSelectionChanged()
{
    qDebug() << "sceneSelectionChanged slot";
    ui->listWidget->clearSelection();
    QList<QGraphicsItem *> items = ui->graphicsView->scene()->selectedItems();

    for (int i = 0; i < items.size(); ++i) {
        static const int ObjectName = 0;
        QString name = items.at(i)->data(ObjectName).toString();
        qDebug() << " name: " << name;
        // find the item in the list widget
        QList<QListWidgetItem *> found = ui->listWidget->findItems( name, Qt::MatchExactly );
        if ( found.count() > 0 ) {
            ui->listWidget->setCurrentItem( found.first(), QItemSelectionModel::ClearAndSelect );
        }
    }
}

void MainWindow::openFileDialog()
{
    qDebug() << "Open fileDialog...";
    QString dirStr = QFileDialog::getExistingDirectory(this, tr("Open Directory"), inDirn,
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDir dir( dirStr );
    if ( !dirStr.isEmpty() && dir.isReadable() ){
        openFolder( dirStr );
    }
}

void MainWindow::packingOptionChanged( int index )
{
    Q_UNUSED( index ) // [Dev note - dont use the index, slot is connected to various signals].
    qDebug() << "packingOptionChanged(int)";
    // rotation supported for MaxRects only:
    ui->rotationCheckBox->setHidden( ui->methodComboBox->currentIndex() >= ROWS_BY_NAME );
    reloadAndRepackAll();
}

void MainWindow::sheetOptionChanged( int index )
{
    Q_UNUSED( index ) // [Dev note - dont use the index, slot is connected to various signals].
    qDebug() << "sheetOptionChanged(int)";
    // update all our sheet properties:
    sheetProp.width = ui->widthSpinBox->value();
    sheetProp.height = ui->heightSpinBox->value();
    sheetProp.border = ui->borderSpinBox->value();
    sheetProp.padding = ui->paddingSpinBox->value();
    repackAll();
}

 void MainWindow::sheetOptionChanged( double value )
 {
    sheetOptionChanged( int(value) );
 }

void MainWindow::exportFiles()
{
    qDebug() << "exportFiles";
    outFilen = ui->basenameLineEdit->text();
    QDir dir( outDirn );
    bool ok = dir.mkpath( outDirn );
    if ( !ok || !dir.exists() ) {
        qWarning( "Could not create/write to output folder.");
        QMessageBox::warning(this, tr("SpriteBuncher"), QString( "Can not create/write to output folder." ), QMessageBox::Ok );
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    saveJsonSettings();

    // Write the canvas image to file.
    QImage sheetImg = renderSheet(); // [could've cached this from updateViewWidgets, but we do another final render anyway].
    if ( !sheetImg.isNull() )
        sheetImg.save( dir.path() + "/" + outFilen + ".png" ); // (note, png format. Todo - needs option/var.)
    else
        QMessageBox::warning(this, tr("SpriteBuncher"), QString( "Could not create sheeet image." ), QMessageBox::Ok );

    // Now export the text file == DataExporter class does the work.
    bool okx = DataExporter::Export( sheetProp, DataExporter::DataFormats(ui->formatComboBox->currentIndex()), dir.path(),
                                     outFilen, packedsprites );

    QString formatName = DataExporter::displayName( DataExporter::DataFormats(ui->formatComboBox->currentIndex()) );
    QApplication::restoreOverrideCursor();
    if ( okx )
        QMessageBox::information(this, tr("SpriteBuncher"), QString( formatName + " files exported successfully!" ), QMessageBox::Ok );
    else
        QMessageBox::warning(this, tr("SpriteBuncher"), QString( "Could not write file/data for format: " + formatName ), QMessageBox::Ok );
}

void MainWindow::zoomIn()
{
    ui->graphicsView->scale( 1.25,1.25 );
}

void MainWindow::zoomOut()
{
    ui->graphicsView->scale( 0.8, 0.8 );
}

void MainWindow::zoomReset()
{
    ui->graphicsView->resetMatrix();
    ui->graphicsView->resetTransform(); // (required?).
}

void MainWindow::zoomBestFit()
{
    // This is very approximate, based on window size [viewport does NOT have correct geometry at this point in execution].
    double scl = 0.5* this->width() / qMax( ui->graphicsView->sceneRect().width(), ui->graphicsView->sceneRect().height() );
    ui->graphicsView->scale( scl, scl );
}

void MainWindow::readAppSettings()
{
    qDebug() << "readAppSettings";
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    ui->splitter->restoreState(settings.value("splitterState").toByteArray());
    useCustomStyleSheet = settings.value("useCustomStyleSheet").toBool();

    ui->actionUse_Dark_UI_Theme->blockSignals( true ); // dont want to trigger slots from this setup.
    ui->actionUse_Dark_UI_Theme->setChecked( useCustomStyleSheet );
    ui->actionUse_Dark_UI_Theme->blockSignals( false );
    if ( useCustomStyleSheet )
        setStyleSheet( mainStyleSheet );
    qDebug() << "readAppSettings finished";
}

// (Required for window to accept dnd events)
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData=event->mimeData();
    qDebug() << "hasUrls " << mimeData->hasUrls();
    if (mimeData->hasUrls()){
        qDebug() << "dropped mimeData";
        QList<QUrl> urls =  event->mimeData()->urls();
        // take first, and use it as the folder to open
        qDebug() << urls.first().toLocalFile();
        QFileInfo fi( urls.first().toLocalFile() );
        openFolder( fi.filePath() ); // we dont check anything here, let openFolder do it.
    }
    else{
        qDebug() << "ignoring drop";
        event->ignore();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "closeEvent in mainwindow - saving settings first";
    QSettings settings("GoodReactions", "SpriteBuncher");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("splitterState", ui->splitter->saveState());
     settings.setValue("useCustomStyleSheet", useCustomStyleSheet );
    QMainWindow::closeEvent(event);
}

void MainWindow::reloadAndRepackAll()
{
    qDebug() << "reloadAndRepackAll";
    processFolder();
    repackAll();
}

void MainWindow::repackAll()
{
    qDebug() << "repackAll";
    int nfails = pack();
    updateViewWidgets( nfails );
}

bool MainWindow::loadJsonSettings()
{   // [ Dev notes - we use blockSignals here to prevent getting a signal from each
    // widget, which would trigger a reload for each].
    qDebug() << "loadJsonSettings";
    bool ok = false;
    QDir odir( outDirn );
    QFile outjsf;
    outjsf.setFileName( odir.path() + jsonFilen );
    if ( outjsf.exists() ){
        qDebug() << "Previous json file exists, reading it.";
        ok = true;
        outjsf.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray val = outjsf.readAll();
        outjsf.close();
        qDebug() << "Read json: " << val;
        QJsonDocument jd = QJsonDocument::fromJson(val);
        QJsonObject obj = jd.object();

        if ( obj.contains( "sheetw" )){
            QJsonValue jsval = obj.value( "sheetw");
            if ( jsval.toDouble() > 0 ){
                sheetProp.width = jsval.toDouble();
                ui->widthSpinBox->blockSignals( true );
                ui->widthSpinBox->setValue( sheetProp.width );
                ui->widthSpinBox->blockSignals( false );
            }
        }
        if ( obj.contains( "sheeth" )){
            QJsonValue jsval = obj.value( "sheeth");
            if ( jsval.toDouble() > 0 ){
                sheetProp.height = jsval.toDouble();
                ui->heightSpinBox->blockSignals( true );
                ui->heightSpinBox->setValue( sheetProp.height );
                ui->heightSpinBox->blockSignals( false );
            }
        }
        if ( obj.contains( "padding" )){
            QJsonValue jsval = obj.value( "padding");
            if ( jsval.toDouble() >= 0 ){
                int val = jsval.toDouble();
                sheetProp.padding = val;
                ui->paddingSpinBox->blockSignals( true );
                ui->paddingSpinBox->setValue( val );
                ui->paddingSpinBox->blockSignals( false );
            }
        }
        if ( obj.contains( "border" )){
            QJsonValue jsval = obj.value( "border");
            if ( jsval.toDouble() >= 0 ){
                int val = jsval.toDouble();
                sheetProp.border = val;
                ui->borderSpinBox->blockSignals( true );
                ui->borderSpinBox->setValue( val );
                ui->borderSpinBox->blockSignals( false );
            }
        }
        if ( obj.contains( "expand" )){
            QJsonValue jsval = obj.value( "expand");
            if ( jsval.toDouble() >= 0 ){
                int val = jsval.toDouble();
                //sheetProp.expand = val;
                ui->expandSpinBox->blockSignals( true );
                ui->expandSpinBox->setValue( val );
                ui->expandSpinBox->blockSignals( false );
            }
        }
        if ( obj.contains( "extrude" )){
            QJsonValue jsval = obj.value( "extrude");
            if ( jsval.toDouble() >= 0 ){
                int val = jsval.toDouble();
                ui->extrudeSpinBox->blockSignals( true );
                ui->extrudeSpinBox->setValue( val );
                ui->extrudeSpinBox->blockSignals( false );
            }
        }
        if ( obj.contains( "method" )){
            QJsonValue jsval = obj.value( "method");
            if ( jsval.toDouble() >= 0 ){
                int val = jsval.toDouble();
                ui->methodComboBox->blockSignals( true );
                ui->methodComboBox->setCurrentIndex( val );
                ui->rotationCheckBox->setHidden( ui->methodComboBox->currentIndex() >= ROWS_BY_NAME );
                ui->methodComboBox->blockSignals( false );
            }
        }
        if ( obj.contains( "format" )){ // data format
            QJsonValue jsval = obj.value( "format");
            if ( jsval.toDouble() >= 0 ){
                int val = jsval.toDouble();
                ui->formatComboBox->blockSignals( true );
                ui->formatComboBox->setCurrentIndex( val );
                ui->formatComboBox->blockSignals( false );
            }
        }
        if ( obj.contains( "imgformat" )){ // image format
            QJsonValue jsval = obj.value( "imgformat");
            if ( jsval.toDouble() >= 0 ){
                int val = jsval.toDouble();
                ui->imgFormatComboBox->blockSignals( true );
                ui->imgFormatComboBox->setCurrentIndex( val );
                ui->imgFormatComboBox->blockSignals( false );
            }
        }
        if ( obj.contains( "rotation" )){
            QJsonValue jsval = obj.value( "rotation");
            bool val = jsval.toBool();
            ui->rotationCheckBox->blockSignals( true );
            ui->rotationCheckBox->setChecked( val );
            ui->rotationCheckBox->blockSignals( false );
        }
        if ( obj.contains( "cropping" )){
            QJsonValue jsval = obj.value( "cropping");
            bool val = jsval.toBool();
            ui->croppingCheckBox->blockSignals( true );
            ui->croppingCheckBox->setChecked( val );
            ui->croppingCheckBox->blockSignals( false );
        }
        if ( obj.contains( "basename" )){
            QJsonValue jsval = obj.value( "basename");
            if ( jsval.toString().length() > 0 ){
                QString str = jsval.toString();
                ui->basenameLineEdit->setText( str );
                outFilen =  str;
            }
        }
        if ( obj.contains( "version" )) { // (SpriteBuncher app version)
            QJsonValue jsval = obj.value( "version");
            qDebug() << "version string = "  << jsval.toString();
        }
    }
    else
        qDebug() << "(no previous json file exists).";

    return ok;
}

void MainWindow::saveJsonSettings()
{   qDebug() << "saveJsonSettings";
    QDir dir( outDirn );
    QFile saveFile( dir.path() + jsonFilen );
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open json file for saving.");
        return;
    }
    QJsonObject gameObject;
    gameObject.insert( "sheetw", sheetProp.width );
    gameObject.insert( "sheeth", sheetProp.height );
    gameObject.insert( "expand", ui->expandSpinBox->value() );
    gameObject.insert( "extrude", ui->extrudeSpinBox->value() );
    gameObject.insert( "padding", ui->paddingSpinBox->value() );
    gameObject.insert( "border", ui->borderSpinBox->value() );
    gameObject.insert( "method", ui->methodComboBox->currentIndex() );
    gameObject.insert( "format", ui->formatComboBox->currentIndex() ); // data export format
    gameObject.insert( "imgformat", ui->imgFormatComboBox->currentIndex() ); // image export format
    gameObject.insert( "rotation", ui->rotationCheckBox->isChecked() );
    gameObject.insert( "cropping", ui->croppingCheckBox->isChecked() );
    gameObject.insert( "basename", ui->basenameLineEdit->text() );
    gameObject.insert( "version", QCoreApplication::applicationVersion() );
    QJsonDocument saveDoc(gameObject);
    saveFile.write( saveDoc.toJson() );
}

void MainWindow::openFolder( const QString &path, bool ignoreIfCurrent )
{
    qDebug() << "openFolder: " << path;
    if ( path == inDirn && ignoreIfCurrent ){
        qDebug() << "(openFolder - same as current folder, returning)";
        return;
    }
    QDir dir( path );

    // Do some sanity checks first. Until we have a better soln, prevent opening of
    // a 'buncher' folder itself, since its confusing to open the sheet *output*!
    bool okdir = false;
    if ( dir.exists() && dir.isReadable() && dir.count() > 0 ) okdir = true;
    if ( dir.dirName() == "buncher" && dir.exists( "buncher.data" ) ) okdir = false;

    if ( okdir ) {
        qDebug() << "Valid folder in openFolder(): " << path;
        ui->inputPathEdit->setText( path );
        inDirn = path;
        outDirn = inDirn + "/buncher";
        loadJsonSettings();
        reloadAndRepackAll();
        zoomBestFit(); // (probably better than keeping prev zoom level)
    }
    else
        qDebug() << "Unreadable or empty folder in openFolder(): " << path;
}

void MainWindow::processFolder()
{   qDebug() << "processFolder";
    QDir dir( inDirn );
    if (!dir.exists() ){
        qWarning( "Could not open the input folder. " );
        QMessageBox::warning(this, tr( "SpriteBuncher" ), QString( "Could not open the input folder." ), QMessageBox::Ok );
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    dir.setFilter(QDir::Files); // ignores subfolders.
    QFileInfoList fulllist = dir.entryInfoList();
    for (int i = 0; i < fulllist.size(); ++i) {
        qDebug() << "Processing...  " << fulllist[i].fileName();
    }
    // Sort the initial list and get the fileinfo for each:
    packedsprites.clear();
    for (int i = 0; i < fulllist.size(); ++i) {
        QFileInfo fileInfo = fulllist.at(i);
        QPixmap px( fileInfo.filePath() );
        if ( !px.isNull() ){ // [we could check file format/extension, but instead we just try to open everything]
            bool inserted = false;
            for (int is = 0; is < packedsprites.size(); ++is) {
                // For the MaxRects methods we sort by descending area first [not sure if this is always best, I assumed it is!].
                if ( ( ui->methodComboBox->currentIndex() <= MAXRECTS_CONTACTPOINT || ui->methodComboBox->currentIndex() == ROWS_BY_AREA ) &&
                     px.width()*px.height() >= packedsprites[is].pixmap().width()*packedsprites[is].pixmap().height() ) { // orders by area (descending)
                    if ( !inserted ) {
                        packedsprites.insert( is, PackSprite( px, fileInfo ));
                        inserted = true;
                        break;
                    }
                }
                else if ( ui->methodComboBox->currentIndex() == ROWS_BY_NAME ) { // ordering by name (ascending).
                    if ( !inserted ) { // (simple - always inserts at end.)
                        packedsprites.insert( packedsprites.size(), PackSprite( px, fileInfo ) );
                        inserted = true;
                        break;
                    }
                }
                else if ( ui->methodComboBox->currentIndex() == ROWS_BY_WIDTH &&
                          px.width() >= packedsprites[is].pixmap().width() ) { // order by width (descending)
                    if ( !inserted ) {
                        packedsprites.insert( is, PackSprite( px, fileInfo ) );
                        inserted = true;
                        break;
                    }
                }
                else if ( ui->methodComboBox->currentIndex() == ROWS_BY_HEIGHT &&
                          px.height() >= packedsprites[is].pixmap().height() ) { // order by width (descending)
                    if ( !inserted ) {
                        packedsprites.insert( is, PackSprite( px, fileInfo ) );
                        inserted = true;
                        break;
                    }
                }
            }
            // if not inserted after scan, always insert it at end
            if ( !inserted)
                packedsprites.insert( packedsprites.size(), PackSprite( px, fileInfo ) );
        }
    }
    qDebug() << "Packed sprite list has " << packedsprites.size() << " entries." << " (full file list has " << fulllist.size() << " entries).";
    for (int is = 0; is < packedsprites.size(); ++is){
        qDebug() << is << packedsprites[is].fileInfo().fileName();
    }
    QApplication::restoreOverrideCursor();
}

int MainWindow::pack()
{
    if ( packedsprites.size() == 0 ) {
        qDebug() << "pack(): Empty list - nothing to pack.";
        ui->statusImage->setVisible( false );
        ui->statusLabel->setText( "(No images loaded)" );
        return 0;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int nfails = 0;
    qDebug() << "pack(): Packing method selected: " << ui->methodComboBox->currentIndex() << " = " << ui->methodComboBox->currentText();

    if ( ui->methodComboBox->currentIndex() <= MAXRECTS_CONTACTPOINT ) {
        rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic;
        switch ( ui->methodComboBox->currentIndex() ){
        case MAXRECTS_BESTAREA:
            heuristic = rbp::MaxRectsBinPack::RectBestAreaFit;
            break;
        case MAXRECTS_SHORTSIDE:
            heuristic = rbp::MaxRectsBinPack::RectBestShortSideFit;
            break;
        case MAXRECTS_LONGSIDE:
            heuristic = rbp::MaxRectsBinPack::RectBestLongSideFit;
            break;
        case MAXRECTS_BOTTOMLEFT:
            heuristic = rbp::MaxRectsBinPack::RectBottomLeftRule;
            break;
        case MAXRECTS_CONTACTPOINT:
            heuristic = rbp::MaxRectsBinPack::RectContactPointRule;
            break;
        default:
            qDebug() << "Warning - using default method in pack() - check indexes";
            heuristic = rbp::MaxRectsBinPack::RectBestAreaFit;
            break;
        }
        nfails = Packer::MaxRects( sheetProp, packedsprites, heuristic, ui->rotationCheckBox->isChecked(), ui->croppingCheckBox->isChecked(),
                                   ui->expandSpinBox->value(), ui->extrudeSpinBox->value(), ui->scalingSpinBox->value() );
    }
    else if ( ui->methodComboBox->currentIndex() >= ROWS_BY_NAME ){
        nfails = Packer::Rows( sheetProp, packedsprites, ui->rotationCheckBox->isChecked(), ui->croppingCheckBox->isChecked(),
                               ui->expandSpinBox->value(), ui->extrudeSpinBox->value(), ui->scalingSpinBox->value() );
    }
    QString validStr;
    validStr.setNum( packedsprites.size() - nfails );
    QString failStr;
    failStr.setNum( nfails );
    if ( nfails == 0 ) {
        ui->statusImage->setVisible( true );
        ui->statusLabel->setText( validStr + " images successfully packed!" );
        ui->statusImage->setPixmap( QPixmap( ":/res1/images/tick.png" ));
        ui->publishButton->setEnabled( true );
    }
    else {
        ui->statusImage->setVisible( true );
        ui->statusLabel->setText( failStr + " image(s) failed to pack (" + validStr + " Ok)." );
        ui->statusImage->setPixmap( QPixmap( ":/res1/images/cross.png" ));
        ui->publishButton->setEnabled( false ); // we choose to only allow export if all items were packed.
    }
    QApplication::restoreOverrideCursor();
    return nfails;
}

QImage::Format MainWindow::currentQImageFormat() const
{
    QImage::Format format = QImage::Format_ARGB32;
    switch ( ui->imgFormatComboBox->currentIndex() ) {
        case 0: format = QImage::Format_ARGB32;
        break;
        case 1: format = QImage::Format_ARGB32_Premultiplied;
        break;
        case 2: format = QImage::Format_ARGB4444_Premultiplied;
        break;
        case 3: format = QImage::Format_RGB888;
        break;
        case 4: format = QImage::Format_RGB16; // 'RGB565'
        break;
        case 5: format = QImage::Format_ARGB8565_Premultiplied;
        break;
        case 6: format = QImage::Format_RGB555;
        break;
        default: format = QImage::Format_ARGB32;
        qWarning() << "warning: renderSheet -- using 'default' img format, check combo values";
    }
    return format;
}

// e.g. see http://stackoverflow.com/questions/7451183/how-to-create-image-file-from-qgraphicsscene-qgraphicsview
QImage MainWindow::renderSheet()
{
    ui->graphicsView->scene()->clearSelection(); // (else, selection rects will render to the image!)

    // Since this fn can be called any time, need to hide the other canvas items:
    if ( canvasBG ) canvasBG->setVisible( false );
    if ( canvasSheet ) canvasSheet->setVisible( false );
    qDebug() << "renderSheet - total canvas item count is " << ui->graphicsView->scene()->items().count();

    // make all items fully visible (we used opacity trick to 'hide' them):
    static const int ObjectName = 0;
    QList<QGraphicsItem *> items = ui->graphicsView->scene()->items();
    foreach( QGraphicsItem* itm, items )
        if ( !itm->data(ObjectName).isNull() ) itm->setOpacity( 1.0 );

    // Store current user view settings so we can restore them afterwards:
    QRectF srect = ui->graphicsView->scene()->sceneRect();
    QPainter::RenderHints renderHints = ui->graphicsView->renderHints();

   QImage::Format qiformat = currentQImageFormat();
    qDebug() << "QImage format is " << qiformat << " for our value " << ui->imgFormatComboBox->currentIndex() << " = " << ui->imgFormatComboBox->currentText();
    ui->graphicsView->scene()->setSceneRect(  QRect( 0,0, sheetProp.width, sheetProp.height ) ); // always export exact sheet size
    ui->graphicsView->setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // create transparent image with the correct scene rect size and render settings:
    QImage image(  ui->graphicsView->scene()->sceneRect().size().toSize(), qiformat );
    image.fill(Qt::transparent);
    // [Dev note - could add some dithering options here?]
    QPainter painter(&image);
    ui->graphicsView->scene()->render(&painter);

    // Paint extrusions, if required. This paints beyond the edges of each items.
    // It affects packing only in the sense that we add on to the user's padding and border
    // settings so the extruded pixels dont overlap another item).
    qDebug() << "In renderSheet - sheetProp.extrude =  " << ui->extrudeSpinBox->value();
    int extrude = ui->extrudeSpinBox->value();
    if ( extrude > 0 ) {
        foreach( QGraphicsItem* itm, items ){
            QGraphicsPixmapItem *pmi = qgraphicsitem_cast<QGraphicsPixmapItem*>( itm );
            if (pmi && !pmi->data(ObjectName).isNull()) {
                painter.drawPixmap( QRect( pmi->pos().x(), pmi->pos().y() -extrude, pmi->pixmap().width(), extrude ),
                                    pmi->pixmap(), QRectF( 0.0, 0.0, pmi->pixmap().width(), 1.0 ) ); // top edge

                painter.drawPixmap( QRectF( pmi->pos().x(), pmi->pos().y() + pmi->pixmap().height(), pmi->pixmap().width(), extrude ),
                                    pmi->pixmap(), QRectF( 0.0, pmi->pixmap().height()-1, pmi->pixmap().width(), 1.0 ) ); // bot edge

                painter.drawPixmap( QRectF( pmi->pos().x() - extrude, pmi->pos().y(), extrude, pmi->pixmap().height() ),
                                    pmi->pixmap(), QRectF( 0.0, 0.0, 1.0, pmi->pixmap().height() ) );  // left edge
                painter.drawPixmap( QRectF( pmi->pos().x() + pmi->pixmap().width(), pmi->pos().y(), extrude, pmi->pixmap().height() ),
                                    pmi->pixmap(), QRectF( pmi->pixmap().width() -1.0, 0.0, 1.0, pmi->pixmap().height() ) ); // right edge
            }
        }
    }

    // restore view settings etc now that we're done:
    ui->graphicsView->scene()->setSceneRect( srect );
    ui->graphicsView->setRenderHints( renderHints );
    if ( canvasBG ) canvasBG->setVisible( true );
    if ( canvasSheet ) canvasSheet->setVisible( true );
    // make all items 'nearly invisible' again, for normal view
    foreach( QGraphicsItem* itm, items )
        if ( !itm->data(ObjectName).isNull() ) itm->setOpacity( 0.01 );

    return image;
}

void MainWindow::updateViewWidgets( int nfails )
{
    qDebug() << "updateViewWidgets";
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // always clear the previous data
    ui->listWidget->clear();
    ui->graphicsView->scene()->clear(); // (will also delete the canvasBG and canvasSheet).
qDebug() << " ui->graphicsView->scene() count = " << ui->graphicsView->scene()->items().count();
    canvasBG = NULL;
    canvasSheet = NULL;
    ui->graphicsView->scene()->setSceneRect(  QRect( 0,0, sheetProp.width, sheetProp.height ) ); // required, else view retains old rect size to view
    double curZoom = ui->graphicsView->transform().m11();
    zoomReset();

    // create new canvas bg elements
    ui->graphicsView->setBackgroundBrush( QImage( ":/res1/images/black-bg.png" ));
    ui->graphicsView->setCacheMode( QGraphicsView::CacheNone ); // (Qt docs recommend this when using a tiled background brush).

    if ( nfails > 0 )
        canvasBG = ui->graphicsView->scene()->addRect( 0, 0, sheetProp.width, sheetProp.height, Qt::NoPen, QBrush( QImage( ":/res1/images/redcheckerbg.png" ) ) );
    else
        canvasBG = ui->graphicsView->scene()->addRect( 0, 0, sheetProp.width, sheetProp.height, Qt::NoPen, QBrush( QImage( ":/res1/images/checkerbg.png" ) ) );
    canvasBG->setFlag( QGraphicsItem::ItemIsSelectable, false );
    canvasBG->setZValue( 0 );

    // populate canvas and listWidget. Remember, the packedrects dont include the border pixels, but do include their padding.
    for (int i = 0; i < packedsprites.size(); ++i) {
        rbp::Rect packedRect =  packedsprites[i].packedRect();
        if (packedRect.height > 0) {
            QPixmap pm;
                pm = packedsprites[i].pixmap();
            // if rotated, need to display a rotated pixmap on the canvas:
            if ( packedsprites[i].isRotated() ){
                QTransform trans;
                trans = trans.rotate( 90 );
                QPixmap rotpm = pm.transformed( trans );
                pm = rotpm;
            }
            PixmapItem *item = new PixmapItem( pm ); // (PixmapItem derives from QGraphicsPixmapItem).
            item->setEnabled( true );
            item->setZValue( 1 );
            ui->graphicsView->scene()->addItem( item );
            static const int ObjectName = 0;
            item->setData(ObjectName, packedsprites[i].fileInfo().fileName() ); // store the name as data
            item->setFlag( QGraphicsItem::ItemIsSelectable, true );
            item->setPos( packedRect.x + ui->borderSpinBox->value(), packedRect.y + ui->borderSpinBox->value() );

            // make the listwidget item and a useful tooltip:
            QPixmap opm = packedsprites[i].originalPixmap(); // we choose to use orig pm, before crop/rot/expand.
            ui->listWidget->addItem(new QListWidgetItem( QIcon( opm ), packedsprites[i].fileInfo().fileName() ) );
            QString pxstr, pystr, widstr, hgtstr;
            pxstr.setNum( int(item->pos().x() ));
            pystr.setNum( int(item->pos().y() ));
            widstr.setNum( pm.width() ); // use actual current pm for any data!
            hgtstr.setNum( pm.height() );
            QString rotStr;
            if ( packedsprites[i].isRotated() )
                rotStr = " (rotated)";
            QString szStr;
            if ( packedsprites[i].isCropped() )
                szStr = " (cropped)";
            else if ( packedsprites[i].isExpanded() ) // (could actually be cropped and then expanded).
                szStr = " (expanded)";
            item->setToolTip( packedsprites[i].fileInfo().fileName() + "\nPos: " + pxstr + ", " + pystr + rotStr + "\nSize: " + widstr + " x " + hgtstr + szStr );
        }
    }

    // [At this point we could just let the graphicsview to show as-is. However, we want to show the sheet rendered with the users chosen
    // output format, and therefore its better to display the *actual* rendered sheet instead of the graphicsview items. By making the original
    // items almost transparent we retain selection and other events].

    QImage sheet = renderSheet();
    QPixmap sc;
    sc.convertFromImage( sheet );
    canvasSheet= new QGraphicsPixmapItem( sc );
    canvasSheet->setFlag( QGraphicsItem::ItemIsSelectable, false );
    ui->graphicsView->scene()->addItem( canvasSheet );
    canvasSheet->setZValue( 2 ); // in front of actual items (however, does mean we wont see some selection rects behind sheet).
    ui->graphicsView->scale( curZoom, curZoom ); // restore prev zoom
    QApplication::restoreOverrideCursor();
}
