// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBVIEWLOADINGINFO_H
#define QWEBVIEWLOADINGINFO_H

#include <QtWebView/qwebview.h>

#include <QtCore/qstring.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

class QWebViewLoadingInfo
{
    Q_GADGET_EXPORT(Q_WEBVIEW_EXPORT)
    Q_PROPERTY(QUrl url READ url CONSTANT)
    Q_PROPERTY(LoadStatus status READ status CONSTANT)
    Q_PROPERTY(QString errorString READ errorString CONSTANT)
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

public:
    enum class LoadStatus {
        Started,
        Stopped,
        Succeeded,
        Failed
    };
    Q_ENUM(LoadStatus)

    Q_WEBVIEW_EXPORT QWebViewLoadingInfo();
    Q_WEBVIEW_EXPORT Q_IMPLICIT QWebViewLoadingInfo(const QUrl &url, LoadStatus status, const QString &errorString);
    Q_WEBVIEW_EXPORT QWebViewLoadingInfo(const QWebViewLoadingInfo &other);
    Q_WEBVIEW_EXPORT QWebViewLoadingInfo &operator=(const QWebViewLoadingInfo &other);
    QWebViewLoadingInfo(QWebViewLoadingInfo &&other) noexcept
        : m_url{std::move(other.m_url)},
          m_status{other.m_status},
          m_errorString{std::move(other.m_errorString)},
          m_reserved{std::exchange(other.m_reserved, nullptr)}
    {}
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QWebViewLoadingInfo)
    Q_WEBVIEW_EXPORT ~QWebViewLoadingInfo();

    void swap(QWebViewLoadingInfo &other) noexcept
    {
        m_url.swap(other.m_url);
        std::swap(m_status, other.m_status);
        m_errorString.swap(other.m_errorString);
        qt_ptr_swap(m_reserved, other.m_reserved);
    }

    Q_WEBVIEW_EXPORT QUrl url() const;
    LoadStatus status() const { return m_status; }
    Q_WEBVIEW_EXPORT QString errorString() const;

private:
    QUrl m_url;
    LoadStatus m_status;
    QString m_errorString;
    Q_DECL_UNUSED_MEMBER void *m_reserved = nullptr;
};

Q_DECLARE_SHARED(QWebViewLoadingInfo)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QWebViewLoadingInfo)

#endif // QWEBVIEWLOADINGINFO_H
