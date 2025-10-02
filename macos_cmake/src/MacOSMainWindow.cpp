#include "MacOSMainWindow.h"
#include "MacOSApplication.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QSettings>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

// 暫時的簡化版本 Widget 類別
class SceneTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SceneTreeWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setMinimumSize(200, 300);
        auto layout = new QVBoxLayout(this);
        auto label = new QLabel("場景樹狀檢視", this);
        layout->addWidget(label);
    }
};

class PropertyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setMinimumSize(250, 300);
        auto layout = new QVBoxLayout(this);
        auto label = new QLabel("屬性編輯器", this);
        layout->addWidget(label);
    }
};

class ViewportWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewportWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setMinimumSize(640, 480);
        auto layout = new QVBoxLayout(this);
        auto label = new QLabel("3D 視口\n\n這裡將顯示物理場景的 3D 視覺化", this);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("QLabel { background-color: #2b2b2b; color: white; font-size: 14px; }");
        layout->addWidget(label);
    }
};

MacOSMainWindow::MacOSMainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_viewportWidget(nullptr)
    , m_sceneTreeDock(nullptr)
    , m_propertyDock(nullptr)
    , m_outputDock(nullptr)
    , m_sceneTreeWidget(nullptr)
    , m_propertyWidget(nullptr)
    , m_outputWidget(nullptr)
    , m_mainToolBar(nullptr)
    , m_simulationToolBar(nullptr)
    , m_viewToolBar(nullptr)
    , m_statusLabel(nullptr)
    , m_objectCountLabel(nullptr)
    , m_simulationTimeLabel(nullptr)
    , m_progressBar(nullptr)
    , m_sceneModified(false)
    , m_simulationRunning(false)
    , m_statusUpdateTimer(nullptr)
    , m_settings(nullptr)
{
    setObjectName("mainWindow");  // 設定主視窗 objectName
    setupUI();
    createActions();
    createCentralWidget();
    createDockWidgets();
    createToolBars();
    createStatusBar();
    connectSignals();
    setupMacOSWindowProperties();
    loadSettings();
    updateWindowTitle();
    
    // 啟動狀態更新計時器
    m_statusUpdateTimer = new QTimer(this);
    connect(m_statusUpdateTimer, &QTimer::timeout, this, &MacOSMainWindow::updateStatusBar);
    m_statusUpdateTimer->start(1000); // 每秒更新一次
}

MacOSMainWindow::~MacOSMainWindow()
{
    saveSettings();
}

void MacOSMainWindow::setupUI()
{
    setWindowTitle("Physics Scene Editor");
    setMinimumSize(1024, 768);
    resize(1280, 800);
    
    // 設定視窗圖示
    // setWindowIcon(QIcon(":/icons/app_icon.png"));
    
    // 啟用拖拽
    setAcceptDrops(true);
}

void MacOSMainWindow::createCentralWidget()
{
    m_viewportWidget = new ViewportWidget(this);
    m_viewportWidget->setObjectName("viewportWidget");  // 設定 objectName
    setCentralWidget(m_viewportWidget);
}

void MacOSMainWindow::createDockWidgets()
{
    // 場景樹狀檢視停靠視窗
    m_sceneTreeDock = new QDockWidget("場景樹狀檢視", this);
    m_sceneTreeDock->setObjectName("sceneTreeDock");  // 設定 objectName
    m_sceneTreeWidget = new SceneTreeWidget(this);
    m_sceneTreeDock->setWidget(m_sceneTreeWidget);
    m_sceneTreeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, m_sceneTreeDock);
    
    // 屬性面板停靠視窗
    m_propertyDock = new QDockWidget("屬性", this);
    m_propertyDock->setObjectName("propertyDock");  // 設定 objectName
    m_propertyWidget = new PropertyWidget(this);
    m_propertyDock->setWidget(m_propertyWidget);
    m_propertyDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_propertyDock);
    
    // 輸出面板停靠視窗
    m_outputDock = new QDockWidget("輸出", this);
    m_outputDock->setObjectName("outputDock");  // 設定 objectName
    m_outputWidget = new QTextEdit(this);
    m_outputWidget->setReadOnly(true);
    m_outputWidget->setMaximumHeight(150);
    m_outputWidget->append("Physics Scene Editor 已啟動");
    m_outputWidget->append("OGC 物理引擎已初始化");
    m_outputWidget->append("Bullet Physics 引擎已初始化");
    m_outputDock->setWidget(m_outputWidget);
    m_outputDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);
}

void MacOSMainWindow::createToolBars()
{
    // 主工具列
    m_mainToolBar = addToolBar("主要");
    m_mainToolBar->setObjectName("MainToolBar");
    
    // 模擬工具列
    m_simulationToolBar = addToolBar("模擬");
    m_simulationToolBar->setObjectName("SimulationToolBar");
    
    // 檢視工具列
    m_viewToolBar = addToolBar("檢視");
    m_viewToolBar->setObjectName("ViewToolBar");
}

void MacOSMainWindow::createStatusBar()
{
    statusBar()->setObjectName("statusBar");  // 設定狀態列 objectName
    
    m_statusLabel = new QLabel("就緒", this);
    m_statusLabel->setObjectName("statusLabel");
    statusBar()->addWidget(m_statusLabel);
    
    statusBar()->addPermanentWidget(new QLabel(" | ", this));
    
    m_objectCountLabel = new QLabel("物件: 0", this);
    m_objectCountLabel->setObjectName("objectCountLabel");
    statusBar()->addPermanentWidget(m_objectCountLabel);
    
    statusBar()->addPermanentWidget(new QLabel(" | ", this));
    
    m_simulationTimeLabel = new QLabel("時間: 0.00s", this);
    m_simulationTimeLabel->setObjectName("simulationTimeLabel");
    statusBar()->addPermanentWidget(m_simulationTimeLabel);
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setObjectName("progressBar");
    m_progressBar->setVisible(false);
    statusBar()->addPermanentWidget(m_progressBar);
}

void MacOSMainWindow::createActions()
{
    // 檔案動作
    m_newAction = new QAction("新建", this);
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setStatusTip("建立新的物理場景");
    
    m_openAction = new QAction("開啟", this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip("開啟現有的物理場景");
    
    m_saveAction = new QAction("儲存", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip("儲存當前場景");
    
    m_saveAsAction = new QAction("另存新檔", this);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveAsAction->setStatusTip("將場景儲存為新檔案");
    
    // 模擬動作
    m_playAction = new QAction("播放", this);
    m_playAction->setShortcut(Qt::Key_Space);
    m_playAction->setStatusTip("開始物理模擬");
    
    m_pauseAction = new QAction("暫停", this);
    m_pauseAction->setStatusTip("暫停物理模擬");
    
    m_stopAction = new QAction("停止", this);
    m_stopAction->setStatusTip("停止物理模擬");
    
    m_resetAction = new QAction("重設", this);
    m_resetAction->setStatusTip("重設場景到初始狀態");
    
    // 檢視動作
    m_toggleSceneTreeAction = new QAction("場景樹狀檢視", this);
    m_toggleSceneTreeAction->setCheckable(true);
    m_toggleSceneTreeAction->setChecked(true);
    
    m_togglePropertyAction = new QAction("屬性面板", this);
    m_togglePropertyAction->setCheckable(true);
    m_togglePropertyAction->setChecked(true);
    
    m_toggleOutputAction = new QAction("輸出面板", this);
    m_toggleOutputAction->setCheckable(true);
    m_toggleOutputAction->setChecked(true);
    
    m_resetLayoutAction = new QAction("重設佈局", this);
    m_resetLayoutAction->setStatusTip("重設視窗佈局到預設狀態");
    
    m_fullScreenAction = new QAction("全螢幕", this);
    m_fullScreenAction->setShortcut(Qt::Key_F11);
    m_fullScreenAction->setCheckable(true);
    m_fullScreenAction->setStatusTip("切換全螢幕模式");
}

void MacOSMainWindow::connectSignals()
{
    // 檔案動作
    connect(m_newAction, &QAction::triggered, this, &MacOSMainWindow::newScene);
    connect(m_openAction, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "開啟場景", "", "場景檔案 (*.pscene)");
        if (!fileName.isEmpty()) {
            loadScene(fileName);
        }
    });
    connect(m_saveAction, &QAction::triggered, this, [this]() { saveScene(); });
    connect(m_saveAsAction, &QAction::triggered, this, [this]() { saveScene(QString()); });
    
    // 模擬動作
    connect(m_playAction, &QAction::triggered, this, [this]() {
        m_simulationRunning = true;
        onSimulationStateChanged(true);
        m_outputWidget->append("開始物理模擬");
    });
    connect(m_pauseAction, &QAction::triggered, this, [this]() {
        m_simulationRunning = false;
        onSimulationStateChanged(false);
        m_outputWidget->append("暫停物理模擬");
    });
    connect(m_stopAction, &QAction::triggered, this, [this]() {
        m_simulationRunning = false;
        onSimulationStateChanged(false);
        m_outputWidget->append("停止物理模擬");
    });
    connect(m_resetAction, &QAction::triggered, this, [this]() {
        m_outputWidget->append("重設場景");
    });
    
    // 檢視動作
    connect(m_toggleSceneTreeAction, &QAction::toggled, this, &MacOSMainWindow::toggleSceneTree);
    connect(m_togglePropertyAction, &QAction::toggled, this, &MacOSMainWindow::togglePropertyPanel);
    connect(m_toggleOutputAction, &QAction::toggled, this, &MacOSMainWindow::toggleOutputPanel);
    connect(m_resetLayoutAction, &QAction::triggered, this, &MacOSMainWindow::resetLayout);
    connect(m_fullScreenAction, &QAction::toggled, this, &MacOSMainWindow::toggleFullScreen);
    
    // 停靠視窗可見性
    connect(m_sceneTreeDock, &QDockWidget::visibilityChanged, m_toggleSceneTreeAction, &QAction::setChecked);
    connect(m_propertyDock, &QDockWidget::visibilityChanged, m_togglePropertyAction, &QAction::setChecked);
    connect(m_outputDock, &QDockWidget::visibilityChanged, m_toggleOutputAction, &QAction::setChecked);
}

void MacOSMainWindow::setupMacOSWindowProperties()
{
    // 在 Linux 環境中，這些設定可能不會有效果，但保留以維持相容性
    setUnifiedTitleAndToolBarOnMac(true);
}

void MacOSMainWindow::loadSettings()
{
    m_settings = new QSettings(this);
    
    // 恢復視窗幾何
    restoreGeometry(m_settings->value("geometry").toByteArray());
    restoreState(m_settings->value("windowState").toByteArray());
    
    // 恢復最近檔案
    m_currentSceneFile = m_settings->value("lastOpenFile").toString();
}

void MacOSMainWindow::saveSettings()
{
    if (m_settings) {
        m_settings->setValue("geometry", saveGeometry());
        m_settings->setValue("windowState", saveState());
        m_settings->setValue("lastOpenFile", m_currentSceneFile);
    }
}

void MacOSMainWindow::updateWindowTitle()
{
    QString title = "Physics Scene Editor";
    
    if (!m_currentSceneFile.isEmpty()) {
        QFileInfo fileInfo(m_currentSceneFile);
        title += " - " + fileInfo.baseName();
    }
    
    if (m_sceneModified) {
        title += " *";
    }
    
    setWindowTitle(title);
}

bool MacOSMainWindow::maybeSave()
{
    if (m_sceneModified) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, "Physics Scene Editor",
                                                              "場景已修改。\n是否要儲存變更？",
                                                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
            return saveScene();
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool MacOSMainWindow::loadScene(const QString& fileName)
{
    if (!maybeSave()) {
        return false;
    }
    
    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists()) {
        QMessageBox::warning(this, "錯誤", "檔案不存在: " + fileName);
        return false;
    }
    
    m_currentSceneFile = fileName;
    m_sceneModified = false;
    updateWindowTitle();
    
    m_outputWidget->append("載入場景: " + fileInfo.baseName());
    
    return true;
}

bool MacOSMainWindow::saveScene(const QString& fileName)
{
    QString saveFileName = fileName;
    
    if (saveFileName.isEmpty()) {
        if (m_currentSceneFile.isEmpty()) {
            saveFileName = QFileDialog::getSaveFileName(this, "儲存場景", "", "場景檔案 (*.pscene)");
            if (saveFileName.isEmpty()) {
                return false;
            }
        } else {
            saveFileName = m_currentSceneFile;
        }
    }
    
    // 這裡應該實現實際的儲存邏輯
    m_currentSceneFile = saveFileName;
    m_sceneModified = false;
    updateWindowTitle();
    
    QFileInfo fileInfo(saveFileName);
    m_outputWidget->append("儲存場景: " + fileInfo.baseName());
    
    return true;
}

void MacOSMainWindow::newScene()
{
    if (!maybeSave()) {
        return;
    }
    
    m_currentSceneFile.clear();
    m_sceneModified = false;
    updateWindowTitle();
    
    m_outputWidget->append("建立新場景");
}

void MacOSMainWindow::closeEvent(QCloseEvent* event)
{
    if (maybeSave()) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MacOSMainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MacOSMainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if (!urlList.isEmpty()) {
            QString fileName = urlList.first().toLocalFile();
            if (fileName.endsWith(".pscene", Qt::CaseInsensitive)) {
                loadScene(fileName);
            }
        }
    }
}

void MacOSMainWindow::onSceneModified()
{
    m_sceneModified = true;
    updateWindowTitle();
}

void MacOSMainWindow::onSelectionChanged()
{
    // 處理選擇變更
}

void MacOSMainWindow::onSimulationStateChanged(bool running)
{
    m_simulationRunning = running;
    
    // 更新動作狀態
    m_playAction->setEnabled(!running);
    m_pauseAction->setEnabled(running);
    m_stopAction->setEnabled(running);
}

void MacOSMainWindow::updateStatusBar()
{
    if (m_simulationRunning) {
        static double simulationTime = 0.0;
        simulationTime += 0.016; // 假設 60 FPS
        m_simulationTimeLabel->setText(QString("時間: %1s").arg(simulationTime, 0, 'f', 2));
        m_statusLabel->setText("模擬中...");
    } else {
        m_statusLabel->setText("就緒");
    }
    
    // 更新物件數量（示例）
    m_objectCountLabel->setText("物件: 5");
}

void MacOSMainWindow::toggleSceneTree(bool visible)
{
    m_sceneTreeDock->setVisible(visible);
}

void MacOSMainWindow::togglePropertyPanel(bool visible)
{
    m_propertyDock->setVisible(visible);
}

void MacOSMainWindow::toggleOutputPanel(bool visible)
{
    m_outputDock->setVisible(visible);
}

void MacOSMainWindow::resetLayout()
{
    // 重設停靠視窗佈局
    addDockWidget(Qt::LeftDockWidgetArea, m_sceneTreeDock);
    addDockWidget(Qt::RightDockWidgetArea, m_propertyDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);
    
    // 顯示所有停靠視窗
    m_sceneTreeDock->setVisible(true);
    m_propertyDock->setVisible(true);
    m_outputDock->setVisible(true);
    
    m_outputWidget->append("重設視窗佈局");
}

void MacOSMainWindow::toggleFullScreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

MacOSMainWindow* macOSMainWindow()
{
    return qobject_cast<MacOSMainWindow*>(QApplication::activeWindow());
}

#include "MacOSMainWindow.moc"
