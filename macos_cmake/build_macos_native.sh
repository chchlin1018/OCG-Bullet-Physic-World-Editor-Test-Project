#!/bin/bash

# Physics Scene Editor macOS 原生建置腳本
# 專為 macOS 環境設計，支援 Intel 和 Apple Silicon

set -e  # 遇到錯誤時停止

# 顏色定義
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 函數定義
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 檢查命令是否存在
check_command() {
    if ! command -v $1 &> /dev/null; then
        print_error "$1 未找到，請先安裝"
        return 1
    fi
    return 0
}

# 檢查 macOS 版本
check_macos_version() {
    local version=$(sw_vers -productVersion)
    local major_version=$(echo $version | cut -d. -f1)
    local minor_version=$(echo $version | cut -d. -f2)
    
    print_info "檢測到 macOS 版本: $version"
    
    # 檢查是否為支援的版本（macOS 10.15+）
    if [[ $major_version -ge 11 ]] || [[ $major_version -eq 10 && $minor_version -ge 15 ]]; then
        print_success "macOS 版本支援"
        return 0
    else
        print_error "需要 macOS 10.15 或更高版本"
        return 1
    fi
}

# 檢查 Xcode Command Line Tools
check_xcode_tools() {
    print_info "檢查 Xcode Command Line Tools..."
    
    if xcode-select -p &> /dev/null; then
        local xcode_path=$(xcode-select -p)
        print_success "找到 Xcode Command Line Tools: $xcode_path"
        return 0
    else
        print_error "未找到 Xcode Command Line Tools"
        print_info "請執行: xcode-select --install"
        return 1
    fi
}

# 檢查 Homebrew
check_homebrew() {
    print_info "檢查 Homebrew..."
    
    if command -v brew &> /dev/null; then
        local brew_version=$(brew --version | head -n1)
        print_success "找到 Homebrew: $brew_version"
        return 0
    else
        print_warning "未找到 Homebrew"
        return 1
    fi
}

# 安裝 Homebrew
install_homebrew() {
    print_info "安裝 Homebrew..."
    
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    
    # 設定 PATH（針對 Apple Silicon）
    if [[ $(uname -m) == "arm64" ]]; then
        echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
        eval "$(/opt/homebrew/bin/brew shellenv)"
    else
        echo 'eval "$(/usr/local/bin/brew shellenv)"' >> ~/.zprofile
        eval "$(/usr/local/bin/brew shellenv)"
    fi
    
    print_success "Homebrew 安裝完成"
}

# 安裝依賴
install_dependencies() {
    print_info "安裝依賴套件..."
    
    # 更新 Homebrew
    brew update
    
    # 安裝必要套件
    local packages=(
        "cmake"
        "pkg-config"
        "qt@6"
        "glfw"
        "glew"
        "glm"
        "bullet"
        "eigen"
        "nlohmann-json"
        "assimp"
    )
    
    for package in "${packages[@]}"; do
        print_info "安裝 $package..."
        if brew list $package &> /dev/null; then
            print_success "$package 已安裝"
        else
            brew install $package
            print_success "$package 安裝完成"
        fi
    done
    
    # 連結 Qt6（如果需要）
    if ! brew list --versions qt@6 &> /dev/null; then
        print_error "Qt6 安裝失敗"
        return 1
    fi
    
    # 設定 Qt6 環境變數
    local qt_path=$(brew --prefix qt@6)
    export PATH="$qt_path/bin:$PATH"
    export CMAKE_PREFIX_PATH="$qt_path:$CMAKE_PREFIX_PATH"
    
    print_success "依賴安裝完成"
}

# 檢查 Qt 安裝
check_qt() {
    print_info "檢查 Qt 安裝..."
    
    # 檢查 Homebrew Qt6
    if brew list qt@6 &> /dev/null; then
        local qt_path=$(brew --prefix qt@6)
        export PATH="$qt_path/bin:$PATH"
        export CMAKE_PREFIX_PATH="$qt_path:$CMAKE_PREFIX_PATH"
        
        if command -v qmake &> /dev/null; then
            local qt_version=$(qmake -query QT_VERSION)
            print_success "找到 Qt $qt_version (Homebrew)"
            return 0
        fi
    fi
    
    # 檢查系統 Qt
    if command -v qmake6 &> /dev/null; then
        local qt_version=$(qmake6 -query QT_VERSION)
        print_success "找到 Qt $qt_version (系統)"
        return 0
    elif command -v qmake &> /dev/null; then
        local qt_version=$(qmake -query QT_VERSION)
        print_success "找到 Qt $qt_version (系統)"
        return 0
    fi
    
    print_error "未找到 Qt6 安裝"
    return 1
}

# 檢查其他依賴
check_dependencies() {
    print_info "檢查其他依賴..."
    
    local deps=("cmake" "pkg-config")
    for dep in "${deps[@]}"; do
        if check_command $dep; then
            local version=$($dep --version | head -n1)
            print_success "找到 $dep: $version"
        else
            print_error "缺少依賴: $dep"
            return 1
        fi
    done
    
    # 檢查 pkg-config 套件
    local pkgs=("glfw3" "glew")
    for pkg in "${pkgs[@]}"; do
        if pkg-config --exists $pkg 2>/dev/null; then
            local version=$(pkg-config --modversion $pkg 2>/dev/null || echo "unknown")
            print_success "找到 $pkg: $version"
        else
            print_warning "未找到 $pkg，可能影響建置"
        fi
    done
    
    return 0
}

# 建置專案
build_project() {
    print_info "開始建置專案..."
    
    # 建立建置目錄
    BUILD_DIR="build"
    if [[ "$CLEAN_BUILD" == "true" ]] && [[ -d "$BUILD_DIR" ]]; then
        print_info "清理舊的建置目錄..."
        rm -rf "$BUILD_DIR"
    fi
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # 設定建置類型
    local build_type="Release"
    if [[ "$BUILD_DEBUG" == "true" ]]; then
        build_type="Debug"
    fi
    
    # 設定 macOS 特定變數
    local macos_target="10.15"
    local arch=$(uname -m)
    
    # CMake 配置（macOS 版本）
    local cmake_args=(
        "-DCMAKE_BUILD_TYPE=$build_type"
        "-DCMAKE_OSX_DEPLOYMENT_TARGET=$macos_target"
        "-DCMAKE_OSX_ARCHITECTURES=$arch"
        "-DCMAKE_INSTALL_PREFIX=/usr/local"
    )
    
    # Qt6 路徑設定
    if brew list qt@6 &> /dev/null; then
        local qt_path=$(brew --prefix qt@6)
        cmake_args+=("-DQt6_DIR=$qt_path/lib/cmake/Qt6")
        cmake_args+=("-DCMAKE_PREFIX_PATH=$qt_path")
    fi
    
    # 可選功能
    if [[ "$BUILD_TESTS" == "true" ]]; then
        cmake_args+=("-DBUILD_TESTS=ON")
    fi
    
    if [[ "$CREATE_DMG" == "true" ]]; then
        cmake_args+=("-DCREATE_DMG=ON")
    fi
    
    print_info "CMake 配置: ${cmake_args[*]}"
    
    # 執行 CMake 配置
    cmake .. "${cmake_args[@]}"
    
    if [[ $? -ne 0 ]]; then
        print_error "CMake 配置失敗"
        exit 1
    fi
    
    # 建置
    print_info "開始編譯..."
    
    # 檢測 CPU 核心數（macOS）
    local cores=$(sysctl -n hw.ncpu)
    print_info "使用 $cores 個核心進行並行編譯"
    
    make -j$cores
    
    if [[ $? -ne 0 ]]; then
        print_error "編譯失敗"
        exit 1
    fi
    
    print_success "建置完成！"
    
    # 顯示輸出位置
    if [[ -d "PhysicsSceneEditor.app" ]]; then
        print_info "應用程式位置: $PWD/PhysicsSceneEditor.app"
    elif [[ -f "PhysicsSceneEditor" ]]; then
        print_info "執行檔位置: $PWD/PhysicsSceneEditor"
    fi
    
    cd ..
}

# 執行測試
run_tests() {
    if [[ "$RUN_TESTS" == "true" ]]; then
        print_info "執行測試..."
        cd build
        
        if [[ -f "PhysicsSceneEditorTests" ]]; then
            ./PhysicsSceneEditorTests
            if [[ $? -eq 0 ]]; then
                print_success "所有測試通過"
            else
                print_warning "部分測試失敗"
            fi
        else
            print_warning "未找到測試執行檔"
        fi
        
        cd ..
    fi
}

# 執行應用程式
run_app() {
    if [[ "$RUN_APP" == "true" ]]; then
        local app_path="build/PhysicsSceneEditor.app"
        local exec_path="build/PhysicsSceneEditor"
        
        if [[ -d "$app_path" ]]; then
            print_info "執行應用程式..."
            open "$app_path"
        elif [[ -f "$exec_path" ]]; then
            print_info "執行應用程式..."
            "$exec_path" &
        else
            print_error "未找到應用程式，請先建置專案"
        fi
    fi
}

# 建立 DMG
create_dmg() {
    if [[ "$CREATE_DMG" == "true" ]]; then
        print_info "建立 DMG 安裝檔..."
        cd build
        
        if [[ -d "PhysicsSceneEditor.app" ]]; then
            # 使用 macdeployqt 打包依賴
            if command -v macdeployqt &> /dev/null; then
                macdeployqt PhysicsSceneEditor.app -dmg
                print_success "DMG 建立完成: PhysicsSceneEditor.dmg"
            else
                print_warning "macdeployqt 未找到，無法建立 DMG"
            fi
        else
            print_warning "未找到應用程式 bundle"
        fi
        
        cd ..
    fi
}

# 顯示使用說明
show_usage() {
    echo "用法: $0 [選項]"
    echo ""
    echo "選項:"
    echo "  -h, --help              顯示此說明"
    echo "  -d, --dependencies      安裝依賴"
    echo "  -c, --clean             清理建置"
    echo "  -b, --build             建置專案"
    echo "  -t, --test              執行測試"
    echo "  -r, --run               執行應用程式"
    echo "  -m, --dmg               建立 DMG 安裝檔"
    echo "  --debug                 建置 Debug 版本"
    echo "  --release               建置 Release 版本（預設）"
    echo ""
    echo "環境變數:"
    echo "  QT_DIR                  Qt6 安裝路徑"
    echo "  BUILD_DEBUG             建置 Debug 版本"
    echo "  CLEAN_BUILD             清理建置"
    echo "  CREATE_DMG              建立 DMG 安裝檔"
    echo ""
    echo "範例:"
    echo "  $0 -d -b               安裝依賴並建置"
    echo "  $0 -c -b -t            清理、建置並測試"
    echo "  $0 -b -r               建置並執行"
    echo "  $0 -b -m               建置並建立 DMG"
}

# 顯示系統資訊
show_system_info() {
    print_info "系統資訊"
    print_info "========"
    echo "作業系統: $(sw_vers -productName) $(sw_vers -productVersion)"
    echo "建置版本: $(sw_vers -buildVersion)"
    echo "硬體: $(uname -m)"
    echo "CPU 核心: $(sysctl -n hw.ncpu)"
    echo "記憶體: $(echo $(($(sysctl -n hw.memsize) / 1024 / 1024 / 1024)))GB"
    
    if command -v qmake &> /dev/null; then
        echo "Qt: $(qmake -query QT_VERSION)"
    fi
    
    if command -v cmake &> /dev/null; then
        echo "CMake: $(cmake --version | head -n1)"
    fi
    
    if command -v brew &> /dev/null; then
        echo "Homebrew: $(brew --version | head -n1)"
    fi
    
    echo ""
}

# 主程式
main() {
    print_info "Physics Scene Editor macOS 原生建置腳本"
    print_info "========================================"
    
    # 檢查 macOS 環境
    check_macos_version || exit 1
    
    # 解析命令列參數
    INSTALL_DEPS=false
    CLEAN_BUILD=false
    BUILD_PROJECT=false
    RUN_TESTS=false
    RUN_APP=false
    BUILD_DEBUG=false
    CREATE_DMG=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_usage
                exit 0
                ;;
            -d|--dependencies)
                INSTALL_DEPS=true
                shift
                ;;
            -c|--clean)
                CLEAN_BUILD=true
                shift
                ;;
            -b|--build)
                BUILD_PROJECT=true
                shift
                ;;
            -t|--test)
                RUN_TESTS=true
                shift
                ;;
            -r|--run)
                RUN_APP=true
                shift
                ;;
            -m|--dmg)
                CREATE_DMG=true
                shift
                ;;
            --debug)
                BUILD_DEBUG=true
                shift
                ;;
            --release)
                BUILD_DEBUG=false
                shift
                ;;
            --info)
                show_system_info
                exit 0
                ;;
            *)
                print_error "未知選項: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # 如果沒有指定任何操作，預設建置專案
    if [[ "$INSTALL_DEPS" == "false" ]] && [[ "$BUILD_PROJECT" == "false" ]] && \
       [[ "$RUN_TESTS" == "false" ]] && [[ "$RUN_APP" == "false" ]] && \
       [[ "$CREATE_DMG" == "false" ]]; then
        BUILD_PROJECT=true
    fi
    
    # 顯示系統資訊
    show_system_info
    
    # 檢查 Xcode Command Line Tools
    check_xcode_tools || exit 1
    
    # 安裝依賴
    if [[ "$INSTALL_DEPS" == "true" ]]; then
        if ! check_homebrew; then
            install_homebrew
        fi
        install_dependencies
    fi
    
    # 檢查依賴
    check_qt || exit 1
    check_dependencies || exit 1
    
    # 建置專案
    if [[ "$BUILD_PROJECT" == "true" ]]; then
        build_project
    fi
    
    # 執行測試
    run_tests
    
    # 建立 DMG
    create_dmg
    
    # 執行應用程式
    run_app
    
    print_success "所有操作完成！"
    
    if [[ "$BUILD_PROJECT" == "true" ]]; then
        if [[ -d "build/PhysicsSceneEditor.app" ]]; then
            print_info "應用程式位置: build/PhysicsSceneEditor.app"
            print_info "執行應用程式: open build/PhysicsSceneEditor.app"
        else
            print_info "執行檔位置: build/PhysicsSceneEditor"
            print_info "執行應用程式: ./build/PhysicsSceneEditor"
        fi
    fi
}

# 執行主程式
main "$@"
