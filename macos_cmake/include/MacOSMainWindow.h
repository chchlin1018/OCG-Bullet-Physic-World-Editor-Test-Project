#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QDockWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QSettings>

// 前向宣告
class SceneTreeWidget;
class PropertyWidget;
class ViewportWidget;
class QTextEdit;

/**
 * @brief macOS 特化的主視窗類別
 * 
 * 這個類別實現了針對 macOS 平台最佳化的主視窗，包括：
 * - macOS 風格的界面佈局
 * - 原生的工具列和停靠視窗
 * - 系統整合功能
 * - 觸控列支援（如果可用）
 */
class MacOSMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 建構函式
     * @param parent 父視窗
     */
    explicit MacOSMainWindow(QWidget* parent = nullptr);
    
    /**
     * @brief 解構函式
     */
    ~MacOSMainWindow() override;
    
    /**
     * @brief 載入場景檔案
     * @param fileName 場景檔案路徑
     * @return 是否成功載入
     */
    bool loadScene(const QString& fileName);
    
    /**
     * @brief 儲存場景檔案
     * @param fileName 場景檔案路徑
     * @return 是否成功儲存
     */
    bool saveScene(const QString& fileName = QString());
    
    /**
     * @brief 新建場景
     */
    void newScene();
    
    /**
     * @brief 取得當前場景檔案路徑
     * @return 場景檔案路徑
     */
    QString getCurrentSceneFile() const { return m_currentSceneFile; }
    
    /**
     * @brief 檢查場景是否已修改
     * @return 是否已修改
     */
    bool isSceneModified() const { return m_sceneModified; }

protected:
    /**
     * @brief 關閉事件處理
     * @param event 關閉事件
     */
    void closeEvent(QCloseEvent* event) override;
    
    /**
     * @brief 拖拽進入事件
     * @param event 拖拽事件
     */
    void dragEnterEvent(QDragEnterEvent* event) override;
    
    /**
     * @brief 拖拽放下事件
     * @param event 拖拽事件
     */
    void dropEvent(QDropEvent* event) override;

private Q_SLOTS:
    /**
     * @brief 場景修改槽函式
     */
    void onSceneModified();
    
    /**
     * @brief 物件選擇變更槽函式
     */
    void onSelectionChanged();
    
    /**
     * @brief 模擬狀態變更槽函式
     * @param running 是否正在執行
     */
    void onSimulationStateChanged(bool running);
    
    /**
     * @brief 更新狀態列
     */
    void updateStatusBar();
    
    /**
     * @brief 顯示/隱藏場景樹狀檢視
     * @param visible 是否顯示
     */
    void toggleSceneTree(bool visible);
    
    /**
     * @brief 顯示/隱藏屬性面板
     * @param visible 是否顯示
     */
    void togglePropertyPanel(bool visible);
    
    /**
     * @brief 顯示/隱藏輸出面板
     * @param visible 是否顯示
     */
    void toggleOutputPanel(bool visible);
    
    /**
     * @brief 重設視窗佈局
     */
    void resetLayout();
    
    /**
     * @brief 進入全螢幕模式
     */
    void toggleFullScreen();

private:
    /**
     * @brief 初始化使用者界面
     */
    void setupUI();
    
    /**
     * @brief 建立中央視窗
     */
    void createCentralWidget();
    
    /**
     * @brief 建立停靠視窗
     */
    void createDockWidgets();
    
    /**
     * @brief 建立工具列
     */
    void createToolBars();
    
    /**
     * @brief 建立狀態列
     */
    void createStatusBar();
    
    /**
     * @brief 建立動作
     */
    void createActions();
    
    /**
     * @brief 連接信號和槽
     */
    void connectSignals();
    
    /**
     * @brief 載入設定
     */
    void loadSettings();
    
    /**
     * @brief 儲存設定
     */
    void saveSettings();
    
    /**
     * @brief 更新視窗標題
     */
    void updateWindowTitle();
    
    /**
     * @brief 檢查是否可以關閉
     * @return 是否可以關閉
     */
    bool maybeSave();
    
    /**
     * @brief 設定 macOS 特定的視窗屬性
     */
    void setupMacOSWindowProperties();

private:
    // 中央視窗
    ViewportWidget* m_viewportWidget;   ///< 3D 視口
    
    // 停靠視窗
    QDockWidget* m_sceneTreeDock;       ///< 場景樹狀檢視停靠視窗
    QDockWidget* m_propertyDock;        ///< 屬性面板停靠視窗
    QDockWidget* m_outputDock;          ///< 輸出面板停靠視窗
    
    // 視窗元件
    SceneTreeWidget* m_sceneTreeWidget; ///< 場景樹狀檢視
    PropertyWidget* m_propertyWidget;   ///< 屬性編輯器
    QTextEdit* m_outputWidget;          ///< 輸出視窗
    
    // 工具列
    QToolBar* m_mainToolBar;            ///< 主工具列
    QToolBar* m_simulationToolBar;      ///< 模擬工具列
    QToolBar* m_viewToolBar;            ///< 檢視工具列
    
    // 狀態列
    QLabel* m_statusLabel;              ///< 狀態標籤
    QLabel* m_objectCountLabel;         ///< 物件數量標籤
    QLabel* m_simulationTimeLabel;      ///< 模擬時間標籤
    QProgressBar* m_progressBar;        ///< 進度條
    
    // 動作
    QAction* m_newAction;               ///< 新建動作
    QAction* m_openAction;              ///< 開啟動作
    QAction* m_saveAction;              ///< 儲存動作
    QAction* m_saveAsAction;            ///< 另存新檔動作
    QAction* m_playAction;              ///< 播放動作
    QAction* m_pauseAction;             ///< 暫停動作
    QAction* m_stopAction;              ///< 停止動作
    QAction* m_resetAction;             ///< 重設動作
    QAction* m_fullScreenAction;        ///< 全螢幕動作
    
    // 檢視動作
    QAction* m_toggleSceneTreeAction;   ///< 切換場景樹狀檢視
    QAction* m_togglePropertyAction;    ///< 切換屬性面板
    QAction* m_toggleOutputAction;      ///< 切換輸出面板
    QAction* m_resetLayoutAction;       ///< 重設佈局
    
    // 狀態
    QString m_currentSceneFile;         ///< 當前場景檔案
    bool m_sceneModified;               ///< 場景是否已修改
    bool m_simulationRunning;           ///< 模擬是否正在執行
    
    // 計時器
    QTimer* m_statusUpdateTimer;        ///< 狀態更新計時器
    
    // 設定
    QSettings* m_settings;              ///< 應用程式設定
};

/**
 * @brief macOS 主視窗的便利函式
 * @return 當前活動的 MacOSMainWindow 指標
 */
MacOSMainWindow* macOSMainWindow();
