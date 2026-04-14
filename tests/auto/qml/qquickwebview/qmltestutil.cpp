// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "qmltestutil.h"

#include <testutil.h>
#include <QtWebView/private/qwebviewfactory_p.h>

bool QQuickWebViewTestUtilsSingleton::loadedPluginHasKey(const QString &key) const
{
    return QWebViewFactory::loadedPluginHasKey(key);
}

QUrl QQuickWebViewTestUtilsSingleton::makeTestFileUrl(const QString &testFile) const
{
    return ::makeTestFileUrl(testFile);
}
