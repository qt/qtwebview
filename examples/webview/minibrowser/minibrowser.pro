TEMPLATE = app

QT += qml quick webview

winrt: WINRT_MANIFEST.capabilities += internetClient

SOURCES += main.cpp

RESOURCES += qml.qrc

EXAMPLE_FILES += doc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
