/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWebView module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwebview_p.h"
#include "qwebview_android_p.h"
#include <QtCore/private/qjnihelpers_p.h>
#include <QtCore/private/qjni_p.h>

#include <QtCore/qmap.h>
#include <android/bitmap.h>
#include <QtGui/qguiapplication.h>

QT_BEGIN_NAMESPACE

static const char qtAndroidWebViewControllerClass[] = "org/qtproject/qt5/android/view/QtAndroidWebViewController";

QWebViewPrivate *QWebViewPrivate::create(QWebView *q)
{
    QWebViewPrivate *result = new QAndroidWebViewPrivate(q);
    result->ensureNativeWebView();
    return result;
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
    m_viewController = QJNIObjectPrivate(qtAndroidWebViewControllerClass,
                                         "(Landroid/app/Activity;J)V",
                                         QtAndroidPrivate::activity(),
                                         m_id);
    m_webView = m_viewController.callObjectMethod("getWebView",
                                                  "()Landroid/webkit/WebView;");
    g_webViews->insert(m_id, this);
    connect(qApp, &QGuiApplication::applicationStateChanged,
            this, &QAndroidWebViewPrivate::onApplicationStateChanged);
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
                                      QJNIObjectPrivate::fromString(url).object());
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

void QAndroidWebViewPrivate::onApplicationStateChanged(Qt::ApplicationState state)
{
    if (QtAndroidPrivate::androidSdkVersion() < 11)
        return;

    if (state == Qt::ApplicationActive)
        m_viewController.callMethod<void>("onResume");
    else
        m_viewController.callMethod<void>("onPause");
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

    Q_EMIT wc->pageFinished(QJNIObjectPrivate(url).toString());
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

    Q_EMIT wc->pageStarted(QJNIObjectPrivate(url).toString());

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

    Q_EMIT wc->titleChanged(QJNIObjectPrivate(title).toString());
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

    JNIEnv *env = uenv.nativeEnvironment;

    jclass clazz = QJNIEnvironmentPrivate::findClass(qtAndroidWebViewControllerClass, env);
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

    if (env->RegisterNatives(clazz, methods, nMethods) != JNI_OK)
        return JNI_ERR;

    return JNI_VERSION_1_4;
}
