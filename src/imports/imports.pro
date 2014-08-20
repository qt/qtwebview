CXX_MODULE = webview
TARGET  = declarative_webview
TARGETPATH = QtWebView
IMPORT_VERSION = 1.0

QT += qml quick webview webview-private
SOURCES += \
    $$PWD/webview.cpp

!android: qtHaveModule(webengine) {
    QT += webengine webengine-private
    DEFINES += QT_WEBVIEW_WEBENGINE_BACKEND
}


load(qml_plugin)

OTHER_FILES += qmldir
