TARGET = addtickersfrom-finviz
TARGET_EXT = .dll
TEMPLATE = lib

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += FINVIZ=\\\"finviz.com\\\"
DEFINES += FINVIZ_ELITE=\\\"elite.finviz.com\\\"

LIBS += -L$${OUT_PWD}/$(DESTDIR_TARGET)/.. -lTHT

SOURCES += finvizurlmanager.cpp \
    finvizdownloader.cpp \
    finvizlinkselector.cpp \
    finvizcookiejar.cpp \
    finvizaccesssetup.cpp \
    finvizplugin.cpp

HEADERS += finvizurlmanager.h \
    finvizdownloader.h \
    finvizlinkselector.h \
    finvizcookiejar.h \
    finvizaccesssetup.h \
    finvizplugin.h \
    finvizurl.h

FORMS += finvizdownloader.ui \
    finvizlinkselector.ui \
    finvizaccesssetup.ui

TRANSLATIONS += ts/addtickersfrom_finviz_ru.ts \
                ts/addtickersfrom_finviz_uk.ts

RESOURCES += tht-plugin-finviz.qrc

include(THT-common.pri)
include(THT-plugins.pri)
