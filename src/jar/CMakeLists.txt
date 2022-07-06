# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


qt_internal_add_jar(QtAndroidWebView
    INCLUDE_JARS ${QT_ANDROID_JAR}
    SOURCES src/org/qtproject/qt/android/view/QtAndroidWebViewController.java
    OUTPUT_DIR "${QT_BUILD_DIR}/jar"
)

install_jar(QtAndroidWebView
    DESTINATION jar
    COMPONENT Devel
)

