QT       += core gui network
QT += mqtt sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SDK/DIY/QSimpleSliderNavigation.cpp \
    SDK/MaterialSDK/components/layouts/qtmaterialsnackbarlayout.cpp \
    SDK/MaterialSDK/components/lib/qtmaterialcheckable.cpp \
    SDK/MaterialSDK/components/lib/qtmaterialcheckable_internal.cpp \
    SDK/MaterialSDK/components/lib/qtmaterialoverlaywidget.cpp \
    SDK/MaterialSDK/components/lib/qtmaterialripple.cpp \
    SDK/MaterialSDK/components/lib/qtmaterialrippleoverlay.cpp \
    SDK/MaterialSDK/components/lib/qtmaterialstatetransition.cpp \
    SDK/MaterialSDK/components/lib/qtmaterialstyle.cpp \
    SDK/MaterialSDK/components/lib/qtmaterialtheme.cpp \
    SDK/MaterialSDK/components/qtmaterialappbar.cpp \
    SDK/MaterialSDK/components/qtmaterialautocomplete.cpp \
    SDK/MaterialSDK/components/qtmaterialautocomplete_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialavatar.cpp \
    SDK/MaterialSDK/components/qtmaterialbadge.cpp \
    SDK/MaterialSDK/components/qtmaterialcheckbox.cpp \
    SDK/MaterialSDK/components/qtmaterialcircularprogress.cpp \
    SDK/MaterialSDK/components/qtmaterialcircularprogress_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialdialog.cpp \
    SDK/MaterialSDK/components/qtmaterialdialog_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialdrawer.cpp \
    SDK/MaterialSDK/components/qtmaterialdrawer_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialfab.cpp \
    SDK/MaterialSDK/components/qtmaterialflatbutton.cpp \
    SDK/MaterialSDK/components/qtmaterialflatbutton_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialiconbutton.cpp \
    SDK/MaterialSDK/components/qtmateriallist.cpp \
    SDK/MaterialSDK/components/qtmateriallistitem.cpp \
    SDK/MaterialSDK/components/qtmaterialmenu.cpp \
    SDK/MaterialSDK/components/qtmaterialmenu_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialpaper.cpp \
    SDK/MaterialSDK/components/qtmaterialprogress.cpp \
    SDK/MaterialSDK/components/qtmaterialprogress_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialradiobutton.cpp \
    SDK/MaterialSDK/components/qtmaterialraisedbutton.cpp \
    SDK/MaterialSDK/components/qtmaterialscrollbar.cpp \
    SDK/MaterialSDK/components/qtmaterialscrollbar_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialslider.cpp \
    SDK/MaterialSDK/components/qtmaterialslider_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialsnackbar.cpp \
    SDK/MaterialSDK/components/qtmaterialsnackbar_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialtable.cpp \
    SDK/MaterialSDK/components/qtmaterialtabs.cpp \
    SDK/MaterialSDK/components/qtmaterialtabs_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialtextfield.cpp \
    SDK/MaterialSDK/components/qtmaterialtextfield_internal.cpp \
    SDK/MaterialSDK/components/qtmaterialtoggle.cpp \
    SDK/MaterialSDK/components/qtmaterialtoggle_internal.cpp \
    aiot_calc.cpp \
    global.cpp \
    main.cpp \
    qlabelstatus.cpp \
    widget.cpp

HEADERS += \
    SDK/DIY/QSimpleSliderNavigation.h \
    SDK/MaterialSDK/components/layouts/qtmaterialsnackbarlayout.h \
    SDK/MaterialSDK/components/layouts/qtmaterialsnackbarlayout_p.h \
    SDK/MaterialSDK/components/lib/qtmaterialcheckable.h \
    SDK/MaterialSDK/components/lib/qtmaterialcheckable_internal.h \
    SDK/MaterialSDK/components/lib/qtmaterialcheckable_p.h \
    SDK/MaterialSDK/components/lib/qtmaterialoverlaywidget.h \
    SDK/MaterialSDK/components/lib/qtmaterialripple.h \
    SDK/MaterialSDK/components/lib/qtmaterialrippleoverlay.h \
    SDK/MaterialSDK/components/lib/qtmaterialstatetransition.h \
    SDK/MaterialSDK/components/lib/qtmaterialstatetransitionevent.h \
    SDK/MaterialSDK/components/lib/qtmaterialstyle.h \
    SDK/MaterialSDK/components/lib/qtmaterialstyle_p.h \
    SDK/MaterialSDK/components/lib/qtmaterialtheme.h \
    SDK/MaterialSDK/components/lib/qtmaterialtheme_p.h \
    SDK/MaterialSDK/components/qtmaterialappbar.h \
    SDK/MaterialSDK/components/qtmaterialappbar_p.h \
    SDK/MaterialSDK/components/qtmaterialautocomplete.h \
    SDK/MaterialSDK/components/qtmaterialautocomplete_internal.h \
    SDK/MaterialSDK/components/qtmaterialautocomplete_p.h \
    SDK/MaterialSDK/components/qtmaterialavatar.h \
    SDK/MaterialSDK/components/qtmaterialavatar_p.h \
    SDK/MaterialSDK/components/qtmaterialbadge.h \
    SDK/MaterialSDK/components/qtmaterialbadge_p.h \
    SDK/MaterialSDK/components/qtmaterialcheckbox.h \
    SDK/MaterialSDK/components/qtmaterialcheckbox_p.h \
    SDK/MaterialSDK/components/qtmaterialcircularprogress.h \
    SDK/MaterialSDK/components/qtmaterialcircularprogress_internal.h \
    SDK/MaterialSDK/components/qtmaterialcircularprogress_p.h \
    SDK/MaterialSDK/components/qtmaterialdialog.h \
    SDK/MaterialSDK/components/qtmaterialdialog_internal.h \
    SDK/MaterialSDK/components/qtmaterialdialog_p.h \
    SDK/MaterialSDK/components/qtmaterialdrawer.h \
    SDK/MaterialSDK/components/qtmaterialdrawer_internal.h \
    SDK/MaterialSDK/components/qtmaterialdrawer_p.h \
    SDK/MaterialSDK/components/qtmaterialfab.h \
    SDK/MaterialSDK/components/qtmaterialfab_p.h \
    SDK/MaterialSDK/components/qtmaterialflatbutton.h \
    SDK/MaterialSDK/components/qtmaterialflatbutton_internal.h \
    SDK/MaterialSDK/components/qtmaterialflatbutton_p.h \
    SDK/MaterialSDK/components/qtmaterialiconbutton.h \
    SDK/MaterialSDK/components/qtmaterialiconbutton_p.h \
    SDK/MaterialSDK/components/qtmateriallist.h \
    SDK/MaterialSDK/components/qtmateriallist_p.h \
    SDK/MaterialSDK/components/qtmateriallistitem.h \
    SDK/MaterialSDK/components/qtmateriallistitem_p.h \
    SDK/MaterialSDK/components/qtmaterialmenu.h \
    SDK/MaterialSDK/components/qtmaterialmenu_internal.h \
    SDK/MaterialSDK/components/qtmaterialmenu_p.h \
    SDK/MaterialSDK/components/qtmaterialpaper.h \
    SDK/MaterialSDK/components/qtmaterialpaper_p.h \
    SDK/MaterialSDK/components/qtmaterialprogress.h \
    SDK/MaterialSDK/components/qtmaterialprogress_internal.h \
    SDK/MaterialSDK/components/qtmaterialprogress_p.h \
    SDK/MaterialSDK/components/qtmaterialradiobutton.h \
    SDK/MaterialSDK/components/qtmaterialradiobutton_p.h \
    SDK/MaterialSDK/components/qtmaterialraisedbutton.h \
    SDK/MaterialSDK/components/qtmaterialraisedbutton_p.h \
    SDK/MaterialSDK/components/qtmaterialscrollbar.h \
    SDK/MaterialSDK/components/qtmaterialscrollbar_internal.h \
    SDK/MaterialSDK/components/qtmaterialscrollbar_p.h \
    SDK/MaterialSDK/components/qtmaterialslider.h \
    SDK/MaterialSDK/components/qtmaterialslider_internal.h \
    SDK/MaterialSDK/components/qtmaterialslider_p.h \
    SDK/MaterialSDK/components/qtmaterialsnackbar.h \
    SDK/MaterialSDK/components/qtmaterialsnackbar_internal.h \
    SDK/MaterialSDK/components/qtmaterialsnackbar_p.h \
    SDK/MaterialSDK/components/qtmaterialtable.h \
    SDK/MaterialSDK/components/qtmaterialtable_p.h \
    SDK/MaterialSDK/components/qtmaterialtabs.h \
    SDK/MaterialSDK/components/qtmaterialtabs_internal.h \
    SDK/MaterialSDK/components/qtmaterialtabs_p.h \
    SDK/MaterialSDK/components/qtmaterialtextfield.h \
    SDK/MaterialSDK/components/qtmaterialtextfield_internal.h \
    SDK/MaterialSDK/components/qtmaterialtextfield_p.h \
    SDK/MaterialSDK/components/qtmaterialtoggle.h \
    SDK/MaterialSDK/components/qtmaterialtoggle_internal.h \
    SDK/MaterialSDK/components/qtmaterialtoggle_p.h \
    aiot_calc.h \
    global.h \
    qlabelstatus.h \
    widget.h

FORMS += \
    widget.ui

TRANSLATIONS += \
    IotSys_zh_CN.ts

# SDK

LIBS += $$PWD/SDK/MaterialSDK/staticlib/libcomponents.a
INCLUDEPATH += $$PWD/SDK/MaterialSDK/components
PRE_TARGETDEPS += $$PWD/SDK/MaterialSDK/staticlib/libcomponents.a

# mqtt SDK

LIBS += -lpthread -luuid

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SUBDIRS += \
    SDK/MaterialSDK/components/components.pro \
    SDK/components/components.pro

DISTFILES += \
    SDK/libQt5Mqtt.so.5.15.2 \
    SDK/qt_lib_mqtt.pri \
    SDK/qt_lib_mqtt_private.pri
