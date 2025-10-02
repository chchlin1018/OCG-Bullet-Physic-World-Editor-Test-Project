# 建置指南

本文件詳細說明如何在不同平台上建置 Physics Scene Editor。

## 目錄

- [系統需求](#系統需求)
- [依賴安裝](#依賴安裝)
- [Windows 建置](#windows-建置)
- [macOS 建置](#macos-建置)
- [Linux 建置](#linux-建置)
- [建置選項](#建置選項)
- [疑難排解](#疑難排解)

## 系統需求

### 通用需求
- CMake 3.16 或更高版本
- C++17 相容編譯器
- Git (用於取得原始碼)

### Windows
- Visual Studio 2019 或更高版本
- Windows 10 SDK
- CUDA Toolkit 11.0+ (可選，用於 GPU 加速)

### macOS
- Xcode 12 或更高版本
- macOS 10.15 或更高版本
- Homebrew (建議用於依賴管理)

### Linux
- GCC 9+ 或 Clang 10+
- 相關開發套件 (build-essential, cmake, etc.)

## 依賴安裝

### 必要依賴
- **OpenGL**: 圖形渲染
- **GLFW**: 視窗管理
- **GLEW**: OpenGL 擴展載入
- **GLM**: 數學函式庫
- **Bullet Physics**: 物理引擎
- **Eigen3**: 線性代數
- **nlohmann/json**: JSON 解析

### 可選依賴
- **CUDA Toolkit**: GPU 加速
- **FBX SDK**: FBX 檔案支援
- **Assimp**: 3D 模型載入 (FBX 替代方案)
- **Google Test**: 單元測試

## Windows 建置

### 1. 安裝 Visual Studio

下載並安裝 Visual Studio 2019 或更高版本，確保包含以下組件：
- MSVC v142 編譯器
- Windows 10 SDK
- CMake 工具
- Git for Windows

### 2. 安裝 vcpkg (建議)

```cmd
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

### 3. 安裝依賴

```cmd
# 必要依賴
vcpkg install glfw3:x64-windows
vcpkg install glew:x64-windows
vcpkg install glm:x64-windows
vcpkg install bullet3:x64-windows
vcpkg install eigen3:x64-windows
vcpkg install nlohmann-json:x64-windows

# 可選依賴
vcpkg install assimp:x64-windows
vcpkg install gtest:x64-windows
```

### 4. 建置專案

```cmd
git clone https://github.com/physics-scene-editor/physics-scene-editor.git
cd physics-scene-editor
mkdir build
cd build

# 使用 vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake -DCMAKE_GENERATOR_PLATFORM=x64

# 建置
cmake --build . --config Release
```

### 5. 建置 MFC 編輯器

```cmd
# 開啟 Visual Studio 解決方案
start mfc_editor\PhysicsSceneEditor.sln

# 或使用命令列
msbuild mfc_editor\PhysicsSceneEditor.sln /p:Configuration=Release /p:Platform=x64
```

## macOS 建置

### 1. 安裝 Xcode

從 App Store 安裝 Xcode 和 Command Line Tools：

```bash
xcode-select --install
```

### 2. 安裝 Homebrew

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### 3. 安裝依賴

```bash
# 必要依賴
brew install cmake
brew install glfw
brew install glew
brew install glm
brew install bullet
brew install eigen
brew install nlohmann-json

# 可選依賴
brew install assimp
brew install googletest
```

### 4. 建置專案

```bash
git clone https://github.com/physics-scene-editor/physics-scene-editor.git
cd physics-scene-editor
mkdir build
cd build

# 配置
cmake .. -DCMAKE_BUILD_TYPE=Release

# 建置
make -j$(sysctl -n hw.ncpu)
```

## Linux 建置

### Ubuntu/Debian

```bash
# 安裝基本建置工具
sudo apt update
sudo apt install build-essential cmake git

# 安裝依賴
sudo apt install libglfw3-dev libglew-dev libglm-dev
sudo apt install libbullet-dev libeigen3-dev nlohmann-json3-dev
sudo apt install libassimp-dev libgtest-dev

# 建置專案
git clone https://github.com/physics-scene-editor/physics-scene-editor.git
cd physics-scene-editor
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### CentOS/RHEL/Fedora

```bash
# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake git
sudo yum install glfw-devel glew-devel glm-devel
sudo yum install bullet-devel eigen3-devel

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git
sudo dnf install glfw-devel glew-devel glm-devel
sudo dnf install bullet-devel eigen3-devel

# 建置專案 (同 Ubuntu)
```

### Arch Linux

```bash
# 安裝依賴
sudo pacman -S base-devel cmake git
sudo pacman -S glfw-x11 glew glm bullet eigen nlohmann-json
sudo pacman -S assimp gtest

# 建置專案 (同 Ubuntu)
```

## 建置選項

### CMake 選項

```bash
# 啟用 CUDA 支援
cmake .. -DENABLE_CUDA=ON

# 啟用 FBX 支援
cmake .. -DENABLE_FBX=ON -DFBX_SDK_ROOT=/path/to/fbx/sdk

# 啟用測試
cmake .. -DBUILD_TESTS=ON

# 啟用文件生成
cmake .. -DBUILD_DOCS=ON

# 除錯建置
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 自訂安裝路徑
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
```

### 完整建置範例

```bash
# 包含所有功能的建置
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_CUDA=ON \
  -DENABLE_FBX=ON \
  -DBUILD_TESTS=ON \
  -DBUILD_DOCS=ON \
  -DCMAKE_INSTALL_PREFIX=/usr/local

make -j$(nproc)
make test
make install
```

## CUDA 支援

### 安裝 CUDA Toolkit

1. 從 [NVIDIA 官網](https://developer.nvidia.com/cuda-downloads) 下載 CUDA Toolkit
2. 按照平台特定指示安裝
3. 確認安裝：

```bash
nvcc --version
nvidia-smi
```

### 建置 CUDA 支援

```bash
cmake .. -DENABLE_CUDA=ON -DCUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda
make -j$(nproc)
```

## FBX SDK 支援

### 下載 FBX SDK

1. 從 [Autodesk 官網](https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0) 下載 FBX SDK
2. 解壓到適當位置
3. 設定環境變數或 CMake 參數

### 建置 FBX 支援

```bash
# 設定 FBX SDK 路徑
export FBX_SDK_ROOT=/path/to/fbx/sdk

# 或在 CMake 中指定
cmake .. -DENABLE_FBX=ON -DFBX_SDK_ROOT=/path/to/fbx/sdk
```

## 測試

### 執行單元測試

```bash
# 建置測試
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)

# 執行測試
make test

# 或直接執行
./tests/test_scene_format
./tests/integration_test
```

### 執行範例

```bash
# 基本測試場景
./cross_platform_runner/PhysicsSceneRunner ../examples/basic_drop_test.pscene

# 複雜約束測試
./cross_platform_runner/PhysicsSceneRunner ../examples/complex_constraint_test.pscene
```

## 安裝

### 系統安裝

```bash
# 安裝到系統路徑
sudo make install

# 或安裝到自訂路徑
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/physics-scene-editor
make install
```

### 建立安裝包

```bash
# 建立 DEB 包 (Ubuntu/Debian)
make package

# 建立 RPM 包 (CentOS/RHEL/Fedora)
cmake .. -DCPACK_GENERATOR=RPM
make package

# 建立 DMG 包 (macOS)
cmake .. -DCPACK_GENERATOR=DragNDrop
make package

# 建立 NSIS 安裝程式 (Windows)
cmake .. -DCPACK_GENERATOR=NSIS
cmake --build . --target package
```

## 疑難排解

### 常見問題

**Q: CMake 找不到依賴**
```bash
# 設定 CMAKE_PREFIX_PATH
export CMAKE_PREFIX_PATH=/usr/local:/opt/local

# 或在 CMake 中指定
cmake .. -DCMAKE_PREFIX_PATH="/usr/local;/opt/local"
```

**Q: GLFW 編譯錯誤**
```bash
# 確認已安裝 X11 開發套件 (Linux)
sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev

# 或使用 Wayland
sudo apt install libwayland-dev libxkbcommon-dev
```

**Q: Bullet Physics 版本不相容**
```bash
# 確認使用正確版本
cmake .. -DBULLET_ROOT=/path/to/bullet/3.x
```

**Q: CUDA 編譯失敗**
```bash
# 檢查 CUDA 版本相容性
nvcc --version
cmake .. -DCUDA_ARCHITECTURES="50;60;70;75;80"
```

**Q: 連結錯誤**
```bash
# 確認函式庫路徑
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# 或設定 RPATH
cmake .. -DCMAKE_INSTALL_RPATH=/usr/local/lib
```

### 除錯建置

```bash
# 詳細輸出
make VERBOSE=1

# 或
cmake --build . --verbose

# 檢查 CMake 配置
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
```

### 清理建置

```bash
# 清理建置檔案
make clean

# 完全重新建置
rm -rf build
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## 效能最佳化

### 編譯器最佳化

```bash
# GCC/Clang 最佳化
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"

# MSVC 最佳化
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="/O2 /arch:AVX2"
```

### 連結時間最佳化 (LTO)

```bash
cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

### 平行建置

```bash
# Linux/macOS
make -j$(nproc)

# Windows
cmake --build . --config Release --parallel
```

## 開發環境設定

### IDE 配置

**Visual Studio Code:**
```json
{
  "cmake.configureArgs": [
    "-DBUILD_TESTS=ON",
    "-DCMAKE_BUILD_TYPE=Debug"
  ],
  "cmake.buildArgs": [
    "--parallel"
  ]
}
```

**CLion:**
- 開啟專案根目錄
- 設定 CMake 選項在 Settings > Build > CMake

**Visual Studio:**
- 使用 "Open Folder" 開啟專案
- 編輯 CMakeSettings.json 配置建置選項

### 程式碼格式化

```bash
# 安裝 clang-format
sudo apt install clang-format  # Ubuntu
brew install clang-format     # macOS

# 格式化程式碼
find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

---

如有任何建置問題，請參考 [疑難排解指南](TROUBLESHOOTING.md) 或在 [GitHub Issues](https://github.com/physics-scene-editor/physics-scene-editor/issues) 中回報。
