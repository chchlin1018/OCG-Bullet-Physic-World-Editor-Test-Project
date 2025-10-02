# Physics Scene Editor - Xcode 專案

這是 Physics Scene Editor 的 macOS Xcode 專案版本，基於 Qt 跨平台編輯器建構，專為 macOS 平台最佳化。

## 專案概述

### 🍎 原生 macOS 體驗
- **Xcode 整合**：完整的 Xcode IDE 支援
- **macOS 最佳化**：針對 macOS 平台的特殊最佳化
- **原生界面**：符合 macOS 設計指南的使用者界面
- **系統整合**：深度整合 macOS 系統功能

### ⚡ 核心功能
- **OGC 物理引擎**：最新的 Offset Geometric Contact 模型
- **Bullet Physics**：成熟的物理引擎支援
- **Qt6 界面**：現代化的跨平台 UI 框架
- **OpenGL 渲染**：硬體加速的 3D 渲染
- **CUDA 支援**：GPU 並行計算加速（可選）

## 系統需求

### 開發環境
- **macOS**: 10.15 (Catalina) 或更高版本
- **Xcode**: 12.0 或更高版本
- **Command Line Tools**: 最新版本
- **Homebrew**: 套件管理器（推薦）

### 執行環境
- **macOS**: 10.15 或更高版本
- **記憶體**: 最少 4GB RAM，建議 8GB
- **顯示卡**: 支援 OpenGL 3.3 或更高版本
- **硬碟空間**: 最少 2GB 可用空間

## 安裝和設定

### 1. 安裝依賴

#### 使用 Homebrew（推薦）
```bash
# 安裝 Homebrew（如果尚未安裝）
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安裝必要依賴
brew install qt6 cmake pkg-config
brew install glfw glew glm eigen
brew install bullet nlohmann-json
brew install assimp  # 3D 模型載入支援
```

#### 手動安裝 Qt6
```bash
# 下載 Qt6 安裝程式
# 從 https://www.qt.io/download 下載 Qt Online Installer
# 安裝 Qt6.5 或更高版本，包含以下模組：
# - Qt Core
# - Qt Widgets  
# - Qt OpenGL
# - Qt OpenGL Widgets
```

### 2. 設定環境變數

在 `~/.zshrc` 或 `~/.bash_profile` 中新增：

```bash
# Qt6 路徑（根據實際安裝路徑調整）
export QT_DIR=/usr/local/qt6
# 或者如果使用 Homebrew 安裝的 Qt
export QT_DIR=$(brew --prefix qt6)

# 更新 PATH
export PATH="$QT_DIR/bin:$PATH"

# 設定框架路徑
export DYLD_FRAMEWORK_PATH="$QT_DIR/lib:$DYLD_FRAMEWORK_PATH"
```

重新載入設定：
```bash
source ~/.zshrc  # 或 source ~/.bash_profile
```

### 3. 驗證安裝

```bash
# 檢查 Qt 安裝
qmake --version
moc --version

# 檢查其他依賴
pkg-config --modversion bullet
pkg-config --modversion glfw3
```

## 建置專案

### 使用 Xcode GUI

1. **開啟專案**
   ```bash
   open PhysicsSceneEditor.xcodeproj
   ```

2. **設定建置目標**
   - 選擇 "PhysicsSceneEditor" scheme
   - 選擇目標裝置（My Mac）

3. **配置建置設定**
   - 檢查 "Build Settings" 中的路徑設定
   - 確認 `QT_DIR` 環境變數正確設定
   - 驗證框架搜尋路徑

4. **建置專案**
   - 按 `Cmd+B` 建置專案
   - 或選擇 Product → Build

5. **執行應用程式**
   - 按 `Cmd+R` 執行應用程式
   - 或選擇 Product → Run

### 使用命令列

```bash
# 進入專案目錄
cd xcode_project

# 建置專案
xcodebuild -project PhysicsSceneEditor.xcodeproj -scheme PhysicsSceneEditor -configuration Release

# 執行應用程式
open build/Release/PhysicsSceneEditor.app
```

## 專案結構

```
xcode_project/
├── PhysicsSceneEditor.xcodeproj/     # Xcode 專案檔案
│   ├── project.pbxproj              # 專案設定
│   └── xcshareddata/                # 共享設定
│       └── xcschemes/               # Build schemes
├── PhysicsSceneEditor/              # 主要應用程式
│   ├── Sources/                     # C++ 源碼檔案
│   │   ├── main.cpp                # 應用程式進入點
│   │   ├── MainWindow.cpp          # 主視窗實現
│   │   ├── SceneTreeWidget.cpp     # 場景樹狀檢視
│   │   ├── PropertyWidget.cpp      # 屬性編輯器
│   │   ├── ViewportWidget.cpp      # 3D 視口
│   │   ├── physics_scene_format.cpp # 場景格式處理
│   │   ├── scene_loader.cpp        # 場景載入器
│   │   ├── physics_engine.cpp      # 物理引擎
│   │   └── renderer.cpp            # OpenGL 渲染器
│   ├── Headers/                     # 標頭檔案
│   │   ├── MainWindow.h            # 主視窗定義
│   │   ├── SceneTreeWidget.h       # 場景樹狀檢視
│   │   ├── PropertyWidget.h        # 屬性編輯器
│   │   ├── ViewportWidget.h        # 3D 視口
│   │   ├── physics_scene_format.h  # 場景格式定義
│   │   ├── scene_loader.h          # 場景載入器
│   │   ├── physics_engine.h        # 物理引擎
│   │   └── renderer.h              # OpenGL 渲染器
│   ├── Resources/                   # 資源檔案
│   ├── Frameworks/                  # 外部框架
│   ├── Info.plist                  # 應用程式資訊
│   └── PhysicsSceneEditor.entitlements # 權限設定
├── External/                        # 外部依賴
└── README_XCODE.md                 # 本檔案
```

## 建置設定

### Debug 配置
- **最佳化等級**: 無最佳化 (-O0)
- **除錯資訊**: 完整除錯符號
- **預處理器定義**: DEBUG=1
- **警告等級**: 所有警告

### Release 配置
- **最佳化等級**: 速度最佳化 (-O2)
- **除錯資訊**: 最小除錯資訊
- **程式碼簽署**: 自動簽署
- **部署目標**: macOS 10.15

### 自訂建置設定

#### 環境變數
- `QT_DIR`: Qt6 安裝路徑
- `DYLD_FRAMEWORK_PATH`: 框架搜尋路徑

#### 編譯器旗標
- `-DQT_NO_KEYWORDS`: 避免 Qt 關鍵字衝突
- `-DQT_DEPRECATED_WARNINGS`: 啟用過時警告
- `-DENABLE_OGC`: 啟用 OGC 物理引擎
- `-DENABLE_BULLET`: 啟用 Bullet Physics

#### 連結器設定
- 連結 Qt 框架：QtCore, QtWidgets, QtGui, QtOpenGL, QtOpenGLWidgets
- 連結系統框架：OpenGL, Cocoa
- 連結第三方函式庫：Bullet Physics, GLM, Eigen

## 除錯和測試

### 使用 Xcode 除錯器

1. **設定中斷點**
   - 在程式碼行號左側點擊設定中斷點
   - 使用條件中斷點進行精確除錯

2. **變數檢視**
   - 在除錯模式下檢視變數值
   - 使用 "Variables View" 監控物件狀態

3. **記憶體除錯**
   - 啟用 Address Sanitizer 檢測記憶體問題
   - 使用 Instruments 進行效能分析

### 日誌和診斷

```cpp
// 使用 Qt 日誌系統
qDebug() << "Debug message";
qWarning() << "Warning message";
qCritical() << "Critical error";

// 設定日誌過濾
export QT_LOGGING_RULES="*.debug=false;app.debug=true"
```

### 單元測試

```bash
# 執行單元測試（如果已實現）
xcodebuild test -project PhysicsSceneEditor.xcodeproj -scheme PhysicsSceneEditor
```

## 效能最佳化

### 編譯時最佳化

1. **啟用 Link-Time Optimization (LTO)**
   ```
   LLVM_LTO = YES
   ```

2. **使用 Release 配置**
   - 自動啟用編譯器最佳化
   - 移除除錯符號以減少檔案大小

3. **Profile-Guided Optimization**
   - 使用 Instruments 收集效能資料
   - 根據實際使用模式最佳化

### 執行時最佳化

1. **記憶體管理**
   - 使用智慧指標避免記憶體洩漏
   - 實現物件池減少記憶體分配

2. **渲染最佳化**
   - 啟用視錐剔除
   - 使用 LOD (Level of Detail) 系統
   - 批次渲染相似物件

3. **物理模擬最佳化**
   - 使用空間分割加速碰撞檢測
   - 啟用物體睡眠機制
   - 根據場景複雜度選擇適當的物理演算法

## 部署和分發

### 建立 App Bundle

```bash
# 建置 Release 版本
xcodebuild -project PhysicsSceneEditor.xcodeproj -scheme PhysicsSceneEditor -configuration Release

# 使用 macdeployqt 打包依賴
macdeployqt build/Release/PhysicsSceneEditor.app

# 建立 DMG 安裝檔
hdiutil create -volname "Physics Scene Editor" -srcfolder build/Release/PhysicsSceneEditor.app -ov -format UDZO PhysicsSceneEditor.dmg
```

### 程式碼簽署和公證

```bash
# 簽署應用程式
codesign --deep --force --verify --verbose --sign "Developer ID Application: Your Name" PhysicsSceneEditor.app

# 建立簽署的 DMG
codesign --sign "Developer ID Application: Your Name" PhysicsSceneEditor.dmg

# 提交公證（需要 Apple Developer 帳號）
xcrun altool --notarize-app --primary-bundle-id "com.physicssceneeditor.macos" --username "your-apple-id" --password "@keychain:AC_PASSWORD" --file PhysicsSceneEditor.dmg
```

## 疑難排解

### 常見問題

**Q: 編譯時找不到 Qt 標頭檔**
A: 檢查以下設定：
```bash
# 確認 QT_DIR 環境變數
echo $QT_DIR

# 檢查 Qt 安裝
ls $QT_DIR/include

# 在 Xcode 中檢查 Header Search Paths
```

**Q: 執行時找不到 Qt 框架**
A: 設定正確的框架路徑：
```bash
# 設定 DYLD_FRAMEWORK_PATH
export DYLD_FRAMEWORK_PATH="$QT_DIR/lib:$DYLD_FRAMEWORK_PATH"

# 或使用 macdeployqt 打包框架
macdeployqt PhysicsSceneEditor.app
```

**Q: 物理模擬效能不佳**
A: 最佳化建議：
- 檢查是否啟用 Release 模式
- 減少物件數量或使用簡化碰撞形狀
- 啟用 CUDA 加速（如果有支援的顯示卡）
- 調整物理模擬參數

**Q: OpenGL 渲染問題**
A: 檢查項目：
- 確認顯示卡支援 OpenGL 3.3+
- 更新顯示卡驅動程式
- 檢查 OpenGL 上下文設定

### 日誌收集

```bash
# 啟用詳細日誌
export QT_LOGGING_RULES="*.debug=true"

# 執行應用程式並收集日誌
./PhysicsSceneEditor.app/Contents/MacOS/PhysicsSceneEditor > app.log 2>&1

# 檢查系統日誌
log show --predicate 'process == "PhysicsSceneEditor"' --last 1h
```

## 貢獻和開發

### 開發環境設定

1. **Fork 專案**
   ```bash
   git clone https://github.com/your-username/OCG-Bullet-Physic-World-Editor-Test-Project.git
   cd OCG-Bullet-Physic-World-Editor-Test-Project/xcode_project
   ```

2. **設定開發分支**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **配置 Xcode**
   - 開啟 PhysicsSceneEditor.xcodeproj
   - 設定開發團隊和簽署憑證
   - 配置建置設定

### 程式碼風格

- 遵循 Qt 程式碼風格指南
- 使用 clang-format 格式化程式碼
- 添加適當的註解和文件
- 編寫單元測試

### 提交流程

1. 實現功能並測試
2. 執行程式碼格式化
3. 更新文件
4. 建立 Pull Request
5. 通過程式碼審查

## 授權和致謝

### 授權條款
本專案採用 MIT 授權條款，詳見根目錄的 [LICENSE](../LICENSE) 檔案。

### 致謝
- **Apple**：優秀的 macOS 開發平台和工具
- **Qt 社群**：跨平台應用程式框架
- **Homebrew**：macOS 套件管理器
- **所有貢獻者**：感謝每一位為專案做出貢獻的開發者

## 聯絡資訊

- **專案主頁**：https://github.com/chchlin1018/OCG-Bullet-Physic-World-Editor-Test-Project
- **問題回報**：https://github.com/chchlin1018/OCG-Bullet-Physic-World-Editor-Test-Project/issues
- **macOS 特定問題**：請在 issue 中標註 `[macOS]` 或 `[Xcode]`
- **電子郵件**：macos-support@physics-scene-editor.com

---

**Physics Scene Editor Xcode** - 專為 macOS 最佳化的專業物理場景編輯器

*原生 macOS 體驗 | Xcode 完整整合 | 先進物理模擬*
