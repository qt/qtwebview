/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtWebView module of the Qt Toolkit.
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

package org.qtproject.qt5.android.view;

import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.WebChromeClient;
import java.lang.Runnable;
import android.app.Activity;
import java.lang.String;
import android.webkit.WebSettings;
import android.webkit.URLUtil;
import android.util.Log;
import android.webkit.WebSettings.PluginState;
import android.graphics.Bitmap;
import java.util.concurrent.Semaphore;

public class QtAndroidWebViewController
{
    private final Activity m_activity;
    private final long m_id;
    private WebView m_webView = null;
    private static final String TAG = "QtAndroidWebViewController";
    // Native callbacks
    private native void c_onPageFinished(long id, String url);
    private native void c_onPageStarted(long id, String url, Bitmap icon);
    private native void c_onProgressChanged(long id, int newProgress);
    private native void c_onReceivedIcon(long id, Bitmap icon);
    private native void c_onReceivedTitle(long id, String title);

    private class QtAndroidWebViewClient extends WebViewClient
    {
        QtAndroidWebViewClient() { super(); }

        @Override
        public void onLoadResource(WebView view, String url)
        {
            super.onLoadResource(view, url);
        }

        @Override
        public void onPageFinished(WebView view, String url)
        {
            super.onPageFinished(view, url);
            c_onPageFinished(m_id, url);
        }

        @Override
        public void onPageStarted(WebView view, String url, Bitmap favicon)
        {
            super.onPageStarted(view, url, favicon);
            c_onPageStarted(m_id, url, favicon);
        }
    }

    private class QtAndroidWebChromeClient extends WebChromeClient
    {
        QtAndroidWebChromeClient() { super(); }
        @Override
        public void onProgressChanged(WebView view, int newProgress)
        {
            super.onProgressChanged(view, newProgress);
            c_onProgressChanged(m_id, newProgress);
        }

        @Override
        public void onReceivedIcon(WebView view, Bitmap icon)
        {
            super.onReceivedIcon(view, icon);
            c_onReceivedIcon(m_id, icon);
        }

        @Override
        public void onReceivedTitle(WebView view, String title)
        {
            super.onReceivedTitle(view, title);
            c_onReceivedTitle(m_id, title);
        }
    }

    public QtAndroidWebViewController(final Activity activity, final long id)
    {
        m_activity = activity;
        m_id = id;
        final Semaphore sem = new Semaphore(0);
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                m_webView = new WebView(m_activity);
                WebSettings webSettings = m_webView.getSettings();
                webSettings.setJavaScriptEnabled(true);
                webSettings.setBuiltInZoomControls(true);
                webSettings.setPluginState(PluginState.ON);
                m_webView.setWebViewClient((WebViewClient)new QtAndroidWebViewClient());
                m_webView.setWebChromeClient((WebChromeClient)new QtAndroidWebChromeClient());
                sem.release();
            }
        });

        try {
            sem.acquire();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void loadUrl(final String url)
    {
        if (url == null) {
            return;
        }

        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { m_webView.loadUrl(URLUtil.guessUrl(url)); }
        });
    }

    public void goBack()
    {
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { m_webView.goBack(); }
        });
    }

    public boolean canGoBack()
    {
        final boolean[] back = {false};
        final Semaphore sem = new Semaphore(0);
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { back[0] = m_webView.canGoBack(); sem.release(); }
        });

        try {
            sem.acquire();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return back[0];
    }

    public void goForward()
    {
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { m_webView.goForward(); }
        });
    }

    public boolean canGoForward()
    {
        final boolean[] forward = {false};
        final Semaphore sem = new Semaphore(0);
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { forward[0] = m_webView.canGoForward(); sem.release(); }
        });

        try {
            sem.acquire();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return forward[0];
    }

    public void stopLoading()
    {
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { m_webView.stopLoading(); }
        });
    }

    public String getTitle()
    {
        final String[] title = {""};
        final Semaphore sem = new Semaphore(0);
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { title[0] = m_webView.getTitle(); sem.release(); }
        });

        try {
            sem.acquire();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return title[0];
    }

    public String getUrl()
    {
        final String[] url = {""};
        final Semaphore sem = new Semaphore(0);
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { url[0] = m_webView.getUrl(); sem.release(); }
        });

        try {
            sem.acquire();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return url[0];
    }

    public WebView getWebView()
    {
       return m_webView;
    }
}
