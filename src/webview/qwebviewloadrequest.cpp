// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include <QtWebView/qwebviewloadrequest.h>

QT_BEGIN_NAMESPACE

QWebViewLoadRequest::QWebViewLoadRequest() { }

QWebViewLoadRequest::QWebViewLoadRequest(const QUrl &url, LoadStatus status,
                                         const QString &errorString)
    : m_url(url), m_status(status), m_errorString(errorString)
{
}

QWebViewLoadRequest::~QWebViewLoadRequest() { }

// FIXME add c++ docs

QUrl QWebViewLoadRequest::url() const
{
    return m_url;
}

QWebViewLoadRequest::LoadStatus QWebViewLoadRequest::status() const
{
    return m_status;
}

QString QWebViewLoadRequest::errorString() const
{
    return m_errorString;
}

QT_END_NAMESPACE
