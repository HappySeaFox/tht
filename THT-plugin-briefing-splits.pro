TARGET = addtickersfrom-briefing-splits
TEMPLATE = lib

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets

NVER1=0
NVER2=9
NVER3=0

SOURCES += briefingsplitsplugin.cpp \
    briefingsplitsdownloader.cpp

HEADERS += briefingsplitsplugin.h \
    briefingsplitsdownloader.h

RESOURCES += THT-plugin-briefing-splits.qrc

THT_PLUGIN_NAME="Briefing Stock Splits"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID="E9E5E9BE-AC4F-47DC-AF4D-F04370F5DEB5"
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"
THT_PLUGIN_URL="http://sourceforge.net/projects/tht"

TS_PREFIX=addtickersfrom_briefing_splits

include(THT-common.pri)
include(THT-plugins.pri)
