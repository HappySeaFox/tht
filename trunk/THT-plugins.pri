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
DEFINES += THT_PLUGIN_URL=$$sprintf("\"\\\"%1\\\"\"", $$THT_PLUGIN_URL)
DEFINES += THT_PLUGIN_COPYRIGHT=$$sprintf("\"\\\"%1\\\"\"", $$THT_PLUGIN_COPYRIGHT)

defineReplace(copyFilesToZip) {
    q=$$1
    FILES=$$join(q, " \$\$", "\$\$")
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

    distsrc.commands += $$mle(del /F /Q \"$${TA}.zip\")
    distsrc.commands += $$mle($$ZIP a -r -tzip -mx=9 \"$${TA}.zip\" \"$$T\")
    distsrc.commands += $$mle(rd /S /Q \"$$T\")

    QMAKE_EXTRA_TARGETS += distsrc

    # standalone binary
    TA="tht-standalone-$$TARGET-$$VERSION"
    T="$${OUT_PWD}\\$$TA"

    distbin.commands += $$mle(if exist \"$$T\" rd /S /Q \"$$T\")
    distbin.commands += $$mle(mkdir \"$$T\")
    distbin.commands += $$mle(mkdir \"$$T/translations\")
    distbin.commands += $$mle(mkdir \"$$T/plugins\")

    # binary & necessary files
    distbin.commands += $$mle(copy /y \"$${OUT_PWD}/$(DESTDIR_TARGET)\" \"$$T/plugins\")

    exists($$PLUGIN_LICENSE) {
        distbin.commands += $$mle(copy /y \"$$PLUGIN_LICENSE\" \"$$T/plugins/$${TARGET}-LICENSE.txt\")
    }

    for(qm, QMFILES) {
        distbin.commands += $$mle(copy /y \"$${_PRO_FILE_PWD_}\\$$qm\" \"$$T/translations\")
    }

    # compress
    distbin.commands += $$mle(del /F /Q $${TA}$${HOST64}.zip)
    distbin.commands += $$mle($$ZIP a -r -tzip -mx=9 $${TA}$${HOST64}.zip \"$$T\")
    distbin.commands += $$mle(rd /S /Q \"$$T\")

    QMAKE_EXTRA_TARGETS += distbin
} else {
    warning("7Z is not found, will not create custom dist targets")
}

INNO_APPID="{{$$THT_PLUGIN_UUID}"

exists($$INNO) {
    message("Inno Setup is found, will create a setup file for $$TARGET in a custom dist target")

    LANGS=$$system(dir /B \"$$INNO\\..\\Languages\")

    ISS="tht-$$TARGET-$${VERSION}.iss"

    iss.commands += $$mle(echo $${LITERAL_HASH}define MyAppName \"Trader\'s Home Task"," $$TARGET plugin\" > $$ISS)
    iss.commands += $$mle(echo $${LITERAL_HASH}define MyAppPublisher \"$$THT_PLUGIN_AUTHOR\" >> $$ISS)

    !isEmpty(THT_PLUGIN_URL) {
        iss.commands += $$mle(echo $${LITERAL_HASH}define MyAppURL \"$$THT_PLUGIN_URL\" >> $$ISS)
    }

    iss.commands += $$mle(echo [Setup] >> $$ISS)

    !isEmpty(HOST64) {
        iss.commands += $$mle(echo ArchitecturesAllowed=x64 >> $$ISS)
        iss.commands += $$mle(echo ArchitecturesInstallIn64BitMode=x64 >> $$ISS)
    }

    iss.commands += $$mle(echo AppId=$$INNO_APPID >> $$ISS)
    iss.commands += $$mle(echo AppName={$${LITERAL_HASH}MyAppName} >> $$ISS)
    iss.commands += $$mle(echo AppVersion=$$VERSION >> $$ISS)
    iss.commands += $$mle(echo AppPublisher={$${LITERAL_HASH}MyAppPublisher} >> $$ISS)

    !isEmpty(THT_PLUGIN_URL) {
        iss.commands += $$mle(echo AppPublisherURL={$${LITERAL_HASH}MyAppURL} >> $$ISS)
        iss.commands += $$mle(echo AppUpdatesURL={$${LITERAL_HASH}MyAppURL} >> $$ISS)
    }

    iss.commands += $$mle(echo DefaultDirName={pf}\\Trader\'s Home Task >> $$ISS)
    iss.commands += $$mle(echo DefaultGroupName={$${LITERAL_HASH}MyAppName} >> $$ISS)

    exists($$PLUGIN_LICENSE) {
        iss.commands += $$mle(echo LicenseFile=$$PLUGIN_LICENSE >> $$ISS)
    }

    iss.commands += $$mle(echo OutputDir=. >> $$ISS)
    iss.commands += $$mle(echo OutputBaseFilename=tht-setup-$${TARGET}-$${VERSION}$${HOST64} >> $$ISS)
    iss.commands += $$mle(echo SetupIconFile=$${_PRO_FILE_PWD_}\\images\\chart.ico >> $$ISS)
    iss.commands += $$mle(echo Compression=lzma >> $$ISS)
    iss.commands += $$mle(echo SolidCompression=yes >> $$ISS)
    iss.commands += $$mle(echo UninstallDisplayName={$${LITERAL_HASH}MyAppName} >> $$ISS)
    iss.commands += $$mle(echo UninstallDisplayIcon={app}\\THT.exe >> $$ISS)
    iss.commands += $$mle(echo MinVersion="0,5.1" >> $$ISS)

    iss.commands += $$mle(echo [Languages] >> $$ISS)
    iss.commands += $$mle(echo Name: \"english\"; MessagesFile: \"compiler:Default.isl\" >> $$ISS)

    for(lng, LANGS) {
        lng = $$find(lng, .isl$)

        !isEmpty(lng) {
            lngname=$$lng
            lngname ~= s/\\./x
            iss.commands += $$mle(echo Name: \"$$lngname\"; MessagesFile: \"compiler:Languages\\$$lng\" >> $$ISS)
        }
    }

    iss.commands += $$mle(echo [Files] >> $$ISS)
    iss.commands += $$mle(echo Source: \"$${OUT_PWD}/$(DESTDIR_TARGET)\"; DestDir: \"{app}/plugins\"; Flags: ignoreversion >> $$ISS)

    exists($$PLUGIN_LICENSE) {
        iss.commands += $$mle(echo Source: \"$$PLUGIN_LICENSE\"; DestDir: \"{app}/plugins\"; DestName: \"$${TARGET}-LICENSE.txt\"; Flags: ignoreversion >> $$ISS)
    }

    for(qm, QMFILES) {
        iss.commands += $$mle(echo Source: \"$${_PRO_FILE_PWD_}\\$$qm\"; DestDir: \"{app}/translations\"; Flags: ignoreversion >> $$ISS)
    }

    iss.commands += $$mle(echo [Icons] >> $$ISS)
    iss.commands += $$mle(echo Name: \"{group}\\{cm:UninstallProgram","{$${LITERAL_HASH}MyAppName}}\"; Filename: \"{uninstallexe}\" >> $$ISS)

    iss.commands += $$mle(echo [Code] >> $$ISS)
    iss.commands += $$mle(echo procedure CurStepChanged(CurStep: TSetupStep); >> $$ISS)
    iss.commands += $$mle(echo var >> $$ISS)
    iss.commands += $$mle(echo ResultCode: Integer; >> $$ISS)
    iss.commands += $$mle(echo Uninstall: String; >> $$ISS)
    iss.commands += $$mle(echo UninstallQuery : String; >> $$ISS)
    iss.commands += $$mle(echo begin >> $$ISS)
    iss.commands += $$mle(echo   UninstallQuery := ExpandConstant(\'Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{$${LITERAL_HASH}emit SetupSetting(\"AppId\")}_is1\'); >> $$ISS)

    iss.commands += $$mle(echo   if (CurStep = ssInstall) then begin >> $$ISS)
    iss.commands += $$mle(echo     if RegQueryStringValue(HKLM, UninstallQuery, \'UninstallString\', Uninstall) >> $$ISS)
    iss.commands += $$mle(echo        or RegQueryStringValue(HKCU, UninstallQuery, \'UninstallString\', Uninstall) then begin >> $$ISS)
    iss.commands += $$mle(echo       if not Exec(RemoveQuotes(Uninstall), \'/VERYSILENT\', \'\', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode) then begin >> $$ISS)
    iss.commands += $$mle(echo         MsgBox(SysErrorMessage(ResultCode), mbCriticalError, MB_OK); >> $$ISS)
    iss.commands += $$mle(echo         Abort(); >> $$ISS)
    iss.commands += $$mle(echo       end; >> $$ISS)
    iss.commands += $$mle(echo     end; >> $$ISS)
    iss.commands += $$mle(echo   end; >> $$ISS)
    iss.commands += $$mle(echo end; >> $$ISS)

    QMAKE_EXTRA_TARGETS += iss
    QMAKE_EXTRA_TARGETS *= distbin

    distbin.depends += iss
    distbin.commands += $$mle(\"$$INNO\" /o. \"$$ISS\")
    distbin.commands += $$mle(del /F /Q \"$$ISS\")
} else {
    warning("Inno Setup is not found, will not create a setup file for $$TARGET in a custom dist target")
}
