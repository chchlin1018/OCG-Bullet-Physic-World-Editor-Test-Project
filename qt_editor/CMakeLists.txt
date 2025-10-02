cmake_minimum_required(VERSION 3.16)

project(PhysicsSceneEditorQt VERSION 1.0.0 LANGUAGES CXX)

# 設定 C++ 標準
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    LinguistTools
)

# 尋找 OpenGL
find_package(OpenGL REQUIRED)

# 尋找其他依賴
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Bullet REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(nlohmann_json REQUIRED)

# 可選依賴
find_package(assimp QUIET)
find_package(CUDA QUIET)

# 包含目錄
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../scene_format
    ${CMAKE_CURRENT_SOURCE_DIR}/../cross_platform_runner
    ${CMAKE_CURRENT_SOURCE_DIR}/../ogc_integration
)

# 標頭檔案
set(HEADERS
    include/MainWindow.h
    include/SceneTreeWidget.h
    include/PropertyWidget.h
    include/ViewportWidget.h
    include/MaterialWidget.h
    include/OutputWidget.h
    include/ToolboxWidget.h
    include/PreferencesDialog.h
    include/AboutDialog.h
    include/NewSceneDialog.h
    include/SimulationSettingsDialog.h
    include/MaterialEditorDialog.h
    include/SceneStatisticsDialog.h
    include/ExportDialog.h
    include/ImportDialog.h
)

# 源碼檔案
set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/SceneTreeWidget.cpp
    src/PropertyWidget.cpp
    src/ViewportWidget.cpp
    src/MaterialWidget.cpp
    src/OutputWidget.cpp
    src/ToolboxWidget.cpp
    src/PreferencesDialog.cpp
    src/AboutDialog.cpp
    src/NewSceneDialog.cpp
    src/SimulationSettingsDialog.cpp
    src/MaterialEditorDialog.cpp
    src/SceneStatisticsDialog.cpp
    src/ExportDialog.cpp
    src/ImportDialog.cpp
)

# UI 檔案
set(UI_FILES
    ui/MainWindow.ui
    ui/PreferencesDialog.ui
    ui/AboutDialog.ui
    ui/NewSceneDialog.ui
    ui/SimulationSettingsDialog.ui
    ui/MaterialEditorDialog.ui
    ui/SceneStatisticsDialog.ui
    ui/ExportDialog.ui
    ui/ImportDialog.ui
)

# 資源檔案
set(RESOURCE_FILES
    resources/icons.qrc
    resources/shaders.qrc
    resources/textures.qrc
    resources/translations.qrc
)

# 翻譯檔案
set(TS_FILES
    translations/physics_scene_editor_zh_TW.ts
    translations/physics_scene_editor_zh_CN.ts
    translations/physics_scene_editor_en_US.ts
    translations/physics_scene_editor_ja_JP.ts
)

# 場景格式函式庫
add_library(SceneFormat STATIC
    ../scene_format/physics_scene_format.cpp
)

target_include_directories(SceneFormat PUBLIC
    ../scene_format
)

target_link_libraries(SceneFormat
    nlohmann_json::nlohmann_json
)

# 跨平台執行器函式庫
add_library(CrossPlatformRunner STATIC
    ../cross_platform_runner/scene_loader.cpp
    ../cross_platform_runner/physics_engine.cpp
    ../cross_platform_runner/renderer.cpp
)

target_include_directories(CrossPlatformRunner PUBLIC
    ../cross_platform_runner
    ../scene_format
)

target_link_libraries(CrossPlatformRunner
    SceneFormat
    ${BULLET_LIBRARIES}
    ${OPENGL_LIBRARIES}
    glfw
    glm::glm
    Eigen3::Eigen
)

# OGC 整合函式庫
add_library(OGCIntegration STATIC
    ../ogc_integration/ogc_contact_solver.cpp
    ../ogc_integration/hybrid_physics_world.cpp
)

target_include_directories(OGCIntegration PUBLIC
    ../ogc_integration
    ../scene_format
)

target_link_libraries(OGCIntegration
    SceneFormat
    CrossPlatformRunner
    ${BULLET_LIBRARIES}
    Eigen3::Eigen
)

# CUDA 支援
if(CUDA_FOUND AND ENABLE_CUDA)
    enable_language(CUDA)
    
    add_library(CUDAKernels STATIC
        ../ogc_integration/cuda_kernels.cu
        ../ogc_integration/cuda_memory_manager.cpp
    )
    
    target_include_directories(CUDAKernels PUBLIC
        ../ogc_integration
    )
    
    target_link_libraries(OGCIntegration CUDAKernels)
    target_compile_definitions(OGCIntegration PRIVATE ENABLE_CUDA)
endif()

# FBX 支援
if(assimp_FOUND OR ENABLE_FBX)
    if(ENABLE_FBX AND FBX_SDK_ROOT)
        # 使用 FBX SDK
        find_library(FBX_LIBRARY
            NAMES fbxsdk
            PATHS ${FBX_SDK_ROOT}/lib
            PATH_SUFFIXES vs2019/x64/release vs2019/x64/debug
        )
        
        target_include_directories(CrossPlatformRunner PRIVATE
            ${FBX_SDK_ROOT}/include
        )
        
        target_link_libraries(CrossPlatformRunner ${FBX_LIBRARY})
        target_compile_definitions(CrossPlatformRunner PRIVATE ENABLE_FBX)
    else()
        # 使用 Assimp
        target_link_libraries(CrossPlatformRunner assimp::assimp)
        target_compile_definitions(CrossPlatformRunner PRIVATE ENABLE_ASSIMP)
    endif()
endif()

# 建立翻譯檔案
qt6_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

# 主要可執行檔
qt6_add_executable(PhysicsSceneEditorQt
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCE_FILES}
    ${QM_FILES}
)

# 設定目標屬性
set_target_properties(PhysicsSceneEditorQt PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    OUTPUT_NAME "Physics Scene Editor"
)

# 連結函式庫
target_link_libraries(PhysicsSceneEditorQt
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Gui
    Qt6::Network
    Qt6::Concurrent
    SceneFormat
    CrossPlatformRunner
    OGCIntegration
    ${OPENGL_LIBRARIES}
)

# 編譯定義
target_compile_definitions(PhysicsSceneEditorQt PRIVATE
    QT_NO_KEYWORDS
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
    APP_VERSION="${PROJECT_VERSION}"
)

# 除錯模式設定
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(PhysicsSceneEditorQt PRIVATE
        QT_QML_DEBUG
        DEBUG_BUILD
    )
endif()

# 平台特定設定
if(WIN32)
    # Windows 特定設定
    target_compile_definitions(PhysicsSceneEditorQt PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    # 設定 Windows 圖示
    target_sources(PhysicsSceneEditorQt PRIVATE resources/app.rc)
    
elseif(APPLE)
    # macOS 特定設定
    set_target_properties(PhysicsSceneEditorQt PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/Info.plist.in
        MACOSX_BUNDLE_BUNDLE_NAME "Physics Scene Editor"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_IDENTIFIER "com.physics-scene-editor.qt"
    )
    
elseif(UNIX)
    # Linux 特定設定
    install(FILES resources/physics-scene-editor.desktop
            DESTINATION share/applications)
    install(FILES resources/physics-scene-editor.png
            DESTINATION share/pixmaps)
endif()

# 安裝設定
install(TARGETS PhysicsSceneEditorQt
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Qt 部署
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET PhysicsSceneEditorQt POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:PhysicsSceneEditorQt>
            COMMENT "Deploying Qt libraries"
        )
    endif()
elseif(APPLE)
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(MACDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET PhysicsSceneEditorQt POST_BUILD
            COMMAND ${MACDEPLOYQT_EXECUTABLE} $<TARGET_BUNDLE_DIR:PhysicsSceneEditorQt>
            COMMENT "Deploying Qt libraries"
        )
    endif()
endif()

# 測試
if(BUILD_TESTS)
    enable_testing()
    
    find_package(Qt6 REQUIRED COMPONENTS Test)
    
    add_executable(PhysicsSceneEditorQtTests
        tests/test_main.cpp
        tests/test_scene_format.cpp
        tests/test_property_widget.cpp
        tests/test_viewport_widget.cpp
    )
    
    target_link_libraries(PhysicsSceneEditorQtTests
        Qt6::Test
        Qt6::Core
        Qt6::Widgets
        SceneFormat
        CrossPlatformRunner
    )
    
    add_test(NAME PhysicsSceneEditorQtTests COMMAND PhysicsSceneEditorQtTests)
endif()

# 文件生成
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()

# 打包設定
set(CPACK_PACKAGE_NAME "Physics Scene Editor Qt")
set(CPACK_PACKAGE_VENDOR "Physics Scene Editor Team")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "跨平台物理場景編輯器")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "Physics Scene Editor Qt")
    set(CPACK_NSIS_PACKAGE_NAME "Physics Scene Editor Qt")
    set(CPACK_NSIS_CONTACT "support@physics-scene-editor.com")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://physics-scene-editor.com")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_FORMAT "UDBZ")
    set(CPACK_DMG_VOLUME_NAME "Physics Scene Editor Qt")
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Physics Scene Editor Team")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6core6, libqt6widgets6, libqt6opengl6")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase, qt6-qtbase-gui")
endif()

include(CPack)

# 顯示配置摘要
message(STATUS "=== Physics Scene Editor Qt Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt version: ${Qt6_VERSION}")
message(STATUS "OpenGL found: ${OPENGL_FOUND}")
message(STATUS "CUDA support: ${CUDA_FOUND}")
message(STATUS "FBX support: ${ENABLE_FBX}")
message(STATUS "Assimp support: ${assimp_FOUND}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Build docs: ${BUILD_DOCS}")
message(STATUS "==============================================")
