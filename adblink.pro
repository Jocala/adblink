#-------------------------------------------------
#
# Project created by QtCreator 2014-05-21T01:43:07
#
#-------------------------------------------------

QT       += core gui sql network concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = adblink
TEMPLATE = app

# ==================== macOS SDK Configuration ====================
macx {
    QMAKE_MAC_SDK = macosx15.5 
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 15.5
    CONFIG += sdk_no_version_check        
    QMAKE_LIBS_OPENGL = -framework OpenGL
    ICON = adblink.icns
}



# ==================== Sources / Headers / Forms ====================
SOURCES += main.cpp \
    adboutput.cpp \
    adbutils.cpp \
    connectadb.cpp \
    customlistwidget.cpp \
    getadbdata.cpp \
    getdevices.cpp \
    getlocaladb.cpp \
    getreturncode.cpp \
    logfile.cpp \
    mainwindow.cpp \
    about.cpp \
    helpdialog.cpp \
    returncode.cpp \
    scpdialog.cpp \
    uninstalldialog.cpp \
    preferencesdialog.cpp \
    usbfiledialog.cpp \
    adblogdialog.cpp \
    datadialog.cpp \
    restdialog.cpp \
    backupdialog.cpp \
    cachedialog.cpp \
    keyboarddialog.cpp \
    listfiledialog.cpp \
    logdialog.cpp \
    editordialog.cpp \
    searchdialog.cpp \
    tcpipdialog.cpp \
    forcequitdialog.cpp \
    adbprefdialog.cpp \
    sleepdialog.cpp \
    adbstring.cpp \
    oculusdialog.cpp

HEADERS += mainwindow.h \
    about.h \
    adboutput.h \
    adbutils.h \
    connectadb.h \
    customlistwidget.h \
    devicerecord.h \
    getadbdata.h \
    getdevices.h \
    getlocaladb.h \
    getreturncode.h \
    helpdialog.h \
    logfile.h \
    point.h \
    returncode.h \
    scpdialog.h \
    uninstalldialog.h \
    preferencesdialog.h \
    usbfiledialog.h \
    adblogdialog.h \
    datadialog.h \
    restdialog.h \
    listfiledialog.h \
    backupdialog.h \
    cachedialog.h \
    keyboarddialog.h \
    logdialog.h \
    editordialog.h \
    searchdialog.h \
    tcpipdialog.h \
    forcequitdialog.h \
    adbprefdialog.h \
    sleepdialog.h \
    adbstring.h \
    program.h \
    version.h \
    oculusdialog.h

FORMS += \
    helpdialog.ui \
    restdialog.ui \
    backupdialog.ui \
    listfiledialog.ui \
    keyboarddialog.ui \
    editordialog.ui \
    searchdialog.ui \
    tcpipdialog.ui \
    sleepdialog.ui

RESOURCES = adbLink.qrc

# ==================== Build flags ====================
QMAKE_CXXFLAGS += -Wno-deprecated -Wno-deprecated-declarations

linux {
    QMAKE_LFLAGS += -no-pie
}

# RC_FILE is Windows-only
win32 {
    RC_FILE = adblink.rc
}

