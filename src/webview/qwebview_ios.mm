/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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

#include "qwebview_ios_p.h"
#include "qwebview_p.h"

#include <QtQuick/qquickitem.h>
#include <QtCore/qmap.h>

#include <CoreFoundation/CoreFoundation.h>
#include <UIKit/UIKit.h>

#import <UIKit/UIView.h>
#import <UIKit/UIWindow.h>
#import <UIKit/UIViewController.h>
#import <UIKit/UITapGestureRecognizer.h>
#import <UIKit/UIGestureRecognizerSubclass.h>

QT_BEGIN_NAMESPACE

QWebViewPrivate *QWebViewPrivate::create(QWebView *q)
{
    return new QIosWebViewPrivate(q);
}

static inline CGRect toCGRect(const QRectF &rect)
{
    return CGRectMake(rect.x(), rect.y(), rect.width(), rect.height());
}

// -------------------------------------------------------------------------

@interface QIOSNativeViewSelectedRecognizer : UIGestureRecognizer <UIGestureRecognizerDelegate>
{
@public
    QNativeViewController *m_item;
}
@end

@implementation QIOSNativeViewSelectedRecognizer

- (id)initWithQWindowControllerItem:(QNativeViewController *)item
{
    self = [super initWithTarget:self action:@selector(nativeViewSelected:)];
    if (self) {
        self.cancelsTouchesInView = NO;
        self.delaysTouchesEnded = NO;
        m_item = item;
    }
    return self;
}

- (BOOL)canPreventGestureRecognizer:(UIGestureRecognizer *)other
{
    Q_UNUSED(other);
    return NO;
}

- (BOOL)canBePreventedByGestureRecognizer:(UIGestureRecognizer *)other
{
    Q_UNUSED(other);
    return NO;
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    Q_UNUSED(touches);
    Q_UNUSED(event);
    self.state = UIGestureRecognizerStateRecognized;
}

- (void)nativeViewSelected:(UIGestureRecognizer *)gestureRecognizer
{
    Q_UNUSED(gestureRecognizer);
    m_item->setFocus(true);
}

@end

// -------------------------------------------------------------------------

class QWebViewInterface;

@interface QtWebViewDelegate : NSObject<UIWebViewDelegate> {
    QIosWebViewPrivate *qIosWebViewPrivate;
}
- (QtWebViewDelegate *)initWithQAbstractWebView:(QIosWebViewPrivate *)webViewPrivate;
- (void)pageDone;

// protocol:
- (void)webViewDidStartLoad:(UIWebView *)webView;
- (void)webViewDidFinishLoad:(UIWebView *)webView;
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error;
@end

@implementation QtWebViewDelegate
- (QtWebViewDelegate *)initWithQAbstractWebView:(QIosWebViewPrivate *)webViewPrivate
{
    qIosWebViewPrivate = webViewPrivate;
    return self;
}

- (void)pageDone
{
    Q_EMIT qIosWebViewPrivate->loadProgressChanged();
    Q_EMIT qIosWebViewPrivate->loadingChanged();
    Q_EMIT qIosWebViewPrivate->titleChanged();
    Q_EMIT qIosWebViewPrivate->urlChanged();
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    Q_UNUSED(webView);
    // UIWebViewDelegate gives us per-frame notifications while the QWebView API
    // should provide per-page notifications. Keep track of started frame loads
    // and emit notifications when the final frame completes.
    ++qIosWebViewPrivate->requestFrameCount;
    Q_EMIT qIosWebViewPrivate->loadingChanged();
    Q_EMIT qIosWebViewPrivate->loadProgressChanged();
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

QIosWebViewPrivate::QIosWebViewPrivate(QObject *p)
    : QWebViewPrivate(p)
    , uiWebView(0)
    , m_recognizer(0)
{
    CGRect frame = CGRectMake(0.0, 0.0, 400, 400);
    uiWebView = [[UIWebView alloc] initWithFrame:frame];
    uiWebView.delegate = [[QtWebViewDelegate alloc] initWithQAbstractWebView:this];

    m_recognizer = [[QIOSNativeViewSelectedRecognizer alloc] initWithQWindowControllerItem:this];
    [uiWebView addGestureRecognizer:m_recognizer];

}

QIosWebViewPrivate::~QIosWebViewPrivate()
{
    [uiWebView.delegate release];
    uiWebView.delegate = nil; // reset as per UIWebViewDelegate documentation
    [uiWebView release];
    [m_recognizer release];
}

QUrl QIosWebViewPrivate::url() const
{
    NSString *currentURL = [uiWebView stringByEvaluatingJavaScriptFromString:@"window.location.href"];
    return QString::fromNSString(currentURL);
}

void QIosWebViewPrivate::setUrl(const QUrl &url)
{
    requestFrameCount = 0;
    [uiWebView loadRequest:[NSURLRequest requestWithURL:url.toNSURL()]];
}

bool QIosWebViewPrivate::canGoBack() const
{
    return uiWebView.canGoBack;
}

bool QIosWebViewPrivate::canGoForward() const
{
    return uiWebView.canGoForward;
}

QString QIosWebViewPrivate::title() const
{
    NSString *title = [uiWebView stringByEvaluatingJavaScriptFromString:@"document.title"];
    return QString::fromNSString(title);
}

int QIosWebViewPrivate::loadProgress() const
{
    // TODO:
    return isLoading() ? 100 : 0;
}

bool QIosWebViewPrivate::isLoading() const
{
    return uiWebView.loading;
}

void QIosWebViewPrivate::setParentView(QObject *view)
{
    if (!uiWebView)
        return;

    QWindow *window = qobject_cast<QWindow *>(view);
    if (window != 0) {
        UIView *parentView = reinterpret_cast<UIView *>(window->winId());
        [parentView addSubview:uiWebView];
    } else {
        [uiWebView removeFromSuperview];
    }
}

void QIosWebViewPrivate::setGeometry(const QRect &geometry)
{
    [uiWebView setFrame:toCGRect(geometry)];
}

void QIosWebViewPrivate::setVisibility(QWindow::Visibility visibility)
{
    Q_UNUSED(visibility);
}

void QIosWebViewPrivate::setVisible(bool visible)
{
    [uiWebView setHidden:visible];
}

void QIosWebViewPrivate::setFocus(bool focus)
{
    Q_EMIT requestFocus(focus);
}

void QIosWebViewPrivate::goBack()
{
    [uiWebView goBack];
}

void QIosWebViewPrivate::goForward()
{
    [uiWebView goForward];
}

void QIosWebViewPrivate::stop()
{
    [uiWebView stopLoading];
}

void QIosWebViewPrivate::reload()
{
    [uiWebView reload];
}

void QIosWebViewPrivate::runJavaScriptPrivate(const QString &script, int callbackId)
{
    // ### TODO needs more async
    NSString *result = [uiWebView stringByEvaluatingJavaScriptFromString:script.toNSString()];
    if (callbackId != -1)
        Q_EMIT javaScriptResult(callbackId, QString::fromNSString(result));
}

QT_END_NAMESPACE
