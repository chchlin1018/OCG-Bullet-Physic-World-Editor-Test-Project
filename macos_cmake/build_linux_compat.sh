#!/bin/bash

# Physics Scene Editor Linux 相容版本建置腳本
# 在 Linux 環境中模擬 macOS CMake 建置流程

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

# 檢查 Linux 版本（模擬 macOS 檢查）
check_linux_version() {
    local version=$(lsb_release -rs 2>/dev/null || echo "unknown")
    local distro=$(lsb_release -is 2>/dev/null || echo "unknown")
    
    print_info "檢測到 Linux 發行版: $distro $version"
    print_info "在 Linux 環境中執行 macOS 相容建置"
    
    return 0
}

# 檢查開發工具
check_development_tools() {
    print_info "檢查開發工具..."
    
    local tools=("gcc" "g++" "make" "cmake")
    for tool in "${tools[@]}"; do
        if check_command $tool; then
            local version=$($tool --version | head -n1)
            print_success "找到 $tool: $version"
        else
            print_error "缺少開發工具: $tool"
            return 1
        fi
    done
    
    return 0
}

# 檢查套件管理器
check_package_manager() {
    if command -v apt &> /dev/null; then
        PACKAGE_MANAGER="apt"
        print_success "找到套件管理器: apt"
    elif command -v yum &> /dev/null; then
        PACKAGE_MANAGER="yum"
        print_success "找到套件管理器: yum"
    elif command -v dnf &> /dev/null; then
        PACKAGE_MANAGER="dnf"
        print_success "找到套件管理器: dnf"
    elif command -v pacman &> /dev/null; then
        PACKAGE_MANAGER="pacman"
        print_success "找到套件管理器: pacman"
    else
        print_error "未找到支援的套件管理器"
        return 1
    fi
    
    return 0
}

# 安裝依賴
install_dependencies() {
    print_info "安裝依賴套件..."
    
    case $PACKAGE_MANAGER in
        "apt")
            print_info "使用 apt 安裝依賴..."
            sudo apt update
            sudo apt install -y \
                build-essential \
                cmake \
                pkg-config \
                qt6-base-dev \
                qt6-tools-dev \
                qt6-tools-dev-tools \
                libqt6opengl6-dev \
                libqt6openglwidgets6 \
                libgl1-mesa-dev \
                libglu1-mesa-dev \
                libglfw3-dev \
                libglew-dev \
                libglm-dev \
                libbullet-dev \
                libeigen3-dev \
                nlohmann-json3-dev \
                libassimp-dev
            ;;
        "yum"|"dnf")
            print_info "使用 $PACKAGE_MANAGER 安裝依賴..."
            sudo $PACKAGE_MANAGER install -y \
                gcc-c++ \
                cmake \
                pkgconfig \
                qt6-qtbase-devel \
                qt6-qttools-devel \
                mesa-libGL-devel \
                mesa-libGLU-devel \
                glfw-devel \
                glew-devel \
                glm-devel \
                bullet-devel \
                eigen3-devel \
                json-devel \
                assimp-devel
            ;;
        "pacman")
            print_info "使用 pacman 安裝依賴..."
            sudo pacman -S --noconfirm \
                base-devel \
                cmake \
                pkgconf \
                qt6-base \
                qt6-tools \
                mesa \
                glfw \
                glew \
                glm \
                bullet \
                eigen \
                nlohmann-json \
                assimp
            ;;
        *)
            print_error "不支援的套件管理器: $PACKAGE_MANAGER"
            return 1
            ;;
    esac
    
    print_success "依賴安裝完成"
}

# 檢查 Qt 安裝
check_qt() {
    print_info "檢查 Qt 安裝..."
    
    # 嘗試找到 Qt 安裝路徑
    local qt_paths=(
        "/usr/lib/x86_64-linux-gnu/qt6"
        "/usr/lib/qt6"
        "/usr/local/qt6"
        "/opt/qt6"
    )
    
    for path in "${qt_paths[@]}"; do
        if [[ -d "$path" ]]; then
            QT_PATH="$path"
            break
        fi
    done
    
    # 檢查 qmake6 或 qmake
    if command -v qmake6 &> /dev/null; then
        local qt_version=$(qmake6 -query QT_VERSION)
        print_success "找到 Qt $qt_version (qmake6)"
        export QT_DIR="$QT_PATH"
        return 0
    elif command -v qmake &> /dev/null; then
        local qt_version=$(qmake -query QT_VERSION)
        print_success "找到 Qt $qt_version (qmake)"
        export QT_DIR="$QT_PATH"
        return 0
    else
        print_error "未找到 Qt6 安裝"
        return 1
    fi
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
    
    # CMake 配置（Linux 版本）
    local cmake_args=(
        "-DCMAKE_BUILD_TYPE=$build_type"
        "-DCMAKE_INSTALL_PREFIX=/usr/local"
    )
    
    # 嘗試找到 Qt6 配置
    if [[ -d "/usr/lib/x86_64-linux-gnu/cmake/Qt6" ]]; then
        cmake_args+=("-DQt6_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt6")
    elif [[ -d "/usr/lib/cmake/Qt6" ]]; then
        cmake_args+=("-DQt6_DIR=/usr/lib/cmake/Qt6")
    fi
    
    # 可選功能
    if [[ "$BUILD_TESTS" == "true" ]]; then
        cmake_args+=("-DBUILD_TESTS=ON")
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
    
    # 檢測 CPU 核心數
    local cores=$(nproc)
    print_info "使用 $cores 個核心進行並行編譯"
    
    make -j$cores
    
    if [[ $? -ne 0 ]]; then
        print_error "編譯失敗"
        exit 1
    fi
    
    print_success "建置完成！"
    
    # 顯示輸出位置
    if [[ -f "PhysicsSceneEditorMacOS" ]]; then
        print_info "執行檔位置: $PWD/PhysicsSceneEditorMacOS"
    fi
    
    cd ..
}

# 執行測試
run_tests() {
    if [[ "$RUN_TESTS" == "true" ]]; then
        print_info "執行測試..."
        cd build
        
        if [[ -f "PhysicsSceneEditorMacOSTests" ]]; then
            ./PhysicsSceneEditorMacOSTests
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
        local app_path="build/PhysicsSceneEditorMacOS"
        
        if [[ -f "$app_path" ]]; then
            print_info "執行應用程式..."
            "$app_path" &
        else
            print_error "未找到應用程式，請先建置專案"
        fi
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
    echo "  --debug                 建置 Debug 版本"
    echo "  --release               建置 Release 版本（預設）"
    echo ""
    echo "環境變數:"
    echo "  QT_DIR                  Qt6 安裝路徑"
    echo "  BUILD_DEBUG             建置 Debug 版本"
    echo "  CLEAN_BUILD             清理建置"
    echo ""
    echo "範例:"
    echo "  $0 -d -b               安裝依賴並建置"
    echo "  $0 -c -b -t            清理、建置並測試"
    echo "  $0 -b -r               建置並執行"
}

# 顯示系統資訊
show_system_info() {
    print_info "系統資訊"
    print_info "========"
    echo "作業系統: $(uname -s) $(uname -r)"
    echo "發行版: $(lsb_release -ds 2>/dev/null || echo 'Unknown')"
    echo "硬體: $(uname -m)"
    echo "CPU 核心: $(nproc)"
    echo "記憶體: $(free -h | awk '/^Mem:/ {print $2}')"
    
    if command -v qmake6 &> /dev/null; then
        echo "Qt: $(qmake6 -query QT_VERSION)"
    elif command -v qmake &> /dev/null; then
        echo "Qt: $(qmake -query QT_VERSION)"
    fi
    
    if command -v cmake &> /dev/null; then
        echo "CMake: $(cmake --version | head -n1)"
    fi
    
    echo ""
}

# 主程式
main() {
    print_info "Physics Scene Editor Linux 相容建置腳本"
    print_info "========================================"
    
    # 檢查 Linux 環境
    check_linux_version || exit 1
    
    # 解析命令列參數
    INSTALL_DEPS=false
    CLEAN_BUILD=false
    BUILD_PROJECT=false
    RUN_TESTS=false
    RUN_APP=false
    BUILD_DEBUG=false
    
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
       [[ "$RUN_TESTS" == "false" ]] && [[ "$RUN_APP" == "false" ]]; then
        BUILD_PROJECT=true
    fi
    
    # 顯示系統資訊
    show_system_info
    
    # 檢查基本工具
    check_development_tools || exit 1
    
    # 安裝依賴
    if [[ "$INSTALL_DEPS" == "true" ]]; then
        check_package_manager || exit 1
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
    
    # 執行應用程式
    run_app
    
    print_success "所有操作完成！"
    
    if [[ "$BUILD_PROJECT" == "true" ]]; then
        print_info "建置輸出位置: build/PhysicsSceneEditorMacOS"
        print_info "執行應用程式: ./build/PhysicsSceneEditorMacOS"
    fi
}

# 執行主程式
main "$@"
