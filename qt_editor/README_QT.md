# Physics Scene Editor Qt 版本

這是 Physics Scene Editor 的 Qt 跨平台版本，提供與 Windows MFC 版本相同的功能，但支援 Windows、macOS 和 Linux 平台。

## 主要特色

### 🌍 真正的跨平台支援
- **Windows**：完整支援 Windows 10/11
- **macOS**：原生 macOS 應用程式
- **Linux**：支援主要 Linux 發行版

### 🎯 功能對等性
- 與 MFC 版本功能完全對等
- 相同的場景格式相容性
- 統一的使用者體驗

### 🛠️ 現代化界面
- 基於 Qt6 的現代化界面
- 響應式設計
- 支援高 DPI 顯示器
- 可客製化的工作區佈局

### ⚡ 高效能渲染
- 基於 OpenGL 的硬體加速渲染
- 即時物理模擬預覽
- 多執行緒渲染管道

## 系統需求

### 通用需求
- **記憶體**：最少 4GB RAM，建議 8GB
- **顯示卡**：支援 OpenGL 3.3 或更高版本
- **硬碟空間**：最少 2GB 可用空間

### Windows
- Windows 10 或更高版本 (64-bit)
- Visual C++ Redistributable 2019 或更高版本

### macOS
- macOS 10.15 (Catalina) 或更高版本
- 支援 Intel 和 Apple Silicon (M1/M2) 處理器

### Linux
- 現代 Linux 發行版 (Ubuntu 20.04+, Fedora 34+, 等)
- X11 或 Wayland 顯示伺服器
- 相關圖形驅動程式

## 快速開始

### 1. 安裝依賴

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev libqt6opengl6-dev \
                 libgl1-mesa-dev libglfw3-dev libglew-dev libglm-dev \
                 libbullet-dev libeigen3-dev nlohmann-json3-dev
```

#### macOS (使用 Homebrew)
```bash
brew install qt6 glfw glew glm bullet eigen nlohmann-json
```

#### Windows (使用 vcpkg)
```cmd
vcpkg install qt6:x64-windows glfw3:x64-windows glew:x64-windows ^
              glm:x64-windows bullet3:x64-windows eigen3:x64-windows ^
              nlohmann-json:x64-windows
```

### 2. 建構專案

#### 使用建構腳本（推薦）
```bash
# 安裝依賴並建構
./build_qt.sh -d

# 僅建構
./build_qt.sh

# 建構並執行測試
./build_qt.sh -t

# 建立安裝包
./build_qt.sh -p
```

#### 手動建構
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

### 3. 執行編輯器

```bash
# Linux/macOS
./build/PhysicsSceneEditorQt

# Windows
.\build\Release\PhysicsSceneEditorQt.exe
```

## 主要功能

### 場景編輯
- **視覺化編輯**：拖拽式物件放置和編輯
- **場景樹狀檢視**：階層式場景管理
- **屬性編輯器**：即時屬性編輯和驗證
- **材質編輯器**：完整的物理和視覺材質管理

### 3D 視口
- **多種檢視模式**：實體、線框、除錯模式
- **變換工具**：移動、旋轉、縮放操作器
- **相機控制**：軌道、平移、縮放操作
- **即時預覽**：物理模擬即時預覽

### 物理模擬
- **OGC 整合**：最新的 Offset Geometric Contact 模型
- **Bullet Physics**：成熟的物理引擎支援
- **混合模式**：智慧的演算法切換
- **CUDA 加速**：GPU 並行計算支援

### 檔案格式
- **標準 .pscene 格式**：與其他版本完全相容
- **FBX 匯入**：支援複雜 3D 模型匯入
- **多種匯出格式**：支援多種標準格式匯出

## 界面介紹

### 主視窗佈局

```
┌─────────────────────────────────────────────────────────┐
│ 選單列                                                   │
├─────────────────────────────────────────────────────────┤
│ 工具列                                                   │
├───────────┬─────────────────────────────┬───────────────┤
│           │                             │               │
│ 場景樹狀  │        3D 視口              │   屬性編輯器  │
│ 檢視      │                             │               │
│           │                             │               │
├───────────┼─────────────────────────────┼───────────────┤
│           │                             │               │
│ 工具箱    │        輸出視窗             │   材質編輯器  │
│           │                             │               │
└───────────┴─────────────────────────────┴───────────────┘
```

### 場景樹狀檢視
- 顯示場景中所有物件的階層結構
- 支援拖拽重新排序和分組
- 右鍵選單提供快速操作
- 搜尋和篩選功能

### 屬性編輯器
- 根據選中物件動態顯示屬性
- 支援多種資料類型編輯
- 即時驗證和錯誤提示
- 預設值和預設組合

### 3D 視口
- 硬體加速的 OpenGL 渲染
- 多種檢視和投影模式
- 直觀的變換操作器
- 豐富的視覺化選項

### 材質編輯器
- 物理材質屬性編輯
- 視覺材質和紋理管理
- 即時預覽效果
- 材質庫管理

## 進階功能

### 自訂工作區
```cpp
// 儲存當前佈局
mainWindow->SaveLayout();

// 恢復預設佈局
mainWindow->ResetLayout();

// 顯示/隱藏停靠視窗
mainWindow->ShowDockWidget("SceneTree", true);
```

### 外掛系統
Qt 版本支援外掛擴展：

```cpp
// 載入外掛
PluginManager::LoadPlugin("MyCustomPlugin.dll");

// 註冊自訂工具
ToolManager::RegisterTool(new MyCustomTool());
```

### 腳本支援
支援 JavaScript 腳本自動化：

```javascript
// 建立新場景
scene.clear();

// 新增物件
var box = scene.addRigidBody("box", {
    position: [0, 5, 0],
    shape: "box",
    size: [1, 1, 1]
});

// 執行模擬
simulation.play();
```

## 效能最佳化

### 渲染最佳化
- **視錐剔除**：只渲染可見物件
- **LOD 系統**：根據距離調整細節層次
- **批次渲染**：減少繪製呼叫次數
- **多執行緒渲染**：並行渲染管道

### 物理最佳化
- **空間分割**：高效的碰撞檢測
- **睡眠機制**：靜止物體最佳化
- **適應性時間步長**：動態調整模擬精度
- **GPU 加速**：CUDA 並行計算

### 記憶體最佳化
- **物件池**：減少記憶體分配
- **紋理壓縮**：降低顯存使用
- **幾何體共享**：相同形狀共享資料
- **智慧快取**：常用資料快取

## 開發指南

### 建置環境設定

#### Qt Creator
1. 開啟 Qt Creator
2. 開啟專案檔案 `CMakeLists.txt`
3. 選擇適當的 Kit
4. 配置建置設定
5. 建置並執行

#### Visual Studio Code
1. 安裝 CMake Tools 擴展
2. 開啟專案資料夾
3. 配置 CMake 設定
4. 使用 Ctrl+Shift+P 執行 CMake 命令

#### CLion
1. 開啟專案資料夾
2. CLion 會自動檢測 CMake 專案
3. 配置工具鏈和建置類型
4. 建置並執行

### 除錯技巧

#### 啟用除錯輸出
```bash
export QT_LOGGING_RULES="*.debug=true"
./PhysicsSceneEditorQt --debug
```

#### 使用 Qt Creator 除錯器
1. 設定中斷點
2. 啟動除錯模式
3. 使用變數檢視器
4. 分析呼叫堆疊

#### 效能分析
```bash
# 使用 perf (Linux)
perf record ./PhysicsSceneEditorQt
perf report

# 使用 Instruments (macOS)
instruments -t "Time Profiler" ./PhysicsSceneEditorQt

# 使用 Visual Studio Profiler (Windows)
```

## 自訂和擴展

### 新增自訂 Widget
```cpp
class MyCustomWidget : public QWidget
{
    Q_OBJECT

public:
    MyCustomWidget(QWidget* parent = nullptr);
    
    // 實現自訂功能
    void SetData(const MyData& data);
    
signals:
    void DataChanged(const MyData& data);
};

// 註冊到主視窗
mainWindow->AddCustomWidget(new MyCustomWidget());
```

### 新增自訂工具
```cpp
class MyCustomTool : public Tool
{
public:
    QString GetName() const override { return "My Tool"; }
    QIcon GetIcon() const override { return QIcon(":/icons/mytool.png"); }
    
    void Activate() override;
    void Deactivate() override;
    void HandleMouseEvent(QMouseEvent* event) override;
};
```

### 新增自訂渲染器
```cpp
class MyCustomRenderer : public Renderer
{
public:
    void Initialize() override;
    void Render(const Scene& scene) override;
    void Cleanup() override;
};
```

## 疑難排解

### 常見問題

**Q: 編輯器無法啟動**
A: 檢查以下項目：
- Qt6 是否正確安裝
- OpenGL 驅動程式是否最新
- 相關動態函式庫是否可找到

**Q: 3D 視口顯示黑屏**
A: 可能的原因：
- OpenGL 版本不支援（需要 3.3+）
- 顯示卡驅動程式問題
- 著色器編譯失敗

**Q: 物理模擬效能不佳**
A: 最佳化建議：
- 減少物件數量
- 使用簡單碰撞形狀
- 啟用物體睡眠
- 使用 CUDA 加速

**Q: 場景檔案無法載入**
A: 檢查項目：
- 檔案格式是否正確
- 檔案是否損壞
- 版本相容性問題

### 日誌和除錯

#### 啟用詳細日誌
```bash
export QT_LOGGING_RULES="app.debug=true;physics.debug=true;renderer.debug=true"
```

#### 檢視系統資訊
```bash
./PhysicsSceneEditorQt --system-info
```

#### 產生除錯報告
```bash
./PhysicsSceneEditorQt --generate-debug-report
```

## 貢獻指南

### 程式碼風格
- 遵循 Qt 程式碼風格指南
- 使用 clang-format 格式化程式碼
- 添加適當的註解和文件

### 提交流程
1. Fork 專案
2. 建立功能分支
3. 實現功能並測試
4. 提交 Pull Request
5. 程式碼審查和合併

### 測試
- 編寫單元測試
- 執行整合測試
- 效能基準測試
- 跨平台測試

## 授權和致謝

### 授權條款
本專案採用 MIT 授權條款，詳見 [LICENSE](../LICENSE) 檔案。

### 致謝
- **Qt 社群**：優秀的跨平台應用程式框架
- **OpenGL 社群**：現代圖形渲染技術
- **所有貢獻者**：感謝每一位為專案做出貢獻的開發者

## 聯絡資訊

- **專案主頁**：https://github.com/physics-scene-editor/physics-scene-editor
- **問題回報**：https://github.com/physics-scene-editor/physics-scene-editor/issues
- **討論區**：https://github.com/physics-scene-editor/physics-scene-editor/discussions
- **電子郵件**：qt-support@physics-scene-editor.com

---

**Physics Scene Editor Qt** - 跨平台的專業物理場景編輯器

*版本 1.0.0 | 支援 Windows、macOS、Linux*
