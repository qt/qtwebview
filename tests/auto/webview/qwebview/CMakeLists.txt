#####################################################################
## tst_qwebview Test:
#####################################################################

qt_internal_add_test(tst_qwebview
    SOURCES
        tst_qwebview.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::WebViewPrivate
)

qt_internal_extend_target(tst_qwebview CONDITION NOT TARGET Qt::Quick
    DEFINES
        QT_NO_QQUICKWEBVIEW_TESTS
)

qt_internal_extend_target(tst_qwebview CONDITION TARGET Qt::WebEngineQuick
    DEFINES
        QT_WEBVIEW_WEBENGINE_BACKEND
    PUBLIC_LIBRARIES
        Qt::WebEngineQuickPrivate
)
