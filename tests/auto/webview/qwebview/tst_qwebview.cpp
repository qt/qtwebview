// Copyright (C) 2015 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QtTest>
#include <QtCore/qstandardpaths.h>
#include <QtCore/qdir.h>
#include <QtCore/qtemporarydir.h>
#include <QtCore/qfileinfo.h>
#include <QtWebView/qwebview.h>
#include <QtQml/qqmlengine.h>
#include <QtWebView/qwebviewsettings.h>
#include <QtWebView/qwebviewloadrequest.h>
#include <QtWebView/private/qwebviewfactory_p.h>

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
    void loadHtml_data();
    void loadHtml();
    void loadRequest();
    void setAndDeleteCookie();

private:
    const QString m_cacheLocation;
};

void tst_QWebView::initTestCase()
{
    if (!qEnvironmentVariableIsEmpty("QEMU_LD_PREFIX"))
        QSKIP("This test is unstable on QEMU, so it will be skipped.");
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
    view.settings()->setAllowFileAccess(true);
    view.settings()->setLocalContentCanAccessFileUrls(true);
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
    ANDROID_REQUIRES_API_LEVEL(19)
    const QString tstProperty = QString(QLatin1String("Qt.tst_data"));
    const QString title = QString(QLatin1String("WebViewTitle"));

    QWebView view;
    QCOMPARE(view.loadProgress(), 0);
    view.loadHtml(QString("<html><head><title>%1</title></head><body/></html>").arg(title));
    QTRY_COMPARE(view.loadProgress(), 100);
    QTRY_VERIFY(!view.isLoading());
    QCOMPARE(view.title(), title);
    bool called = false;
    QString documentTitle;
    auto callback = [&](const QVariant &result) {
        called = true;
        documentTitle = result.value<QString>();
    };
    view.runJavaScript(QString(QLatin1String("document.title")), callback);
    QTRY_COMPARE(called, true);
    QCOMPARE(documentTitle, title);
}

void tst_QWebView::loadHtml_data()
{
    QTest::addColumn<QByteArray>("content");
    QTest::addColumn<QUrl>("loadUrl");
    QTest::addColumn<QUrl>("resultUrl");
    QWebView view;
    QCOMPARE(view.loadProgress(), 0);
    QSignalSpy loadChangedSingalSpy(&view, SIGNAL(loadingChanged(QWebViewLoadRequest)));
    const QByteArray content(
            QByteArrayLiteral("<html><title>WebViewTitle</title>"
                              "<body><span style=\"color:#ff0000\">Hello</span></body></html>"));
    QByteArray encoded("data:text/html;charset=UTF-8,");
    encoded.append(content.toPercentEncoding());

    if (!QWebViewFactory::loadedPluginHasKey("webkit")) {
        QTest::newRow("set conent without base url") << content << QUrl() << QUrl(encoded);
    } else {
        QTest::newRow("set conent without base url") << content << QUrl() << QUrl("about:blank");
    }
    QTest::newRow("set content with data base url") << content << QUrl(encoded) << QUrl(encoded);

    if (!QWebViewFactory::loadedPluginHasKey("webview2")) {
        QTest::newRow("set content with non-data base url")
                << content << QUrl("http://foobar.com/") << QUrl("http://foobar.com/");
    } else {
        QTest::newRow("set content with non-data base url")
                << content << QUrl("http://foobar.com/") << QUrl(encoded);
    }
}

void tst_QWebView::loadHtml()
{
    QFETCH(QByteArray, content);
    QFETCH(QUrl, loadUrl);
    QFETCH(QUrl, resultUrl);

    QWebView view;
    QCOMPARE(view.loadProgress(), 0);
    QSignalSpy loadChangedSingalSpy(&view, SIGNAL(loadingChanged(QWebViewLoadRequest)));
    view.loadHtml(content, loadUrl);
    QTRY_COMPARE(view.loadProgress(), 100);
    QTRY_VERIFY(!view.isLoading());
    QCOMPARE(view.title(), QStringLiteral("WebViewTitle"));
    QTRY_COMPARE(loadChangedSingalSpy.size(), 2);
    // take load finished
    const QWebViewLoadRequest &lr = loadChangedSingalSpy.at(1).at(0).value<QWebViewLoadRequest>();
    QCOMPARE(lr.status(), QWebViewLoadRequest::LoadStatus::LoadSucceededStatus);
    bool called = false;
    QUrl url;
    auto callback = [&](const QVariant &result) {
        called = true;
        url = result.value<QUrl>();
    };
    view.runJavaScript("document.baseURI", callback);
    QTRY_COMPARE(called, true);
    QCOMPARE(url, resultUrl);

    QVERIFY(view.url().isValid());
    QCOMPARE(view.url(), resultUrl);
}

void tst_QWebView::loadRequest()
{
    // LoadSucceeded
    {
        QTemporaryFile file(m_cacheLocation + QStringLiteral("/XXXXXXfile.html"));
        QVERIFY2(file.open(),
                 qPrintable(QStringLiteral("Cannot create temporary file:") + file.errorString()));

        file.write("<html><head><title>FooBar</title></head><body/></html>");
        const QString fileName = file.fileName();
        file.close();

        QWebView view;

        view.settings()->setAllowFileAccess(true);
        view.settings()->setLocalContentCanAccessFileUrls(true);
        QCOMPARE(view.loadProgress(), 0);
        const QUrl url = QUrl::fromLocalFile(fileName);
        QSignalSpy loadChangedSingalSpy(&view, SIGNAL(loadingChanged(QWebViewLoadRequest)));
        view.setUrl(url);
        QTRY_VERIFY(!view.isLoading());
        QTRY_COMPARE(view.loadProgress(), 100);
        QTRY_COMPARE(view.title(), QStringLiteral("FooBar"));
        QCOMPARE(view.url(), url);
        QTRY_COMPARE(loadChangedSingalSpy.size(), 2);
        {
            const QList<QVariant> &loadStartedArgs = loadChangedSingalSpy.takeFirst();
            const QWebViewLoadRequest &lr = loadStartedArgs.at(0).value<QWebViewLoadRequest>();
            QCOMPARE(lr.status(), QWebViewLoadRequest::LoadStatus::LoadStartedStatus);
        }
        {
            const QList<QVariant> &loadStartedArgs = loadChangedSingalSpy.takeFirst();
            const QWebViewLoadRequest &lr = loadStartedArgs.at(0).value<QWebViewLoadRequest>();
            QCOMPARE(lr.status(), QWebViewLoadRequest::LoadStatus::LoadSucceededStatus);
        }
    }

    // LoadFailed
    {
        QWebView view;
        view.settings()->setAllowFileAccess(true);
        view.settings()->setLocalContentCanAccessFileUrls(true);
        QCOMPARE(view.loadProgress(), 0);
        QSignalSpy loadChangedSingalSpy(&view, SIGNAL(loadingChanged(QWebViewLoadRequest)));
        view.setUrl(QUrl(QStringLiteral("file:///file_that_does_not_exist.html")));
        QTRY_VERIFY(!view.isLoading());
        QTRY_COMPARE(loadChangedSingalSpy.size(), 2);
        {
            const QList<QVariant> &loadStartedArgs = loadChangedSingalSpy.takeFirst();
            const QWebViewLoadRequest &lr = loadStartedArgs.at(0).value<QWebViewLoadRequest>();
            QCOMPARE(lr.status(), QWebViewLoadRequest::LoadStatus::LoadStartedStatus);
        }
        {
            const QList<QVariant> &loadStartedArgs = loadChangedSingalSpy.takeFirst();
            const QWebViewLoadRequest &lr = loadStartedArgs.at(0).value<QWebViewLoadRequest>();
            QCOMPARE(lr.status(), QWebViewLoadRequest::LoadStatus::LoadFailedStatus);
        }
        if (QWebViewFactory::loadedPluginHasKey("webengine"))
            QCOMPARE(view.loadProgress(), 100);
    }
}

void tst_QWebView::setAndDeleteCookie()
{
    QWebView view;
    view.settings()->setLocalStorageEnabled(true);
    view.settings()->setAllowFileAccess(true);
    view.settings()->setLocalContentCanAccessFileUrls(true);

    QSignalSpy cookieAddedSpy(&view, SIGNAL(cookieAdded(QString,QString)));
    QSignalSpy cookieRemovedSpy(&view, SIGNAL(cookieRemoved(QString,QString)));

    view.setCookie(".example.com", "TestCookie", "testValue");
    view.setCookie(".example2.com", "TestCookie2", "testValue2");
    view.setCookie(".example3.com", "TestCookie3", "testValue3");
    QTRY_COMPARE(cookieAddedSpy.size(), 3);

    view.deleteCookie(".example.com", "TestCookie");
    QTRY_COMPARE(cookieRemovedSpy.size(), 1);

    // deleting a cookie using a name that has not been set
    view.deleteCookie(".example.com", "NewCookieName");
    QTRY_COMPARE(cookieRemovedSpy.size(), 1);

    // deleting a cookie using a domain that has not been set
    view.deleteCookie(".new.domain.com", "TestCookie2");
    QTRY_COMPARE(cookieRemovedSpy.size(), 1);

    view.deleteAllCookies();
    if (QWebViewFactory::loadedPluginHasKey("android_view"))
        QEXPECT_FAIL("", "Notification for deleteAllCookies() is not implemented on Android, yet!", Continue);
    QTRY_COMPARE(cookieRemovedSpy.size(), 3);
}

QTEST_MAIN(tst_QWebView)

#include "tst_qwebview.moc"
