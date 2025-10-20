// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "browserwindow.h"

#include <QtCore/qurl.h>
#include <QtGui/qaction.h>
#include <QtWebView/qwebview.h>
#include <QtWebView/qwebviewloadinginfo.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qprogressbar.h>

using namespace Qt::StringLiterals;

BrowserWindow::BrowserWindow()
    : webView(new QWebView())
    , toolBar(addToolBar("Navigation"))
    , lineEdit(new QLineEdit(this))
    , progressBar(new QProgressBar(this))
    , backAction(new QAction(QIcon(":left-32.png"_L1), "Go back"_L1, this))
    , forwardAction(new QAction(QIcon(":right-32.png"_L1), "Go forward"_L1, this))
    , reloadAction(new QAction(QIcon(":refresh-32.png"_L1), "Reload"_L1, this))
    , stopAction(new QAction(QIcon(":stop-32.png"_L1), "Stop"_L1, this))
{
    // Wrap the QWebView in a QWidget
    QWidget *webViewContainer = QWidget::createWindowContainer(webView);
    webViewContainer->setMinimumSize(QSize(1280, 720));
    setCentralWidget(webViewContainer);

    // Set up toolbar
    toolBar->setMovable(false);
    toolBar->addAction(backAction);
    toolBar->addAction(forwardAction);
    toolBar->addAction(reloadAction);
    toolBar->addAction(stopAction);
    toolBar->addSeparator();

    lineEdit->setReadOnly(false);
    toolBar->addWidget(lineEdit);

    stopAction->setVisible(false);

    progressBar->setMaximumHeight(1);
    progressBar->setTextVisible(false);
    progressBar->setStyleSheet("QProgressBar {border: 0px} QProgressBar::chunk {background-color: #da4453}"_L1);
    layout()->addWidget(progressBar);

    // Set up signal/slot connections
    connect(backAction, &QAction::triggered, webView, &QWebView::goBack);
    connect(forwardAction, &QAction::triggered, webView, &QWebView::goForward);
    connect(reloadAction, &QAction::triggered, webView, &QWebView::reload);
    connect(stopAction, &QAction::triggered, webView, &QWebView::stop);
    connect(webView, &QWebView::loadingChanged, this, &BrowserWindow::onLoadingChanged);
    connect(webView, &QWebView::loadProgressChanged, this, &BrowserWindow::onLoadProgressChanged);
    connect(webView, &QWebView::titleChanged, this, &BrowserWindow::onTitleChanged);
    connect(lineEdit, &QLineEdit::returnPressed, this, &BrowserWindow::onUrlInput);

    // Load the startup URL
    webView->setUrl(QUrl("https://qt.io"_L1));
}

void BrowserWindow::onLoadingChanged(const QWebViewLoadingInfo &loadingInfo)
{
    switch (loadingInfo.status()) {
    case QWebViewLoadingInfo::LoadStatus::Started:
        reloadAction->setEnabled(false);
        reloadAction->setVisible(false);
        stopAction->setEnabled(true);
        stopAction->setVisible(true);
        break;
    case QWebViewLoadingInfo::LoadStatus::Stopped:
    case QWebViewLoadingInfo::LoadStatus::Succeeded:
    case QWebViewLoadingInfo::LoadStatus::Failed:
        reloadAction->setEnabled(true);
        reloadAction->setVisible(true);
        stopAction->setEnabled(false);
        stopAction->setVisible(false);
        break;
    }

    if (webView->canGoBack()) {
        backAction->setEnabled(true);
    } else {
        backAction->setEnabled(false);
    }

    if (webView->canGoForward()) {
        forwardAction->setEnabled(true);
    } else {
        forwardAction->setEnabled(false);
    }

    lineEdit->setText(loadingInfo.url().toString());
}

void BrowserWindow::onTitleChanged(const QString &title)
{
    setWindowTitle(title);
}

void BrowserWindow::onLoadProgressChanged(int loadProgress)
{
    if (0 < loadProgress && loadProgress < 100) {
        progressBar->setValue(loadProgress);
    } else {
        progressBar->setValue(0);
    }
}

void BrowserWindow::onUrlInput()
{
    webView->setUrl(QUrl::fromUserInput(lineEdit->text()));
}
