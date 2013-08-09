TARGET = common-chat
TEMPLATE = lib

INCLUDEPATH += qxmpp
DEPENDPATH += qxmpp

DEFINES += QXMPP_STATIC

QT += core gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets

NVER1=0
NVER2=9
NVER3=1

LIBS += -ldnsapi -lws2_32

# qxmpp files
FILES = $$system(dir /ON /B "qxmpp\\*.h" 2>nul)

for(file, FILES) {
    HEADERS += qxmpp\\$$file
}

FILES = $$system(dir /ON /B "qxmpp\\*.cpp" 2>nul)

for(file, FILES) {
    SOURCES += qxmpp\\$$file
}

# chat files
SOURCES += chatwindow.cpp \
    chatpage.cpp \
    roominfo.cpp \
    messagedialog.cpp \
    coloranimation.cpp \
    chatplugin.cpp \
    chatoptions.cpp

HEADERS += chatwindow.h \
    chatpage.h \
    roominfo.h \
    messagedialog.h \
    coloranimation.h \
    chatplugin.h \
    chatsettings.h \
    chatoptions.h

FORMS += chatwindow.ui \
    chatpage.ui \
    messagedialog.ui \
    chatoptions.ui

TRANSLATIONS += ts/common_chat_ru.ts \
                ts/common_chat_uk.ts

RESOURCES += THT-plugin-chat.qrc

THT_PLUGIN_NAME="Chat"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID="6064096F-223C-45E7-A971-0CCBD395F054"
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"
THT_PLUGIN_URL="http://sourceforge.net/projects/tht"

include(THT-common.pri)
include(THT-plugins.pri)

# QXMPP doesn't support QT_USE_QSTRINGBUILDER
DEFINES -= QT_USE_QSTRINGBUILDER
