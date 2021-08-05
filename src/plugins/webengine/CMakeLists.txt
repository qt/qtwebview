#####################################################################
## QWebEngineWebViewPlugin Plugin:
#####################################################################

qt_internal_add_plugin(QWebEngineWebViewPlugin
    OUTPUT_NAME qtwebview_webengine
    PLUGIN_TYPE webview
    SOURCES
        qwebenginewebview.cpp qwebenginewebview_p.h
        qwebenginewebviewplugin.cpp
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::WebEngineCorePrivate
        Qt::WebEngineQuickPrivate
        Qt::WebViewPrivate
        Qt::WebViewQuickPrivate
)
