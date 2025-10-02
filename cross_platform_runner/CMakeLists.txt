cmake_minimum_required(VERSION 3.16)

# 專案設定
project(PhysicsSceneRunner 
    VERSION 1.0.0
    DESCRIPTION "Cross-platform Physics Scene Runner with OGC and Bullet Physics"
    LANGUAGES CXX C
)

# C++ 標準設定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 建置類型設定
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 編譯器特定設定
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 平台檢測
if(WIN32)
    add_compile_definitions(PLATFORM_WINDOWS)
elseif(APPLE)
    add_compile_definitions(PLATFORM_MACOS)
elseif(UNIX)
    add_compile_definitions(PLATFORM_LINUX)
endif()

# 尋找必要的套件
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(PkgConfig REQUIRED)

# GLEW
find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
    message(FATAL_ERROR "GLEW not found. Please install GLEW development libraries.")
endif()

# Bullet Physics
find_package(Bullet REQUIRED)
if(NOT BULLET_FOUND)
    message(FATAL_ERROR "Bullet Physics not found. Please install Bullet Physics development libraries.")
endif()

# nlohmann/json
find_package(nlohmann_json REQUIRED)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann/json not found via find_package, trying pkg-config...")
    pkg_check_modules(NLOHMANN_JSON REQUIRED nlohmann_json)
endif()

# Eigen3 (用於數學運算)
find_package(Eigen3 REQUIRED)
if(NOT Eigen3_FOUND)
    message(FATAL_ERROR "Eigen3 not found. Please install Eigen3 development libraries.")
endif()

# CUDA (可選)
option(ENABLE_CUDA "Enable CUDA support for GPU acceleration" ON)
if(ENABLE_CUDA)
    find_package(CUDA QUIET)
    if(CUDA_FOUND)
        enable_language(CUDA)
        add_compile_definitions(ENABLE_CUDA)
        message(STATUS "CUDA support enabled")
    else()
        message(STATUS "CUDA not found, disabling CUDA support")
        set(ENABLE_CUDA OFF)
    endif()
endif()

# FBX SDK (可選)
option(ENABLE_FBX "Enable FBX support for 3D model loading" OFF)
if(ENABLE_FBX)
    find_path(FBX_INCLUDE_DIR fbxsdk.h
        PATHS
        $ENV{FBX_SDK_ROOT}/include
        /usr/local/include
        /usr/include
        DOC "FBX SDK include directory"
    )
    
    find_library(FBX_LIBRARY
        NAMES fbxsdk libfbxsdk
        PATHS
        $ENV{FBX_SDK_ROOT}/lib
        /usr/local/lib
        /usr/lib
        DOC "FBX SDK library"
    )
    
    if(FBX_INCLUDE_DIR AND FBX_LIBRARY)
        add_compile_definitions(ENABLE_FBX)
        message(STATUS "FBX SDK support enabled")
    else()
        message(STATUS "FBX SDK not found, disabling FBX support")
        set(ENABLE_FBX OFF)
    endif()
endif()

# Assimp (作為 FBX 的替代方案)
if(NOT ENABLE_FBX)
    find_package(assimp REQUIRED)
    if(assimp_FOUND)
        add_compile_definitions(ENABLE_ASSIMP)
        message(STATUS "Assimp support enabled as FBX alternative")
    endif()
endif()

# 包含目錄
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../scene_format
    ${CMAKE_CURRENT_SOURCE_DIR}/../ogc_integration
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
)

# 源檔案
set(SOURCES
    main.cpp
    physics_engine.cpp
    renderer.cpp
    scene_loader.cpp
    input_manager.cpp
    performance_monitor.cpp
    ../scene_format/physics_scene_format.cpp
)

# 標頭檔
set(HEADERS
    physics_engine.h
    renderer.h
    scene_loader.h
    input_manager.h
    performance_monitor.h
    ../scene_format/physics_scene_format.h
)

# OGC 整合源檔案
set(OGC_SOURCES
    ../ogc_integration/ogc_contact_solver.cpp
    ../ogc_integration/ogc_contact_model.cpp
    ../ogc_integration/hybrid_physics_world.cpp
)

set(OGC_HEADERS
    ../ogc_integration/ogc_contact_solver.h
    ../ogc_integration/ogc_contact_model.h
    ../ogc_integration/hybrid_physics_world.h
)

# CUDA 源檔案 (如果啟用)
if(ENABLE_CUDA)
    set(CUDA_SOURCES
        ../ogc_integration/cuda_ogc_solver.cu
        ../ogc_integration/cuda_memory_manager.cu
    )
endif()

# FBX 源檔案 (如果啟用)
if(ENABLE_FBX)
    list(APPEND SOURCES fbx_loader.cpp)
    list(APPEND HEADERS fbx_loader.h)
endif()

# 建立可執行檔
if(ENABLE_CUDA)
    add_executable(${PROJECT_NAME} 
        ${SOURCES} 
        ${HEADERS}
        ${OGC_SOURCES}
        ${OGC_HEADERS}
        ${CUDA_SOURCES}
    )
    
    # CUDA 特定設定
    set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_ARCHITECTURES 50 60 70 75 80)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
else()
    add_executable(${PROJECT_NAME} 
        ${SOURCES} 
        ${HEADERS}
        ${OGC_SOURCES}
        ${OGC_HEADERS}
    )
endif()

# 連結函式庫
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    glfw
    ${BULLET_LIBRARIES}
    Eigen3::Eigen
)

# nlohmann/json 連結
if(nlohmann_json_FOUND)
    target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)
else()
    target_include_directories(${PROJECT_NAME} PRIVATE ${NLOHMANN_JSON_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${NLOHMANN_JSON_LIBRARIES})
endif()

# CUDA 連結 (如果啟用)
if(ENABLE_CUDA)
    target_link_libraries(${PROJECT_NAME} ${CUDA_LIBRARIES})
endif()

# FBX 連結 (如果啟用)
if(ENABLE_FBX)
    target_include_directories(${PROJECT_NAME} PRIVATE ${FBX_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${FBX_LIBRARY})
endif()

# Assimp 連結 (如果啟用)
if(NOT ENABLE_FBX AND assimp_FOUND)
    target_link_libraries(${PROJECT_NAME} assimp::assimp)
endif()

# 平台特定連結
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 gdi32 user32 kernel32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY}
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} dl pthread)
endif()

# 編譯定義
target_compile_definitions(${PROJECT_NAME} PRIVATE
    GLFW_INCLUDE_NONE
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)

# 除錯模式特定設定
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_BUILD)
    if(NOT MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
    endif()
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE RELEASE_BUILD)
    if(NOT MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -DNDEBUG)
    endif()
endif()

# 資源檔案複製
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(OUTPUT_RESOURCE_DIR ${CMAKE_BINARY_DIR}/resources)

if(EXISTS ${RESOURCE_DIR})
    file(COPY ${RESOURCE_DIR} DESTINATION ${CMAKE_BINARY_DIR})
endif()

# 著色器檔案複製
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(OUTPUT_SHADER_DIR ${CMAKE_BINARY_DIR}/shaders)

if(EXISTS ${SHADER_DIR})
    file(COPY ${SHADER_DIR} DESTINATION ${CMAKE_BINARY_DIR})
endif()

# 範例場景檔案複製
set(EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../examples)
set(OUTPUT_EXAMPLES_DIR ${CMAKE_BINARY_DIR}/examples)

if(EXISTS ${EXAMPLES_DIR})
    file(COPY ${EXAMPLES_DIR} DESTINATION ${CMAKE_BINARY_DIR})
endif()

# 安裝設定
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 安裝資源檔案
if(EXISTS ${RESOURCE_DIR})
    install(DIRECTORY ${RESOURCE_DIR} DESTINATION share/${PROJECT_NAME})
endif()

if(EXISTS ${SHADER_DIR})
    install(DIRECTORY ${SHADER_DIR} DESTINATION share/${PROJECT_NAME})
endif()

if(EXISTS ${EXAMPLES_DIR})
    install(DIRECTORY ${EXAMPLES_DIR} DESTINATION share/${PROJECT_NAME})
endif()

# CPack 設定 (用於建立安裝包)
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Physics Scene Editor Team")
set(CPACK_PACKAGE_CONTACT "support@physicssceneeditor.com")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME ${PROJECT_NAME})
    set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_NSIS_CONTACT ${CPACK_PACKAGE_CONTACT})
    set(CPACK_NSIS_HELP_LINK "https://github.com/physicssceneeditor/physicssceneeditor")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
    set(CPACK_DMG_VOLUME_NAME ${PROJECT_NAME})
elseif(UNIX)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)

# 測試設定 (可選)
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    
    add_subdirectory(tests)
endif()

# 文件生成 (可選)
option(BUILD_DOCS "Build documentation" OFF)
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()

# 版本資訊檔案生成
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
    @ONLY
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# 建置資訊輸出
message(STATUS "")
message(STATUS "=== Physics Scene Runner Build Configuration ===")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "")
message(STATUS "Optional Features:")
message(STATUS "  CUDA Support: ${ENABLE_CUDA}")
message(STATUS "  FBX Support: ${ENABLE_FBX}")
message(STATUS "  Assimp Support: ${assimp_FOUND}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  Build Docs: ${BUILD_DOCS}")
message(STATUS "")
message(STATUS "Dependencies:")
message(STATUS "  OpenGL: ${OPENGL_FOUND}")
message(STATUS "  GLFW: ${glfw3_FOUND}")
message(STATUS "  GLEW: ${GLEW_FOUND}")
message(STATUS "  GLM: ${glm_FOUND}")
message(STATUS "  Bullet Physics: ${BULLET_FOUND}")
message(STATUS "  Eigen3: ${Eigen3_FOUND}")
message(STATUS "  nlohmann/json: ${nlohmann_json_FOUND}")
message(STATUS "")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "===============================================")
message(STATUS "")
