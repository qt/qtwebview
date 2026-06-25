# Copyright (C) 2026 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## QOhosWebView Plugin:
#####################################################################

qt_internal_add_plugin(QOhosWebViewPlugin
    OUTPUT_NAME qtwebview_ohos
    PLUGIN_TYPE webview
    EXCEPTIONS
    SOURCES
        qohoswebcomponentlistener.cpp qohoswebcomponentlistener.h
        qohoswebresourceerror.cpp qohoswebresourceerror.h
        qohoswebview.cpp qohoswebview_p.h
        qohoswebviewcontroller.cpp qohoswebviewcontroller.h
        qohoswebviewplugin.cpp
    LIBRARIES
        NodeAddonApi::NodeAddonApi
        ace_ndk.z
        ace_napi.z
        uv
        hilog_ndk.z
        ohweb
        Qt::Core
        Qt::Gui
        Qt::WebViewPrivate
)
