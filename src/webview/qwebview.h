// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:significant reason:default

#ifndef QWEBVIEW_H
#define QWEBVIEW_H

#include <QtWebView/qwebview_global.h>
#include <QtWebView/qwebviewsettings.h>
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>
#include <QtGui/qwindow.h>

#include <memory>

class tst_QWebView;

QT_BEGIN_NAMESPACE

class QAbstractWebView;
class QWebViewLoadRequest;

class Q_WEBVIEW_EXPORT QWebView : public QWindow
{
    Q_OBJECT
    Q_PROPERTY(QString httpUserAgent READ httpUserAgent WRITE setHttpUserAgent NOTIFY
                       httpUserAgentChanged FINAL REVISION(1, 14))
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged FINAL)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged FINAL REVISION(1, 1))
    Q_PROPERTY(int loadProgress READ loadProgress NOTIFY loadProgressChanged FINAL)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged FINAL)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY loadingChanged FINAL)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY loadingChanged FINAL)
    Q_PROPERTY(QWebViewSettings *settings READ settings CONSTANT FINAL REVISION(6, 5))

public:
    explicit QWebView(QScreen *screen = nullptr);
    explicit QWebView(QWindow *parent);
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

    QWebViewSettings *settings();

    Q_INVOKABLE void goBack();
    Q_INVOKABLE void goForward();
    Q_INVOKABLE void reload();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void loadHtml(const QString &html, const QUrl &baseUrl = QUrl());
    Q_INVOKABLE void setCookie(const QString &domain, const QString &name, const QString &value);
    Q_INVOKABLE void deleteCookie(const QString &domain, const QString &name);
    Q_INVOKABLE void deleteAllCookies();

    // FIXME runjavascript

Q_SIGNALS:
    void titleChanged(QString title);
    void urlChanged(QUrl url);
    void loadingChanged(const QWebViewLoadRequest &loadRequest);
    void loadProgressChanged(int loadProgress);
    void javaScriptResult(int id, const QVariant &result);
    void httpUserAgentChanged(QString userAgent);
    void cookieAdded(const QString &domain, const QString &name);
    void cookieRemoved(const QString &domain, const QString &name);

protected:
    void runJavaScriptPrivate(const QString &script, int callbackId);

private:
    Q_DISABLE_COPY(QWebView)

    friend class QQuickWebView;
    friend class ::tst_QWebView;

    std::unique_ptr<QAbstractWebView> d;
};

QT_END_NAMESPACE

#endif // QWEBVIEW_H
