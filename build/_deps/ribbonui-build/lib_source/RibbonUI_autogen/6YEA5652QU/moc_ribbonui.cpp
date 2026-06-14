/****************************************************************************
** Meta object code from reading C++ file 'ribbonui.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../ribbonui-src/lib_source/include/ribbonui.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ribbonui.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8RibbonUIE_t {};
} // unnamed namespace

template <> constexpr inline auto RibbonUI::qt_create_metaobjectdata<qt_meta_tag_ZN8RibbonUIE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RibbonUI",
        "QML.Element",
        "auto",
        "QML.Singleton",
        "true",
        "versionChanged",
        "",
        "qtVersionChanged",
        "isWin11Changed",
        "windowsSetChanged",
        "autoLoadLanguageChanged",
        "initTranslatorFinished",
        "setTranslator",
        "RibbonLocalization*",
        "translator",
        "initTranslator",
        "version",
        "qtVersion",
        "isWin11",
        "windowsSet",
        "QVariantMap",
        "autoLoadLanguage"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'versionChanged'
        QtMocHelpers::SignalData<void()>(5, 6, QMC::AccessPublic, QMetaType::Void),
        // Signal 'qtVersionChanged'
        QtMocHelpers::SignalData<void()>(7, 6, QMC::AccessPublic, QMetaType::Void),
        // Signal 'isWin11Changed'
        QtMocHelpers::SignalData<void()>(8, 6, QMC::AccessPublic, QMetaType::Void),
        // Signal 'windowsSetChanged'
        QtMocHelpers::SignalData<void()>(9, 6, QMC::AccessPublic, QMetaType::Void),
        // Signal 'autoLoadLanguageChanged'
        QtMocHelpers::SignalData<void()>(10, 6, QMC::AccessPublic, QMetaType::Void),
        // Signal 'initTranslatorFinished'
        QtMocHelpers::SignalData<void()>(11, 6, QMC::AccessPublic, QMetaType::Void),
        // Method 'setTranslator'
        QtMocHelpers::MethodData<void(RibbonLocalization *)>(12, 6, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Method 'setTranslator'
        QtMocHelpers::MethodData<void()>(12, 6, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void),
        // Method 'initTranslator'
        QtMocHelpers::MethodData<void()>(15, 6, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'version'
        QtMocHelpers::PropertyData<QString>(16, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Final, 0),
        // property 'qtVersion'
        QtMocHelpers::PropertyData<int>(17, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Final, 1),
        // property 'isWin11'
        QtMocHelpers::PropertyData<int>(18, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Final, 2),
        // property 'windowsSet'
        QtMocHelpers::PropertyData<QVariantMap>(19, 0x80000000 | 20, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::Final, 3),
        // property 'autoLoadLanguage'
        QtMocHelpers::PropertyData<bool>(21, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet | QMC::Final, 4),
    };
    QtMocHelpers::UintData qt_enums {
    };
    QtMocHelpers::UintData qt_constructors {};
    QtMocHelpers::ClassInfos qt_classinfo({
            {    1,    2 },
            {    3,    4 },
    });
    return QtMocHelpers::metaObjectData<RibbonUI, void>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums, qt_constructors, qt_classinfo);
}
Q_CONSTINIT const QMetaObject RibbonUI::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8RibbonUIE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8RibbonUIE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8RibbonUIE_t>.metaTypes,
    nullptr
} };

void RibbonUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RibbonUI *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->versionChanged(); break;
        case 1: _t->qtVersionChanged(); break;
        case 2: _t->isWin11Changed(); break;
        case 3: _t->windowsSetChanged(); break;
        case 4: _t->autoLoadLanguageChanged(); break;
        case 5: _t->initTranslatorFinished(); break;
        case 6: _t->setTranslator((*reinterpret_cast<std::add_pointer_t<RibbonLocalization*>>(_a[1]))); break;
        case 7: _t->setTranslator(); break;
        case 8: _t->initTranslator(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< RibbonLocalization* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RibbonUI::*)()>(_a, &RibbonUI::versionChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonUI::*)()>(_a, &RibbonUI::qtVersionChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonUI::*)()>(_a, &RibbonUI::isWin11Changed, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonUI::*)()>(_a, &RibbonUI::windowsSetChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonUI::*)()>(_a, &RibbonUI::autoLoadLanguageChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonUI::*)()>(_a, &RibbonUI::initTranslatorFinished, 5))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->version(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->qtVersion(); break;
        case 2: *reinterpret_cast<int*>(_v) = _t->isWin11(); break;
        case 3: *reinterpret_cast<QVariantMap*>(_v) = _t->_windowsSet; break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->autoLoadLanguage(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 3:
            if (QtMocHelpers::setProperty(_t->_windowsSet, *reinterpret_cast<QVariantMap*>(_v)))
                Q_EMIT _t->windowsSetChanged();
            break;
        case 4: _t->setAutoLoadLanguage(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *RibbonUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RibbonUI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8RibbonUIE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RibbonUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void RibbonUI::versionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RibbonUI::qtVersionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RibbonUI::isWin11Changed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RibbonUI::windowsSetChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void RibbonUI::autoLoadLanguageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void RibbonUI::initTranslatorFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
