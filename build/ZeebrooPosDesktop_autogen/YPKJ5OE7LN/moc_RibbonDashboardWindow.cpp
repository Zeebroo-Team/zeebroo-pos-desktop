/****************************************************************************
** Meta object code from reading C++ file 'RibbonDashboardWindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../pos-desktop/src/ui/RibbonDashboardWindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RibbonDashboardWindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN3pos21RibbonDashboardWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto pos::RibbonDashboardWindow::qt_create_metaobjectdata<qt_meta_tag_ZN3pos21RibbonDashboardWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "pos::RibbonDashboardWindow",
        "businessNameChanged",
        "",
        "billCreated",
        "billName",
        "billError",
        "message",
        "loansLoaded",
        "QVariantList",
        "loans",
        "accountsLoaded",
        "accounts",
        "billAssignmentTargetsLoaded",
        "QVariantMap",
        "targets",
        "billAssignmentTargetsError",
        "openPos",
        "submitBill",
        "data",
        "fetchLoans",
        "fetchAccounts",
        "fetchBillAssignmentTargets",
        "businessName"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'businessNameChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'billCreated'
        QtMocHelpers::SignalData<void(const QString &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'billError'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'loansLoaded'
        QtMocHelpers::SignalData<void(const QVariantList &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'accountsLoaded'
        QtMocHelpers::SignalData<void(const QVariantList &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 11 },
        }}),
        // Signal 'billAssignmentTargetsLoaded'
        QtMocHelpers::SignalData<void(const QVariantMap &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Signal 'billAssignmentTargetsError'
        QtMocHelpers::SignalData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Method 'openPos'
        QtMocHelpers::MethodData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'submitBill'
        QtMocHelpers::MethodData<void(const QVariantMap &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 18 },
        }}),
        // Method 'fetchLoans'
        QtMocHelpers::MethodData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'fetchAccounts'
        QtMocHelpers::MethodData<void()>(20, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'fetchBillAssignmentTargets'
        QtMocHelpers::MethodData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'businessName'
        QtMocHelpers::PropertyData<QString>(22, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RibbonDashboardWindow, qt_meta_tag_ZN3pos21RibbonDashboardWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject pos::RibbonDashboardWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3pos21RibbonDashboardWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3pos21RibbonDashboardWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN3pos21RibbonDashboardWindowE_t>.metaTypes,
    nullptr
} };

void pos::RibbonDashboardWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RibbonDashboardWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->businessNameChanged(); break;
        case 1: _t->billCreated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->billError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->loansLoaded((*reinterpret_cast<std::add_pointer_t<QVariantList>>(_a[1]))); break;
        case 4: _t->accountsLoaded((*reinterpret_cast<std::add_pointer_t<QVariantList>>(_a[1]))); break;
        case 5: _t->billAssignmentTargetsLoaded((*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 6: _t->billAssignmentTargetsError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->openPos(); break;
        case 8: _t->submitBill((*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 9: _t->fetchLoans(); break;
        case 10: _t->fetchAccounts(); break;
        case 11: _t->fetchBillAssignmentTargets(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RibbonDashboardWindow::*)()>(_a, &RibbonDashboardWindow::businessNameChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonDashboardWindow::*)(const QString & )>(_a, &RibbonDashboardWindow::billCreated, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonDashboardWindow::*)(const QString & )>(_a, &RibbonDashboardWindow::billError, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonDashboardWindow::*)(const QVariantList & )>(_a, &RibbonDashboardWindow::loansLoaded, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonDashboardWindow::*)(const QVariantList & )>(_a, &RibbonDashboardWindow::accountsLoaded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonDashboardWindow::*)(const QVariantMap & )>(_a, &RibbonDashboardWindow::billAssignmentTargetsLoaded, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (RibbonDashboardWindow::*)(const QString & )>(_a, &RibbonDashboardWindow::billAssignmentTargetsError, 6))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->businessName(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setBusinessName(*reinterpret_cast<QString*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *pos::RibbonDashboardWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *pos::RibbonDashboardWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3pos21RibbonDashboardWindowE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int pos::RibbonDashboardWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void pos::RibbonDashboardWindow::businessNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void pos::RibbonDashboardWindow::billCreated(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void pos::RibbonDashboardWindow::billError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void pos::RibbonDashboardWindow::loansLoaded(const QVariantList & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void pos::RibbonDashboardWindow::accountsLoaded(const QVariantList & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void pos::RibbonDashboardWindow::billAssignmentTargetsLoaded(const QVariantMap & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void pos::RibbonDashboardWindow::billAssignmentTargetsError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
