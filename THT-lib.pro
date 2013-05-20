TARGET = THT
TARGET_EXT = .dll
TEMPLATE = lib

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += FINVIZ=\\\"finviz.com\\\"
DEFINES += FINVIZ_ELITE=\\\"elite.finviz.com\\\"

SOURCES += datamanagerbase.cpp \
    settings.cpp \
    plugin.cpp \
    tools.cpp \
    csvreader.cpp \
    networkaccess.cpp

HEADERS += datamanagerbase.h \
    settings.h \
    plugin.h \
    tools.h \
    csvreader.h \
    networkaccess.h

FORMS += datamanagerbase.ui

TRANSLATIONS += ts/tht_lib_ru.ts \
                ts/tht_lib_uk.ts

RC_FILE = tht-lib.rc

include(THT-common.pri)
