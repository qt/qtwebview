# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

if(QT_BUILD_STANDALONE_TESTS)
find_package(Qt6 ${PROJECT_VERSION} CONFIG OPTIONAL_COMPONENTS WebEngineCore WebEngineQuick)
endif()
if((LINUX OR WIN32) AND NOT TARGET Qt::WebEngineQuick) # desktop platform without webengine
    return()
endif()
qt_build_tests()
