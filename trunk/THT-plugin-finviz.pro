TARGET = addtickersfrom-finviz
TEMPLATE = lib

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets

NVER1=0
NVER2=5
NVER3=0

DEFINES += FINVIZ=\\\"finviz.com\\\"
DEFINES += FINVIZ_ELITE=\\\"elite.finviz.com\\\"

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

RESOURCES += THT-plugin-finviz.qrc

THT_PLUGIN_NAME="Finviz"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID=0DB2CEDA-9A88-4E31-9E76-2FD77BEF0993
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"

include(THT-common.pri)
include(THT-plugins.pri)
