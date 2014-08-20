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

#include "qwebview_p.h"
#include "qwebview_android_p.h"
#include <QtAndroidExtras/QtAndroid>

#include <QtQuick/qquickitem.h>
#include <QtCore/qmap.h>
#include <android/bitmap.h>

QT_BEGIN_NAMESPACE

QWebViewPrivate *QWebViewPrivate::create(QWebView *q)
{
    return new QAndroidWebViewPrivate(q);
}

//static bool favIcon(JNIEnv *env, jobject icon, QImage *image)
//{
//    // TODO:
//    AndroidBitmapInfo bitmapInfo;
//    if (AndroidBitmap_getInfo(env, icon, &bitmapInfo) != ANDROID_BITMAP_RESULT_SUCCESS)
//        return false;

//    void *pixelData;
//    if (AndroidBitmap_lockPixels(env, icon, &pixelData) != ANDROID_BITMAP_RESULT_SUCCESS)
//        return false;

//    *image = QImage::fromData(static_cast<const uchar *>(pixelData), bitmapInfo.width * bitmapInfo.height);
//    AndroidBitmap_unlockPixels(env, icon);

//    return true;
//}

typedef QMap<quintptr, QWebViewPrivate *> WebViews;
Q_GLOBAL_STATIC(WebViews, g_webViews)

QAndroidWebViewPrivate::QAndroidWebViewPrivate(QWebView *q)
    : QWebViewPrivate(q)
    , m_id(reinterpret_cast<quintptr>(this))
{
    m_viewController = QAndroidJniObject("org/qtproject/qt5/android/view/QtAndroidWebViewController",
                                         "(Landroid/app/Activity;J)V",
                                         QtAndroid::androidActivity().object(),
                                         m_id);
    m_webView = m_viewController.callObjectMethod("getWebView",
                                                  "()Landroid/webkit/WebView;");
    g_webViews->insert(m_id, this);
}

QAndroidWebViewPrivate::~QAndroidWebViewPrivate()
{
    g_webViews->take(m_id);
}

QString QAndroidWebViewPrivate::getUrl() const
{
    return m_viewController.callObjectMethod<jstring>("getUrl").toString();
}

void QAndroidWebViewPrivate::loadUrl(const QString &url)
{
    m_viewController.callMethod<void>("loadUrl",
                                      "(Ljava/lang/String;)V",
                                      QAndroidJniObject::fromString(url).object());
}

bool QAndroidWebViewPrivate::canGoBack() const
{
    return m_viewController.callMethod<jboolean>("canGoBack");
}

void QAndroidWebViewPrivate::goBack() const
{
    m_viewController.callMethod<void>("goBack");
}

bool QAndroidWebViewPrivate::canGoForward() const
{
    return m_viewController.callMethod<jboolean>("canGoForward");
}

void QAndroidWebViewPrivate::goForward() const
{
    m_viewController.callMethod<void>("goForward");
}

QString QAndroidWebViewPrivate::getTitle() const
{
    return m_viewController.callObjectMethod<jstring>("getTitle").toString();
}

void QAndroidWebViewPrivate::stopLoading() const
{
    m_viewController.callMethod<void>("stopLoading");
}

void *QAndroidWebViewPrivate::nativeWebView() const
{
    return m_webView.object();
}

QT_END_NAMESPACE

static void c_onPageFinished(JNIEnv *env,
                             jobject thiz,
                             jlong id,
                             jstring url)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)
    const WebViews &wv = (*g_webViews);
    QWebViewPrivate *wc = wv[id];
    if (!wc)
        return;

    Q_EMIT wc->pageFinished(QAndroidJniObject(url).toString());
}

static void c_onPageStarted(JNIEnv *env,
                            jobject thiz,
                            jlong id,
                            jstring url,
                            jobject icon)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)
    Q_UNUSED(icon)
    const WebViews &wv = (*g_webViews);
    QWebViewPrivate *wc = wv[id];
    if (!wc)
        return;

    Q_EMIT wc->pageStarted(QAndroidJniObject(url).toString());

//    if (!icon)
//        return;

//    QImage image;
//    if (favIcon(env, icon, &image))
//        Q_EMIT wc->iconChanged(image);
}

static void c_onProgressChanged(JNIEnv *env,
                                jobject thiz,
                                jlong id,
                                jint newProgress)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)
    const WebViews &wv = (*g_webViews);
    QWebViewPrivate *wc = wv[id];
    if (!wc)
        return;

    Q_EMIT wc->progressChanged(newProgress);
}

static void c_onReceivedIcon(JNIEnv *env,
                             jobject thiz,
                             jlong id,
                             jobject icon)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)
    Q_UNUSED(id)
    Q_UNUSED(icon)

    const WebViews &wv = (*g_webViews);
    QWebViewPrivate *wc = wv[id];
    if (!wc)
        return;

    if (!icon)
        return;

//    QImage image;
//    if (favIcon(env, icon, &image))
//        Q_EMIT wc->iconChanged(image);
}

static void c_onReceivedTitle(JNIEnv *env,
                              jobject thiz,
                              jlong id,
                              jstring title)
{
    Q_UNUSED(env)
    Q_UNUSED(thiz)
    const WebViews &wv = (*g_webViews);
    QWebViewPrivate *wc = wv[id];
    if (!wc)
        return;

    Q_EMIT wc->titleChanged(QAndroidJniObject(title).toString());
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    typedef union {
        JNIEnv *nativeEnvironment;
        void *venv;
    } UnionJNIEnvToVoid;

    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
        return JNI_ERR;

    JNIEnv *jniEnv = uenv.nativeEnvironment;

    jclass clazz = jniEnv->FindClass("org/qtproject/qt5/android/view/QtAndroidWebViewController");
    if (!clazz)
        return JNI_ERR;

    JNINativeMethod methods[] = {
        {"c_onPageFinished", "(JLjava/lang/String;)V", reinterpret_cast<void *>(c_onPageFinished)},
        {"c_onPageStarted", "(JLjava/lang/String;Landroid/graphics/Bitmap;)V", reinterpret_cast<void *>(c_onPageStarted)},
        {"c_onProgressChanged", "(JI)V", reinterpret_cast<void *>(c_onProgressChanged)},
        {"c_onReceivedIcon", "(JLandroid/graphics/Bitmap;)V", reinterpret_cast<void *>(c_onReceivedIcon)},
        {"c_onReceivedTitle", "(JLjava/lang/String;)V", reinterpret_cast<void *>(c_onReceivedTitle)}
    };

    const int nMethods = sizeof(methods) / sizeof(methods[0]);

    if (jniEnv->RegisterNatives(clazz, methods, nMethods) != JNI_OK)
        return JNI_ERR;

    return JNI_VERSION_1_4;
}
