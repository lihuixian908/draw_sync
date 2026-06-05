cmake_minimum_required(VERSION 3.16)

project(PaintEngineDemo2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets WebSockets LinguistTools Svg SvgWidgets Xml Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets WebSockets LinguistTools Svg SvgWidgets Xml Sql)

set(TS_FILES PaintEngineDemo_zh_CN.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        connect.h
        connect.cpp
        GameTimer.cpp
        GameTimer.h
        FancyButton.cpp
        FancyButton.h
        SqliteTools.cpp
        SqliteTools.h
        ChatWindow.cpp
        ChatWindow.h
        ${TS_FILES}
)

# 添加 Fancy UI 库源文件
file(GLOB FANCY_SOURCES "src/Fancy/*.cpp")
file(GLOB FANCY_HEADERS "src/Fancy/*.h")


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PaintEngineDemo
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${FANCY_SOURCES}
        ${FANCY_HEADERS}
        canvas.h canvas.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET PaintEngineDemo APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(PaintEngineDemo SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(PaintEngineDemo
            ${PROJECT_SOURCES}
            ${FANCY_SOURCES}
            ${FANCY_HEADERS}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_include_directories(PaintEngineDemo PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/include/magic_enum
)

# Windows 特定设置以支持云母效果
if(WIN32)
    target_compile_definitions(PaintEngineDemo PRIVATE UNICODE _UNICODE)
    # 设置 Windows 10/11 兼容性
    if(MSVC)
        target_compile_options(PaintEngineDemo PRIVATE /utf-8)
    endif()
endif()

target_link_libraries(PaintEngineDemo PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(PaintEngineDemo PRIVATE Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(PaintEngineDemo PRIVATE Qt${QT_VERSION_MAJOR}::Svg)
target_link_libraries(PaintEngineDemo PRIVATE Qt${QT_VERSION_MAJOR}::SvgWidgets)
target_link_libraries(PaintEngineDemo PRIVATE Qt${QT_VERSION_MAJOR}::Xml)
target_link_libraries(PaintEngineDemo PRIVATE Qt${QT_VERSION_MAJOR}::Sql)

# Windows 特定库：云母效果需要 dwmapi
if(WIN32)
    target_link_libraries(PaintEngineDemo PRIVATE dwmapi)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PaintEngineDemo)
endif()
set_target_properties(PaintEngineDemo PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS PaintEngineDemo
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PaintEngineDemo)
endif()
