TARGET = addtickersfrom-stocksinplay
TEMPLATE = lib

QT += core gui network

NVER1=0
NVER2=4
NVER3=1

DEFINES += STOCKSINPLAY=\\\"www.stocksinplay.ru\\\"

SOURCES += stocksinplayurlmanager.cpp \
    stocksinplaydownloader.cpp \
    stocksinplaylinkselector.cpp \
    stocksinplaycookiejar.cpp \
    stocksinplayaccesssetup.cpp \
    stocksinplayplugin.cpp \
    stocksinplaylogincookiejar.cpp \
    stocksinplaycredentials.cpp \
    stocksinplaylogin.cpp \
    stocksinplaytools.cpp

HEADERS += stocksinplayurlmanager.h \
    stocksinplaydownloader.h \
    stocksinplaylinkselector.h \
    stocksinplaycookiejar.h \
    stocksinplayaccesssetup.h \
    stocksinplayplugin.h \
    stocksinplayurl.h \
    stocksinplaylogincookiejar.h \
    stocksinplaycredentials.h \
    stocksinplaylogin.h \
    stocksinplaytools.h

FORMS += \
    stocksinplaylinkselector.ui \
    stocksinplayaccesssetup.ui \
    stocksinplaycredentials.ui

RESOURCES += THT-plugin-stocksinplay.qrc

THT_PLUGIN_NAME="Stocks In Play"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID="A7EACD66-3936-476D-8D0C-125F60108603"
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"
THT_PLUGIN_URL="http://sourceforge.net/projects/tht"

include(THT-common.pri)
include(THT-plugins.pri)
