
#####################################################################
## declarative_webview Plugin:
#####################################################################

if(TARGET Qt::WebEngineQuick)
    set(qmlDependency DEPENDENCIES "QtWebEngine/2.0")
endif()

qt_internal_add_qml_module(declarative_webview
    URI "QtWebView"
    VERSION "${PROJECT_VERSION}"
    CLASS_NAME QWebViewModule
    PLUGIN_TARGET declarative_webview
    NO_GENERATE_PLUGIN_SOURCE
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_QMLTYPES
    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
    ${qmlDependency}
    SOURCES
        webview.cpp
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
        Qt::WebViewPrivate
)


