TARGET = common-chat
TEMPLATE = lib

INCLUDEPATH += qxmpp
DEPENDPATH += qxmpp

DEFINES += QXMPP_STATIC

QT += core gui network xml

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
SOURCES += plugins/chat/chatwindow.cpp \
    plugins/chat/chatmessages.cpp \
    plugins/chat/chatoptions.cpp \
    plugins/chat/chatpage.cpp \
    plugins/chat/chatplugin.cpp \
    plugins/chat/chattools.cpp \
    plugins/chat/coloranimation.cpp \
    plugins/chat/configureroom.cpp \
    plugins/chat/messagedialog.cpp \
    plugins/chat/roominfo.cpp \
    plugins/chat/sendinvitations.cpp

HEADERS += plugins/chat/chatwindow.h \
    plugins/chat/chatmessages.h \
    plugins/chat/chatoptions.h \
    plugins/chat/chatpage.h \
    plugins/chat/chatplugin.h \
    plugins/chat/chatsettings.h \
    plugins/chat/chattools.h \
    plugins/chat/coloranimation.h \
    plugins/chat/configureroom.h \
    plugins/chat/messagedialog.h \
    plugins/chat/roominfo.h \
    plugins/chat/sendinvitations.h

FORMS += plugins/chat/chatwindow.ui \
    plugins/chat/chatmessages.ui \
    plugins/chat/chatoptions.ui \
    plugins/chat/chatpage.ui \
    plugins/chat/configureroom.ui \
    plugins/chat/messagedialog.ui \
    plugins/chat/sendinvitations.ui

RESOURCES += THT-plugin-chat.qrc

THT_PLUGIN_NAME="Chat"
THT_PLUGIN_AUTHOR="Dmitry Baryshev"
THT_PLUGIN_UUID="6064096F-223C-45E7-A971-0CCBD395F054"
THT_PLUGIN_COPYRIGHT="(C) 2013 $$THT_PLUGIN_AUTHOR"
THT_PLUGIN_URL="http://sourceforge.net/projects/tht"
THT_PLUGIN_LICENSE_TEXT="GNU GPLv3+. See http://www.gnu.org/licenses/gpl-3.0.txt"

include(THT-common.pri)
include(THT-plugins.pri)

# QXMPP doesn't support QT_USE_QSTRINGBUILDER
DEFINES -= QT_USE_QSTRINGBUILDER
