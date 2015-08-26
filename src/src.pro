TEMPLATE = subdirs

android|mac|winrt|qtHaveModule(webengine) {
    SUBDIRS += webview imports
    imports.depends = webview
}

android: SUBDIRS += jar
