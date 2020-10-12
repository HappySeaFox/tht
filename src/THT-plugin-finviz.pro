TARGET = addtickersfrom-finviz
TEMPLATE = lib

QT += core gui network

NVER1=0
NVER2=5
NVER3=0

DEFINES += FINVIZ=\\\"finviz.com\\\"
DEFINES += FINVIZ_ELITE=\\\"elite.finviz.com\\\"

SOURCES += plugins/finviz/finvizurlmanager.cpp \
    plugins/finviz/finvizdownloader.cpp \
    plugins/finviz/finvizlinkselector.cpp \
    plugins/finviz/finvizcookiejar.cpp \
    plugins/finviz/finvizaccesssetup.cpp \
    plugins/finviz/finvizplugin.cpp \
    plugins/finviz/finviztools.cpp \
    plugins/finviz/finvizurl.cpp

HEADERS += plugins/finviz/finvizurlmanager.h \
    plugins/finviz/finvizdownloader.h \
    plugins/finviz/finvizlinkselector.h \
    plugins/finviz/finvizcookiejar.h \
    plugins/finviz/finvizaccesssetup.h \
    plugins/finviz/finvizplugin.h \
    plugins/finviz/finvizurl.h \
    plugins/finviz/finviztools.h

FORMS += \
    plugins/finviz/finvizlinkselector.ui \
    plugins/finviz/finvizaccesssetup.ui

RESOURCES += THT-plugin-finviz.qrc

THT_PLUGIN_NAME="Finviz"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID="0DB2CEDA-9A88-4E31-9E76-2FD77BEF0993"
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"
THT_PLUGIN_URL="https://github.com/smoked-herring/tht"
THT_PLUGIN_LICENSE_TEXT="GNU GPLv3+. See http://www.gnu.org/licenses/gpl-3.0.txt"

include(THT-common.pri)
include(THT-plugins.pri)
