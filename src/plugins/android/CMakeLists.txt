# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#####################################################################
## QAndroidWebViewPlugin Plugin:
#####################################################################

qt_internal_add_plugin(QAndroidWebViewPlugin
    OUTPUT_NAME qtwebview_android
    PLUGIN_TYPE webview
    SOURCES
        qandroidwebview.cpp qandroidwebview_p.h
        qandroidwebviewplugin.cpp
    LIBRARIES
        jnigraphics
        Qt::CorePrivate
        Qt::Core
        Qt::Gui
        Qt::WebViewPrivate
)
