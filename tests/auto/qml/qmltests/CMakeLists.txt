# Copyright (C) 2026 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_webview_qmltests LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS Qml QuickTest)

qt_internal_add_test(tst_webview_qmltests
    QMLTEST
    SOURCES
        tst_qmltests.cpp
    LIBRARIES
        Qt::Qml
        Qt::WebView
        Qt::WebViewQuick
    TESTDATA
        data/tst_create_destroy.qml
)
