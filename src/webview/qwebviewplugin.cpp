// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "qwebviewplugin_p.h"

QT_BEGIN_NAMESPACE

QWebViewPlugin::QWebViewPlugin(QObject *parent) : QObject(parent)
{

}

QWebViewPlugin::~QWebViewPlugin()
{

}

void QWebViewPlugin::prepare() const
{
    // Only called for plugins that has "RequiresInit" set to true in their plugin metadata.
}

QT_END_NAMESPACE
