/****************************************************************************
** Generated QML type registration code
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <QtQml/qqml.h>
#include <QtQml/qqmlmoduleregistration.h>

#if __has_include(<definitions.h>)
#  include <definitions.h>
#endif
#if __has_include(<platformsupport.h>)
#  include <platformsupport.h>
#endif
#if __has_include(<ribbonconfig.h>)
#  include <ribbonconfig.h>
#endif
#if __has_include(<ribbonlocalization.h>)
#  include <ribbonlocalization.h>
#endif
#if __has_include(<ribbontheme.h>)
#  include <ribbontheme.h>
#endif
#if __has_include(<ribbonui.h>)
#  include <ribbonui.h>
#endif


#if !defined(QT_STATIC)
#define Q_QMLTYPE_EXPORT Q_DECL_EXPORT
#else
#define Q_QMLTYPE_EXPORT
#endif
Q_QMLTYPE_EXPORT void qml_register_types_RibbonUI()
{
    qmlRegisterModule("RibbonUI", 1, 0);
    QT_WARNING_PUSH QT_WARNING_DISABLE_DEPRECATED
    qmlRegisterTypesAndRevisions<PlatformSupport>("RibbonUI", 1);
    qmlRegisterTypesAndRevisions<RibbonConfig>("RibbonUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &RibbonIconsFilledEnum::staticMetaObject;},
            "RibbonIconsFilledEnum");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&RibbonIconsFilledEnum::staticMetaObject, "RibbonUI", 1, nullptr, &RibbonIconsFilledEnum::staticMetaObject, nullptr);
    qmlRegisterEnum<RibbonIconsFilledEnum::RibbonIcons_Filled>("RibbonIconsFilledEnum::RibbonIcons_Filled");
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &RibbonIconsRegularEnum::staticMetaObject;},
            "RibbonIconsRegularEnum");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&RibbonIconsRegularEnum::staticMetaObject, "RibbonUI", 1, nullptr, &RibbonIconsRegularEnum::staticMetaObject, nullptr);
    qmlRegisterEnum<RibbonIconsRegularEnum::RibbonIcons_Regular>("RibbonIconsRegularEnum::RibbonIcons_Regular");
    qmlRegisterTypesAndRevisions<RibbonLocalization>("RibbonUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &RibbonPopupDialogType::staticMetaObject;},
            "RibbonPopupDialogType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&RibbonPopupDialogType::staticMetaObject, "RibbonUI", 1, nullptr, &RibbonPopupDialogType::staticMetaObject, nullptr);
    qmlRegisterEnum<RibbonPopupDialogType::ButtonFlag>("RibbonPopupDialogType::ButtonFlag");
    qmlRegisterTypesAndRevisions<RibbonTheme>("RibbonUI", 1);
    {
        Q_CONSTINIT static auto metaType = QQmlPrivate::metaTypeForNamespace(
            [](const QtPrivate::QMetaTypeInterface *) {return &RibbonThemeType::staticMetaObject;},
            "RibbonThemeType");
        QMetaType(&metaType).id();
    }
    qmlRegisterNamespaceAndRevisions(&RibbonThemeType::staticMetaObject, "RibbonUI", 1, nullptr, &RibbonThemeType::staticMetaObject, nullptr);
    qmlRegisterEnum<RibbonThemeType::ThemeMode>("RibbonThemeType::ThemeMode");
    qmlRegisterTypesAndRevisions<RibbonUI>("RibbonUI", 1);
    QT_WARNING_POP
    qmlRegisterModule("RibbonUI", 1, 1);
}

static const QQmlModuleRegistration ribbonUIRegistration("RibbonUI", qml_register_types_RibbonUI);
