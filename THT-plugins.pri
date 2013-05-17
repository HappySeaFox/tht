RC_FILE = tht-lib.rc

QMAKE_POST_LINK += $$mle(if not exist \"$${OUT_PWD}/$(DESTDIR_TARGET)/../plugins\" mkdir \"$${OUT_PWD}/$(DESTDIR_TARGET)/../plugins\")
QMAKE_POST_LINK += $$mle(copy /y \"$${OUT_PWD}/$(DESTDIR_TARGET)\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/../plugins\")
