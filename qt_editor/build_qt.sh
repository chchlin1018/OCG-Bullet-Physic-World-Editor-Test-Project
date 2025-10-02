#!/bin/bash

# Physics Scene Editor Qt 建構腳本
# 支援 Windows、macOS 和 Linux 平台

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

# 檢測作業系統
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

# 安裝依賴
install_dependencies() {
    local os=$(detect_os)
    
    print_info "檢測到作業系統: $os"
    
    case $os in
        "linux")
            install_linux_dependencies
            ;;
        "macos")
            install_macos_dependencies
            ;;
        "windows")
            install_windows_dependencies
            ;;
        *)
            print_error "不支援的作業系統: $os"
            exit 1
            ;;
    esac
}

# Linux 依賴安裝
install_linux_dependencies() {
    print_info "安裝 Linux 依賴..."
    
    # 檢測 Linux 發行版
    if command -v apt-get &> /dev/null; then
        # Ubuntu/Debian
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            git \
            qt6-base-dev \
            qt6-tools-dev \
            qt6-tools-dev-tools \
            libqt6opengl6-dev \
            libqt6openglwidgets6 \
            libgl1-mesa-dev \
            libglfw3-dev \
            libglew-dev \
            libglm-dev \
            libbullet-dev \
            libeigen3-dev \
            nlohmann-json3-dev \
            libassimp-dev
            
    elif command -v yum &> /dev/null; then
        # CentOS/RHEL
        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y \
            cmake \
            git \
            qt6-qtbase-devel \
            qt6-qttools-devel \
            mesa-libGL-devel \
            glfw-devel \
            glew-devel \
            glm-devel \
            bullet-devel \
            eigen3-devel \
            assimp-devel
            
    elif command -v dnf &> /dev/null; then
        # Fedora
        sudo dnf groupinstall -y "Development Tools"
        sudo dnf install -y \
            cmake \
            git \
            qt6-qtbase-devel \
            qt6-qttools-devel \
            mesa-libGL-devel \
            glfw-devel \
            glew-devel \
            glm-devel \
            bullet-devel \
            eigen3-devel \
            nlohmann-json-devel \
            assimp-devel
            
    elif command -v pacman &> /dev/null; then
        # Arch Linux
        sudo pacman -S --needed \
            base-devel \
            cmake \
            git \
            qt6-base \
            qt6-tools \
            mesa \
            glfw-x11 \
            glew \
            glm \
            bullet \
            eigen \
            nlohmann-json \
            assimp
    else
        print_error "不支援的 Linux 發行版"
        exit 1
    fi
}

# macOS 依賴安裝
install_macos_dependencies() {
    print_info "安裝 macOS 依賴..."
    
    # 檢查 Homebrew
    if ! command -v brew &> /dev/null; then
        print_info "安裝 Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    # 安裝依賴
    brew install \
        cmake \
        qt6 \
        glfw \
        glew \
        glm \
        bullet \
        eigen \
        nlohmann-json \
        assimp
        
    # 設定 Qt 環境變數
    export Qt6_DIR=$(brew --prefix qt6)/lib/cmake/Qt6
}

# Windows 依賴安裝
install_windows_dependencies() {
    print_info "Windows 依賴安裝..."
    print_warning "請手動安裝以下依賴："
    print_warning "1. Visual Studio 2019 或更高版本"
    print_warning "2. Qt6 (https://www.qt.io/download)"
    print_warning "3. vcpkg 套件管理器"
    print_warning "4. 使用 vcpkg 安裝依賴："
    echo "   vcpkg install glfw3:x64-windows"
    echo "   vcpkg install glew:x64-windows"
    echo "   vcpkg install glm:x64-windows"
    echo "   vcpkg install bullet3:x64-windows"
    echo "   vcpkg install eigen3:x64-windows"
    echo "   vcpkg install nlohmann-json:x64-windows"
    echo "   vcpkg install assimp:x64-windows"
}

# 檢查 Qt 安裝
check_qt() {
    print_info "檢查 Qt 安裝..."
    
    if command -v qmake6 &> /dev/null; then
        QT_VERSION=$(qmake6 -query QT_VERSION)
        print_success "找到 Qt $QT_VERSION"
        return 0
    elif command -v qmake &> /dev/null; then
        QT_VERSION=$(qmake -query QT_VERSION)
        if [[ $QT_VERSION == 6.* ]]; then
            print_success "找到 Qt $QT_VERSION"
            return 0
        else
            print_warning "找到 Qt $QT_VERSION，但需要 Qt 6.x"
        fi
    fi
    
    print_error "未找到 Qt6，請先安裝"
    return 1
}

# 檢查 CUDA
check_cuda() {
    if command -v nvcc &> /dev/null; then
        CUDA_VERSION=$(nvcc --version | grep "release" | sed 's/.*release \([0-9.]*\).*/\1/')
        print_success "找到 CUDA $CUDA_VERSION"
        return 0
    else
        print_warning "未找到 CUDA，將停用 GPU 加速功能"
        return 1
    fi
}

# 建構專案
build_project() {
    print_info "開始建構專案..."
    
    # 建立建構目錄
    BUILD_DIR="build"
    if [ -d "$BUILD_DIR" ]; then
        print_info "清理舊的建構目錄..."
        rm -rf "$BUILD_DIR"
    fi
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # CMake 配置
    CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release"
    
    # 檢查 CUDA 支援
    if check_cuda; then
        CMAKE_ARGS="$CMAKE_ARGS -DENABLE_CUDA=ON"
    else
        CMAKE_ARGS="$CMAKE_ARGS -DENABLE_CUDA=OFF"
    fi
    
    # 檢查 FBX SDK
    if [ -n "$FBX_SDK_ROOT" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DENABLE_FBX=ON -DFBX_SDK_ROOT=$FBX_SDK_ROOT"
        print_info "啟用 FBX SDK 支援: $FBX_SDK_ROOT"
    else
        CMAKE_ARGS="$CMAKE_ARGS -DENABLE_FBX=OFF"
        print_info "使用 Assimp 作為 3D 模型載入器"
    fi
    
    # 平台特定設定
    local os=$(detect_os)
    case $os in
        "windows")
            CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_GENERATOR_PLATFORM=x64"
            if [ -n "$VCPKG_ROOT" ]; then
                CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
            fi
            ;;
        "macos")
            if [ -n "$Qt6_DIR" ]; then
                CMAKE_ARGS="$CMAKE_ARGS -DQt6_DIR=$Qt6_DIR"
            fi
            ;;
    esac
    
    print_info "CMake 配置: $CMAKE_ARGS"
    
    # 執行 CMake
    cmake .. $CMAKE_ARGS
    
    if [ $? -ne 0 ]; then
        print_error "CMake 配置失敗"
        exit 1
    fi
    
    # 建構
    print_info "開始編譯..."
    
    # 檢測 CPU 核心數
    if command -v nproc &> /dev/null; then
        CORES=$(nproc)
    elif command -v sysctl &> /dev/null; then
        CORES=$(sysctl -n hw.ncpu)
    else
        CORES=4
    fi
    
    cmake --build . --config Release --parallel $CORES
    
    if [ $? -ne 0 ]; then
        print_error "編譯失敗"
        exit 1
    fi
    
    print_success "建構完成！"
    cd ..
}

# 執行測試
run_tests() {
    if [ "$RUN_TESTS" = "true" ]; then
        print_info "執行測試..."
        cd build
        ctest --output-on-failure
        cd ..
    fi
}

# 安裝
install_project() {
    if [ "$INSTALL" = "true" ]; then
        print_info "安裝專案..."
        cd build
        cmake --install . --config Release
        cd ..
        print_success "安裝完成！"
    fi
}

# 建立安裝包
create_package() {
    if [ "$CREATE_PACKAGE" = "true" ]; then
        print_info "建立安裝包..."
        cd build
        cpack
        cd ..
        print_success "安裝包建立完成！"
    fi
}

# 顯示使用說明
show_usage() {
    echo "用法: $0 [選項]"
    echo ""
    echo "選項:"
    echo "  -h, --help              顯示此說明"
    echo "  -d, --dependencies      安裝依賴"
    echo "  -c, --clean             清理建構目錄"
    echo "  -t, --tests             執行測試"
    echo "  -i, --install           安裝到系統"
    echo "  -p, --package           建立安裝包"
    echo "  --fbx-sdk PATH          指定 FBX SDK 路徑"
    echo "  --vcpkg-root PATH       指定 vcpkg 根目錄"
    echo ""
    echo "環境變數:"
    echo "  FBX_SDK_ROOT           FBX SDK 安裝路徑"
    echo "  VCPKG_ROOT             vcpkg 安裝路徑"
    echo "  Qt6_DIR                Qt6 CMake 路徑"
    echo ""
    echo "範例:"
    echo "  $0 -d                   安裝依賴並建構"
    echo "  $0 -t                   建構並執行測試"
    echo "  $0 --fbx-sdk /opt/fbx   使用指定的 FBX SDK"
}

# 主程式
main() {
    print_info "Physics Scene Editor Qt 建構腳本"
    print_info "=================================="
    
    # 解析命令列參數
    INSTALL_DEPS=false
    CLEAN=false
    RUN_TESTS=false
    INSTALL=false
    CREATE_PACKAGE=false
    
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
                CLEAN=true
                shift
                ;;
            -t|--tests)
                RUN_TESTS=true
                shift
                ;;
            -i|--install)
                INSTALL=true
                shift
                ;;
            -p|--package)
                CREATE_PACKAGE=true
                shift
                ;;
            --fbx-sdk)
                export FBX_SDK_ROOT="$2"
                shift 2
                ;;
            --vcpkg-root)
                export VCPKG_ROOT="$2"
                shift 2
                ;;
            *)
                print_error "未知選項: $1"
                show_usage
                exit 1
                ;;
        esac
    done
    
    # 檢查基本工具
    check_command cmake || exit 1
    check_command git || exit 1
    
    # 安裝依賴
    if [ "$INSTALL_DEPS" = "true" ]; then
        install_dependencies
    fi
    
    # 檢查 Qt
    check_qt || exit 1
    
    # 清理
    if [ "$CLEAN" = "true" ]; then
        print_info "清理建構目錄..."
        rm -rf build
    fi
    
    # 建構專案
    build_project
    
    # 執行測試
    run_tests
    
    # 安裝
    install_project
    
    # 建立安裝包
    create_package
    
    print_success "所有操作完成！"
    print_info "可執行檔位於: build/PhysicsSceneEditorQt"
}

# 執行主程式
main "$@"
