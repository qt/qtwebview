// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import QtWebView
import QtTest

TestCase {
    id: testCase
    when: windowShown

    // Dynamically load a new WebView for every test case
    property var webView: undefined
    property var webViewComponent: webViewComponent

    // WebView Component:
    Component {
        id: webViewComponent

        WebView {
            id: webView
            settings.allowFileAccess: true
            property bool completed: false
            property var loadingStatuses: []
            property var urls: []

            // Signal spies
            property SignalSpy loadProgressSpy: SignalSpy {
                target: webView
                signalName: "loadProgressChanged"
            }

            property SignalSpy titleSpy: SignalSpy {
                target: webView
                signalName: "titleChanged"
            }

            property SignalSpy cookieAddedSpy: SignalSpy {
                target: webView
                signalName: "cookieAdded"
            }

            property SignalSpy cookieRemovedSpy: SignalSpy {
                target: webView
                signalName: "cookieRemoved"
            }

            // Signal handlers
            onLoadingChanged: (loadRequest) => {
                // We can't use a signal spy because the signal passes a stack-allocated object by reference.
                // Instead, we cache the loading status and URL to separate arrays. TODO fix this
                loadingStatuses.push(loadRequest.status);
                urls.push(loadRequest.url);
            }

            // Helpers
            function expectedUrl(expectedUrl) {
                return webView.url == expectedUrl
                    && urls[urls.length - 1] == expectedUrl;
            }

            TestResult { id: testResult }

            function _waitFor(predicate, timeout) {
                if (timeout === undefined)
                    timeout = 30000;
                var i = 0;
                while (i < timeout && !predicate()) {
                    testResult.wait(50);
                    i += 50;
                }
                return predicate();
            }

            function waitForLoad(loadStatus, timeout) {
                var numArgs = loadingStatuses.length;
                var success = _waitFor(function() {
                    if (loadingStatuses.length)
                        return loadingStatuses.length >= numArgs + 2
                            && loadingStatuses[loadingStatuses.length - 1] == loadStatus;
                    }, timeout);
                return success;
            }

            function pluginIs(pluginName) {
                return WebViewTestUtils.loadedPluginHasKey(pluginName);
            }
        }
    }

    // Test init/cleanup:
    function initTestCase() {
        let completed = false;
        tryVerify(function () { return webViewComponent.status == Component.Ready; });
    }

    function init() {
        webView = webViewComponent.createObject(root);
    }

    function cleanup() {
        webView.deleteAllCookies();
        webView.destroy();
        webView = undefined;
    }
}
