CONFIG += testcase parallel_test
TARGET = tst_qwebview
osx:CONFIG -= app_bundle

!android: qtHaveModule(webengine) {
    QT += webengine webengine-private
    DEFINES += QT_WEBVIEW_WEBENGINE_BACKEND
}

QT += webview webview-private testlib
SOURCES += \
    tst_qwebview.cpp
