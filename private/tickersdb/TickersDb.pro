QT += core gui network sql webkit

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
    defineReplace(mle) {
        return ( $$1$$escape_expand(\\n\\t) )
    }

    RC_FILE = tickersdb.rc

    QTLIBS=QtCore4.dll QtGui4.dll QtNetwork4.dll QtSql4.dll QtWebkit4.dll

    T="$${OUT_PWD}/$(DESTDIR_TARGET)/../"

    for(ql, QTLIBS) {
        QMAKE_POST_LINK += $$mle(@if not exist \"$$T\\$$ql\" copy /y \"$$[QT_INSTALL_BINS]\\$$ql\" \"$$T\")
    }
}
