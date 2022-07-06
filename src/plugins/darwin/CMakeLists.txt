# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#####################################################################
## QDarwinWebViewPlugin Plugin:
#####################################################################

find_library(FWWebKit WebKit)
find_library(FWAppKit AppKit)
find_library(FWUIKit UIKit)
find_library(FWFoundation FoundationKit)

qt_internal_add_plugin(QDarwinWebViewPlugin
    OUTPUT_NAME qtwebview_darwin
    PLUGIN_TYPE webview
    SOURCES
        qdarwinwebview.mm qdarwinwebview_p.h
        qdarwinwebviewplugin.cpp
    LIBRARIES
        ${FWFoundation}
        ${FWWebKit}
        Qt::Core
        Qt::Gui
        Qt::WebViewPrivate
)

qt_internal_extend_target(QDarwinWebViewPlugin CONDITION MACOS
    LIBRARIES
        ${FWAppKit}
)

qt_internal_extend_target(QDarwinWebViewPlugin CONDITION IOS
    LIBRARIES
        ${FWUIKit}
)
