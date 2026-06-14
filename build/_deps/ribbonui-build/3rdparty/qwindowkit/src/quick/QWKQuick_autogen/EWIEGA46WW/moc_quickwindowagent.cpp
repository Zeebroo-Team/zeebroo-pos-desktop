/****************************************************************************
** Meta object code from reading C++ file 'quickwindowagent.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../ribbonui-src/3rdparty/qwindowkit/src/quick/quickwindowagent.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'quickwindowagent.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN3QWK16QuickWindowAgentE_t {};
} // unnamed namespace

template <> constexpr inline auto QWK::QuickWindowAgent::qt_create_metaobjectdata<qt_meta_tag_ZN3QWK16QuickWindowAgentE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QWK::QuickWindowAgent",
        "titleBarWidgetChanged",
        "",
        "QQuickItem*",
        "item",
        "systemButtonChanged",
        "SystemButton",
        "button",
        "setup",
        "QQuickWindow*",
        "window",
        "titleBar",
        "setTitleBar",
        "systemButton",
        "setSystemButton",
        "isHitTestVisible",
        "const QQuickItem*",
        "setHitTestVisible",
        "visible",
        "systemButtonArea",
        "setSystemButtonArea",
        "systemButtonAreaCallback",
        "ScreenRectCallback",
        "setSystemButtonAreaCallback",
        "callback"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'titleBarWidgetChanged'
        QtMocHelpers::SignalData<void(QQuickItem *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'systemButtonChanged'
        QtMocHelpers::SignalData<void(SystemButton, QQuickItem *)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 }, { 0x80000000 | 3, 4 },
        }}),
        // Method 'setup'
        QtMocHelpers::MethodData<bool(QQuickWindow *)>(8, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Method 'titleBar'
        QtMocHelpers::MethodData<QQuickItem *() const>(11, 2, QMC::AccessPublic, 0x80000000 | 3),
        // Method 'setTitleBar'
        QtMocHelpers::MethodData<void(QQuickItem *)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Method 'systemButton'
        QtMocHelpers::MethodData<QQuickItem *(SystemButton) const>(13, 2, QMC::AccessPublic, 0x80000000 | 3, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Method 'setSystemButton'
        QtMocHelpers::MethodData<void(SystemButton, QQuickItem *)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 }, { 0x80000000 | 3, 4 },
        }}),
        // Method 'isHitTestVisible'
        QtMocHelpers::MethodData<bool(const QQuickItem *) const>(15, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { 0x80000000 | 16, 4 },
        }}),
        // Method 'setHitTestVisible'
        QtMocHelpers::MethodData<void(QQuickItem *, bool)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::Bool, 18 },
        }}),
        // Method 'setHitTestVisible'
        QtMocHelpers::MethodData<void(QQuickItem *)>(17, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Method 'systemButtonArea'
        QtMocHelpers::MethodData<QQuickItem *() const>(19, 2, QMC::AccessPublic, 0x80000000 | 3),
        // Method 'setSystemButtonArea'
        QtMocHelpers::MethodData<void(QQuickItem *)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Method 'systemButtonAreaCallback'
        QtMocHelpers::MethodData<ScreenRectCallback() const>(21, 2, QMC::AccessPublic, 0x80000000 | 22),
        // Method 'setSystemButtonAreaCallback'
        QtMocHelpers::MethodData<void(const ScreenRectCallback &)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 22, 24 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QuickWindowAgent, qt_meta_tag_ZN3QWK16QuickWindowAgentE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QWK::QuickWindowAgent::staticMetaObject = { {
    QMetaObject::SuperData::link<WindowAgentBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3QWK16QuickWindowAgentE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3QWK16QuickWindowAgentE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN3QWK16QuickWindowAgentE_t>.metaTypes,
    nullptr
} };

void QWK::QuickWindowAgent::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QuickWindowAgent *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->titleBarWidgetChanged((*reinterpret_cast<std::add_pointer_t<QQuickItem*>>(_a[1]))); break;
        case 1: _t->systemButtonChanged((*reinterpret_cast<std::add_pointer_t<SystemButton>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QQuickItem*>>(_a[2]))); break;
        case 2: { bool _r = _t->setup((*reinterpret_cast<std::add_pointer_t<QQuickWindow*>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { QQuickItem* _r = _t->titleBar();
            if (_a[0]) *reinterpret_cast<QQuickItem**>(_a[0]) = std::move(_r); }  break;
        case 4: _t->setTitleBar((*reinterpret_cast<std::add_pointer_t<QQuickItem*>>(_a[1]))); break;
        case 5: { QQuickItem* _r = _t->systemButton((*reinterpret_cast<std::add_pointer_t<SystemButton>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QQuickItem**>(_a[0]) = std::move(_r); }  break;
        case 6: _t->setSystemButton((*reinterpret_cast<std::add_pointer_t<SystemButton>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QQuickItem*>>(_a[2]))); break;
        case 7: { bool _r = _t->isHitTestVisible((*reinterpret_cast<std::add_pointer_t<const QQuickItem*>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 8: _t->setHitTestVisible((*reinterpret_cast<std::add_pointer_t<QQuickItem*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 9: _t->setHitTestVisible((*reinterpret_cast<std::add_pointer_t<QQuickItem*>>(_a[1]))); break;
        case 10: { QQuickItem* _r = _t->systemButtonArea();
            if (_a[0]) *reinterpret_cast<QQuickItem**>(_a[0]) = std::move(_r); }  break;
        case 11: _t->setSystemButtonArea((*reinterpret_cast<std::add_pointer_t<QQuickItem*>>(_a[1]))); break;
        case 12: { ScreenRectCallback _r = _t->systemButtonAreaCallback();
            if (_a[0]) *reinterpret_cast<ScreenRectCallback*>(_a[0]) = std::move(_r); }  break;
        case 13: _t->setSystemButtonAreaCallback((*reinterpret_cast<std::add_pointer_t<ScreenRectCallback>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QQuickItem* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QQuickItem* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QQuickWindow* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QQuickItem* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QQuickItem* >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QQuickItem* >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QQuickItem* >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QQuickItem* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QuickWindowAgent::*)(QQuickItem * )>(_a, &QuickWindowAgent::titleBarWidgetChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QuickWindowAgent::*)(SystemButton , QQuickItem * )>(_a, &QuickWindowAgent::systemButtonChanged, 1))
            return;
    }
}

const QMetaObject *QWK::QuickWindowAgent::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QWK::QuickWindowAgent::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3QWK16QuickWindowAgentE_t>.strings))
        return static_cast<void*>(this);
    return WindowAgentBase::qt_metacast(_clname);
}

int QWK::QuickWindowAgent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = WindowAgentBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void QWK::QuickWindowAgent::titleBarWidgetChanged(QQuickItem * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QWK::QuickWindowAgent::systemButtonChanged(SystemButton _t1, QQuickItem * _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
QT_WARNING_POP
