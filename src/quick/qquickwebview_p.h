/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWebView module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QQUICKWEBVIEW_H
#define QQUICKWEBVIEW_H

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

#include <QtWebViewQuick/private/qtwebviewquickglobal_p.h>
#include <QtWebViewQuick/private/qquickviewcontroller_p.h>
#include <QtWebView/private/qwebviewinterface_p.h>
#include <QtWebView/private/qwebview_p.h>
#include <QtQml/qqmlregistration.h>
Q_MOC_INCLUDE(<QtWebViewQuick/private/qquickwebviewloadrequest_p.h>)
Q_MOC_INCLUDE(<QtWebView/private/qwebviewloadrequest_p.h>)

QT_BEGIN_NAMESPACE

class QQuickWebViewLoadRequest;
class QWebViewLoadRequestPrivate;

class Q_WEBVIEWQUICK_EXPORT QQuickWebView : public QQuickViewController, public QWebViewInterface
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
    Q_ENUMS(LoadStatus)
    QML_NAMED_ELEMENT(WebView)
    QML_ADDED_IN_VERSION(1, 0)
    QML_EXTRA_VERSION(2, 0)

public:
    enum LoadStatus { // Changes here needs to be done in QWebView as well
        LoadStartedStatus,
        LoadStoppedStatus,
        LoadSucceededStatus,
        LoadFailedStatus
    };
    QQuickWebView(QQuickItem *parent = nullptr);
    ~QQuickWebView();

    QString httpUserAgent() const override;
    void setHttpUserAgent(const QString &userAgent) override;
    QUrl url() const override;
    void setUrl(const QUrl &url) override;
    int loadProgress() const override;
    QString title() const override;
    bool canGoBack() const override;
    bool isLoading() const override;
    bool canGoForward() const override;
    QWebView &webView() const { return *m_webView; };

public Q_SLOTS:
    void goBack() override;
    void goForward() override;
    void reload() override;
    void stop() override;
    Q_REVISION(1, 1) void loadHtml(const QString &html, const QUrl &baseUrl = QUrl()) override;
    Q_REVISION(1, 1)
    void runJavaScript(const QString &script, const QJSValue &callback = QJSValue());

Q_SIGNALS:
    void titleChanged();
    void urlChanged();
    Q_REVISION(1, 1) void loadingChanged(QQuickWebViewLoadRequest *loadRequest);
    void loadProgressChanged();
    Q_REVISION(1, 14) void httpUserAgentChanged();

protected:
    void itemChange(ItemChange change, const ItemChangeData &value) override;
    void runJavaScriptPrivate(const QString &script, int callbackId) override;

private Q_SLOTS:
    void onRunJavaScriptResult(int id, const QVariant &variant);
    void onFocusRequest(bool focus);
    void onLoadingChanged(const QWebViewLoadRequestPrivate &loadRequest);

private:
    friend class QWebEngineWebViewPrivate;
    static QJSValue takeCallback(int id);

    QWebView *m_webView;
};

QT_END_NAMESPACE

#endif // QQUICKWEBVIEW_H
