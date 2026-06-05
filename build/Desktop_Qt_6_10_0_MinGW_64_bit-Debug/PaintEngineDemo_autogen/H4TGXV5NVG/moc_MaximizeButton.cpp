/****************************************************************************
** Meta object code from reading C++ file 'MaximizeButton.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/Fancy/MaximizeButton.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MaximizeButton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.0. It"
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
struct qt_meta_tag_ZN5fancy14MaximizeButtonE_t {};
} // unnamed namespace

template <> constexpr inline auto fancy::MaximizeButton::qt_create_metaobjectdata<qt_meta_tag_ZN5fancy14MaximizeButtonE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "fancy::MaximizeButton",
        "stateChange",
        "",
        "MaximizeBtnState",
        "state",
        "enter",
        "leave",
        "mousePress",
        "mouseRelease",
        "isInSelf",
        "mouseMove",
        "loadIcon",
        "onEnter",
        "onLeave",
        "onMousePress",
        "onMouseRelease",
        "onMouseMove"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'stateChange'
        QtMocHelpers::SignalData<void(MaximizeBtnState) const>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'enter'
        QtMocHelpers::SignalData<void() const>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'leave'
        QtMocHelpers::SignalData<void() const>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'mousePress'
        QtMocHelpers::SignalData<void() const>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'mouseRelease'
        QtMocHelpers::SignalData<void(bool) const>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 9 },
        }}),
        // Signal 'mouseMove'
        QtMocHelpers::SignalData<void(bool) const>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 9 },
        }}),
        // Slot 'loadIcon'
        QtMocHelpers::SlotData<void(MaximizeBtnState) const>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'onEnter'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLeave'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMousePress'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onMouseRelease'
        QtMocHelpers::SlotData<void(bool)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 9 },
        }}),
        // Slot 'onMouseMove'
        QtMocHelpers::SlotData<void(bool)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 9 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MaximizeButton, qt_meta_tag_ZN5fancy14MaximizeButtonE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject fancy::MaximizeButton::staticMetaObject = { {
    QMetaObject::SuperData::link<UniversalTitleBarButton::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5fancy14MaximizeButtonE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5fancy14MaximizeButtonE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN5fancy14MaximizeButtonE_t>.metaTypes,
    nullptr
} };

void fancy::MaximizeButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MaximizeButton *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->stateChange((*reinterpret_cast<std::add_pointer_t<MaximizeBtnState>>(_a[1]))); break;
        case 1: _t->enter(); break;
        case 2: _t->leave(); break;
        case 3: _t->mousePress(); break;
        case 4: _t->mouseRelease((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->mouseMove((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->loadIcon((*reinterpret_cast<std::add_pointer_t<MaximizeBtnState>>(_a[1]))); break;
        case 7: _t->onEnter(); break;
        case 8: _t->onLeave(); break;
        case 9: _t->onMousePress(); break;
        case 10: _t->onMouseRelease((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->onMouseMove((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MaximizeButton::*)(MaximizeBtnState ) const>(_a, &MaximizeButton::stateChange, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MaximizeButton::*)() const>(_a, &MaximizeButton::enter, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MaximizeButton::*)() const>(_a, &MaximizeButton::leave, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MaximizeButton::*)() const>(_a, &MaximizeButton::mousePress, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MaximizeButton::*)(bool ) const>(_a, &MaximizeButton::mouseRelease, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MaximizeButton::*)(bool ) const>(_a, &MaximizeButton::mouseMove, 5))
            return;
    }
}

const QMetaObject *fancy::MaximizeButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *fancy::MaximizeButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5fancy14MaximizeButtonE_t>.strings))
        return static_cast<void*>(this);
    return UniversalTitleBarButton::qt_metacast(_clname);
}

int fancy::MaximizeButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = UniversalTitleBarButton::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void fancy::MaximizeButton::stateChange(MaximizeBtnState _t1)const
{
    QMetaObject::activate<void>(const_cast< fancy::MaximizeButton *>(this), &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void fancy::MaximizeButton::enter()const
{
    QMetaObject::activate(const_cast< fancy::MaximizeButton *>(this), &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void fancy::MaximizeButton::leave()const
{
    QMetaObject::activate(const_cast< fancy::MaximizeButton *>(this), &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void fancy::MaximizeButton::mousePress()const
{
    QMetaObject::activate(const_cast< fancy::MaximizeButton *>(this), &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void fancy::MaximizeButton::mouseRelease(bool _t1)const
{
    QMetaObject::activate<void>(const_cast< fancy::MaximizeButton *>(this), &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void fancy::MaximizeButton::mouseMove(bool _t1)const
{
    QMetaObject::activate<void>(const_cast< fancy::MaximizeButton *>(this), &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
