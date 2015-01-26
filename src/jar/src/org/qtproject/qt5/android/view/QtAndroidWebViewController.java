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

import android.webkit.ValueCallback;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.WebChromeClient;
import java.lang.Runnable;
import android.app.Activity;
import java.lang.String;
import android.webkit.WebSettings;
import android.util.Log;
import android.webkit.WebSettings.PluginState;
import android.graphics.Bitmap;
import java.util.concurrent.Semaphore;
import java.lang.reflect.Method;
import android.os.Build;

public class QtAndroidWebViewController
{
    private final Activity m_activity;
    private final long m_id;
    private WebView m_webView = null;
    private static final String TAG = "QtAndroidWebViewController";
    private volatile boolean m_onPageFinishedCalled = false;
    private volatile int m_progress = 0;
    private volatile int m_frameCount = 0;

    // API 11 methods
    private Method m_webViewOnResume = null;
    private Method m_webViewOnPause = null;
    private Method m_webSettingsSetDisplayZoomControls = null;

    // API 19 methods
    private Method m_webViewEvaluateJavascript = null;

    // Native callbacks
    private native void c_onPageFinished(long id, String url);
    private native void c_onPageStarted(long id, String url, Bitmap icon);
    private native void c_onProgressChanged(long id, int newProgress);
    private native void c_onReceivedIcon(long id, Bitmap icon);
    private native void c_onReceivedTitle(long id, String title);
    private native void c_onRunJavaScriptResult(long id, long callbackId, String result);
    private native void c_onReceivedError(long id, int errorCode, String description, String url);

    private void resetLoadingState()
    {
        m_progress = 0;
        m_onPageFinishedCalled = false;
    }

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
            m_onPageFinishedCalled = true;
            if (m_progress == 100) { // onProgressChanged() will notify Qt if we didn't finish here.
                m_frameCount = 0;
                c_onPageFinished(m_id, url);
            }
        }

        @Override
        public void onPageStarted(WebView view, String url, Bitmap favicon)
        {
            super.onPageStarted(view, url, favicon);
            if (++m_frameCount == 1) { // Only call onPageStarted for the first frame.
                m_onPageFinishedCalled = false;
                c_onPageStarted(m_id, url, favicon);
            }
        }

        @Override
        public void onReceivedError(WebView view,
                                    int errorCode,
                                    String description,
                                    String url)
        {
            super.onReceivedError(view, errorCode, description, url);
            resetLoadingState();
            c_onReceivedError(m_id, errorCode, description, url);
        }
    }

    private class QtAndroidWebChromeClient extends WebChromeClient
    {
        QtAndroidWebChromeClient() { super(); }
        @Override
        public void onProgressChanged(WebView view, int newProgress)
        {
            super.onProgressChanged(view, newProgress);
            m_progress = newProgress;
            c_onProgressChanged(m_id, newProgress);
            if (m_onPageFinishedCalled && m_progress == 100) { // Did we finish?
                m_frameCount = 0;
                c_onPageFinished(m_id, view.getUrl());
            }
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

                if (Build.VERSION.SDK_INT > 10) {
                    try {
                        m_webViewOnResume = m_webView.getClass().getMethod("onResume");
                        m_webViewOnPause = m_webView.getClass().getMethod("onPause");
                        m_webSettingsSetDisplayZoomControls = webSettings.getClass().getMethod("setDisplayZoomControls", boolean.class);
                        if (Build.VERSION.SDK_INT > 18) {
                            m_webViewEvaluateJavascript = m_webView.getClass().getMethod("evaluateJavascript",
                                                                                         String.class,
                                                                                         ValueCallback.class);
                        }
                    } catch (Exception e) { /* Do nothing */ e.printStackTrace(); }
                }

                webSettings.setJavaScriptEnabled(true);
                if (m_webSettingsSetDisplayZoomControls != null) {
                    try { m_webSettingsSetDisplayZoomControls.invoke(webSettings, false); } catch (Exception e) { e.printStackTrace(); }
                }
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

        resetLoadingState();
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { m_webView.loadUrl(url); }
        });
    }

    public void loadData(final String data, final String mimeType, final String encoding)
    {
        if (data == null)
            return;

        resetLoadingState();
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { m_webView.loadData(data, mimeType, encoding); }
        });
    }

    public void loadDataWithBaseURL(final String baseUrl,
                                    final String data,
                                    final String mimeType,
                                    final String encoding,
                                    final String historyUrl)
    {
        if (data == null)
            return;

        resetLoadingState();
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { m_webView.loadDataWithBaseURL(baseUrl, data, mimeType, encoding, historyUrl); }
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

    public void reload()
    {
        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { m_webView.reload(); }
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

    public int getProgress()
    {
        return m_progress;
    }

    public boolean isLoading()
    {
        return (m_progress != 100 && !m_onPageFinishedCalled);
    }

    public void runJavaScript(final String script, final long callbackId)
    {
        if (script == null)
            return;

        if (Build.VERSION.SDK_INT < 19 || m_webViewEvaluateJavascript == null)
            return;

        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    m_webViewEvaluateJavascript.invoke(m_webView, script, callbackId == -1 ? null :
                        new ValueCallback<String>() {
                            @Override
                            public void onReceiveValue(String result) {
                                c_onRunJavaScriptResult(m_id, callbackId, result);
                            }
                        });
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
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

    public void onPause()
    {
        if (m_webViewOnPause == null)
            return;

        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { try { m_webViewOnPause.invoke(m_webView); } catch (Exception e) { e.printStackTrace(); } }
        });
    }

    public void onResume()
    {
        if (m_webViewOnResume == null)
            return;

        m_activity.runOnUiThread(new Runnable() {
            @Override
            public void run() { try { m_webViewOnResume.invoke(m_webView); } catch (Exception e) { e.printStackTrace(); } }
        });
    }
}
