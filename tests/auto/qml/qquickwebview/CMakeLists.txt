#####################################################################
## tst_qquickwebview Binary:
#####################################################################

qt_internal_add_test(tst_qquickwebview
    SOURCES
        util.h testwindow.h
        tst_qquickwebview.cpp
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::Network
        Qt::Quick
        Qt::Test
        Qt::WebViewPrivate
)

set(testdata_resource_files
    "html/basic_page.html"
    "html/basic_page2.html"
    "html/direct-image-compositing.html"
    "html/inputmethod.html"
    "html/scroll.html"
)

qt_internal_add_resource(tst_qquickwebview "testdata"
    PREFIX
        "/"
    BASE
        "html"
    FILES
        ${testdata_resource_files}
)
set(testdata1_resource_files
    "html/resources/simple_image.png"
)

qt_internal_add_resource(tst_qquickwebview "testdata1"
    PREFIX
        "/resources"
    BASE
        "html"
    FILES
        ${testdata1_resource_files}
)
