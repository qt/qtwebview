
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
    LIBRARIES
        Qt::QuickPrivate
        Qt::WebViewPrivate
)


