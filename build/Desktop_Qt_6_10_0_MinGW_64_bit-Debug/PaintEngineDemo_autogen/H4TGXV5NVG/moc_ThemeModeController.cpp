/****************************************************************************
** Meta object code from reading C++ file 'ThemeModeController.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/Fancy/ThemeModeController.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ThemeModeController.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN5fancy19ThemeModeControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto fancy::ThemeModeController::qt_create_metaobjectdata<qt_meta_tag_ZN5fancy19ThemeModeControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "fancy::ThemeModeController",
        "appThemeChange",
        "",
        "Theme",
        "systemThemeChange",
        "setAppTheme",
        "theme",
        "toggleAppLight",
        "toggleAppDark",
        "followSystem",
        "toggleAppTheme",
        "onSystemThemeChanged"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'appThemeChange'
        QtMocHelpers::SignalData<void(Theme)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Signal 'systemThemeChange'
        QtMocHelpers::SignalData<void(Theme)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Slot 'setAppTheme'
        QtMocHelpers::SlotData<void(Theme)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 6 },
        }}),
        // Slot 'toggleAppLight'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleAppDark'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'followSystem'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleAppTheme'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onSystemThemeChanged'
        QtMocHelpers::SlotData<void(Theme)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 6 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ThemeModeController, qt_meta_tag_ZN5fancy19ThemeModeControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject fancy::ThemeModeController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5fancy19ThemeModeControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5fancy19ThemeModeControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN5fancy19ThemeModeControllerE_t>.metaTypes,
    nullptr
} };

void fancy::ThemeModeController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ThemeModeController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->appThemeChange((*reinterpret_cast<std::add_pointer_t<Theme>>(_a[1]))); break;
        case 1: _t->systemThemeChange((*reinterpret_cast<std::add_pointer_t<Theme>>(_a[1]))); break;
        case 2: _t->setAppTheme((*reinterpret_cast<std::add_pointer_t<Theme>>(_a[1]))); break;
        case 3: _t->toggleAppLight(); break;
        case 4: _t->toggleAppDark(); break;
        case 5: _t->followSystem(); break;
        case 6: _t->toggleAppTheme(); break;
        case 7: _t->onSystemThemeChanged((*reinterpret_cast<std::add_pointer_t<Theme>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ThemeModeController::*)(Theme )>(_a, &ThemeModeController::appThemeChange, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ThemeModeController::*)(Theme )>(_a, &ThemeModeController::systemThemeChange, 1))
            return;
    }
}

const QMetaObject *fancy::ThemeModeController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *fancy::ThemeModeController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5fancy19ThemeModeControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int fancy::ThemeModeController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void fancy::ThemeModeController::appThemeChange(Theme _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void fancy::ThemeModeController::systemThemeChange(Theme _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
