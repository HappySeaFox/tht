TARGET_EXT = .dll

RC_FILE = tht-plugins.rc

LIBS += -L$${OUT_PWD}/$(DESTDIR_TARGET)/.. -lTHT-lib

PLUGIN_LICENSE=$$replace(_PRO_FILE_, \\.pro$, -LICENSE.txt)
PLUGIN_LICENSE=$$replace(PLUGIN_LICENSE, /, \\)

QMAKE_POST_LINK += $$mle(if not exist \"$${OUT_PWD}/$(DESTDIR_TARGET)/../plugins\" mkdir \"$${OUT_PWD}/$(DESTDIR_TARGET)/../plugins\")
QMAKE_POST_LINK += $$mle(copy /y \"$${OUT_PWD}/$(DESTDIR_TARGET)\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/../plugins\")

exists($$PLUGIN_LICENSE) {
    QMAKE_POST_LINK += $$mle(copy /y \"$$PLUGIN_LICENSE\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/../plugins/$${TARGET}-LICENSE.txt\")
}

RC_FILE_INCLUDE=$$replace(_PRO_FILE_, \\.pro$, .rc)
RC_FILE_INCLUDE=$$basename(RC_FILE_INCLUDE)-generated

system(echo $${LITERAL_HASH}undef THT_PLUGIN_AUTHOR > $$RC_FILE_INCLUDE)
system(echo $${LITERAL_HASH}undef THT_PLUGIN_COPYRIGHT >> $$RC_FILE_INCLUDE)
system(echo $${LITERAL_HASH}define THT_PLUGIN_AUTHOR \"$$THT_PLUGIN_AUTHOR\" >> $$RC_FILE_INCLUDE)
system(echo $${LITERAL_HASH}define THT_PLUGIN_COPYRIGHT \"$$THT_PLUGIN_COPYRIGHT\" >> $$RC_FILE_INCLUDE)

DEFINES += RC_FILE_INCLUDE=$$RC_FILE_INCLUDE

DEFINES += THT_PLUGIN_NAME=$$sprintf("\"\\\"%1\\\"\"", $$THT_PLUGIN_NAME)
DEFINES += THT_PLUGIN_AUTHOR=$$sprintf("\"\\\"%1\\\"\"", $$THT_PLUGIN_AUTHOR)
DEFINES += THT_PLUGIN_VERSION=$$sprintf("\"\\\"%1\\\"\"", $$VERSION)
DEFINES += THT_PLUGIN_UUID=$$sprintf("\"\\\"%1\\\"\"", $$THT_PLUGIN_UUID)
DEFINES += THT_PLUGIN_DEPRECATES_UUIDS=$$sprintf("\"\\\"%1\\\"\"", $$THT_PLUGIN_DEPRECATES_UUIDS)
DEFINES += THT_PLUGIN_URL=$$sprintf("\"\\\"%1\\\"\"", $$THT_PLUGIN_URL)
DEFINES += THT_PLUGIN_COPYRIGHT=$$sprintf("\"\\\"%1\\\"\"", $$THT_PLUGIN_COPYRIGHT)
DEFINES += THT_PLUGIN_LICENSE_TEXT=$$sprintf("\"\\\"%1\\\"\"", $$THT_PLUGIN_LICENSE_TEXT)
