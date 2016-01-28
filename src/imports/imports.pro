CXX_MODULE = webview
TARGET  = declarative_webview
TARGETPATH = QtWebView
IMPORT_VERSION = 1.0

QT += qml quick webview-private
SOURCES += \
    $$PWD/webview.cpp

load(qml_plugin)

OTHER_FILES += qmldir
