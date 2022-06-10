// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWEBVIEW_P_H
#define QWEBVIEW_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qabstractwebview_p.h"
#include "qwebviewinterface_p.h"
#include "qnativeviewcontroller_p.h"
#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>
#include <QtGui/qimage.h>

QT_BEGIN_NAMESPACE

class QWebViewLoadRequestPrivate;

class Q_WEBVIEW_EXPORT QWebView
        : public QObject
        , public QWebViewInterface
        , public QNativeViewController
{
    Q_OBJECT
public:
    enum LoadStatus { // Changes here needs to be done in QQuickWebView as well
        LoadStartedStatus,
        LoadStoppedStatus,
        LoadSucceededStatus,
        LoadFailedStatus
    };

    explicit QWebView(QObject *p = nullptr);
    ~QWebView() override;

    QString httpUserAgent() const override;
    void setHttpUserAgent(const QString &httpUserAgent) override;
    QUrl url() const override;
    void setUrl(const QUrl &url) override;
    bool canGoBack() const override;
    bool canGoForward() const override;
    QString title() const override;
    int loadProgress() const override;
    bool isLoading() const override;

    void setParentView(QObject *view) override;
    QObject *parentView() const override;
    void setGeometry(const QRect &geometry) override;
    void setVisibility(QWindow::Visibility visibility) override;
    void setVisible(bool visible) override;
    void setFocus(bool focus) override;

public Q_SLOTS:
    void goBack() override;
    void goForward() override;
    void reload() override;
    void stop() override;
    void loadHtml(const QString &html, const QUrl &baseUrl = QUrl()) override;
    void setCookie(const QString &domain, const QString &name,
                          const QString &value) override;
    void deleteCookie(const QString &domain, const QString &name) override;
    void deleteAllCookies() override;

Q_SIGNALS:
    void titleChanged();
    void urlChanged();
    void loadingChanged(const QWebViewLoadRequestPrivate &loadRequest);
    void loadProgressChanged();
    void javaScriptResult(int id, const QVariant &result);
    void requestFocus(bool focus);
    void httpUserAgentChanged();
    void cookieAdded(const QString &domain, const QString &name);
    void cookieRemoved(const QString &domain, const QString &name);

protected:
    void init() override;
    void runJavaScriptPrivate(const QString &script,
                              int callbackId) override;

private Q_SLOTS:
    void onTitleChanged(const QString &title);
    void onUrlChanged(const QUrl &url);
    void onLoadProgressChanged(int progress);
    void onLoadingChanged(const QWebViewLoadRequestPrivate &loadRequest);
    void onHttpUserAgentChanged(const QString &httpUserAgent);

private:
    friend class QQuickViewController;
    friend class QQuickWebView;

    QAbstractWebView *d;

    // provisional data
    int m_progress;
    QString m_title;
    QUrl m_url;
    mutable QString m_httpUserAgent;
};

QT_END_NAMESPACE

#endif // QWEBVIEW_P_H
