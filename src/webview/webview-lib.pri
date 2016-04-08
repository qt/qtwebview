ANDROID_BUNDLED_JAR_DEPENDENCIES = \
    jar/QtAndroidWebView-bundled.jar
ANDROID_JAR_DEPENDENCIES = \
    jar/QtAndroidWebView.jar
ANDROID_PERMISSIONS = \
    android.permission.ACCESS_FINE_LOCATION

INCLUDEPATH += $$PWD

PUBLIC_HEADERS += \
    qwebview_global.h \
    qtwebviewfunctions.h

SOURCES += \
    qtwebviewfunctions.cpp

COMMON_HEADERS += \
    qwebview_p.h \
    qwebviewinterface_p.h \
    qquickwebview_p.h \
    qnativeviewcontroller_p.h \
    qwebview_p_p.h \
    qquickviewcontroller_p.h \
    qwebviewloadrequest_p.h \
    qquickwebviewloadrequest_p.h

COMMON_SOURCES += \
    qwebview.cpp \
    qquickwebview.cpp \
    qquickviewcontroller.cpp \
    qquickwebviewloadrequest.cpp \
    qwebviewloadrequest.cpp

android {
    QT_FOR_PRIVATE += core-private
    LIBS_PRIVATE += -ljnigraphics
    SOURCES += \
        $$COMMON_SOURCES \
        qwebview_android.cpp
    PRIVATE_HEADERS += \
        $$COMMON_HEADERS \
        qwebview_android_p.h

} else:ios {
    SOURCES += \
        $$COMMON_SOURCES
    OBJECTIVE_SOURCES += \
        qwebview_ios.mm
    PRIVATE_HEADERS += \
        $$COMMON_HEADERS \
        qwebview_ios_p.h

} else:osx_webview_experimental {
    DEFINES += QT_WEBVIEW_EXPERIMENTAL
    LIBS_PRIVATE += -framework Cocoa -framework WebKit
    SOURCES += \
       $$COMMON_SOURCES
    OBJECTIVE_SOURCES += \
        qwebview_osx.mm
    PRIVATE_HEADERS += \
        $$COMMON_HEADERS \
        qwebview_osx_p.h

} else: winrt {
    NO_PCH_SOURCES += qwebview_winrt.cpp
    SOURCES += $$COMMON_SOURCES
    PRIVATE_HEADERS += \
        $$COMMON_HEADERS \
        qwebview_winrt_p.h
} else:qtHaveModule(webengine) {
    QT_PRIVATE += webengine-private
    SOURCES += \
        $$COMMON_SOURCES \
        qwebview_webengine.cpp
    PRIVATE_HEADERS += \
        $$COMMON_HEADERS \
        qwebview_webengine_p.h
    DEFINES += QT_WEBVIEW_WEBENGINE_BACKEND
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
