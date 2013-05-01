TEMPLATE = app

TARGET = qt-ts-cleaner

CONFIG += console
CONFIG -= app_bundle

QT += core
QT -= gui

SOURCES += main.cpp

# search an executable in PATH
defineReplace(findexe) {
    return ( $$system(for %i in ($$1) do @echo. %~$PATH:i) )
}

# escape command to allow multiple lines in Makefile
defineReplace(mle) {
    return ( $$1$$escape_expand(\\n\\t) )
}

# check for upx
UPX=$$findexe("upx.exe")

isEmpty(HOST64):!isEmpty(UPX) {
    message("UPX is found, will pack the executable after linking")
    QMAKE_POST_LINK += $$mle($$UPX -9 \"$${OUT_PWD}/$(DESTDIR_TARGET)\")
} else {
    warning("UPX is not found, will not pack the executable")
}
