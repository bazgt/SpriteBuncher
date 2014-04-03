#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T14:54:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = SpriteBuncher
TEMPLATE = app

RC_FILE = myapp.rc
ICON = buncher.icns

SOURCES += main.cpp\
        mainwindow.cpp \
    	maxrects/Rect.cpp \
    	maxrects/MaxRectsBinPack.cpp \
    	packer.cpp \
        pixmapitem.cpp \
        packsprite.cpp \
        dataexporter.cpp

HEADERS  += mainwindow.h \
        maxrects/Rect.h \
        maxrects/MaxRectsBinPack.h \
        packer.h \
        pixmapitem.h \
        packsprite.h \
        customstylesheet.h \
        dataexporter.h

FORMS    += mainwindow.ui \
        aboutbox.ui

RESOURCES += \
    res.qrc

# Currently unused:
#win32 {
#    formats.files = $${PWD}/formats
#    formats.path = $${OUT_PWD} # $$DESTDIR
#    INSTALLS += formats
#}

#mac{
#    formats.files = formats
#    formats.path = Contents/MacOS/
#    QMAKE_BUNDLE_DATA += formats
#}
