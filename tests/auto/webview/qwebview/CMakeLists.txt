# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qwebview LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

#####################################################################
## tst_qwebview Test:
#####################################################################

include(../../util/util.cmake)

qt_internal_add_test(tst_qwebview
    SOURCES
        tst_qwebview.cpp
    LIBRARIES
        Qt::Gui
        Qt::WebViewPrivate
        WebViewTest::Util
)

set(resource_files
    "html/basic_page.html"
    "html/basic_page2.html"
)

qt_internal_add_resource(tst_qwebview "testdata"
    PREFIX
        "/"
    BASE
        "html"
    FILES
        ${resource_files}
)
