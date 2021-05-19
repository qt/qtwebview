#####################################################################
## QAndroidWebViewPlugin Plugin:
#####################################################################

qt_internal_add_plugin(QAndroidWebViewPlugin
    OUTPUT_NAME qtwebview_android
    TYPE webview
    SOURCES
        qandroidwebview.cpp qandroidwebview_p.h
        qandroidwebviewplugin.cpp
    LIBRARIES
        jnigraphics
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::WebViewPrivate
)
