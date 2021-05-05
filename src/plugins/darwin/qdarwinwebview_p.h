/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
**
**
**
******************************************************************************/

#ifndef QDARWINWEBVIEW_P_H
#define QDARWINWEBVIEW_P_H

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

#include <private/qabstractwebview_p.h>

#if defined(Q_OS_IOS) && defined(__OBJC__)
#include <UIKit/UIGestureRecognizer.h>

@interface QIOSNativeViewSelectedRecognizer : UIGestureRecognizer <UIGestureRecognizerDelegate>
{
@public
    QNativeViewController *m_item;
}
- (id)initWithQWindowControllerItem:(QNativeViewController *)item;
@end
#endif

Q_FORWARD_DECLARE_OBJC_CLASS(WKWebView);
Q_FORWARD_DECLARE_OBJC_CLASS(WKNavigation);

#ifdef Q_OS_IOS
Q_FORWARD_DECLARE_OBJC_CLASS(UIGestureRecognizer);
#endif

QT_BEGIN_NAMESPACE

class QDarwinWebViewPrivate : public QAbstractWebView
{
    Q_OBJECT
public:
    explicit QDarwinWebViewPrivate(QObject *p = nullptr);
    ~QDarwinWebViewPrivate() override;

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
    void updatePolish() override;

public Q_SLOTS:
    void goBack() override;
    void goForward() override;
    void reload() override;
    void stop() override;
    void loadHtml(const QString &html, const QUrl &baseUrl = QUrl()) override;

protected:
    void runJavaScriptPrivate(const QString& script,
                              int callbackId) override;

public:
    WKWebView *wkWebView;
    WKNavigation *wkNavigation;
#ifdef Q_OS_IOS
    UIGestureRecognizer *m_recognizer;
#endif
    QPointer<QObject> m_parentView;
};

QT_END_NAMESPACE

#endif // QDARWINWEBVIEW_P_H
