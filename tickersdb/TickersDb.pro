QT += core gui network sql

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
