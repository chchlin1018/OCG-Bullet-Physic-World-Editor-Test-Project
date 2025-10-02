#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QLabel>
#include <QtCore/QTimer>
#include <QtCore/QSettings>
#include <memory>

#include "../scene_format/physics_scene_format.h"
#include "../cross_platform_runner/scene_loader.h"

class SceneTreeWidget;
class PropertyWidget;
class ViewportWidget;
class MaterialWidget;
class OutputWidget;
class ToolboxWidget;

/**
 * @file MainWindow.h
 * @brief Qt 版本物理場景編輯器主視窗
 * 
 * 提供跨平台的物理場景編輯功能，與 MFC 版本功能對等。
 * 支援 Windows、macOS 和 Linux 平台。
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 場景管理
    bool NewScene();
    bool OpenScene(const QString& filename = QString());
    bool SaveScene(const QString& filename = QString());
    bool SaveSceneAs();
    void CloseScene();

    // 獲取當前場景
    const PhysicsScene::PhysicsScene& GetCurrentScene() const { return m_currentScene; }
    PhysicsScene::PhysicsScene& GetCurrentScene() { return m_currentScene; }

    // 場景修改狀態
    bool IsSceneModified() const { return m_sceneModified; }
    void SetSceneModified(bool modified = true);

    // 選擇管理
    void SelectObject(const QString& objectId, const QString& objectType);
    void ClearSelection();
    QString GetSelectedObjectId() const { return m_selectedObjectId; }
    QString GetSelectedObjectType() const { return m_selectedObjectType; }

    // 視窗管理
    void ShowDockWidget(const QString& name, bool show = true);
    void ResetLayout();
    void SaveLayout();
    void RestoreLayout();

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    // 檔案選單
    void OnNewScene();
    void OnOpenScene();
    void OnSaveScene();
    void OnSaveSceneAs();
    void OnRecentFile();
    void OnImportModel();
    void OnExportScene();
    void OnExit();

    // 編輯選單
    void OnUndo();
    void OnRedo();
    void OnCut();
    void OnCopy();
    void OnPaste();
    void OnDelete();
    void OnSelectAll();
    void OnFind();

    // 物件選單
    void OnAddRigidBody();
    void OnAddConstraint();
    void OnAddForceField();
    void OnAddLight();
    void OnAddCamera();
    void OnDuplicateObject();
    void OnGroupObjects();
    void OnUngroupObjects();

    // 模擬選單
    void OnPlaySimulation();
    void OnPauseSimulation();
    void OnStopSimulation();
    void OnStepSimulation();
    void OnResetSimulation();
    void OnSimulationSettings();

    // 視圖選單
    void OnViewportMode();
    void OnShowGrid();
    void OnShowAxes();
    void OnShowBoundingBoxes();
    void OnShowConstraints();
    void OnShowContactPoints();
    void OnFrameSelected();
    void OnFrameAll();

    // 工具選單
    void OnValidateScene();
    void OnOptimizeScene();
    void OnBenchmarkScene();
    void OnSceneStatistics();
    void OnPreferences();

    // 說明選單
    void OnUserGuide();
    void OnAPIReference();
    void OnAbout();

    // 工具列動作
    void OnToolSelect();
    void OnToolMove();
    void OnToolRotate();
    void OnToolScale();

    // 物件事件
    void OnObjectSelected(const QString& objectId, const QString& objectType);
    void OnObjectModified(const QString& objectId, const QString& objectType);
    void OnObjectAdded(const QString& objectId, const QString& objectType);
    void OnObjectRemoved(const QString& objectId, const QString& objectType);

    // 屬性事件
    void OnPropertyChanged(const QString& propertyName, const QVariant& value);

    // 視口事件
    void OnViewportClicked(const QPoint& position);
    void OnViewportDoubleClicked(const QPoint& position);
    void OnViewportContextMenu(const QPoint& position);

    // 狀態更新
    void UpdateUI();
    void UpdateTitle();
    void UpdateStatusBar();
    void UpdateRecentFiles();

private:
    // UI 初始化
    void CreateMenus();
    void CreateToolBars();
    void CreateStatusBar();
    void CreateDockWidgets();
    void CreateCentralWidget();
    void ConnectSignals();

    // 動作建立
    void CreateActions();
    void CreateFileActions();
    void CreateEditActions();
    void CreateObjectActions();
    void CreateSimulationActions();
    void CreateViewActions();
    void CreateToolActions();
    void CreateHelpActions();

    // 設定管理
    void LoadSettings();
    void SaveSettings();
    void ApplySettings();

    // 場景操作
    bool PromptSaveChanges();
    void UpdateSceneInViewport();
    void RefreshAllWidgets();

    // 最近檔案管理
    void AddToRecentFiles(const QString& filename);
    void UpdateRecentFileActions();

    // 錯誤處理
    void ShowError(const QString& title, const QString& message);
    void ShowWarning(const QString& title, const QString& message);
    void ShowInfo(const QString& title, const QString& message);

private:
    // 核心資料
    PhysicsScene::PhysicsScene m_currentScene;
    std::unique_ptr<SceneLoader> m_sceneLoader;
    QString m_currentFilename;
    bool m_sceneModified;

    // 選擇狀態
    QString m_selectedObjectId;
    QString m_selectedObjectType;

    // UI 組件
    QSplitter* m_centralSplitter;
    
    // 停靠視窗
    QDockWidget* m_sceneTreeDock;
    QDockWidget* m_propertyDock;
    QDockWidget* m_materialDock;
    QDockWidget* m_toolboxDock;
    QDockWidget* m_outputDock;

    // 自訂 Widget
    SceneTreeWidget* m_sceneTreeWidget;
    PropertyWidget* m_propertyWidget;
    ViewportWidget* m_viewportWidget;
    MaterialWidget* m_materialWidget;
    ToolboxWidget* m_toolboxWidget;
    OutputWidget* m_outputWidget;

    // 選單
    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_objectMenu;
    QMenu* m_simulationMenu;
    QMenu* m_viewMenu;
    QMenu* m_toolsMenu;
    QMenu* m_helpMenu;
    QMenu* m_recentFilesMenu;

    // 工具列
    QToolBar* m_fileToolBar;
    QToolBar* m_editToolBar;
    QToolBar* m_objectToolBar;
    QToolBar* m_simulationToolBar;
    QToolBar* m_viewToolBar;
    QToolBar* m_toolsToolBar;

    // 動作
    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_saveAsAction;
    QAction* m_exitAction;
    
    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_cutAction;
    QAction* m_copyAction;
    QAction* m_pasteAction;
    QAction* m_deleteAction;
    
    QAction* m_playAction;
    QAction* m_pauseAction;
    QAction* m_stopAction;
    QAction* m_stepAction;
    QAction* m_resetAction;
    
    QAction* m_selectToolAction;
    QAction* m_moveToolAction;
    QAction* m_rotateToolAction;
    QAction* m_scaleToolAction;

    QList<QAction*> m_recentFileActions;

    // 狀態列
    QLabel* m_statusLabel;
    QLabel* m_objectCountLabel;
    QLabel* m_selectionLabel;
    QProgressBar* m_progressBar;

    // 設定
    QSettings* m_settings;
    QTimer* m_autoSaveTimer;

    // 常數
    static const int MAX_RECENT_FILES = 10;
    static const int AUTO_SAVE_INTERVAL = 300000; // 5 分鐘

public:
    // 靜態方法
    static MainWindow* GetInstance() { return s_instance; }

private:
    static MainWindow* s_instance;

signals:
    // 場景信號
    void SceneChanged();
    void SceneModified();
    void ObjectSelected(const QString& objectId, const QString& objectType);
    void ObjectModified(const QString& objectId, const QString& objectType);
    
    // UI 信號
    void StatusMessage(const QString& message, int timeout = 0);
    void ProgressUpdate(int value, int maximum = 100);
};
