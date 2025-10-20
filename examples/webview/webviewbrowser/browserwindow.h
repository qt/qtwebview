// Copyright (C) 2026 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets/qmainwindow.h>

QT_BEGIN_NAMESPACE
class QAction;
class QLineEdit;
class QProgressBar;
class QToolBar;
class QWebView;
class QWebViewLoadingInfo;
QT_END_NAMESPACE

class BrowserWindow : public QMainWindow
{
    Q_OBJECT
public:
    BrowserWindow();
    virtual ~BrowserWindow() = default;

public Q_SLOTS:
    void onLoadingChanged(const QWebViewLoadingInfo &loadingInfo);
    void onTitleChanged(const QString &title);
    void onLoadProgressChanged(int loadProgress);
    void onUrlInput();

private:
    QWebView *webView;
    QToolBar *toolBar;
    QLineEdit *lineEdit;
    QProgressBar *progressBar;

    QAction *backAction;
    QAction *forwardAction;
    QAction *reloadAction;
    QAction *stopAction;
};
