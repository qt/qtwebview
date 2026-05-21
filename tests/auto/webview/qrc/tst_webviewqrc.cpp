// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QSignalSpy>
#include <QtTest/QtTest>
#include <QtWebView/qwebview.h>
#include <QtWebView/qwebviewloadinginfo.h>
#include <QtWebView/private/qwebviewfactory_p.h>

using namespace Qt::StringLiterals;

class TestWebView : public QWebView {
public:
    QSignalSpy urlSpy = QSignalSpy(this, SIGNAL(urlChanged(QUrl)));
    QSignalSpy loadingSpy = QSignalSpy(this, SIGNAL(loadingChanged(QWebViewLoadingInfo)));
};

class tst_WebViewQrc : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void loadQrc();
    void urlWithPrefix();
    void followRelativeLink();
    void followAbsoluteLink();
    void javaScriptNavigation();
    void iframe();
};

void tst_WebViewQrc::initTestCase()
{
    if (QWebViewFactory::loadedPluginHasKey("wasm"))
        QSKIP("qrc support not available for this backend.");
}

void tst_WebViewQrc::loadQrc()
{
    TestWebView webView;
    webView.setUrl(QUrl("qrc://html/success.html"_L1));

    QTRY_COMPARE(webView.loadingSpy.size(), 2);
    QCOMPARE(webView.title(), "Success"_L1);
}

void tst_WebViewQrc::urlWithPrefix()
{
    TestWebView webView;
    webView.setUrl(QUrl("qrc://html/prefix/success.html"_L1));

    QTRY_COMPARE(webView.loadingSpy.size(), 2);
    QCOMPARE(webView.title(), "Success!!!!"_L1);
}

void tst_WebViewQrc::followRelativeLink()
{
    TestWebView webView;
    webView.setUrl(QUrl("qrc://html/link.html"_L1));

    QTRY_COMPARE(webView.loadingSpy.size(), 2);
    webView.runJavaScript("document.getElementById('relative').click();");
    QTRY_COMPARE(webView.loadingSpy.size(), 4);
    QCOMPARE(webView.title(), "Success!!!!"_L1);
}

void tst_WebViewQrc::followAbsoluteLink()
{
    TestWebView webView;
    webView.setUrl(QUrl("qrc://html/link.html"_L1));

    QTRY_COMPARE(webView.loadingSpy.size(), 2);
    webView.runJavaScript("document.getElementById('absolute').click();");
    QTRY_COMPARE(webView.loadingSpy.size(), 4);
    QCOMPARE(webView.title(), "Success"_L1);
}

void tst_WebViewQrc::javaScriptNavigation()
{
    TestWebView webView;
    webView.setUrl(QUrl("qrc://html/link.html"_L1));

    QTRY_COMPARE(webView.loadingSpy.size(), 2);
    webView.runJavaScript("window.location = 'success.html';");
    QTRY_COMPARE(webView.loadingSpy.size(), 4);
    QCOMPARE(webView.title(), "Success"_L1);
    webView.runJavaScript("window.location.replace('qrc://html/prefix/success.html');");
    QTRY_COMPARE(webView.loadingSpy.size(), 6);
    QCOMPARE(webView.title(), "Success!!!!"_L1);
}

void tst_WebViewQrc::iframe()
{
    TestWebView webView;
    webView.setUrl(QUrl("qrc://html/iframe.html"_L1));

    QTRY_COMPARE(webView.loadingSpy.size(), 2);
    bool finished = false;
    webView.runJavaScript("document.getElementById('frame').contentDocument.title", [&](QVariant result) {
        QCOMPARE(result.toString(), "Success");
        finished = true;
    });

    QTRY_VERIFY(finished);
}

QTEST_MAIN(tst_WebViewQrc)

#include "tst_webviewqrc.moc"
