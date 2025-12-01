# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## tst_qwebview Test:
#####################################################################

qt_internal_add_test(tst_qwebview
    SOURCES
        tst_qwebview.cpp
    LIBRARIES
        Qt::Gui
        Qt::WebViewPrivate
)

