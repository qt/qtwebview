/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWebView module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwebview_p.h"
#include "qwebview_ios_p.h"

#include <QtQuick/qquickitem.h>
#include <QtCore/qmap.h>

#include <CoreFoundation/CoreFoundation.h>
#include <UIKit/UIKit.h>

QT_BEGIN_NAMESPACE

class QWebViewPrivate;

@interface QtWebViewDelegate : NSObject<UIWebViewDelegate> {
    QIOSWebViewPrivate *qIosWebViewPrivate;
}
- (QtWebViewDelegate *)initWithQWebViewPrivate:(QIOSWebViewPrivate *)webViewPrivate;
- (void)pageDone;

// protocol:
- (void)webViewDidStartLoad:(UIWebView *)webView;
- (void)webViewDidFinishLoad:(UIWebView *)webView;
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error;
@end

@implementation QtWebViewDelegate
- (QtWebViewDelegate *)initWithQWebViewPrivate:(QIOSWebViewPrivate *)webViewPrivate
{
    qIosWebViewPrivate = webViewPrivate;
    return self;
}

- (void)pageDone
{
    Q_EMIT qIosWebViewPrivate->progressChanged(100);
    Q_EMIT qIosWebViewPrivate->pageFinished(qIosWebViewPrivate->getUrl());
    Q_EMIT qIosWebViewPrivate->titleChanged(qIosWebViewPrivate->getTitle());
    // QWebViewPrivate emits urlChanged.
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    Q_UNUSED(webView);
    // UIWebViewDelegate gives us per-frame notifications while the QWebView API
    // should provide per-page notifications. Keep track of started frame loads
    // and emit notifications when the final frame completes.
    if (++qIosWebViewPrivate->requestFrameCount == 1)
        emit qIosWebViewPrivate->pageStarted(qIosWebViewPrivate->getUrl());
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
    Q_UNUSED(webView);
    if (--qIosWebViewPrivate->requestFrameCount == 0)
        [self pageDone];
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    Q_UNUSED(webView);
    Q_UNUSED(error);
    if (--qIosWebViewPrivate->requestFrameCount == 0)
        [self pageDone];
}
@end

QWebViewPrivate *QWebViewPrivate::create(QWebView *q)
{
    QIOSWebViewPrivate *result = new QIOSWebViewPrivate(q);
    result->ensureNativeWebView();
    return result;
}

QIOSWebViewPrivate::QIOSWebViewPrivate(QWebView *q)
    : QWebViewPrivate(q)
{
    CGRect frame = CGRectMake(0.0, 0.0, 400, 400);
    UIWebView *webView = [[UIWebView alloc] initWithFrame:frame];
    webView.delegate = [[QtWebViewDelegate alloc] initWithQWebViewPrivate:this];
    uiWebView = webView;
}

QIOSWebViewPrivate::~QIOSWebViewPrivate()
{
    [uiWebView.delegate release];
    uiWebView.delegate = nil; // reset as per UIWebViewDelegate documentation
    [uiWebView release];
}

QString QIOSWebViewPrivate::getUrl() const
{
    NSURL *url = [[uiWebView request] URL];
    return QUrl::fromNSURL(url).toString();
}

void QIOSWebViewPrivate::loadUrl(const QString &url)
{
    Q_EMIT progressChanged(0);
    requestFrameCount = 0;
    [uiWebView loadRequest:[NSURLRequest requestWithURL:QUrl(url).toNSURL()]];
}

bool QIOSWebViewPrivate::canGoBack() const
{
    return uiWebView.canGoBack;
}

void QIOSWebViewPrivate::goBack() const
{
    [uiWebView goBack];
}

bool QIOSWebViewPrivate::canGoForward() const
{
    return uiWebView.canGoForward;
}

void QIOSWebViewPrivate::goForward() const
{
    [uiWebView goForward];
}

QString QIOSWebViewPrivate::getTitle() const
{
    NSString *title = [uiWebView stringByEvaluatingJavaScriptFromString:@"document.title"];
    return QString::fromNSString(title);
}

void QIOSWebViewPrivate::stopLoading() const
{
    [uiWebView stopLoading];
}

void *QIOSWebViewPrivate::nativeWebView() const
{
    return uiWebView;
}

QT_END_NAMESPACE
