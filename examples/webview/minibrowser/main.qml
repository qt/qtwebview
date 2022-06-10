// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtWebView
import QtQuick.Layouts


ApplicationWindow {
    visible: true
    x: initialX
    y: initialY
    width: initialWidth
    height: initialHeight
    title: webView.title

    menuBar: ToolBar {
        id: navigationBar
        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolButton {
                id: backButton
                icon.source: "qrc:/left-32.png"
                onClicked: webView.goBack()
                enabled: webView.canGoBack
                Layout.preferredWidth: navigationBar.height
            }

            ToolButton {
                id: forwardButton
                icon.source: "qrc:/right-32.png"
                onClicked: webView.goForward()
                enabled: webView.canGoForward
                Layout.preferredWidth: navigationBar.height
            }

            ToolButton {
                id: reloadButton
                icon.source: webView.loading ? "qrc:/stop-32.png" : "qrc:/refresh-32.png"
                onClicked: webView.loading ? webView.stop() : webView.reload()
                Layout.preferredWidth: navigationBar.height
            }

            Item { Layout.preferredWidth: 5 }

            TextField {
                Layout.fillWidth: true
                id: urlField
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhPreferLowercase
                text: webView.url
                onAccepted: webView.url = utils.fromUserInput(text)
             }

            Item { Layout.preferredWidth: 5 }

            ToolButton {
                id: goButton
                text: qsTr("Go")
                onClicked: {
                    Qt.inputMethod.commit()
                    Qt.inputMethod.hide()
                    webView.url = utils.fromUserInput(urlField.text)
                }
            }

            Item { Layout.preferredWidth: 10 }
         }
         ProgressBar {
             id: progress
             anchors {
                left: parent.left
                top: parent.bottom
                right: parent.right
                leftMargin: parent.leftMargin
                rightMargin: parent.rightMargin
             }
             height:3
             z: Qt.platform.os === "android" ? -1 : -2
             background: Item {}
             visible: Qt.platform.os !== "ios" && Qt.platform.os !== "winrt"
             from: 0
             to: 100
             value: webView.loadProgress < 100 ? webView.loadProgress : 0
        }
    }

    WebView {
        id: webView
        anchors.fill: parent
        url: initialUrl
        onLoadingChanged: function(loadRequest) {
            if (loadRequest.errorString)
                console.error(loadRequest.errorString);
        }
    }
}
