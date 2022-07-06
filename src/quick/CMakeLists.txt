# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


if(TARGET Qt::WebEngineQuick)
    set(qmlDependency DEPENDENCIES "QtWebEngine/2.0")
endif()

qt_internal_add_qml_module(WebViewQuick
    URI "QtWebView"
    VERSION "${PROJECT_VERSION}"
    CLASS_NAME QWebViewQuickPlugin
    PLUGIN_TARGET qtwebviewquickplugin
    NO_PLUGIN_OPTIONAL
    ${qmlDependency}
    SOURCES
        qquickviewcontroller.cpp qquickviewcontroller_p.h
        qquickwebview.cpp qquickwebview_p.h
        qquickwebviewloadrequest.cpp qquickwebviewloadrequest_p.h
    PUBLIC_LIBRARIES # TODO: QTBUG-86533 workaround, PUBLIC_LIBRARIES should be moved to LIBRARIES.
        Qt::Quick
        Qt::QmlModels
    LIBRARIES
        Qt::QuickPrivate
        Qt::WebViewPrivate
)

 # TODO: QTBUG-86533 workaround, PUBLIC_LIBRARIES should be moved to LIBRARIES.
qt_internal_extend_target(WebViewQuick CONDITION TARGET Qt::OpenGL
    PUBLIC_LIBRARIES
        Qt::OpenGL
)
