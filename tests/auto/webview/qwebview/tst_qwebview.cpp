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
#include <QtWebView/private/qwebview_p.h>

class tst_QWebView : public QObject
{
    Q_OBJECT
private slots:
    void load();
};

void tst_QWebView::load()
{
    QString cacheLocation = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QString fileName(cacheLocation + QStringLiteral("/file.html"));

    {
        QFile file(fileName);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("<html><head><title>FooBar</title></head><body />");
    }

    QWebView view;
    QCOMPARE(view.loadProgress(), 0);
    view.setUrl(QUrl(QStringLiteral("file://") + fileName));
    QTRY_COMPARE(view.loadProgress(), 100);
    QTRY_VERIFY(!view.isLoading());
    QCOMPARE(view.title(), QStringLiteral("FooBar"));
    QVERIFY(!view.canGoBack());
    QVERIFY(!view.canGoForward());
    QCOMPARE(view.url(), QUrl(QStringLiteral("file://") + fileName));
}

QTEST_MAIN(tst_QWebView)

#include "tst_qwebview.moc"
