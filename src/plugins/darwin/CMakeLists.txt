#####################################################################
## QDarwinWebViewPlugin Plugin:
#####################################################################

find_library(FWWebKit WebKit)
find_library(FWAppKit AppKit)
find_library(FWUIKit UIKit)
find_library(FWFoundation FoundationKit)

qt_internal_add_plugin(QDarwinWebViewPlugin
    OUTPUT_NAME qtwebview_darwin
    TYPE webview
    SOURCES
        qdarwinwebview.mm qdarwinwebview_p.h
        qdarwinwebviewplugin.cpp
    LIBRARIES
        ${FWFoundation}
        ${FWWebKit}
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::WebViewPrivate
)

qt_internal_extend_target(QDarwinWebViewPlugin CONDITION MACOS
    LIBRARIES
        ${FWAppKit}
)

qt_internal_extend_target(QDarwinWebViewPlugin CONDITION IOS
    LIBRARIES
        ${FWUIKit}
)
