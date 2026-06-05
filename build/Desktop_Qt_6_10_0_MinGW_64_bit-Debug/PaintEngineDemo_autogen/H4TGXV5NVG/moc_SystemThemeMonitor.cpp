/****************************************************************************
** Meta object code from reading C++ file 'SystemThemeMonitor.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/Fancy/SystemThemeMonitor.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SystemThemeMonitor.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN5fancy18SystemThemeMonitorE_t {};
} // unnamed namespace

template <> constexpr inline auto fancy::SystemThemeMonitor::qt_create_metaobjectdata<qt_meta_tag_ZN5fancy18SystemThemeMonitorE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "fancy::SystemThemeMonitor",
        "systemThemeChanged",
        "",
        "Theme",
        "theme",
        "systemAccentColorsChanged",
        "QMap<SysAccentPalette,QColor>",
        "colors"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'systemThemeChanged'
        QtMocHelpers::SignalData<void(Theme)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'systemAccentColorsChanged'
        QtMocHelpers::SignalData<void(QMap<SysAccentPalette,QColor>)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SystemThemeMonitor, qt_meta_tag_ZN5fancy18SystemThemeMonitorE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject fancy::SystemThemeMonitor::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5fancy18SystemThemeMonitorE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5fancy18SystemThemeMonitorE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN5fancy18SystemThemeMonitorE_t>.metaTypes,
    nullptr
} };

void fancy::SystemThemeMonitor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SystemThemeMonitor *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->systemThemeChanged((*reinterpret_cast<std::add_pointer_t<Theme>>(_a[1]))); break;
        case 1: _t->systemAccentColorsChanged((*reinterpret_cast<std::add_pointer_t<QMap<SysAccentPalette,QColor>>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SystemThemeMonitor::*)(Theme )>(_a, &SystemThemeMonitor::systemThemeChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SystemThemeMonitor::*)(QMap<SysAccentPalette,QColor> )>(_a, &SystemThemeMonitor::systemAccentColorsChanged, 1))
            return;
    }
}

const QMetaObject *fancy::SystemThemeMonitor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *fancy::SystemThemeMonitor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN5fancy18SystemThemeMonitorE_t>.strings))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int fancy::SystemThemeMonitor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void fancy::SystemThemeMonitor::systemThemeChanged(Theme _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void fancy::SystemThemeMonitor::systemAccentColorsChanged(QMap<SysAccentPalette,QColor> _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
