TEMPLATE = app
TARGET = minibrowser

QT += qml quick webview

winrt: WINRT_MANIFEST.capabilities += internetClient

SOURCES += main.cpp

RESOURCES += qml.qrc

EXAMPLE_FILES += doc

ios:QMAKE_INFO_PLIST = ios/Info.plist

target.path = $$[QT_INSTALL_EXAMPLES]/webview/minibrowser
INSTALLS += target
