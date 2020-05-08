TARGET = addtickersfrom-briefing-splits
TEMPLATE = lib

QT += core gui network

NVER1=0
NVER2=9
NVER3=0

SOURCES += plugins/briefing/briefingsplitsplugin.cpp \
    plugins/briefing/briefingsplitsdownloader.cpp

HEADERS += plugins/briefing/briefingsplitsplugin.h \
    plugins/briefing/briefingsplitsdownloader.h

RESOURCES += THT-plugin-briefing-splits.qrc

THT_PLUGIN_NAME="Briefing Stock Splits"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID="E9E5E9BE-AC4F-47DC-AF4D-F04370F5DEB5"
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"
THT_PLUGIN_URL="http://sourceforge.net/projects/tht"
THT_PLUGIN_LICENSE_TEXT="GNU GPLv3+. See http://www.gnu.org/licenses/gpl-3.0.txt"

include(THT-common.pri)
include(THT-plugins.pri)
