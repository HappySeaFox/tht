QT += core gui network sql webkit

TARGET = TickersDb
TEMPLATE = app

DEFINES += TICKERS_DIR=$$sprintf("\"\\\"%1\\\"\"", $$_PRO_FILE_PWD_)

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

win32:RC_FILE = tickersdb.rc
