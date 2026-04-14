# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## tst_qquickwebview Binary:
#####################################################################

find_package(Qt6 ${PROJECT_VERSION} CONFIG
    REQUIRED COMPONENTS Qml QuickTest)

include(../../util/util.cmake)

qt_internal_add_test(tst_qquickwebview
    QMLTEST
    SOURCES
        qmltestutil.cpp
        qmltestutil.h
        tst_qquickwebview.cpp
    LIBRARIES
        Qt::WebView
        Qt::WebViewQuick
        Qt::WebViewPrivate
        Qt::Qml
        WebViewTest::Util
)

qt_add_qml_module(tst_qquickwebview
    URI QuickWebViewTest
    VERSION 1.0
    QML_FILES
        tst_qquickwebview.qml
        WebViewTestCase.qml
)

set(testdata_resource_files
    "html/basic_page.html"
    "html/basic_page2.html"
    "html/javascript.html"
)

qt_internal_add_resource(tst_qquickwebview "testdata"
    PREFIX
        "/"
    BASE
        "html"
    FILES
        ${testdata_resource_files}
)
