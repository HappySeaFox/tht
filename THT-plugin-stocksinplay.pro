TARGET = addtickersfrom-stocksinplay
TEMPLATE = lib

QT += core gui network

NVER1=0
NVER2=4
NVER3=1

DEFINES += STOCKSINPLAY=\\\"www.stocksinplay.ru\\\"

SOURCES += plugins/stocksinplay/stocksinplayurlmanager.cpp \
    plugins/stocksinplay/stocksinplaydownloader.cpp \
    plugins/stocksinplay/stocksinplaylinkselector.cpp \
    plugins/stocksinplay/stocksinplaycookiejar.cpp \
    plugins/stocksinplay/stocksinplayaccesssetup.cpp \
    plugins/stocksinplay/stocksinplayplugin.cpp \
    plugins/stocksinplay/stocksinplaylogincookiejar.cpp \
    plugins/stocksinplay/stocksinplaycredentials.cpp \
    plugins/stocksinplay/stocksinplaylogin.cpp \
    plugins/stocksinplay/stocksinplaytools.cpp \
    plugins/stocksinplay/stocksinplayurl.cpp

HEADERS += plugins/stocksinplay/stocksinplayurlmanager.h \
    plugins/stocksinplay/stocksinplaydownloader.h \
    plugins/stocksinplay/stocksinplaylinkselector.h \
    plugins/stocksinplay/stocksinplaycookiejar.h \
    plugins/stocksinplay/stocksinplayaccesssetup.h \
    plugins/stocksinplay/stocksinplayplugin.h \
    plugins/stocksinplay/stocksinplayurl.h \
    plugins/stocksinplay/stocksinplaylogincookiejar.h \
    plugins/stocksinplay/stocksinplaycredentials.h \
    plugins/stocksinplay/stocksinplaylogin.h \
    plugins/stocksinplay/stocksinplaytools.h

FORMS += \
    plugins/stocksinplay/stocksinplaylinkselector.ui \
    plugins/stocksinplay/stocksinplayaccesssetup.ui \
    plugins/stocksinplay/stocksinplaycredentials.ui

RESOURCES += THT-plugin-stocksinplay.qrc

THT_PLUGIN_NAME="Stocks In Play"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID="A7EACD66-3936-476D-8D0C-125F60108603"
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"
THT_PLUGIN_URL="http://sourceforge.net/projects/tht"
THT_PLUGIN_LICENSE_TEXT="GNU GPLv3+. See http://www.gnu.org/licenses/gpl-3.0.txt"

include(THT-common.pri)
include(THT-plugins.pri)
