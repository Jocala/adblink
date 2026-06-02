cmake_minimum_required(VERSION 3.16)
project(adblink VERSION 7.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
    set(OPENSSL_ROOT_DIR "C:/openssl-static" CACHE PATH "Static OpenSSL path")
    set(OPENSSL_INCLUDE_DIR "C:/openssl-static/include" CACHE PATH "OpenSSL include dir")
    set(OPENSSL_SSL_LIBRARY "C:/openssl-static/lib/libssl.lib" CACHE FILEPATH "OpenSSL SSL library")
    set(OPENSSL_CRYPTO_LIBRARY "C:/openssl-static/lib/libcrypto.lib" CACHE FILEPATH "OpenSSL Crypto library")
    set(OPENSSL_USE_STATIC_LIBS ON CACHE BOOL "Use static OpenSSL libs")
endif()

if(UNIX AND NOT APPLE)
    set(OPENSSL_ROOT_DIR "/home/jeff/openssl-static" CACHE PATH "Static OpenSSL path")
    set(OPENSSL_USE_STATIC_LIBS ON CACHE BOOL "Use static OpenSSL libs")
endif()

if(APPLE)
    set(CMAKE_PREFIX_PATH "/Users/jeff/Qt/Qt.6.11.1-static" CACHE PATH "Qt 6.11.1 static installation")
    set(OPENSSL_ROOT_DIR "/Users/jeff/openssl/openssl-universal" CACHE PATH "Static OpenSSL path")
    set(OPENSSL_USE_STATIC_LIBS ON CACHE BOOL "Use static OpenSSL libs")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build universal binary")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Network Concurrent)

set(SOURCES
    main.cpp
    adbconnection.cpp
    adbdevice.cpp
    adboutput.cpp
    adbutils.cpp
    backupmanager.cpp
    cachemanager.cpp
    connectadb.cpp
    connectmanager.cpp
    consolemanager.cpp
    customlistwidget.cpp
    getadbdata.cpp
    getlocaladb.cpp
    getreturncode.cpp
    kodidatamanager.cpp
    logfile.cpp
    mainwindow.cpp
    adhocmanager.cpp
    appmanager.cpp
    apkuidmanager.cpp
    about.cpp
    aboutmanager.cpp
    helpdialog.cpp
    installmanager.cpp
    scpdialog.cpp
    screencapmanager.cpp
    uninstalldialog.cpp
    devicerecorddialog.cpp
    usbfiledialog.cpp
    adblogdialog.cpp
    datadialog.cpp
    restdialog.cpp
    backupdialog.cpp
    cachedialog.cpp
    keyboarddialog.cpp
    keypadmanager.cpp
    killservermanager.cpp
    listfiledialog.cpp
    logdialog.cpp
    editordialog.cpp
    searchdialog.cpp
    sendtextmanager.cpp
    tcpipdialog.cpp
    thumbnailmanager.cpp
    forcequitdialog.cpp
    adbprefdialog.cpp
    sideloadmanager.cpp
    sleepdialog.cpp
    adbstring.cpp
    stringutils.cpp
    oculusdialog.cpp
    oculusmanager.cpp
    kodiarchdialog.cpp
    kodidownloadcoordinator.cpp
    kodidownloader.cpp
    kodilogmanager.cpp
    kodisetupmanager.cpp
    preferencesmanager.cpp
    remotepushmanager.cpp
    rebootmanager.cpp
    datausagemanager.cpp
    databaseresetmanager.cpp
    deleterecordmanager.cpp
    datamovemanager.cpp
    deviceeditor.cpp
    deviceeditormanager.cpp
    devicetableloader.cpp
    disconnectmanager.cpp
    filemanager.cpp
    splashscreenmanager.cpp
    timermanager.cpp
    uninstallmanager.cpp
    versioncheckmanager.cpp
    wirelessadbmanager.cpp
    xmleditormanager.cpp
)

set(HEADERS
    mainwindow.h
    adhocmanager.h
    appmanager.h
    apkuidmanager.h
    adbconnection.h
    adbdevice.h
    about.h
    aboutmanager.h
    adboutput.h
    adbutils.h
    backupmanager.h
    cachemanager.h
    connectadb.h
    connectmanager.h
    consolemanager.h
    customlistwidget.h
    devicerecord.h
    getadbdata.h
    getlocaladb.h
    getreturncode.h
    helpdialog.h
    installmanager.h
    kodidatamanager.h
    logfile.h
    point.h
    scpdialog.h
    screencapmanager.h
    uninstalldialog.h
    devicerecorddialog.h
    usbfiledialog.h
    adblogdialog.h
    datadialog.h
    restdialog.h
    listfiledialog.h
    backupdialog.h
    cachedialog.h
    keyboarddialog.h
    keypadmanager.h
    killservermanager.h
    logdialog.h
    editordialog.h
    searchdialog.h
    sendtextmanager.h
    tcpipdialog.h
    thumbnailmanager.h
    forcequitdialog.h
    adbprefdialog.h
    sideloadmanager.h
    sleepdialog.h
    adbstring.h
    stringutils.h
    program.h
    version.h
    oculusdialog.h
    oculusmanager.h
    kodiarchdialog.h
    kodidownloadcoordinator.h
    kodidownloader.h
    kodilogmanager.h
    kodisetupmanager.h
    preferencesmanager.h
    remotepushmanager.h
    rebootmanager.h
    datausagemanager.h
    databaseresetmanager.h
    deleterecordmanager.h
    datamovemanager.h
    deviceeditor.h
    deviceeditormanager.h
    devicetableloader.h
    disconnectmanager.h
    filemanager.h
    splashscreenmanager.h
    timermanager.h
    uninstallmanager.h
    versioncheckmanager.h
    wirelessadbmanager.h
    xmleditormanager.h
)

set(ADBLINK_RESOURCES adblink.qrc)
if(WIN32)
    list(APPEND ADBLINK_RESOURCES adblink.rc)
endif()

set(ADBLINK_EXTRA_FILES)
if(APPLE)
    list(APPEND ADBLINK_EXTRA_FILES adblink.icns)
    set_source_files_properties(adblink.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
endif()

qt_add_executable(adblink
    ${SOURCES}
    ${HEADERS}
    ${ADBLINK_RESOURCES}
    ${ADBLINK_EXTRA_FILES}
)

target_include_directories(adblink PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(adblink PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    Qt6::Concurrent
)

if(MSVC)
    target_compile_options(adblink PRIVATE /wd4996)
endif()

if(UNIX AND NOT APPLE)
    target_link_options(adblink PRIVATE -no-pie)
endif()

if(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(CARBON_FRAMEWORK Carbon)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(SECURITY_FRAMEWORK Security)
    find_library(SYSTEMCONFIGURATION_FRAMEWORK SystemConfiguration)
    target_link_libraries(adblink PRIVATE
        ${COCOA_FRAMEWORK}
        ${CARBON_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${SECURITY_FRAMEWORK}
        ${SYSTEMCONFIGURATION_FRAMEWORK}
    )
endif()

set(ADBLINK_COMMON_PLUGINS
    Qt6::QSQLiteDriverPlugin
    Qt6::QSvgIconPlugin
    Qt6::QSvgPlugin
    Qt6::QGifPlugin
    Qt6::QICOPlugin
    Qt6::QJpegPlugin
)

if(WIN32)
    qt_import_plugins(adblink
        INCLUDE Qt6::QWindowsIntegrationPlugin
        INCLUDE Qt6::QModernWindowsStylePlugin
        INCLUDE ${ADBLINK_COMMON_PLUGINS}
        INCLUDE Qt6::QSchannelBackendPlugin
        INCLUDE Qt6::QTlsBackendOpenSSLPlugin
    )
elseif(APPLE)
    set_target_properties(adblink PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "jocala.com.adblink"
        MACOSX_BUNDLE_BUNDLE_NAME "adblink"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_ICON_FILE "adblink.icns"
    )
    qt_import_plugins(adblink
        INCLUDE Qt6::QCocoaIntegrationPlugin
        INCLUDE Qt6::QMacStylePlugin
        INCLUDE ${ADBLINK_COMMON_PLUGINS}
        INCLUDE Qt6::QICNSPlugin
        INCLUDE Qt6::QSecureTransportBackendPlugin
        INCLUDE Qt6::QTlsBackendOpenSSLPlugin
    )
    add_custom_command(TARGET adblink POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/adbfiles/adbfiles.macos"
            "$<TARGET_BUNDLE_CONTENT_DIR:adblink>/MacOS/adbfiles"
        COMMENT "Bundling adbfiles into app"
    )
elseif(UNIX)
    qt_import_plugins(adblink
        INCLUDE Qt6::QXcbIntegrationPlugin
        INCLUDE ${ADBLINK_COMMON_PLUGINS}
        INCLUDE Qt6::QTlsBackendOpenSSLPlugin
    )
endif()

enable_testing()
add_subdirectory(tests)
