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
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::WebViewPrivate
)
