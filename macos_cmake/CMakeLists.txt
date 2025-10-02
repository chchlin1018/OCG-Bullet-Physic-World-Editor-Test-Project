cmake_minimum_required(VERSION 3.16)

project(PhysicsSceneEditorMacOS VERSION 1.0.0 LANGUAGES CXX)

# 設定 C++ 標準
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# macOS 特定設定
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")

# 設定 Qt 相關選項
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 尋找 Qt6 套件
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    OpenGL
    OpenGLWidgets
    Gui
    Network
    Concurrent
)

# 尋找 OpenGL
find_package(OpenGL REQUIRED)

# 尋找其他依賴
find_package(PkgConfig REQUIRED)

# 使用 pkg-config 尋找依賴
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GLM REQUIRED glm)
pkg_check_modules(BULLET REQUIRED bullet)
pkg_check_modules(EIGEN3 REQUIRED eigen3)

# 尋找 nlohmann/json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    # 如果找不到，嘗試使用 pkg-config
    pkg_check_modules(NLOHMANN_JSON nlohmann_json)
    if(NOT NLOHMANN_JSON_FOUND)
        message(WARNING "nlohmann/json not found, will use bundled version")
        set(USE_BUNDLED_JSON ON)
    endif()
endif()

# 可選依賴
find_package(assimp QUIET)
if(assimp_FOUND)
    set(ENABLE_ASSIMP ON)
    message(STATUS "Found Assimp: ${assimp_VERSION}")
else()
    message(STATUS "Assimp not found, 3D model loading will be limited")
    set(ENABLE_ASSIMP OFF)
endif()

# 包含目錄
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../scene_format
    ${CMAKE_CURRENT_SOURCE_DIR}/../cross_platform_runner
    ${CMAKE_CURRENT_SOURCE_DIR}/../qt_editor/include
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
)

# 編譯定義
add_definitions(
    -DQT_NO_KEYWORDS
    -DQT_DEPRECATED_WARNINGS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x060000
    -DAPP_VERSION="${PROJECT_VERSION}"
    -DMACOS_BUILD
)

if(ENABLE_ASSIMP)
    add_definitions(-DENABLE_ASSIMP)
endif()

if(USE_BUNDLED_JSON)
    add_definitions(-DUSE_BUNDLED_JSON)
endif()

# 標頭檔案
set(HEADERS
    include/MacOSMainWindow.h
    include/MacOSApplication.h
    ../qt_editor/include/SceneTreeWidget.h
    ../qt_editor/include/PropertyWidget.h
    ../qt_editor/include/ViewportWidget.h
    ../scene_format/physics_scene_format.h
    ../cross_platform_runner/scene_loader.h
    ../cross_platform_runner/physics_engine.h
    ../cross_platform_runner/renderer.h
)

# 源碼檔案
set(SOURCES
    src/main.cpp
    src/MacOSMainWindow.cpp
    src/MacOSApplication.cpp
    ../scene_format/physics_scene_format.cpp
    ../cross_platform_runner/scene_loader.cpp
    ../cross_platform_runner/physics_engine.cpp
    ../cross_platform_runner/renderer.cpp
)

# 如果有 Qt 編輯器的實現檔案，也包含進來
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../qt_editor/src/SceneTreeWidget.cpp")
    list(APPEND SOURCES ../qt_editor/src/SceneTreeWidget.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../qt_editor/src/PropertyWidget.cpp")
    list(APPEND SOURCES ../qt_editor/src/PropertyWidget.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../qt_editor/src/ViewportWidget.cpp")
    list(APPEND SOURCES ../qt_editor/src/ViewportWidget.cpp)
endif()

# 資源檔案
set(RESOURCE_FILES
    resources/icons.qrc
    resources/shaders.qrc
)

# 建立可執行檔
add_executable(PhysicsSceneEditorMacOS
    ${SOURCES}
    ${HEADERS}
    ${RESOURCE_FILES}
)

# 設定 macOS 特定屬性
set_target_properties(PhysicsSceneEditorMacOS PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    MACOSX_BUNDLE_BUNDLE_NAME "Physics Scene Editor"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_IDENTIFIER "com.physicssceneeditor.macos"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.physicssceneeditor.macos"
    OUTPUT_NAME "Physics Scene Editor"
)

# 連結函式庫
target_link_libraries(PhysicsSceneEditorMacOS
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Gui
    Qt6::Network
    Qt6::Concurrent
    ${OPENGL_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${BULLET_LIBRARIES}
)

# 處理 nlohmann/json
if(nlohmann_json_FOUND)
    target_link_libraries(PhysicsSceneEditorMacOS nlohmann_json::nlohmann_json)
elseif(NLOHMANN_JSON_FOUND)
    target_link_libraries(PhysicsSceneEditorMacOS ${NLOHMANN_JSON_LIBRARIES})
    target_include_directories(PhysicsSceneEditorMacOS PRIVATE ${NLOHMANN_JSON_INCLUDE_DIRS})
endif()

# 處理 Assimp
if(ENABLE_ASSIMP)
    target_link_libraries(PhysicsSceneEditorMacOS assimp::assimp)
endif()

# macOS 系統框架
target_link_libraries(PhysicsSceneEditorMacOS
    "-framework Cocoa"
    "-framework OpenGL"
    "-framework IOKit"
    "-framework CoreVideo"
)

# 編譯器特定設定
target_compile_options(PhysicsSceneEditorMacOS PRIVATE
    ${GLFW_CFLAGS_OTHER}
    ${GLEW_CFLAGS_OTHER}
    ${BULLET_CFLAGS_OTHER}
    ${EIGEN3_CFLAGS_OTHER}
)

# 連結器設定
target_link_directories(PhysicsSceneEditorMacOS PRIVATE
    ${GLFW_LIBRARY_DIRS}
    ${GLEW_LIBRARY_DIRS}
    ${BULLET_LIBRARY_DIRS}
)

# 除錯模式設定
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(PhysicsSceneEditorMacOS PRIVATE
        QT_QML_DEBUG
        DEBUG_BUILD
    )
endif()

# 安裝設定
install(TARGETS PhysicsSceneEditorMacOS
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Qt 部署
find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS ${Qt6_DIR}/../../../bin)
if(MACDEPLOYQT_EXECUTABLE)
    add_custom_command(TARGET PhysicsSceneEditorMacOS POST_BUILD
        COMMAND ${MACDEPLOYQT_EXECUTABLE} $<TARGET_BUNDLE_DIR:PhysicsSceneEditorMacOS>
        COMMENT "Deploying Qt libraries"
    )
else()
    message(WARNING "macdeployqt not found, manual Qt deployment may be required")
endif()

# 建立 DMG（可選）
if(CREATE_DMG)
    add_custom_target(dmg
        COMMAND hdiutil create -volname "Physics Scene Editor" 
                -srcfolder $<TARGET_BUNDLE_DIR:PhysicsSceneEditorMacOS>
                -ov -format UDZO 
                "${CMAKE_BINARY_DIR}/PhysicsSceneEditor-${PROJECT_VERSION}.dmg"
        DEPENDS PhysicsSceneEditorMacOS
        COMMENT "Creating DMG installer"
    )
endif()

# 測試
if(BUILD_TESTS)
    enable_testing()
    
    find_package(Qt6 REQUIRED COMPONENTS Test)
    
    add_executable(PhysicsSceneEditorMacOSTests
        tests/test_main.cpp
        tests/test_scene_format.cpp
        tests/test_macos_integration.cpp
        ../scene_format/physics_scene_format.cpp
    )
    
    target_link_libraries(PhysicsSceneEditorMacOSTests
        Qt6::Test
        Qt6::Core
        Qt6::Widgets
        ${BULLET_LIBRARIES}
    )
    
    target_include_directories(PhysicsSceneEditorMacOSTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../scene_format
        ${BULLET_INCLUDE_DIRS}
        ${EIGEN3_INCLUDE_DIRS}
    )
    
    add_test(NAME PhysicsSceneEditorMacOSTests COMMAND PhysicsSceneEditorMacOSTests)
endif()

# 顯示配置摘要
message(STATUS "=== Physics Scene Editor macOS Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt version: ${Qt6_VERSION}")
message(STATUS "macOS deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")
message(STATUS "OpenGL found: ${OPENGL_FOUND}")
message(STATUS "GLFW found: ${GLFW_FOUND}")
message(STATUS "GLEW found: ${GLEW_FOUND}")
message(STATUS "Bullet found: ${BULLET_FOUND}")
message(STATUS "Eigen3 found: ${EIGEN3_FOUND}")
message(STATUS "Assimp support: ${ENABLE_ASSIMP}")
message(STATUS "nlohmann/json found: ${nlohmann_json_FOUND}")
message(STATUS "macdeployqt found: ${MACDEPLOYQT_EXECUTABLE}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Create DMG: ${CREATE_DMG}")
message(STATUS "==============================================")
