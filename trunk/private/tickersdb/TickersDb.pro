QT += core gui network sql

TARGET = TickersDb
TEMPLATE = app

!exists($${_PRO_FILE_PWD_}/../../THT.pro) {
    error("This is not a THT source tree, don't know what to do")
}

DEFINES += TICKERS_DIR=$$sprintf("\"\\\"%1\\\"\"", $${_PRO_FILE_PWD_}/../../tickersdb)

SOURCES += main.cpp\
        widget.cpp \
        networkaccess.cpp \
    csvreader.cpp

HEADERS  += widget.h \
        networkaccess.h \
    csvreader.h

FORMS    += widget.ui

RESOURCES += \
    tickersdb.qrc

win32 {
    RC_FILE = tickersdb.rc
}
