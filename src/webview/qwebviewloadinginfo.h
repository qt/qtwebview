// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBVIEWLOADINGINFO_H
#define QWEBVIEWLOADINGINFO_H

#include <QtWebView/qwebview.h>

#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qstringfwd.h>

QT_BEGIN_NAMESPACE

class QUrl;

class QWebViewLoadingInfoPrivate;
QT_DECLARE_QESDP_SPECIALIZATION_DTOR(QWebViewLoadingInfoPrivate)

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
    QWebViewLoadingInfo(QWebViewLoadingInfo &&other) = default;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(QWebViewLoadingInfo)
    Q_WEBVIEW_EXPORT ~QWebViewLoadingInfo();

    void swap(QWebViewLoadingInfo &other) noexcept
    {
        d.swap(other.d);
        std::swap(m_status, other.m_status);
    }

    Q_WEBVIEW_EXPORT QUrl url() const;
    LoadStatus status() const { return m_status; }
    Q_WEBVIEW_EXPORT QString errorString() const;

private:
    QExplicitlySharedDataPointer<QWebViewLoadingInfoPrivate> d;
    LoadStatus m_status;
};

Q_DECLARE_SHARED(QWebViewLoadingInfo)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QWebViewLoadingInfo)

#endif // QWEBVIEWLOADINGINFO_H
