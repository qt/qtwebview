# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_commandline_option(webview2-sdk TYPE path CMAKE_VARIABLE WEBVIEW2_SDK_ROOT)
qt_commandline_option(webview-webengine-plugin-only TYPE boolean
    CMAKE_VARIABLE BUILD_ONLY_WEBENGINE_PLUGIN)

qt_commandline_subconfig(src/webview)
