// Copyright (C) 2026 The Qt Company Ltd.
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

class QWebViewPrivate;
class QWebViewLoadingInfo;

namespace QWebViewFactory {
enum class Hint;
}

class Q_WEBVIEW_EXPORT QWebView : public QWindow
{
    Q_OBJECT
    Q_PROPERTY(QString httpUserAgent READ httpUserAgent WRITE setHttpUserAgent NOTIFY
                       httpUserAgentChanged FINAL)
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged FINAL)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged FINAL)
    Q_PROPERTY(int loadProgress READ loadProgress NOTIFY loadProgressChanged FINAL)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged FINAL)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY loadingChanged FINAL)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY loadingChanged FINAL)
    Q_PROPERTY(QWebViewSettings *settings READ settings CONSTANT FINAL)

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

    QWebViewSettings *settings() const;

    void runJavaScript(const QString &script,
                       const std::function<void(const QVariant &)> &resultCallback = {});
public Q_SLOTS:
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
    void loadingChanged(const QWebViewLoadingInfo &loadingInfo);
    void loadProgressChanged(int loadProgress);
    void httpUserAgentChanged(const QString &userAgent);
    void cookieAdded(const QString &domain, const QString &name);
    void cookieRemoved(const QString &domain, const QString &name);

private:
    QWebView(QWebViewFactory::Hint);
    Q_DISABLE_COPY(QWebView)

    friend class QQuickWebView;
    friend class ::tst_QWebView;

    std::unique_ptr<QWebViewPrivate> d;
};

QT_END_NAMESPACE

#endif // QWEBVIEW_H
