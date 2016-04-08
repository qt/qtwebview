TARGET = QtWebView

QT =
QT_FOR_PRIVATE = quick-private

include($$PWD/webview-lib.pri)

QMAKE_DOCS = \
             $$PWD/doc/qtwebview.qdocconf

load(qt_module)
