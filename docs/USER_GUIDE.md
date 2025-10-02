# 使用者指南

歡迎使用 Physics Scene Editor！本指南將帶您了解如何使用這個強大的物理場景編輯和模擬工具。

## 目錄

- [快速入門](#快速入門)
- [MFC 編輯器使用](#mfc-編輯器使用)
- [跨平台執行器](#跨平台執行器)
- [場景格式詳解](#場景格式詳解)
- [進階功能](#進階功能)
- [最佳實踐](#最佳實踐)
- [常見問題](#常見問題)

## 快速入門

### 第一個物理場景

讓我們建立一個簡單的「掉落盒子」場景：

1. **啟動 MFC 編輯器**
   - 執行 `PhysicsSceneEditor.exe`
   - 選擇「新建場景」

2. **新增地面**
   - 在工具列點擊「平面」圖示
   - 在 3D 視口中點擊放置地面
   - 在屬性面板中設定：
     - 位置: (0, 0, 0)
     - 法線: (0, 1, 0)
     - 質量: 0 (靜態物體)

3. **新增盒子**
   - 點擊「盒子」圖示
   - 在地面上方放置盒子
   - 設定屬性：
     - 位置: (0, 5, 0)
     - 尺寸: (1, 1, 1)
     - 質量: 1.0

4. **設定重力**
   - 在「模擬設定」面板中
   - 設定重力: (0, -9.81, 0)

5. **儲存場景**
   - 檔案 → 儲存為 → `my_first_scene.pscene`

6. **執行模擬**
   - 使用跨平台執行器：
   ```bash
   ./PhysicsSceneRunner my_first_scene.pscene
   ```

## MFC 編輯器使用

### 主要界面

MFC 編輯器採用現代化的多面板設計：

```
┌─────────────────────────────────────────────────────────┐
│ 選單列                                                   │
├─────────────────────────────────────────────────────────┤
│ 工具列                                                   │
├───────────┬─────────────────────────────┬───────────────┤
│           │                             │               │
│ 場景樹狀  │        3D 視口              │   屬性面板    │
│ 檢視      │                             │               │
│           │                             │               │
├───────────┼─────────────────────────────┼───────────────┤
│           │                             │               │
│ 材質管理  │        輸出視窗             │   工具面板    │
│           │                             │               │
└───────────┴─────────────────────────────┴───────────────┘
```

### 場景樹狀檢視

場景樹狀檢視顯示場景中的所有物件，按類型組織：

- **剛體 (Rigid Bodies)**
  - 盒子、球體、圓柱體等
  - 右鍵選單：複製、刪除、重新命名
  - 拖拽排序和分組

- **約束 (Constraints)**
  - 鉸鏈、滑動、固定等約束
  - 顯示連接的物體

- **力場 (Force Fields)**
  - 重力場、磁場、風力等
  - 影響範圍視覺化

- **光源 (Lights)**
  - 方向光、點光源、聚光燈
  - 陰影設定

- **相機 (Cameras)**
  - 多相機支援
  - 視角切換

### 3D 視口操作

**滑鼠控制：**
- **左鍵拖拽**：旋轉視角
- **中鍵拖拽**：平移視角
- **滾輪**：縮放
- **右鍵**：物件選擇選單

**鍵盤快捷鍵：**
- `W/A/S/D`：相機移動
- `Q/E`：上下移動
- `F`：聚焦選中物件
- `G`：移動模式
- `R`：旋轉模式
- `S`：縮放模式
- `Delete`：刪除選中物件
- `Ctrl+C/V`：複製貼上
- `Ctrl+Z/Y`：復原/重做

**視口模式：**
- **實體模式**：完整材質渲染
- **線框模式**：顯示網格結構
- **除錯模式**：顯示碰撞形狀、約束等

### 屬性面板

屬性面板根據選中物件類型顯示相關屬性：

#### 剛體屬性
```
變換 (Transform)
├── 位置 (Position): X, Y, Z
├── 旋轉 (Rotation): 四元數或歐拉角
└── 縮放 (Scale): X, Y, Z

物理屬性 (Physics)
├── 質量 (Mass): 0 = 靜態物體
├── 線性阻尼 (Linear Damping)
├── 角度阻尼 (Angular Damping)
├── 碰撞群組 (Collision Group)
└── 碰撞遮罩 (Collision Mask)

形狀屬性 (Shape)
├── 形狀類型: 盒子/球體/圓柱體等
└── 形狀參數: 尺寸、半徑等

材質 (Materials)
├── 物理材質: 摩擦力、彈性等
└── 視覺材質: 顏色、紋理等
```

#### 約束屬性
```
約束類型 (Constraint Type)
├── 鉸鏈 (Hinge)
├── 滑動 (Slider)
├── 固定 (Fixed)
├── 點對點 (Point-to-Point)
└── 6DOF (Generic 6DOF)

連接設定 (Connection)
├── 物體 A (Body A)
├── 物體 B (Body B)
├── 錨點 A (Anchor A)
└── 錨點 B (Anchor B)

限制設定 (Limits)
├── 線性限制 (Linear Limits)
├── 角度限制 (Angular Limits)
└── 破壞閾值 (Breaking Threshold)
```

### 材質管理

材質管理器分為兩個部分：

#### 物理材質
- **密度 (Density)**：影響質量計算
- **摩擦力 (Friction)**：表面摩擦係數
- **彈性 (Restitution)**：碰撞彈性係數
- **滾動摩擦 (Rolling Friction)**：滾動阻力

#### 視覺材質
- **漫反射顏色 (Diffuse Color)**：基本顏色
- **鏡面反射 (Specular)**：反光強度
- **光澤度 (Shininess)**：表面光滑度
- **金屬度 (Metallic)**：金屬材質程度
- **粗糙度 (Roughness)**：表面粗糙程度
- **紋理貼圖**：漫反射、法線、鏡面反射等

### 模擬設定

在「模擬設定」面板中可以調整：

```
時間設定 (Timing)
├── 時間步長 (Time Step): 通常 0.016667 (60 FPS)
├── 最大子步數 (Max Sub Steps): 防止時間累積
└── 固定時間步長 (Fixed Time Step): 確保穩定性

物理設定 (Physics)
├── 重力 (Gravity): 世界重力向量
├── 求解器迭代 (Solver Iterations): 精度 vs 效能
├── 啟用睡眠 (Enable Sleeping): 靜止物體最佳化
└── 睡眠閾值 (Sleep Threshold): 睡眠觸發條件

OGC 設定 (OGC Settings)
├── 啟用 OGC (Enable OGC): 使用 OGC 接觸模型
├── 接觸半徑 (Contact Radius): OGC 參數
├── 混合模式 (Hybrid Mode): OGC + Bullet 混合
└── CUDA 加速 (CUDA Acceleration): GPU 加速
```

### 渲染設定

控制 3D 視口的視覺效果：

```
光照 (Lighting)
├── 環境光 (Ambient Light): 全域照明
├── 啟用陰影 (Enable Shadows): 即時陰影
├── 陰影品質 (Shadow Quality): 陰影映射解析度
└── 陰影距離 (Shadow Distance): 陰影渲染範圍

後處理 (Post-Processing)
├── 抗鋸齒 (Anti-Aliasing): MSAA/FXAA
├── 曝光 (Exposure): HDR 曝光控制
├── 伽馬校正 (Gamma Correction): 色彩校正
└── 色調映射 (Tone Mapping): HDR 到 LDR 轉換

除錯顯示 (Debug Display)
├── 顯示碰撞形狀 (Show Collision Shapes)
├── 顯示約束 (Show Constraints)
├── 顯示接觸點 (Show Contact Points)
└── 顯示統計資訊 (Show Statistics)
```

## 跨平台執行器

### 基本使用

```bash
# 基本執行
./PhysicsSceneRunner scene.pscene

# 指定視窗大小
./PhysicsSceneRunner --width 1920 --height 1080 scene.pscene

# 全螢幕模式
./PhysicsSceneRunner --fullscreen scene.pscene
```

### 命令列選項

```bash
# 物理設定
--ogc                    # 強制使用 OGC 模式
--bullet                 # 強制使用 Bullet 模式
--hybrid                 # 使用混合模式
--cuda                   # 啟用 CUDA 加速
--solver-iterations N    # 設定求解器迭代次數

# 渲染設定
--vsync                  # 啟用垂直同步
--msaa N                 # 多重採樣抗鋸齒 (N=2,4,8,16)
--shadows                # 啟用陰影
--no-shadows             # 停用陰影

# 除錯選項
--debug                  # 啟用除錯模式
--wireframe              # 線框渲染模式
--show-contacts          # 顯示接觸點
--show-constraints       # 顯示約束
--verbose                # 詳細輸出

# 效能選項
--benchmark              # 效能測試模式
--profile                # 效能分析
--fps-limit N            # 限制 FPS
--no-render              # 純物理模擬（無渲染）

# 輸出選項
--screenshot path        # 儲存截圖
--record path            # 錄製影片（需要 FFmpeg）
--export-data path       # 匯出模擬資料
```

### 即時控制

執行器支援即時鍵盤控制：

```
模擬控制
├── Space: 暫停/繼續
├── R: 重置場景
├── S: 單步執行
└── Esc: 退出

相機控制
├── WASD: 移動
├── QE: 上下移動
├── 滑鼠: 視角旋轉
└── 滾輪: 縮放

渲染模式
├── 1: 實體模式
├── 2: 線框模式
├── 3: 除錯模式
├── F: 顯示/隱藏 FPS
├── G: 顯示/隱藏網格
└── C: 顯示/隱藏約束

物理模式切換
├── O: 切換到 OGC 模式
├── B: 切換到 Bullet 模式
├── H: 切換混合模式
└── U: 切換 CUDA 加速
```

## 場景格式詳解

### 檔案結構

`.pscene` 檔案採用 JSON 格式，主要結構如下：

```json
{
  "version": "1.0.0",
  "metadata": {
    "name": "場景名稱",
    "description": "場景描述",
    "author": "作者",
    "created": "2025-01-02T10:00:00Z",
    "modified": "2025-01-02T10:00:00Z",
    "tags": ["標籤1", "標籤2"]
  },
  "simulationSettings": { ... },
  "renderSettings": { ... },
  "physicsMaterials": { ... },
  "visualMaterials": { ... },
  "rigidBodies": { ... },
  "constraints": { ... },
  "forceFields": { ... },
  "lights": { ... },
  "cameras": { ... }
}
```

### 剛體定義

```json
"rigidBodies": {
  "box1": {
    "name": "測試盒子",
    "transform": {
      "position": {"x": 0, "y": 5, "z": 0},
      "rotation": {"w": 1, "x": 0, "y": 0, "z": 0},
      "scale": {"x": 1, "y": 1, "z": 1}
    },
    "shapeType": "box",
    "shapeParameters": {
      "halfExtents": {"x": 0.5, "y": 0.5, "z": 0.5}
    },
    "mass": 1.0,
    "linearVelocity": {"x": 0, "y": 0, "z": 0},
    "angularVelocity": {"x": 0, "y": 0, "z": 0},
    "linearDamping": 0.1,
    "angularDamping": 0.1,
    "collisionGroup": 1,
    "collisionMask": 65535,
    "isTrigger": false,
    "physicsMaterial": "default",
    "visualMaterial": "red"
  }
}
```

### 支援的形狀類型

```json
// 盒子
"shapeType": "box",
"shapeParameters": {
  "halfExtents": {"x": 0.5, "y": 0.5, "z": 0.5}
}

// 球體
"shapeType": "sphere",
"shapeParameters": {
  "radius": 0.5
}

// 圓柱體
"shapeType": "cylinder",
"shapeParameters": {
  "radius": 0.5,
  "height": 2.0
}

// 膠囊
"shapeType": "capsule",
"shapeParameters": {
  "radius": 0.5,
  "height": 1.0
}

// 圓錐
"shapeType": "cone",
"shapeParameters": {
  "radius": 0.5,
  "height": 2.0
}

// 平面
"shapeType": "plane",
"shapeParameters": {
  "normal": {"x": 0, "y": 1, "z": 0},
  "distance": 0.0
}

// 網格
"shapeType": "mesh",
"shapeParameters": {
  "vertices": [...],
  "triangles": [...],
  "convex": true
}
```

### 約束定義

```json
"constraints": {
  "hinge1": {
    "name": "鉸鏈約束",
    "constraintType": "hinge",
    "bodyA": "box1",
    "bodyB": "box2",
    "frameA": {
      "position": {"x": 0.5, "y": 0, "z": 0},
      "rotation": {"w": 1, "x": 0, "y": 0, "z": 0}
    },
    "frameB": {
      "position": {"x": -0.5, "y": 0, "z": 0},
      "rotation": {"w": 1, "x": 0, "y": 0, "z": 0}
    },
    "linearLowerLimit": {"x": 0, "y": 0, "z": 0},
    "linearUpperLimit": {"x": 0, "y": 0, "z": 0},
    "angularLowerLimit": {"x": 0, "y": 0, "z": -1.57},
    "angularUpperLimit": {"x": 0, "y": 0, "z": 1.57},
    "breakingThreshold": 1000.0,
    "enableCollision": false
  }
}
```

### 力場定義

```json
"forceFields": {
  "wind": {
    "name": "風力場",
    "transform": {
      "position": {"x": -5, "y": 0, "z": 0},
      "rotation": {"w": 1, "x": 0, "y": 0, "z": 0},
      "scale": {"x": 1, "y": 1, "z": 1}
    },
    "forceFieldType": "directional",
    "direction": {"x": 1, "y": 0, "z": 0},
    "strength": 10.0,
    "radius": 5.0,
    "falloffExponent": 1.0,
    "affectedGroups": [1, 2]
  }
}
```

## 進階功能

### OGC 接觸模型

OGC (Offset Geometric Contact) 是基於最新學術研究的接觸處理方法：

**優勢：**
- 效能提升 2-3 個數量級
- 支援更大的接觸半徑
- 避免昂貴的連續碰撞檢測
- 完美適合 GPU 並行處理

**使用建議：**
```json
// 小型場景 (< 100 物件)
"useOGCContact": false,
"hybridMode": false

// 中型場景 (100-1000 物件)
"useOGCContact": true,
"hybridMode": true,
"ogcContactRadius": 0.1

// 大型場景 (> 1000 物件)
"useOGCContact": true,
"hybridMode": true,
"ogcContactRadius": 0.05,
"enableCUDA": true
```

### 混合物理模式

系統可以智慧地在 OGC 和 Bullet 之間切換：

- **簡單接觸**：使用 OGC 處理，效能最佳
- **複雜約束**：使用 Bullet 處理，精度最高
- **自動切換**：根據場景複雜度動態選擇

### CUDA 加速

啟用 CUDA 加速可以大幅提升大型場景的模擬效能：

```bash
# 檢查 CUDA 支援
./PhysicsSceneRunner --check-cuda

# 啟用 CUDA 加速
./PhysicsSceneRunner --cuda scene.pscene

# 指定 GPU 裝置
./PhysicsSceneRunner --cuda --gpu-device 0 scene.pscene
```

### 效能分析

```bash
# 基本效能統計
./PhysicsSceneRunner --benchmark scene.pscene

# 詳細效能分析
./PhysicsSceneRunner --profile --output profile.json scene.pscene

# 記憶體使用分析
./PhysicsSceneRunner --memory-profile scene.pscene
```

### 批次處理

```bash
# 批次執行多個場景
for scene in *.pscene; do
    ./PhysicsSceneRunner --benchmark "$scene" >> results.txt
done

# 自動化測試
./PhysicsSceneRunner --test-suite test_scenes/
```

## 最佳實踐

### 場景設計

1. **合理的物件數量**
   - 小型場景：< 100 物件
   - 中型場景：100-1000 物件
   - 大型場景：> 1000 物件

2. **適當的時間步長**
   - 標準：0.016667 (60 FPS)
   - 高精度：0.008333 (120 FPS)
   - 快速模擬：0.033333 (30 FPS)

3. **碰撞形狀選擇**
   - 簡單形狀：盒子、球體、膠囊
   - 複雜形狀：凸包網格
   - 避免：非凸網格（除非必要）

### 效能最佳化

1. **物理設定**
   ```json
   {
     "solverIterations": 10,        // 平衡精度和效能
     "enableSleeping": true,        // 靜止物體最佳化
     "sleepThreshold": 0.1,         // 適當的睡眠閾值
     "maxSubSteps": 3               // 限制子步數
   }
   ```

2. **碰撞最佳化**
   ```json
   {
     "collisionGroup": 1,           // 使用碰撞群組
     "collisionMask": 65535,        // 精確的碰撞遮罩
     "isTrigger": false             // 避免不必要的觸發器
   }
   ```

3. **渲染最佳化**
   ```json
   {
     "enableShadows": false,        // 大型場景關閉陰影
     "shadowMapSize": 512,          // 降低陰影解析度
     "enableAntiAliasing": false    // 關閉抗鋸齒
   }
   ```

### 除錯技巧

1. **視覺化除錯**
   ```bash
   # 顯示碰撞形狀
   ./PhysicsSceneRunner --show-collision-shapes scene.pscene
   
   # 顯示約束
   ./PhysicsSceneRunner --show-constraints scene.pscene
   
   # 顯示接觸點
   ./PhysicsSceneRunner --show-contacts scene.pscene
   ```

2. **效能除錯**
   ```bash
   # 效能分析
   ./PhysicsSceneRunner --profile scene.pscene
   
   # 記憶體分析
   ./PhysicsSceneRunner --memory-profile scene.pscene
   
   # CPU vs GPU 比較
   ./PhysicsSceneRunner --compare-cpu-gpu scene.pscene
   ```

3. **物理除錯**
   ```bash
   # 單步執行
   ./PhysicsSceneRunner --step-mode scene.pscene
   
   # 慢動作
   ./PhysicsSceneRunner --slow-motion 0.1 scene.pscene
   
   # 物理統計
   ./PhysicsSceneRunner --physics-stats scene.pscene
   ```

## 常見問題

### Q: 物理模擬不穩定，物件穿透或抖動

**A: 調整模擬參數**
```json
{
  "timeStep": 0.008333,           // 減少時間步長
  "solverIterations": 20,         // 增加求解器迭代
  "enableCCD": true,              // 啟用連續碰撞檢測
  "ogcContactRadius": 0.05        // 減少 OGC 接觸半徑
}
```

### Q: 效能不佳，FPS 過低

**A: 效能最佳化**
1. 減少物件數量
2. 使用簡單碰撞形狀
3. 啟用物體睡眠
4. 使用 CUDA 加速
5. 關閉不必要的視覺效果

### Q: 約束不工作或破裂

**A: 檢查約束設定**
```json
{
  "breakingThreshold": 10000.0,   // 增加破裂閾值
  "enableCollision": false,       // 停用約束物體間碰撞
  "solverIterations": 15          // 增加求解器迭代
}
```

### Q: 材質效果不明顯

**A: 調整材質參數**
```json
{
  "friction": 0.8,                // 增加摩擦力
  "restitution": 0.9,             // 增加彈性
  "rollingFriction": 0.3,         // 增加滾動摩擦
  "density": 2000.0               // 調整密度
}
```

### Q: CUDA 加速無效

**A: 檢查 CUDA 環境**
```bash
# 檢查 CUDA 安裝
nvidia-smi
nvcc --version

# 檢查程式 CUDA 支援
./PhysicsSceneRunner --check-cuda

# 確認使用正確的 GPU
./PhysicsSceneRunner --list-gpus
./PhysicsSceneRunner --gpu-device 0 scene.pscene
```

---

更多詳細資訊請參考 [API 文件](API_REFERENCE.md) 和 [範例教學](EXAMPLES.md)。
