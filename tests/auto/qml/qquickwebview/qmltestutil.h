// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QMLTESTUTIL_H
#define QMLTESTUTIL_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qtemporaryfile.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class QQuickWebViewTestUtilsSingleton : public QObject {
    Q_OBJECT
    QML_SINGLETON
    QML_NAMED_ELEMENT(WebViewTestUtils)

public:
    Q_INVOKABLE bool loadedPluginHasKey(const QString &key) const;
    Q_INVOKABLE QUrl makeTestFileUrl(const QString &testFile) const;
};

QT_END_NAMESPACE

#endif /* QMLTESTUTIL_H */
