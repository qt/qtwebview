/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtCore/qstandardpaths.h>
#include <QtCore/qdir.h>
#include <QtCore/qtemporarydir.h>
#include <QtCore/qfileinfo.h>
#include <QtWebView/private/qwebview_p.h>
#include <QtQml/qqmlengine.h>
#include <QtWebView/private/qwebviewloadrequest_p.h>

#ifdef QT_QQUICKWEBVIEW_TESTS
#include <QtWebViewQuick/private/qquickwebview_p.h>
#endif // QT_NO_QQUICKWEBVIEW_TESTS

#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
#include <QtWebEngineQuick>
#endif // QT_WEBVIEW_WEBENGINE_BACKEND

#if defined(Q_OS_ANDROID) && !defined(Q_OS_ANDROID_NO_SDK)
#include <QtCore/private/qjnihelpers_p.h>
#define ANDROID_REQUIRES_API_LEVEL(N) \
    if (QtAndroidPrivate::androidSdkVersion() < N) \
        QSKIP("This feature is not supported on this version of Android");
#else
#define ANDROID_REQUIRES_API_LEVEL(N)
#endif

class tst_QWebView : public QObject
{
    Q_OBJECT
public:
    tst_QWebView() : m_cacheLocation(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)) {}

private slots:
    void initTestCase();
    void load();
    void runJavaScript();
    void loadHtml();
    void loadRequest();
    void setAndDeleteCookie();

private:
    const QString m_cacheLocation;
};

void tst_QWebView::initTestCase()
{
#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
    QtWebEngineQuick::initialize();
#endif // QT_WEBVIEW_WEBENGINE_BACKEND
    if (!QFileInfo(m_cacheLocation).isDir()) {
        QDir dir;
        QVERIFY(dir.mkpath(m_cacheLocation));
    }
}

void tst_QWebView::load()
{
    QTemporaryFile file(m_cacheLocation + QStringLiteral("/XXXXXXfile.html"));
    QVERIFY2(file.open(),
             qPrintable(QStringLiteral("Cannot create temporary file:") + file.errorString()));

    file.write("<html><head><title>FooBar</title></head><body />");
    const QString fileName = file.fileName();
    file.close();

#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
    QQmlEngine engine;
    QQmlContext *rootContext = engine.rootContext();
    QQuickWebView qview;
    QQmlEngine::setContextForObject(&qview, rootContext);
    QWebView &view = qview.webView();
#else
    QWebView view;
#endif
    QCOMPARE(view.loadProgress(), 0);
    const QUrl url = QUrl::fromLocalFile(fileName);
    view.setUrl(url);
    QTRY_COMPARE(view.loadProgress(), 100);
    QTRY_VERIFY(!view.isLoading());
    QCOMPARE(view.title(), QStringLiteral("FooBar"));
    QVERIFY(!view.canGoBack());
    QVERIFY(!view.canGoForward());
    QCOMPARE(view.url(), url);
}

void tst_QWebView::runJavaScript()
{
#ifdef QT_QQUICKWEBVIEW_TESTS
#ifndef QT_WEBVIEW_WEBENGINE_BACKEND
    ANDROID_REQUIRES_API_LEVEL(19)
#endif
    const QString tstProperty = QString(QLatin1String("Qt.tst_data"));
    const QString title = QString(QLatin1String("WebViewTitle"));

    QQmlEngine engine;
    QQmlContext *rootContext = engine.rootContext();
    QQuickWebView view;
    QQmlEngine::setContextForObject(&view, rootContext);

    QCOMPARE(view.loadProgress(), 0);
    view.loadHtml(QString("<html><head><title>%1</title></head><body /></html>").arg(title));
    QTRY_COMPARE(view.loadProgress(), 100);
    QTRY_VERIFY(!view.isLoading());
    QCOMPARE(view.title(), title);
    QJSValue callback = engine.evaluate(QString("(function(result) { %1 = result; })").arg(tstProperty));
    QVERIFY2(!callback.isError(), qPrintable(callback.toString()));
    QVERIFY(!callback.isUndefined());
    QVERIFY(callback.isCallable());
    view.runJavaScript(QString(QLatin1String("document.title")), callback);
    QTRY_COMPARE(engine.evaluate(tstProperty).toString(), title);
#endif // QT_QQUICKWEBVIEW_TESTS
}

void tst_QWebView::loadHtml()
{
#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
    QQmlEngine engine;
    QQmlContext *rootContext = engine.rootContext();
    QQuickWebView qview;
    QQmlEngine::setContextForObject(&qview, rootContext);
    QWebView &view = qview.webView();
#else
    QWebView view;
#endif
    QCOMPARE(view.loadProgress(), 0);
    view.loadHtml(QString("<html><head><title>WebViewTitle</title></head><body />"));
    QTRY_COMPARE(view.loadProgress(), 100);
    QTRY_VERIFY(!view.isLoading());
    QCOMPARE(view.title(), QStringLiteral("WebViewTitle"));
}

void tst_QWebView::loadRequest()
{
    // LoadSucceeded
    {
        QTemporaryFile file(m_cacheLocation + QStringLiteral("/XXXXXXfile.html"));
        QVERIFY2(file.open(),
                 qPrintable(QStringLiteral("Cannot create temporary file:") + file.errorString()));

        file.write("<html><head><title>FooBar</title></head><body />");
        const QString fileName = file.fileName();
        file.close();
#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
        QQmlEngine engine;
        QQmlContext *rootContext = engine.rootContext();
        QQuickWebView qview;
        QQmlEngine::setContextForObject(&qview, rootContext);
        QWebView &view = qview.webView();
#else
        QWebView view;
#endif
        QCOMPARE(view.loadProgress(), 0);
        const QUrl url = QUrl::fromLocalFile(fileName);
        QSignalSpy loadChangedSingalSpy(&view, SIGNAL(loadingChanged(const QWebViewLoadRequestPrivate &)));
        view.setUrl(url);
        QTRY_VERIFY(!view.isLoading());
        QTRY_COMPARE(view.loadProgress(), 100);
        QTRY_COMPARE(view.title(), QStringLiteral("FooBar"));
        QCOMPARE(view.url(), url);
        QTRY_COMPARE(loadChangedSingalSpy.count(), 2);
        {
            const QList<QVariant> &loadStartedArgs = loadChangedSingalSpy.takeFirst();
            const QWebViewLoadRequestPrivate &lr = loadStartedArgs.at(0).value<QWebViewLoadRequestPrivate>();
            QCOMPARE(lr.m_status, QWebView::LoadStartedStatus);
        }
        {
            const QList<QVariant> &loadStartedArgs = loadChangedSingalSpy.takeFirst();
            const QWebViewLoadRequestPrivate &lr = loadStartedArgs.at(0).value<QWebViewLoadRequestPrivate>();
            QCOMPARE(lr.m_status, QWebView::LoadSucceededStatus);
        }
    }

    // LoadFailed
    {
#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
        QQmlEngine engine;
        QQmlContext *rootContext = engine.rootContext();
        QQuickWebView qview;
        QQmlEngine::setContextForObject(&qview, rootContext);
        QWebView &view = qview.webView();
#else
        QWebView view;
#endif
        QCOMPARE(view.loadProgress(), 0);
        QSignalSpy loadChangedSingalSpy(&view, SIGNAL(loadingChanged(const QWebViewLoadRequestPrivate &)));
        view.setUrl(QUrl(QStringLiteral("file:///file_that_does_not_exist.html")));
        QTRY_VERIFY(!view.isLoading());
        QTRY_COMPARE(loadChangedSingalSpy.count(), 2);
        {
            const QList<QVariant> &loadStartedArgs = loadChangedSingalSpy.takeFirst();
            const QWebViewLoadRequestPrivate &lr = loadStartedArgs.at(0).value<QWebViewLoadRequestPrivate>();
            QCOMPARE(lr.m_status, QWebView::LoadStartedStatus);
        }
        {
            const QList<QVariant> &loadStartedArgs = loadChangedSingalSpy.takeFirst();
            const QWebViewLoadRequestPrivate &lr = loadStartedArgs.at(0).value<QWebViewLoadRequestPrivate>();
            QCOMPARE(lr.m_status, QWebView::LoadFailedStatus);
        }
#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
        QCOMPARE(view.loadProgress(), 0); // darwin plugin returns 100
#endif
    }
}

void tst_QWebView::setAndDeleteCookie()
{
#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
    QQmlEngine engine;
    QQmlContext * rootContext = engine.rootContext();
    QQuickWebView qview;
    QQmlEngine::setContextForObject(&qview, rootContext);
    QWebView & view = qview.webView();
#else
    QWebView view;
#endif

    QSignalSpy cookieAddedSpy(&view, SIGNAL(cookieAdded(const QString &, const QString &)));
    QSignalSpy cookieRemovedSpy(&view, SIGNAL(cookieRemoved(const QString &, const QString &)));

    view.setCookie(".example.com", "TestCookie", "testValue");
    view.setCookie(".example2.com", "TestCookie2", "testValue2");
    view.setCookie(".example3.com", "TestCookie3", "testValue3");
    QTRY_COMPARE(cookieAddedSpy.count(), 3);

    view.deleteCookie(".example.com", "TestCookie");
    QTRY_COMPARE(cookieRemovedSpy.count(), 1);

    // deleting a cookie using a name that has not been set
    view.deleteCookie(".example.com", "NewCookieName");
    QTRY_COMPARE(cookieRemovedSpy.count(), 1);

    // deleting a cookie using a domain that has not been set
    view.deleteCookie(".new.domain.com", "TestCookie2");
    QTRY_COMPARE(cookieRemovedSpy.count(), 1);

    view.deleteAllCookies();
#ifdef Q_OS_ANDROID
    QEXPECT_FAIL("", "Notification for deleteAllCookies() is not implemented on Android, yet!", Continue);
#endif
    QTRY_COMPARE(cookieRemovedSpy.count(), 3);
}

QTEST_MAIN(tst_QWebView)

#include "tst_qwebview.moc"
