/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest/QtTest>
#include <QtCore/qstandardpaths.h>
#include <QtCore/qdir.h>
#include <QtCore/qtemporarydir.h>
#include <QtCore/qfileinfo.h>
#include <QtWebView/private/qwebview_p.h>
#include <QtQml/qqmlengine.h>

#ifndef QT_NO_QQUICKWEBVIEW_TESTS
#include <QtWebView/qquickwebview.h>
#endif // QT_NO_QQUICKWEBVIEW_TESTS

#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
#include <QtWebEngine>
#endif // QT_WEBVIEW_WEBENGINE_BACKEND

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

private:
    const QString m_cacheLocation;
};

void tst_QWebView::initTestCase()
{
#ifdef QT_WEBVIEW_WEBENGINE_BACKEND
    QtWebEngine::initialize();
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

    QWebView view;
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
#ifndef QT_NO_QQUICKWEBVIEW_TESTS
    const QString tstProperty = QString(QLatin1String("Qt.tst_data"));
    const QString title = QString(QLatin1String("WebViewTitle"));

    QQuickWebView view;
    QQmlEngine engine;
    QQmlContext *rootContext = engine.rootContext();
    QQmlEngine::setContextForObject(&view, rootContext);

    QCOMPARE(view.loadProgress(), 0);
    view.loadHtml(QString("<html><head><title>%1</title></head><body /></html>").arg(title));
    QTRY_COMPARE(view.loadProgress(), 100);
    QTRY_VERIFY(!view.isLoading());
    QCOMPARE(view.title(), title);
    QJSValue callback = engine.evaluate(QString("function(result) { %1 = result; }").arg(tstProperty));
    QVERIFY2(!callback.isError(), qPrintable(callback.toString()));
    QVERIFY(!callback.isUndefined());
    QVERIFY(callback.isCallable());
    view.runJavaScript(QString(QLatin1String("document.title")), callback);
    QTRY_COMPARE(engine.evaluate(tstProperty).toString(), title);
#endif // QT_NO_QQUICKWEBVIEW_TESTS
}

void tst_QWebView::loadHtml()
{
    QWebView view;
    QCOMPARE(view.loadProgress(), 0);
    view.loadHtml(QString("<html><head><title>WebViewTitle</title></head><body />"));
    QTRY_COMPARE(view.loadProgress(), 100);
    QTRY_VERIFY(!view.isLoading());
    QCOMPARE(view.title(), QStringLiteral("WebViewTitle"));
}

QTEST_MAIN(tst_QWebView)

#include "tst_qwebview.moc"
