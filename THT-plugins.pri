RC_FILE = tht-plugins.rc

QMAKE_POST_LINK += $$mle(if not exist \"$${OUT_PWD}/$(DESTDIR_TARGET)/../plugins\" mkdir \"$${OUT_PWD}/$(DESTDIR_TARGET)/../plugins\")
QMAKE_POST_LINK += $$mle(copy /y \"$${OUT_PWD}/$(DESTDIR_TARGET)\" \"$${OUT_PWD}/$(DESTDIR_TARGET)/../plugins\")

RC_FILE_INCLUDE=$$replace(_PRO_FILE_, .pro$, .rc)
RC_FILE_INCLUDE=$$basename(RC_FILE_INCLUDE)

DEFINES += RC_FILE_INCLUDE=$$RC_FILE_INCLUDE

defineReplace(copyFilesToZip) {
    q=$$1
    FILES=$$join(q, " $$", $$)
    eval(FILES=$$FILES)

    for(file, FILES) {
        dir=$$dirname(file)

        !isEmpty(dir) {
            distsrc.commands += $$mle(mkdir \"$$2\\$$dir\")
            distsrc.commands += $$mle(copy /y \"$$_PRO_FILE_PWD_\\$$file\" \"$$2\\$$dir\")
        } else {
            distsrc.commands += $$mle(copy /y \"$$_PRO_FILE_PWD_\\$$file\" \"$$2\")
        }
    }

    export(distsrc.commands)

    return ( true )
}

# check for 7z
ZIP=$$findexe("7z.exe")

!isEmpty(ZIP) {
    message("7Z is found, will create custom dist targets")

    # source archive
    TA="tht-$$TARGET-$$VERSION"
    T="$${OUT_PWD}\\$$TA"

    distsrc.commands += $$mle(if exist \"$$T\" rd /S /Q \"$$T\")
    distsrc.commands += $$mle(mkdir \"$$T\")

    PROFILE=$$basename(_PRO_FILE_)

    X=$$copyFilesToZip(PROFILE, $$T)
    X=$$copyFilesToZip(SOURCES, $$T)
    X=$$copyFilesToZip(HEADERS, $$T)
    X=$$copyFilesToZip(FORMS, $$T)
    X=$$copyFilesToZip(RESOURCES, $$T)
    X=$$copyFilesToZip(RC_FILE_INCLUDE, $$T)

    distsrc.commands += $$mle(del /F /Q \"$${TA}.zip\")
    distsrc.commands += $$mle($$ZIP a -r -tzip -mx=9 \"$${TA}.zip\" \"$$T\")
    distsrc.commands += $$mle(rd /S /Q \"$$T\")

    QMAKE_EXTRA_TARGETS += distsrc

} else {
    warning("7Z is not found, will not create custom dist targets")
}
