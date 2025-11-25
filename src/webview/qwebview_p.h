// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

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
#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>
#include <QtGui/qimage.h>

#include <QtCore/qpointer.h>

class tst_QWebView;

QT_BEGIN_NAMESPACE

class QWebViewLoadRequestPrivate;
class QWindow;
class QWebViewSettings;

class Q_WEBVIEW_EXPORT QWebView : public QWindow
{
    Q_OBJECT
public:
    enum LoadStatus { // Changes here needs to be done in QQuickWebView as well
        LoadStartedStatus,
        LoadStoppedStatus,
        LoadSucceededStatus,
        LoadFailedStatus
    };

    explicit QWebView(QWindow *parent = nullptr);
    ~QWebView() override;

    QString httpUserAgent() const;
    void setHttpUserAgent(const QString &httpUserAgent);
    QUrl url() const;
    void setUrl(const QUrl &url);
    bool canGoBack() const;
    bool canGoForward() const;
    QString title() const;
    int loadProgress() const;
    bool isLoading() const;

    QWebViewSettings *settings() const;

    void goBack();
    void goForward();
    void reload();
    void stop();
    void loadHtml(const QString &html, const QUrl &baseUrl = QUrl());
    void setCookie(const QString &domain, const QString &name, const QString &value);
    void deleteCookie(const QString &domain, const QString &name);
    void deleteAllCookies();

Q_SIGNALS:
    void titleChanged(const QString &title);
    void urlChanged(const QUrl &url);
    void loadingChanged(const QWebViewLoadRequestPrivate &loadRequest);
    void loadProgressChanged(int progress);
    void javaScriptResult(int id, const QVariant &result);
    void httpUserAgentChanged(const QString &agent);
    void cookieAdded(const QString &domain, const QString &name);
    void cookieRemoved(const QString &domain, const QString &name);

protected:
    void runJavaScriptPrivate(const QString &script, int callbackId);

private:
    friend class QQuickWebView;
    friend class ::tst_QWebView;

    QAbstractWebView *d = nullptr;
    QWebViewSettings *m_settings = nullptr;
};

QT_END_NAMESPACE

#endif // QWEBVIEW_P_H
