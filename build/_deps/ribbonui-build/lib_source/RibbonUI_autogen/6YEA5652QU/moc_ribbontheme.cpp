/****************************************************************************
** Meta object code from reading C++ file 'ribbontheme.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../ribbonui-src/lib_source/include/ribbontheme.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ribbontheme.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11RibbonThemeE_t {};
} // unnamed namespace

template <> constexpr inline auto RibbonTheme::qt_create_metaobjectdata<qt_meta_tag_ZN11RibbonThemeE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RibbonTheme",
        "QML.Element",
        "auto",
        "QML.Singleton",
        "true",
        "themeModeChanged",
        "",
        "modernStyleChanged",
        "nativeTextChanged",
        "isDarkModeChanged",
        "isDarkMode",
        "themeMode",
        "RibbonThemeType::ThemeMode",
        "modernStyle",
        "nativeText"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'themeModeChanged'
        QtMocHelpers::SignalData<void()>(5, 6, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modernStyleChanged'
        QtMocHelpers::SignalData<void()>(7, 6, QMC::AccessPublic, QMetaType::Void),
        // Signal 'nativeTextChanged'
        QtMocHelpers::SignalData<void()>(8, 6, QMC::AccessPublic, QMetaType::Void),
        // Signal 'isDarkModeChanged'
        QtMocHelpers::SignalData<void()>(9, 6, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'isDarkMode'
        QtMocHelpers::PropertyData<bool>(10, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Final, 3),
        // property 'themeMode'
        QtMocHelpers::PropertyData<RibbonThemeType::ThemeMode>(11, 0x80000000 | 12, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::Final, 0),
        // property 'modernStyle'
        QtMocHelpers::PropertyData<bool>(13, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::Final, 1),
        // property 'nativeText'
        QtMocHelpers::PropertyData<bool>(14, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::Final, 2),
    };
    QtMocHelpers::UintData qt_enums {
    };
    QtMocHelpers::UintData qt_constructors {};
    QtMocHelpers::ClassInfos qt_classinfo({
            {    1,    2 },
            {    3,    4 },
    });
    return QtMocHelpers::metaObjectData<RibbonTheme, void>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums, qt_constructors, qt_classinfo);
}
Q_CONSTINIT static const QMetaObject::SuperData qt_meta_extradata_ZN11RibbonThemeE[] = {
    QMetaObject::SuperData::link<RibbonThemeType::staticMetaObject>(),
    nullptr
};

Q_CONSTINIT const QMetaObject RibbonTheme::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11RibbonThemeE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11RibbonThemeE_t>.data,
    qt_static_metacall,
    qt_meta_extradata_ZN11RibbonThemeE,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11RibbonThemeE_t>.metaTypes,
    nullptr
} };

void RibbonTheme::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RibbonTheme *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->themeModeChanged(); break;
        case 1: _t->modernStyleChanged(); break;
        case 2: _t->nativeTextChanged(); break;
        case 3: _t->isDarkModeChanged(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RibbonTheme::*)()>(_a, &RibbonTheme::themeModeChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonTheme::*)()>(_a, &RibbonTheme::modernStyleChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonTheme::*)()>(_a, &RibbonTheme::nativeTextChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonTheme::*)()>(_a, &RibbonTheme::isDarkModeChanged, 3))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->isDarkMode(); break;
        case 1: *reinterpret_cast<RibbonThemeType::ThemeMode*>(_v) = _t->_themeMode; break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->_modernStyle; break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->_nativeText; break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1:
            if (QtMocHelpers::setProperty(_t->_themeMode, *reinterpret_cast<RibbonThemeType::ThemeMode*>(_v)))
                Q_EMIT _t->themeModeChanged();
            break;
        case 2:
            if (QtMocHelpers::setProperty(_t->_modernStyle, *reinterpret_cast<bool*>(_v)))
                Q_EMIT _t->modernStyleChanged();
            break;
        case 3:
            if (QtMocHelpers::setProperty(_t->_nativeText, *reinterpret_cast<bool*>(_v)))
                Q_EMIT _t->nativeTextChanged();
            break;
        default: break;
        }
    }
}

const QMetaObject *RibbonTheme::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RibbonTheme::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11RibbonThemeE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RibbonTheme::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void RibbonTheme::themeModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RibbonTheme::modernStyleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RibbonTheme::nativeTextChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RibbonTheme::isDarkModeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
