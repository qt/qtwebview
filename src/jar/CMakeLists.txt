
qt_internal_add_jar(QtAndroidWebView
    INCLUDE_JARS ${QT_ANDROID_JAR}
    SOURCES src/org/qtproject/qt/android/view/QtAndroidWebViewController.java
    OUTPUT_DIR "${QT_BUILD_DIR}/jar"
)

install_jar(QtAndroidWebView
    DESTINATION jar
    COMPONENT Devel
)

