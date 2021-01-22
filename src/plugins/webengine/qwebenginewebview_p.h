/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebView module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QWEBENGINEWEBVIEW_P_H
#define QWEBENGINEWEBVIEW_P_H

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

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtGui/qwindow.h>

#include <QtQml/qqmlcomponent.h>

#include <private/qabstractwebview_p.h>
#include <QtWebEngine/QQuickWebEngineProfile>


QT_BEGIN_NAMESPACE

class QQuickWebEngineView;
class QQuickWebEngineLoadRequest;

class QWebEngineWebViewPrivate : public QAbstractWebView
{
    Q_OBJECT
public:
    explicit QWebEngineWebViewPrivate(QObject *p = 0);
    ~QWebEngineWebViewPrivate() Q_DECL_OVERRIDE;

    QString httpUserAgent() const Q_DECL_OVERRIDE;
    void setHttpUserAgent(const QString &userAgent) Q_DECL_OVERRIDE;
    QUrl url() const Q_DECL_OVERRIDE;
    void setUrl(const QUrl &url) Q_DECL_OVERRIDE;
    bool canGoBack() const Q_DECL_OVERRIDE;
    bool canGoForward() const Q_DECL_OVERRIDE;
    QString title() const Q_DECL_OVERRIDE;
    int loadProgress() const Q_DECL_OVERRIDE;
    bool isLoading() const Q_DECL_OVERRIDE;

    void setParentView(QObject *parentView) Q_DECL_OVERRIDE;
    QObject *parentView() const Q_DECL_OVERRIDE;
    void setGeometry(const QRect &geometry) Q_DECL_OVERRIDE;
    void setVisibility(QWindow::Visibility visibility) Q_DECL_OVERRIDE;
    void setVisible(bool visible) Q_DECL_OVERRIDE;
    void setFocus(bool focus) Q_DECL_OVERRIDE;

public Q_SLOTS:
    void goBack() Q_DECL_OVERRIDE;
    void goForward() Q_DECL_OVERRIDE;
    void reload() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;
    void loadHtml(const QString &html, const QUrl &baseUrl = QUrl()) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void q_urlChanged();
    void q_loadProgressChanged();
    void q_titleChanged();
    void q_loadingChanged(QQuickWebEngineLoadRequest *loadRequest);
    void q_profileChanged();
    void q_httpUserAgentChanged();

protected:
    void runJavaScriptPrivate(const QString& script,
                              int callbackId) Q_DECL_OVERRIDE;

private:
    QQuickWebEngineProfile *m_profile;
    QString m_httpUserAgent;
    struct QQuickWebEngineViewPtr
    {
        inline QQuickWebEngineView *operator->() const
        {
            if (!m_webEngineView)
                init();
            return m_webEngineView.data();
        }
        void init() const;

        QWebEngineWebViewPrivate *m_parent;
        mutable QScopedPointer<QQuickWebEngineView> m_webEngineView;
    } m_webEngineView;
};

QT_END_NAMESPACE

#endif // QWEBENGINEWEBVIEW_P_H
