CXX_MODULE = webview
TARGET  = declarative_webview
TARGETPATH = QtWebView
IMPORT_VERSION = 1.0

QT += qml quick
SOURCES += \
    $$PWD/webview.cpp

android|mac {
    QT += webview-private
} else:qtHaveModule(webengine) {
    QT += webengine webengine-private
    DEFINES += QT_WEBVIEW_WEBENGINE_BACKEND
} else {
    error(No WebView backend found!)
}

load(qml_plugin)

OTHER_FILES += qmldir
