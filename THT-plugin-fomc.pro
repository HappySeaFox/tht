TARGET = common-fomc
TEMPLATE = lib

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets

NVER1=0
NVER2=8
NVER3=1

SOURCES += fomcplugin.cpp
HEADERS += fomcplugin.h

TRANSLATIONS += ts/common_fomc_ru.ts \
                ts/common_fomc_uk.ts

THT_PLUGIN_NAME="FOMC"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID=B5FD1969-B414-472D-B5E6-8900F0C086FD
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"
THT_PLUGIN_URL="http://sourceforge.net/projects/tht"

include(THT-common.pri)
include(THT-plugins.pri)
