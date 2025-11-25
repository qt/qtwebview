// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBVIEWLOADREQUEST_H
#define QWEBVIEWLOADREQUEST_H

#include <QtWebView/qwebview.h>

#include <QtCore/qstring.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

class Q_WEBVIEW_EXPORT QWebViewLoadRequest
{
    Q_GADGET
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(LoadStatus status READ status CONSTANT)
    Q_PROPERTY(QString errorString READ errorString CONSTANT)
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    enum class LoadStatus {
        LoadStartedStatus,
        LoadStoppedStatus,
        LoadSucceededStatus,
        LoadFailedStatus
    };
    Q_ENUM(LoadStatus)

    QWebViewLoadRequest();
    QWebViewLoadRequest(const QUrl &url, LoadStatus status, const QString &errorString);
    ~QWebViewLoadRequest();

    QUrl url() const;
    LoadStatus status() const;
    QString errorString() const;

private:
    QUrl m_url;
    LoadStatus m_status;
    QString m_errorString;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QWebViewLoadRequest)

#endif // QWEBVIEWLOADREQUEST_H
