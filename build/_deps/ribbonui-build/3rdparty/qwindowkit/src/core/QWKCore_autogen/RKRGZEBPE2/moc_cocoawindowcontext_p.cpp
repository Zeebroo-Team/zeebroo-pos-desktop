/****************************************************************************
** Meta object code from reading C++ file 'cocoawindowcontext_p.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../ribbonui-src/3rdparty/qwindowkit/src/core/contexts/cocoawindowcontext_p.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cocoawindowcontext_p.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN3QWK18CocoaWindowContextE_t {};
} // unnamed namespace

template <> constexpr inline auto QWK::CocoaWindowContext::qt_create_metaobjectdata<qt_meta_tag_ZN3QWK18CocoaWindowContextE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QWK::CocoaWindowContext"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CocoaWindowContext, qt_meta_tag_ZN3QWK18CocoaWindowContextE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QWK::CocoaWindowContext::staticMetaObject = { {
    QMetaObject::SuperData::link<AbstractWindowContext::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3QWK18CocoaWindowContextE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3QWK18CocoaWindowContextE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN3QWK18CocoaWindowContextE_t>.metaTypes,
    nullptr
} };

void QWK::CocoaWindowContext::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CocoaWindowContext *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *QWK::CocoaWindowContext::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QWK::CocoaWindowContext::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN3QWK18CocoaWindowContextE_t>.strings))
        return static_cast<void*>(this);
    return AbstractWindowContext::qt_metacast(_clname);
}

int QWK::CocoaWindowContext::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractWindowContext::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
