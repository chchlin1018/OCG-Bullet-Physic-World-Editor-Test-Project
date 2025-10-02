# Physics Scene Editor - macOS CMake 版本

這是 Physics Scene Editor 的 macOS CMake 版本，**不需要完整的 Xcode IDE**，只需要 Command Line Tools 即可建置和執行。

## 🚀 快速解決方案

如果您遇到 `xcodebuild` 需要 Xcode 的錯誤，這個 CMake 版本是完美的替代方案！

### ✅ 優勢
- **無需 Xcode**：只需要免費的 Command Line Tools
- **更快建置**：CMake 比 Xcode 專案建置更快
- **跨平台相容**：與其他平台的 CMake 建置系統一致
- **自動化友好**：更適合 CI/CD 和自動化建置

## 系統需求

### 最低需求
- **macOS**: 10.15 (Catalina) 或更高版本
- **Command Line Tools**: Xcode Command Line Tools
- **記憶體**: 最少 4GB RAM，建議 8GB
- **硬碟空間**: 最少 2GB 可用空間

### 不需要
- ❌ 完整的 Xcode IDE
- ❌ Xcode 開發者帳號
- ❌ 複雜的專案設定

## 快速開始

### 1. 安裝 Command Line Tools

```bash
# 安裝 Command Line Tools（如果尚未安裝）
xcode-select --install
```

### 2. 一鍵建置

```bash
# 進入 macOS CMake 目錄
cd macos_cmake

# 安裝依賴並建置（推薦）
./build_macos.sh -d -b

# 或者分步執行
./build_macos.sh -d    # 安裝依賴
./build_macos.sh -b    # 建置專案
```

### 3. 執行應用程式

```bash
# 執行應用程式
./build_macos.sh -r

# 或直接開啟
open build/PhysicsSceneEditorMacOS.app
```

## 詳細安裝指南

### 檢查系統狀態

```bash
# 檢查 macOS 版本
sw_vers

# 檢查 Command Line Tools
xcode-select -p

# 檢查系統資訊
./build_macos.sh --info
```

### 手動安裝依賴

如果自動安裝失敗，可以手動安裝：

```bash
# 安裝 Homebrew（如果尚未安裝）
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安裝必要套件
brew install cmake pkg-config qt6
brew install glfw glew glm eigen bullet nlohmann-json assimp
```

### 手動建置

```bash
# 建立建置目錄
mkdir build && cd build

# CMake 配置
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
         -DQt6_DIR=$(brew --prefix qt6)/lib/cmake/Qt6

# 編譯
make -j$(sysctl -n hw.ncpu)

# 執行
open PhysicsSceneEditorMacOS.app
```

## 建置選項

### 基本選項

```bash
# 建置 Debug 版本
./build_macos.sh -b --debug

# 建置 Release 版本（預設）
./build_macos.sh -b --release

# 清理並重新建置
./build_macos.sh -c -b

# 建置並執行測試
./build_macos.sh -b -t
```

### 進階選項

```bash
# 建立 DMG 安裝檔
./build_macos.sh -b -m

# 完整流程：安裝依賴、建置、測試、打包
./build_macos.sh -d -b -t -m

# 建置並立即執行
./build_macos.sh -b -r
```

## 專案結構

```
macos_cmake/
├── CMakeLists.txt              # CMake 建置配置
├── build_macos.sh              # 自動化建置腳本
├── README_MACOS_CMAKE.md       # 本檔案
├── Info.plist.in               # macOS 應用程式資訊模板
├── include/                    # macOS 特定標頭檔
│   ├── MacOSApplication.h      # macOS 應用程式類別
│   └── MacOSMainWindow.h       # macOS 主視窗類別
├── src/                        # macOS 特定源碼
│   ├── main.cpp               # 主程式進入點
│   ├── MacOSApplication.cpp   # macOS 應用程式實現
│   └── MacOSMainWindow.cpp    # macOS 主視窗實現
├── resources/                  # 資源檔案
│   ├── icons.qrc              # 圖示資源
│   └── shaders.qrc            # 著色器資源
├── tests/                      # 測試程式
└── build/                      # 建置輸出目錄
```

## 功能特色

### macOS 原生整合

- **App Bundle**：標準的 macOS 應用程式包結構
- **檔案關聯**：支援 .pscene、.fbx、.obj 檔案
- **拖拽支援**：直接拖拽檔案到應用程式
- **Retina 支援**：高解析度顯示器最佳化
- **深色模式**：自動適應系統外觀

### 物理引擎

- **OGC 支援**：最新的 Offset Geometric Contact 模型
- **Bullet Physics**：成熟的物理引擎
- **混合模式**：智慧切換演算法
- **GPU 加速**：CUDA 支援（可選）

### 開發友好

- **CMake 建置**：現代化的建置系統
- **自動依賴**：自動檢測和安裝依賴
- **並行編譯**：多核心並行建置
- **測試支援**：內建測試框架

## 疑難排解

### 常見問題

**Q: Command Line Tools 安裝失敗**
A: 嘗試以下解決方案：
```bash
# 重新安裝 Command Line Tools
sudo rm -rf /Library/Developer/CommandLineTools
xcode-select --install

# 或手動下載安裝
# 從 https://developer.apple.com/download/more/ 下載
```

**Q: Homebrew 安裝失敗**
A: 檢查網路連線和權限：
```bash
# 檢查網路
curl -I https://raw.githubusercontent.com

# 檢查權限
sudo chown -R $(whoami) /usr/local/Homebrew
```

**Q: Qt6 找不到**
A: 手動設定 Qt 路徑：
```bash
# 設定 Qt 路徑
export QT_DIR=$(brew --prefix qt6)
export CMAKE_PREFIX_PATH="$QT_DIR:$CMAKE_PREFIX_PATH"

# 或在建置時指定
cmake .. -DQt6_DIR=$(brew --prefix qt6)/lib/cmake/Qt6
```

**Q: 編譯錯誤：找不到標頭檔**
A: 檢查依賴安裝：
```bash
# 檢查 pkg-config 套件
pkg-config --list-all | grep -E "(glfw|glew|bullet|eigen)"

# 重新安裝問題套件
brew reinstall glfw glew bullet eigen
```

**Q: 應用程式無法啟動**
A: 檢查依賴和權限：
```bash
# 檢查應用程式依賴
otool -L build/PhysicsSceneEditorMacOS.app/Contents/MacOS/PhysicsSceneEditorMacOS

# 使用 macdeployqt 打包依賴
$(brew --prefix qt6)/bin/macdeployqt build/PhysicsSceneEditorMacOS.app
```

### 除錯技巧

```bash
# 啟用詳細輸出
./build_macos.sh -b --debug

# 檢查 CMake 配置
cd build && cmake .. -LAH

# 檢查編譯日誌
make VERBOSE=1

# 執行時除錯
export QT_LOGGING_RULES="*.debug=true"
./PhysicsSceneEditorMacOS.app/Contents/MacOS/PhysicsSceneEditorMacOS
```

## 效能最佳化

### 編譯時最佳化

```bash
# 使用 Release 模式
cmake .. -DCMAKE_BUILD_TYPE=Release

# 啟用 LTO（連結時最佳化）
cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

# 針對本機 CPU 最佳化
cmake .. -DCMAKE_CXX_FLAGS="-march=native"
```

### 執行時最佳化

```bash
# 停用除錯輸出
export QT_LOGGING_RULES="*.debug=false"

# 啟用 GPU 加速（如果支援）
export ENABLE_CUDA=1

# 設定物理引擎
export PHYSICS_ENGINE=hybrid  # ogc, bullet, hybrid
```

## 部署和分發

### 建立 DMG 安裝檔

```bash
# 自動建立 DMG
./build_macos.sh -b -m

# 手動建立 DMG
hdiutil create -volname "Physics Scene Editor" \
               -srcfolder build/PhysicsSceneEditorMacOS.app \
               -ov -format UDZO \
               PhysicsSceneEditor.dmg
```

### 程式碼簽署（可選）

```bash
# 簽署應用程式（需要開發者憑證）
codesign --deep --force --verify --verbose \
         --sign "Developer ID Application: Your Name" \
         build/PhysicsSceneEditorMacOS.app

# 簽署 DMG
codesign --sign "Developer ID Application: Your Name" \
         PhysicsSceneEditor.dmg
```

## 與其他版本比較

| 特性 | Xcode 專案 | CMake 版本 |
|------|------------|------------|
| **需要 Xcode** | ✅ 完整 Xcode | ❌ 只需 Command Line Tools |
| **建置速度** | 中等 | 快速 |
| **IDE 整合** | 完美 | 良好 |
| **自動化** | 困難 | 簡單 |
| **跨平台** | macOS 專用 | 跨平台相容 |
| **學習曲線** | 陡峭 | 平緩 |
| **檔案大小** | 大 | 小 |

## 開發指南

### 新增功能

1. **新增源碼檔案**
   ```cmake
   # 在 CMakeLists.txt 中新增
   list(APPEND SOURCES src/new_feature.cpp)
   list(APPEND HEADERS include/new_feature.h)
   ```

2. **新增依賴**
   ```cmake
   # 尋找新的依賴
   find_package(NewLibrary REQUIRED)
   target_link_libraries(PhysicsSceneEditorMacOS NewLibrary::NewLibrary)
   ```

3. **新增測試**
   ```cmake
   # 新增測試檔案
   list(APPEND TEST_SOURCES tests/test_new_feature.cpp)
   ```

### 除錯配置

```cmake
# 在 CMakeLists.txt 中新增除錯選項
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(PhysicsSceneEditorMacOS PRIVATE
        DEBUG_BUILD
        ENABLE_LOGGING
    )
    target_compile_options(PhysicsSceneEditorMacOS PRIVATE
        -g -O0 -fsanitize=address
    )
endif()
```

## 貢獻指南

### 開發環境設定

```bash
# Fork 並 clone 專案
git clone https://github.com/your-username/OCG-Bullet-Physic-World-Editor-Test-Project.git
cd OCG-Bullet-Physic-World-Editor-Test-Project/macos_cmake

# 設定開發分支
git checkout -b feature/your-feature

# 安裝開發依賴
./build_macos.sh -d

# 建置並測試
./build_macos.sh -b -t
```

### 程式碼風格

- 遵循 Qt 程式碼風格指南
- 使用 clang-format 格式化程式碼
- 添加適當的註解和文件
- 編寫單元測試

### 提交流程

1. 實現功能並測試
2. 更新文件
3. 執行完整測試
4. 建立 Pull Request

## 授權和致謝

### 授權條款
本專案採用 MIT 授權條款，詳見根目錄的 [LICENSE](../LICENSE) 檔案。

### 致謝
- **CMake 社群**：優秀的跨平台建置系統
- **Homebrew**：macOS 套件管理器
- **Qt 社群**：跨平台應用程式框架
- **所有貢獻者**：感謝每一位為專案做出貢獻的開發者

## 聯絡資訊

- **專案主頁**：https://github.com/chchlin1018/OCG-Bullet-Physic-World-Editor-Test-Project
- **問題回報**：https://github.com/chchlin1018/OCG-Bullet-Physic-World-Editor-Test-Project/issues
- **macOS CMake 特定問題**：請在 issue 中標註 `[macOS-CMake]`
- **電子郵件**：macos-cmake-support@physics-scene-editor.com

---

**Physics Scene Editor macOS CMake** - 無需 Xcode 的 macOS 原生物理場景編輯器

*Command Line Tools 即可 | CMake 現代建置 | 完整功能支援*
