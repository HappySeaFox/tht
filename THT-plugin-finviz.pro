TARGET = addtickersfrom-finviz
TEMPLATE = lib

QT += core gui network

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

FORMS += \
    finvizlinkselector.ui \
    finvizaccesssetup.ui

RESOURCES += THT-plugin-finviz.qrc

THT_PLUGIN_NAME="Finviz"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID="0DB2CEDA-9A88-4E31-9E76-2FD77BEF0993"
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"
THT_PLUGIN_URL="http://sourceforge.net/projects/tht"

include(THT-common.pri)
include(THT-plugins.pri)
