// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtWebView
import QtTest
import QuickWebViewTest

RowLayout {
    id: root

    WebViewTestCase {
        name: "HiddenNavigation"

        function test_loadEmptyPageViewHidden() {
            webView.url = WebViewTestUtils.makeTestFileUrl("basic_page.html");
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
        }
    }

    WebViewTestCase {
        name: "Navigation"
        when: windowShown

        function test_startupState() {
            compare(webView.canGoBack, false);
            compare(webView.canGoForward, false);
            compare(webView.loading, false);
            compare(webView.url, "");
        }

        function test_loadNonexistentFileUrl() {
            webView.url = Qt.resolvedUrl("file_that_does_not_exist.html");
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadFailedStatus));
        }

        function test_backAndForward() {
            let page1 = WebViewTestUtils.makeTestFileUrl("basic_page.html");
            let page2 = WebViewTestUtils.makeTestFileUrl("basic_page2.html");

            webView.url = page1;
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
            verify(webView.expectedUrl(page1));

            verify(!webView.canGoBack);
            verify(!webView.canGoForward);

            webView.url = page2;
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
            verify(webView.expectedUrl(page2));

            verify(webView.canGoBack);
            verify(!webView.canGoForward);

            webView.goBack();
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
            verify(!webView.canGoBack);
            verify(webView.canGoForward);

            webView.goForward();
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
            verify(webView.canGoBack);
            verify(!webView.canGoForward);
        }

        function test_reload() {
            let u = WebViewTestUtils.makeTestFileUrl("basic_page.html");
            webView.url = u;
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
            verify(webView.expectedUrl(u));

            webView.reload();
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
            verify(webView.expectedUrl(u));
        }

        function test_loadProgress() {
            // TODO check for values less than 100. Is that guaranteed to happen on all platforms?
            webView.url = WebViewTestUtils.makeTestFileUrl("basic_page.html");
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
            verify(webView.loadProgressSpy.signalArguments.length >= 1);
            compare(webView.loadProgress, 100);
        }
    }

    WebViewTestCase {
        name: "PageData"

        function test_title() {
            webView.url = WebViewTestUtils.makeTestFileUrl("basic_page2.html");

            if (webView.pluginIs("webengine")
                    || webView.pluginIs("android_view")
                    || webView.pluginIs("webview2") ) {
                // Signal does not get emitted on some platforms, because page has no <title>
                // TODO maybe fix that?
                webView.titleSpy.wait();
                webView.titleSpy.clear();
            }

            webView.url = WebViewTestUtils.makeTestFileUrl("basic_page.html");
            webView.titleSpy.wait();
            tryCompare(webView, "title", "Basic Page");

            // No titleChanged signal for failed load
            webView.titleSpy.clear();
            webView.loadingStatuses = [];
            webView.urls = [];
            webView.url = Qt.resolvedUrl("file_that_does_not_exist.html");
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadFailedStatus));

            if (webView.pluginIs("android_view")
                    || webView.pluginIs("webview2")) {
                webView.titleSpy.wait();
            } else {
                wait(200);
                compare(webView.titleSpy.signalArguments.length, 0);
            }
        }
    }

    WebViewTestCase {
        name: "UserAgent"

        function test_changeUserAgent() {
            // Wait for backend to initialize. TODO maybe add signal for that?
            webView.url = WebViewTestUtils.makeTestFileUrl("basic_page.html");
            let waitForBackend = function() { return webView.httpUserAgent.length != 0; };
            tryVerify(waitForBackend, 10000);

            let defaultAgent = webView.httpUserAgent.slice(0);

            // Test setting at object creation
            webView.destroy();
            webView = webViewComponent.createObject(root, { httpUserAgent: "TestAgent" });
            webView.url = WebViewTestUtils.makeTestFileUrl("basic_page.html");
            tryVerify(waitForBackend, 10000);
            verify(webView.httpUserAgent == "TestAgent");

            // Test that the setting is not global
            webView.destroy();
            webView = webViewComponent.createObject(root);
            webView.url = WebViewTestUtils.makeTestFileUrl("basic_page.html");
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
            if (webView.pluginIs("webengine")) {
                expectFail("", "The setting is global for the WebEngine backend");
            }
            verify(webView.httpUserAgent == defaultAgent);

            // Test setting dynamically
            webView.url = WebViewTestUtils.makeTestFileUrl("basic_page.html");
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
            webView.httpUserAgent = "TestAgent";
            verify(webView.httpUserAgent == "TestAgent");
        }
    }

    WebViewTestCase {
        name: "JavaScript"

        function test_javascriptSettings_data() {
            return [
                { tag: "Test JS enabled",  jsEnabled: true,  expectedTitle: "JavaScript Test" },
                { tag: "Test JS disabled", jsEnabled: false, expectedTitle: "JavaScript" },
            ]
        }

        function test_javascriptSettings(data) {
            webView.settings.javaScriptEnabled = data.jsEnabled;
            webView.url = WebViewTestUtils.makeTestFileUrl("javascript.html");
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));
            tryCompare(webView, "title", data.expectedTitle);
        }

        function test_runJavaScript() {
            webView.url = WebViewTestUtils.makeTestFileUrl("basic_page.html");
            verify(webView.waitForLoad(WebViewLoadRequest.LoadStatus.LoadSucceededStatus));

            var title = undefined;
            webView.runJavaScript("document.title", function(result) {
                title = result;
            });

            tryVerify(function() { return title === "Basic Page" }, 12000);
        }
    }

    WebViewTestCase {
        name: "Cookies"

        function test_setAndDeleteCookies() {
            let cookieList = [
                { domain: ".example1.com", name: "TestCookie1", value: "testValue1" },
                { domain: ".example2.com", name: "TestCookie2", value: "testValue2" },
                { domain: ".example3.com", name: "TestCookie3", value: "testValue3" }
            ]
            let deletedCookie = { domain: ".example3.com", name: "TestCookie3", value: "testValue3" };

            for (const cookie of cookieList) {
                webView.setCookie(cookie.domain, cookie.name, cookie.value);
            }

            tryCompare(webView.cookieAddedSpy.signalArguments, "length", cookieList.length, 10000);

            // Delete an existing cookie
            webView.deleteCookie(deletedCookie.domain, deletedCookie.name);
            webView.cookieRemovedSpy.wait();
            compare(webView.cookieRemovedSpy.signalArguments[0][0], deletedCookie.domain);
            compare(webView.cookieRemovedSpy.signalArguments[0][1], deletedCookie.name);

            // Try to delete a cookie using a name that has not been set
            webView.deleteCookie(".example.com", "NewCookieName");
            wait(200); // We need to check that no signal has been emitted. Wait time is arbitrary
            compare(webView.cookieRemovedSpy.signalArguments.length, 1);

            // Try to delete a cookie using a domain that has not been set
            webView.deleteCookie("new.domain.com", "TestCookie2");
            wait(200);
            compare(webView.cookieRemovedSpy.signalArguments.length, 1);

            webView.deleteAllCookies();
            if (webView.pluginIs("android_view")) {
                expectFail("", "Notification for deleteAllCookies() is not implemented on Android yet!");
            }
            tryCompare(webView.cookieRemovedSpy.signalArguments, "length", 3);
        }

        function test_setCookie() {
            // These should not be accepted, and shouldn't cause a crash
            webView.setCookie("", "", "testValue");
            webView.setCookie(".fake.domain", "", "testValue");
            webView.setCookie("", "fakeName", "testValue");

            wait(200);
            compare(webView.cookieAddedSpy.signalArguments.length, 0);

            // This should be accepted, as a cookie with an empty value is valid
            webView.setCookie(".fake.domain", "fakeName", "");
            tryCompare(webView.cookieAddedSpy.signalArguments, "length", 1)
        }
    }
}
