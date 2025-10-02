#!/bin/bash

# Physics Scene Editor Xcode 建置腳本
# 專為 macOS 平台設計

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
    local major=$(echo $version | cut -d. -f1)
    local minor=$(echo $version | cut -d. -f2)
    
    print_info "檢測到 macOS 版本: $version"
    
    if [[ $major -lt 10 ]] || [[ $major -eq 10 && $minor -lt 15 ]]; then
        print_error "需要 macOS 10.15 或更高版本"
        return 1
    fi
    
    return 0
}

# 檢查 Xcode 安裝
check_xcode() {
    print_info "檢查 Xcode 安裝..."
    
    if ! command -v xcodebuild &> /dev/null; then
        print_error "未找到 Xcode，請從 App Store 安裝"
        return 1
    fi
    
    local xcode_version=$(xcodebuild -version | head -n1 | awk '{print $2}')
    print_success "找到 Xcode $xcode_version"
    
    # 檢查 Command Line Tools
    if ! xcode-select -p &> /dev/null; then
        print_warning "Command Line Tools 未安裝，正在安裝..."
        xcode-select --install
        print_info "請完成 Command Line Tools 安裝後重新執行此腳本"
        exit 1
    fi
    
    return 0
}

# 檢查 Homebrew
check_homebrew() {
    if ! command -v brew &> /dev/null; then
        print_warning "未找到 Homebrew，正在安裝..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        
        # 設定 PATH
        if [[ -f "/opt/homebrew/bin/brew" ]]; then
            eval "$(/opt/homebrew/bin/brew shellenv)"
        elif [[ -f "/usr/local/bin/brew" ]]; then
            eval "$(/usr/local/bin/brew shellenv)"
        fi
    fi
    
    local brew_version=$(brew --version | head -n1 | awk '{print $2}')
    print_success "找到 Homebrew $brew_version"
    
    return 0
}

# 安裝依賴
install_dependencies() {
    print_info "安裝依賴套件..."
    
    # 更新 Homebrew
    brew update
    
    # 安裝必要依賴
    local packages=(
        "qt6"
        "cmake"
        "pkg-config"
        "glfw"
        "glew"
        "glm"
        "eigen"
        "bullet"
        "nlohmann-json"
        "assimp"
    )
    
    for package in "${packages[@]}"; do
        if brew list $package &> /dev/null; then
            print_info "$package 已安裝"
        else
            print_info "安裝 $package..."
            brew install $package
        fi
    done
    
    print_success "所有依賴已安裝"
}

# 檢查 Qt 安裝
check_qt() {
    print_info "檢查 Qt 安裝..."
    
    # 嘗試找到 Qt 安裝路徑
    if [[ -n "$QT_DIR" ]]; then
        QT_PATH="$QT_DIR"
    elif command -v brew &> /dev/null; then
        QT_PATH=$(brew --prefix qt6 2>/dev/null || echo "")
    else
        QT_PATH="/usr/local/qt6"
    fi
    
    if [[ -z "$QT_PATH" ]] || [[ ! -d "$QT_PATH" ]]; then
        print_error "未找到 Qt6 安裝"
        print_info "請安裝 Qt6: brew install qt6"
        return 1
    fi
    
    # 檢查 Qt 版本
    if [[ -f "$QT_PATH/bin/qmake" ]]; then
        local qt_version=$($QT_PATH/bin/qmake -query QT_VERSION)
        print_success "找到 Qt $qt_version at $QT_PATH"
        
        # 設定環境變數
        export QT_DIR="$QT_PATH"
        export PATH="$QT_PATH/bin:$PATH"
        export DYLD_FRAMEWORK_PATH="$QT_PATH/lib:$DYLD_FRAMEWORK_PATH"
        
        return 0
    else
        print_error "Qt 安裝不完整"
        return 1
    fi
}

# 檢查 CUDA（可選）
check_cuda() {
    if command -v nvcc &> /dev/null; then
        local cuda_version=$(nvcc --version | grep "release" | sed 's/.*release \([0-9.]*\).*/\1/')
        print_success "找到 CUDA $cuda_version"
        return 0
    else
        print_warning "未找到 CUDA，將停用 GPU 加速功能"
        return 1
    fi
}

# 建置專案
build_project() {
    print_info "開始建置 Xcode 專案..."
    
    # 檢查專案檔案
    if [[ ! -f "PhysicsSceneEditor.xcodeproj/project.pbxproj" ]]; then
        print_error "未找到 Xcode 專案檔案"
        exit 1
    fi
    
    # 設定建置參數
    local scheme="PhysicsSceneEditor"
    local configuration="Release"
    local destination="platform=macOS"
    
    if [[ "$BUILD_DEBUG" == "true" ]]; then
        configuration="Debug"
    fi
    
    # 清理舊的建置
    if [[ "$CLEAN_BUILD" == "true" ]]; then
        print_info "清理舊的建置..."
        xcodebuild clean -project PhysicsSceneEditor.xcodeproj -scheme $scheme -configuration $configuration
    fi
    
    # 建置專案
    print_info "建置配置: $configuration"
    print_info "目標平台: $destination"
    
    xcodebuild build \
        -project PhysicsSceneEditor.xcodeproj \
        -scheme $scheme \
        -configuration $configuration \
        -destination "$destination" \
        QT_DIR="$QT_DIR" \
        DYLD_FRAMEWORK_PATH="$QT_DIR/lib"
    
    if [[ $? -eq 0 ]]; then
        print_success "建置完成！"
        
        # 顯示輸出位置
        local build_dir="build/$configuration"
        if [[ -d "$build_dir" ]]; then
            print_info "應用程式位置: $build_dir/PhysicsSceneEditor.app"
        fi
    else
        print_error "建置失敗"
        exit 1
    fi
}

# 執行測試
run_tests() {
    if [[ "$RUN_TESTS" == "true" ]]; then
        print_info "執行測試..."
        
        xcodebuild test \
            -project PhysicsSceneEditor.xcodeproj \
            -scheme PhysicsSceneEditor \
            -destination "platform=macOS"
        
        if [[ $? -eq 0 ]]; then
            print_success "所有測試通過"
        else
            print_warning "部分測試失敗"
        fi
    fi
}

# 打包應用程式
package_app() {
    if [[ "$PACKAGE_APP" == "true" ]]; then
        print_info "打包應用程式..."
        
        local app_path="build/Release/PhysicsSceneEditor.app"
        
        if [[ ! -d "$app_path" ]]; then
            print_error "未找到應用程式: $app_path"
            return 1
        fi
        
        # 使用 macdeployqt 打包依賴
        if [[ -f "$QT_DIR/bin/macdeployqt" ]]; then
            print_info "打包 Qt 依賴..."
            "$QT_DIR/bin/macdeployqt" "$app_path"
        fi
        
        # 建立 DMG
        print_info "建立 DMG 安裝檔..."
        local dmg_name="PhysicsSceneEditor-$(date +%Y%m%d).dmg"
        
        hdiutil create -volname "Physics Scene Editor" \
                      -srcfolder "$app_path" \
                      -ov -format UDZO \
                      "$dmg_name"
        
        if [[ $? -eq 0 ]]; then
            print_success "DMG 建立完成: $dmg_name"
        else
            print_error "DMG 建立失敗"
        fi
    fi
}

# 開啟 Xcode
open_xcode() {
    if [[ "$OPEN_XCODE" == "true" ]]; then
        print_info "開啟 Xcode..."
        open PhysicsSceneEditor.xcodeproj
    fi
}

# 執行應用程式
run_app() {
    if [[ "$RUN_APP" == "true" ]]; then
        local app_path="build/Release/PhysicsSceneEditor.app"
        
        if [[ -d "$app_path" ]]; then
            print_info "執行應用程式..."
            open "$app_path"
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
    echo "  -p, --package           打包應用程式"
    echo "  -o, --open              開啟 Xcode"
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
    echo "  $0 -b -p               建置並打包"
    echo "  $0 -o                  開啟 Xcode"
}

# 顯示系統資訊
show_system_info() {
    print_info "系統資訊"
    print_info "========"
    echo "作業系統: $(sw_vers -productName) $(sw_vers -productVersion)"
    echo "硬體: $(uname -m)"
    
    if command -v xcodebuild &> /dev/null; then
        echo "Xcode: $(xcodebuild -version | head -n1)"
    fi
    
    if [[ -n "$QT_DIR" ]] && [[ -f "$QT_DIR/bin/qmake" ]]; then
        echo "Qt: $($QT_DIR/bin/qmake -query QT_VERSION)"
    fi
    
    if command -v brew &> /dev/null; then
        echo "Homebrew: $(brew --version | head -n1)"
    fi
    
    echo ""
}

# 主程式
main() {
    print_info "Physics Scene Editor Xcode 建置腳本"
    print_info "===================================="
    
    # 檢查 macOS
    check_macos_version || exit 1
    
    # 解析命令列參數
    INSTALL_DEPS=false
    CLEAN_BUILD=false
    BUILD_PROJECT=false
    RUN_TESTS=false
    PACKAGE_APP=false
    OPEN_XCODE=false
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
            -p|--package)
                PACKAGE_APP=true
                shift
                ;;
            -o|--open)
                OPEN_XCODE=true
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
       [[ "$RUN_TESTS" == "false" ]] && [[ "$PACKAGE_APP" == "false" ]] && \
       [[ "$OPEN_XCODE" == "false" ]] && [[ "$RUN_APP" == "false" ]]; then
        BUILD_PROJECT=true
    fi
    
    # 顯示系統資訊
    show_system_info
    
    # 檢查基本工具
    check_xcode || exit 1
    
    # 安裝依賴
    if [[ "$INSTALL_DEPS" == "true" ]]; then
        check_homebrew || exit 1
        install_dependencies
    fi
    
    # 檢查 Qt
    check_qt || exit 1
    
    # 檢查 CUDA（可選）
    check_cuda
    
    # 建置專案
    if [[ "$BUILD_PROJECT" == "true" ]]; then
        build_project
    fi
    
    # 執行測試
    run_tests
    
    # 打包應用程式
    package_app
    
    # 開啟 Xcode
    open_xcode
    
    # 執行應用程式
    run_app
    
    print_success "所有操作完成！"
    
    if [[ "$BUILD_PROJECT" == "true" ]]; then
        print_info "建置輸出位置: build/Release/PhysicsSceneEditor.app"
        print_info "執行應用程式: open build/Release/PhysicsSceneEditor.app"
    fi
}

# 執行主程式
main "$@"
