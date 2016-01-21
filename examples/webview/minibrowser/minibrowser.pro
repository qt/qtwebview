TEMPLATE = app
TARGET = minibrowser

QT += qml quick webview

winrt: WINRT_MANIFEST.capabilities += internetClient

SOURCES += main.cpp

RESOURCES += qml.qrc

EXAMPLE_FILES += doc

target.path = $$[QT_INSTALL_EXAMPLES]/webview/minibrowser
INSTALLS += target
