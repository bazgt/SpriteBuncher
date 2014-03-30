#ifndef CUSTOMSTYLESHEET_H
#define CUSTOMSTYLESHEET_H

const QString mainStyleSheet = "QWidget, QWidget *{ background: QLinearGradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 rgb(64, 64, 64), stop: 1 rgb(120, 120, 120) );"
        "color: rgb(255, 255, 255);"
        "selection-background-color: rgb( 28, 120, 222 );"
        "}"
        "QListWidget, QListWidget *{ background: QLinearGradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 rgb(120, 120, 120), stop: 1 rgb(90, 90, 90) );"
        "color: rgb(255, 255, 255);"
        "}"
        "QLabel, QLabel *{ background: rgba(0, 0, 0, 0);"
        "color: rgb(255, 255, 255);"
        "}"
        "    QScrollBar:vertical  {"
        "     background: rbg(40,40,40);"

        "}"
        " QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical  {"
        "image: url(:/res1/images/up_arrow.png);"
        " width: 7px;"
        " height: 7px;"
        "}"

        "QSizeGrip { "
        "background:rgb(110,110,110);"
        "}";

#endif // CUSTOMSTYLESHEET_H
