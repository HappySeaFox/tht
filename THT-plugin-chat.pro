TARGET = common-chat
TEMPLATE = lib

INCLUDEPATH += qxmpp
DEPENDPATH += qxmpp

DEFINES += QXMPP_STATIC

QT += core gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT *= widgets

NVER1=0
NVER2=9
NVER3=2

LIBS += -ldnsapi -lws2_32

# qxmpp files
HEADERS += $$files(qxmpp\\QXmpp*.h)
SOURCES += $$files(qxmpp\\QXmpp*.cpp)

lessThan(QT_MAJOR_VERSION, 5) {
    HEADERS += qxmpp/qdnslookup.h qxmpp/qdnslookup_p.h
    SOURCES += qxmpp/qdnslookup.cpp qxmpp/qdnslookup_win.cpp
}

# chat files
SOURCES += chatwindow.cpp \
    chatpage.cpp \
    roominfo.cpp \
    messagedialog.cpp \
    coloranimation.cpp \
    chatplugin.cpp \
    chatoptions.cpp \
    chattools.cpp \
    chatmessages.cpp \
    configureroom.cpp \
    sendinvitations.cpp

HEADERS += chatwindow.h \
    chatpage.h \
    roominfo.h \
    messagedialog.h \
    coloranimation.h \
    chatplugin.h \
    chatsettings.h \
    chatoptions.h \
    chattools.h \
    chatmessages.h \
    configureroom.h \
    sendinvitations.h

FORMS += chatwindow.ui \
    chatpage.ui \
    messagedialog.ui \
    chatoptions.ui \
    chatmessages.ui \
    configureroom.ui \
    sendinvitations.ui

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
