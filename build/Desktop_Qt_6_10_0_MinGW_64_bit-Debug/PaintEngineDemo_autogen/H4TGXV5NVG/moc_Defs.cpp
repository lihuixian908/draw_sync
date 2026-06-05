/****************************************************************************
** Meta object code from reading C++ file 'Defs.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/Fancy/Defs.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Defs.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN5fancyE_t {};
} // unnamed namespace

template <> constexpr inline auto fancy::qt_create_metaobjectdata<qt_meta_tag_ZN5fancyE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "fancy",
        "Theme",
        "Light",
        "Dark",
        "ColorGroups",
        "System",
        "Default",
        "Custom",
        "SysAccentPalette",
        "AccentLight3",
        "AccentLight2",
        "AccentLight1",
        "Accent",
        "AccentDark1",
        "AccentDark2",
        "AccentDark3",
        "ColorRole",
        "Window",
        "WindowText",
        "WindowBright",
        "Border",
        "Text",
        "Button",
        "ButtonBorder",
        "ButtonBottomLine",
        "RadioButtonInnerCircle",
        "Link",
        "ImportantLink",
        "LinkVisited",
        "DisEnabled",
        "DisEnabledText",
        "AppAccent",
        "SysAccent",
        "AppAccentText",
        "MaskNormal",
        "MaskHover",
        "MaskPressed",
        "MaskBrightPressed",
        "MaskSelected",
        "MaskSelectedHover",
        "MaskSelectedPressed",
        "MSWindow",
        "MSWindowSplitLine",
        "ScrollBarNormal",
        "ScrollBarFocus",
        "ScrollBarEndArrow",
        "ScrollBarSlider",
        "TextEditPanelNormal",
        "TextEditPanelFocus",
        "TextEditPanelHover",
        "TextEditBorder",
        "TextEditIndicatorLine",
        "PlaceholderText",
        "Last",
        "TitleBtnState",
        "Normal",
        "Hover",
        "Pressed",
        "PressedOut",
        "MaximizeBtnState",
        "Maximize",
        "VisualState",
        "Selected",
        "SelectedHover",
        "SelectedPressed",
        "Disabled",
        "SelectedDisabled",
        "Focus",
        "FocusHover",
        "UnKnown",
        "ControlType",
        "IndicatorCheckable",
        "Widget",
        "WidgetCheckable",
        "TextEdit",
        "IconCachePolicy",
        "NoCache",
        "PerInstance",
        "SharedCache",
        "Direction",
        "Up",
        "Down",
        "Left",
        "Right",
        "UpLeft",
        "UpRight",
        "DownLeft",
        "DownRight",
        "None"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Theme'
        QtMocHelpers::EnumData<Theme>(1, 1, QMC::EnumIsScoped).add({
            {    2, Theme::Light },
            {    3, Theme::Dark },
        }),
        // enum 'ColorGroups'
        QtMocHelpers::EnumData<ColorGroups>(4, 4, QMC::EnumIsScoped).add({
            {    5, ColorGroups::System },
            {    6, ColorGroups::Default },
            {    7, ColorGroups::Custom },
        }),
        // enum 'SysAccentPalette'
        QtMocHelpers::EnumData<SysAccentPalette>(8, 8, QMC::EnumIsScoped).add({
            {    9, SysAccentPalette::AccentLight3 },
            {   10, SysAccentPalette::AccentLight2 },
            {   11, SysAccentPalette::AccentLight1 },
            {   12, SysAccentPalette::Accent },
            {   13, SysAccentPalette::AccentDark1 },
            {   14, SysAccentPalette::AccentDark2 },
            {   15, SysAccentPalette::AccentDark3 },
        }),
        // enum 'ColorRole'
        QtMocHelpers::EnumData<ColorRole>(16, 16, QMC::EnumIsScoped).add({
            {   17, ColorRole::Window },
            {   18, ColorRole::WindowText },
            {   19, ColorRole::WindowBright },
            {   20, ColorRole::Border },
            {   21, ColorRole::Text },
            {   22, ColorRole::Button },
            {   23, ColorRole::ButtonBorder },
            {   24, ColorRole::ButtonBottomLine },
            {   25, ColorRole::RadioButtonInnerCircle },
            {   26, ColorRole::Link },
            {   27, ColorRole::ImportantLink },
            {   28, ColorRole::LinkVisited },
            {   29, ColorRole::DisEnabled },
            {   30, ColorRole::DisEnabledText },
            {   31, ColorRole::AppAccent },
            {   32, ColorRole::SysAccent },
            {   33, ColorRole::AppAccentText },
            {   34, ColorRole::MaskNormal },
            {   35, ColorRole::MaskHover },
            {   36, ColorRole::MaskPressed },
            {   37, ColorRole::MaskBrightPressed },
            {   38, ColorRole::MaskSelected },
            {   39, ColorRole::MaskSelectedHover },
            {   40, ColorRole::MaskSelectedPressed },
            {   41, ColorRole::MSWindow },
            {   42, ColorRole::MSWindowSplitLine },
            {   43, ColorRole::ScrollBarNormal },
            {   44, ColorRole::ScrollBarFocus },
            {   45, ColorRole::ScrollBarEndArrow },
            {   46, ColorRole::ScrollBarSlider },
            {   47, ColorRole::TextEditPanelNormal },
            {   48, ColorRole::TextEditPanelFocus },
            {   49, ColorRole::TextEditPanelHover },
            {   50, ColorRole::TextEditBorder },
            {   51, ColorRole::TextEditIndicatorLine },
            {   52, ColorRole::PlaceholderText },
            {   53, ColorRole::Last },
        }),
        // enum 'TitleBtnState'
        QtMocHelpers::EnumData<TitleBtnState>(54, 54, QMC::EnumIsScoped).add({
            {   55, TitleBtnState::Normal },
            {   56, TitleBtnState::Hover },
            {   57, TitleBtnState::Pressed },
            {   58, TitleBtnState::PressedOut },
        }),
        // enum 'MaximizeBtnState'
        QtMocHelpers::EnumData<MaximizeBtnState>(59, 59, QMC::EnumIsScoped).add({
            {   55, MaximizeBtnState::Normal },
            {   60, MaximizeBtnState::Maximize },
        }),
        // enum 'VisualState'
        QtMocHelpers::EnumData<VisualState>(61, 61, QMC::EnumIsScoped).add({
            {   55, VisualState::Normal },
            {   57, VisualState::Pressed },
            {   56, VisualState::Hover },
            {   62, VisualState::Selected },
            {   63, VisualState::SelectedHover },
            {   64, VisualState::SelectedPressed },
            {   65, VisualState::Disabled },
            {   66, VisualState::SelectedDisabled },
            {   67, VisualState::Focus },
            {   68, VisualState::FocusHover },
            {   53, VisualState::Last },
            {   69, VisualState::UnKnown },
        }),
        // enum 'ControlType'
        QtMocHelpers::EnumData<ControlType>(70, 70, QMC::EnumIsScoped).add({
            {   71, ControlType::IndicatorCheckable },
            {   72, ControlType::Widget },
            {   73, ControlType::WidgetCheckable },
            {   74, ControlType::TextEdit },
            {   53, ControlType::Last },
        }),
        // enum 'IconCachePolicy'
        QtMocHelpers::EnumData<IconCachePolicy>(75, 75, QMC::EnumIsScoped).add({
            {   76, IconCachePolicy::NoCache },
            {   77, IconCachePolicy::PerInstance },
            {   78, IconCachePolicy::SharedCache },
        }),
        // enum 'Direction'
        QtMocHelpers::EnumData<Direction>(79, 79, QMC::EnumIsScoped).add({
            {   80, Direction::Up },
            {   81, Direction::Down },
            {   82, Direction::Left },
            {   83, Direction::Right },
            {   84, Direction::UpLeft },
            {   85, Direction::UpRight },
            {   86, Direction::DownLeft },
            {   87, Direction::DownRight },
            {   88, Direction::None },
        }),
    };
    return QtMocHelpers::metaObjectData<void, qt_meta_tag_ZN5fancyE_t>(QMC::PropertyAccessInStaticMetaCall, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}

static constexpr auto qt_staticMetaObjectContent_ZN5fancyE =
    fancy::qt_create_metaobjectdata<qt_meta_tag_ZN5fancyE_t>();
static constexpr auto qt_staticMetaObjectStaticContent_ZN5fancyE =
    qt_staticMetaObjectContent_ZN5fancyE.staticData;
static constexpr auto qt_staticMetaObjectRelocatingContent_ZN5fancyE =
    qt_staticMetaObjectContent_ZN5fancyE.relocatingData;

Q_CONSTINIT const QMetaObject fancy::staticMetaObject = { {
    nullptr,
    qt_staticMetaObjectStaticContent_ZN5fancyE.stringdata,
    qt_staticMetaObjectStaticContent_ZN5fancyE.data,
    nullptr,
    nullptr,
    qt_staticMetaObjectRelocatingContent_ZN5fancyE.metaTypes,
    nullptr
} };

QT_WARNING_POP
