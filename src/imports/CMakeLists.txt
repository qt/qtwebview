
#####################################################################
## declarative_webview Plugin:
#####################################################################

if(TARGET Qt::WebEngineQuick)
    set(qmlDependency DEPENDENCIES "QtWebEngine/2.0")
endif()

qt_internal_add_qml_module(declarative_webview
    URI "QtWebView"
    VERSION ${PROJECT_VERSION}
    CLASSNAME QWebViewModule
    SKIP_TYPE_REGISTRATION
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


