# Copyright (C) 2026 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## tst_qwebview Test:
#####################################################################

include(../../util/util.cmake)

qt_internal_add_test(tst_webviewqrc
    SOURCES
        tst_webviewqrc.cpp
    LIBRARIES
        Qt::Gui
        Qt::WebViewPrivate
        WebViewTest::Util
)

set(resource_files
    "html/iframe.html"
    "html/link.html"
    "html/success.html"
    "html/prefix/success.html"
)

qt_internal_add_resource(tst_webviewqrc "testdata"
    PREFIX
        "/"
    BASE
        "html"
    FILES
        ${resource_files}
)
