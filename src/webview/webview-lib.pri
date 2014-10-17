ANDROID_BUNDLED_JAR_DEPENDENCIES = \
    jar/QtAndroidWebView-bundled.jar
ANDROID_JAR_DEPENDENCIES = \
    jar/QtAndroidWebView.jar

INCLUDEPATH += $$PWD

PUBLIC_HEADERS += \
    qwebview_global.h

PRIVATE_HEADERS += \
    qwebview_p.h

android {
    QT += core-private
    LIBS += -ljnigraphics
    SOURCES += \
        qwebview_android.cpp \
        qwebview.cpp \
        qwindowcontrolleritem_android.cpp
    PRIVATE_HEADERS += \
        qwebview_android_p.h \
        qwindowcontrolleritem_p.h

} else:ios {
    SOURCES += \
        qwebview.cpp
    OBJECTIVE_SOURCES += \
        qwebview_ios.mm \
        qwindowcontrolleritem_ios.mm
    PRIVATE_HEADERS += \
        qwebview_ios_p.h \
        qwindowcontrolleritem_p.h

} else {
    qtHaveModule(webengine) {
        QT += webengine webengine-private
        DEFINES += QT_WEBVIEW_WEBENGINE_BACKEND
    } else {
        SOURCES += \
            qwebview_default.cpp \
            qwebview.cpp \
            qwindowcontrolleritem.cpp
        PRIVATE_HEADERS += \
            qwebview_default_p.h \
            qwindowcontrolleritem_p.h
    }
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
