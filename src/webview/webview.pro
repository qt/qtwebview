include($$PWD/webview-lib.pri)

load(qt_build_config)

TARGET = QtWebView
CONFIG += internal_module

QT += core-private gui-private qml-private quick-private quick qml

QMAKE_DOCS = \
             $$PWD/doc/qtwebview.qdocconf

load(qt_module)
