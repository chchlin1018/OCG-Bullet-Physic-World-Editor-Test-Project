# Physics Scene Editor - macOS 原生版本

這是專為 macOS 設計的 Physics Scene Editor 原生建置系統，支援 Intel 和 Apple Silicon Mac。

## 🍎 macOS 特色

### 原生 macOS 體驗
- **完整的 macOS 應用程式 Bundle**：符合 Apple 設計規範
- **Retina 顯示器支援**：高解析度圖形渲染
- **macOS 整合**：檔案關聯、拖拽支援、原生選單列
- **Apple Silicon 最佳化**：原生 ARM64 支援

### 專業建置系統
- **Homebrew 整合**：自動依賴管理
- **CMake 現代化建置**：比 Xcode 更快的編譯速度
- **DMG 打包**：專業的安裝檔案建立
- **程式碼簽名準備**：支援 App Store 分發

## 🚀 快速開始

### 系統需求
- **macOS 10.15+**（Catalina 或更新版本）
- **Xcode Command Line Tools**
- **Homebrew**（腳本會自動安裝）

### 一鍵安裝和建置
```bash
# 下載專案
git clone https://github.com/chchlin1018/OCG-Bullet-Physic-World-Editor-Test-Project.git
cd OCG-Bullet-Physic-World-Editor-Test-Project/macos_cmake

# 使用 macOS 原生建置腳本
./build_macos_native.sh -d -b
```

### 詳細步驟

#### 1. 安裝 Xcode Command Line Tools
```bash
xcode-select --install
```

#### 2. 安裝依賴並建置
```bash
# 安裝 Homebrew 和所有依賴
./build_macos_native.sh -d

# 建置專案
./build_macos_native.sh -b

# 或者一次完成
./build_macos_native.sh -d -b
```

#### 3. 執行應用程式
```bash
# 執行應用程式
./build_macos_native.sh -r

# 或手動執行
open build/PhysicsSceneEditor.app
```

## 🛠️ 建置選項

### 基本選項
```bash
# 顯示說明
./build_macos_native.sh --help

# 顯示系統資訊
./build_macos_native.sh --info

# 清理並重新建置
./build_macos_native.sh -c -b

# Debug 模式建置
./build_macos_native.sh -b --debug
```

### 進階選項
```bash
# 建置並執行測試
./build_macos_native.sh -b -t

# 建立 DMG 安裝檔
./build_macos_native.sh -b -m

# 完整流程（清理、建置、測試、打包、執行）
./build_macos_native.sh -c -b -t -m -r
```

## 📦 依賴套件

腳本會自動透過 Homebrew 安裝以下套件：

### 核心依賴
- **Qt6**：現代化 GUI 框架
- **CMake**：建置系統
- **pkg-config**：套件配置工具

### 圖形和物理
- **OpenGL**：3D 圖形渲染
- **GLFW**：視窗和輸入管理
- **GLEW**：OpenGL 擴展載入
- **GLM**：數學函式庫

### 物理引擎
- **Bullet Physics**：成熟的物理模擬引擎
- **Eigen3**：線性代數函式庫

### 檔案格式
- **nlohmann-json**：JSON 處理
- **Assimp**：3D 模型載入

## 🎯 輸出檔案

建置完成後，您會得到：

### 應用程式 Bundle
```
build/PhysicsSceneEditor.app/
├── Contents/
│   ├── Info.plist          # 應用程式資訊
│   ├── MacOS/
│   │   └── PhysicsSceneEditor  # 主執行檔
│   ├── Resources/          # 資源檔案
│   └── Frameworks/         # Qt 框架（如果使用 macdeployqt）
```

### DMG 安裝檔（可選）
```
build/PhysicsSceneEditor.dmg    # 可分發的安裝檔
```

## 🔧 疑難排解

### 常見問題

#### 1. Xcode Command Line Tools 未安裝
```bash
# 錯誤：xcode-select: error: invalid developer directory
xcode-select --install
```

#### 2. Homebrew 未安裝
```bash
# 腳本會自動安裝，或手動安裝：
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

#### 3. Qt6 找不到
```bash
# 重新安裝 Qt6
brew uninstall qt@6
brew install qt@6
```

#### 4. 編譯錯誤
```bash
# 清理並重新建置
./build_macos_native.sh -c -b --debug
```

### Apple Silicon 特殊注意事項

#### Homebrew 路徑
Apple Silicon Mac 上，Homebrew 安裝在 `/opt/homebrew/`，腳本會自動處理路徑設定。

#### Rosetta 2
如果遇到相容性問題，可以嘗試在 Rosetta 2 模式下執行：
```bash
arch -x86_64 ./build_macos_native.sh -d -b
```

## 🌟 進階功能

### 自訂建置
```bash
# 設定環境變數
export CMAKE_BUILD_TYPE=Debug
export QT_DIR=/opt/homebrew/opt/qt@6

# 手動 CMake 建置
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
```

### 程式碼簽名（App Store 分發）
```bash
# 建置後簽名
codesign --force --deep --sign "Developer ID Application: Your Name" build/PhysicsSceneEditor.app
```

### 公證（Notarization）
```bash
# 建立公證用的 DMG
./build_macos_native.sh -b -m
xcrun notarytool submit build/PhysicsSceneEditor.dmg --keychain-profile "AC_PASSWORD"
```

## 📊 效能比較

| 特性 | Xcode 專案 | **macOS 原生腳本** |
|------|------------|-------------------|
| **安裝大小** | ~10GB | ~500MB |
| **建置速度** | 中等 | **🚀 快速** |
| **自動化** | 困難 | **✅ 簡單** |
| **依賴管理** | 手動 | **🤖 自動** |
| **DMG 建立** | 複雜 | **📦 一鍵** |
| **CI/CD 支援** | 有限 | **💯 完整** |

## 🎊 總結

這個 macOS 原生建置系統提供了：

- ✅ **完整的 macOS 體驗**：原生應用程式 Bundle
- ✅ **專業的建置流程**：從依賴到分發的完整自動化
- ✅ **現代化工具鏈**：Homebrew + CMake + Qt6
- ✅ **跨架構支援**：Intel 和 Apple Silicon
- ✅ **分發準備**：DMG 打包和簽名支援

無論您是 macOS 開發新手還是專家，這個建置系統都能為您提供最佳的開發體驗！
