// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#include <QtWebView/qwebviewloadinginfo.h>

QT_BEGIN_NAMESPACE

QWebViewLoadingInfo::QWebViewLoadingInfo()
    : m_status{LoadStatus::Started}
{}

QWebViewLoadingInfo::QWebViewLoadingInfo(const QUrl &url, LoadStatus status,
                                         const QString &errorString)
    : m_url(url), m_status(status), m_errorString(errorString)
{
}

QWebViewLoadingInfo::~QWebViewLoadingInfo() { }

// FIXME add c++ docs

QUrl QWebViewLoadingInfo::url() const
{
    return m_url;
}

QWebViewLoadingInfo::LoadStatus QWebViewLoadingInfo::status() const
{
    return m_status;
}

QString QWebViewLoadingInfo::errorString() const
{
    return m_errorString;
}

QT_END_NAMESPACE

#include "moc_qwebviewloadinginfo.cpp"
