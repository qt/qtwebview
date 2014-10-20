TEMPLATE = subdirs
SUBDIRS += webview imports
imports.depends = webview
android: SUBDIRS += jar
