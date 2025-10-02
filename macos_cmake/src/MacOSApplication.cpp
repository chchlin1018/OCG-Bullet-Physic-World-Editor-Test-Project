#include "MacOSApplication.h"
#include "MacOSMainWindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QFileOpenEvent>
#include <QTimer>

MacOSApplication::MacOSApplication(int& argc, char** argv)
    : QApplication(argc, argv)
    , m_globalMenuBar(nullptr)
    , m_fileMenu(nullptr)
    , m_editMenu(nullptr)
    , m_viewMenu(nullptr)
    , m_simulationMenu(nullptr)
    , m_helpMenu(nullptr)
    , m_recentFilesMenu(nullptr)
    , m_newAction(nullptr)
    , m_openAction(nullptr)
    , m_saveAction(nullptr)
    , m_saveAsAction(nullptr)
    , m_closeAction(nullptr)
    , m_quitAction(nullptr)
    , m_preferencesAction(nullptr)
    , m_aboutAction(nullptr)
{
    initialize();
}

MacOSApplication::~MacOSApplication()
{
    saveRecentFiles();
}

MacOSApplication* MacOSApplication::instance()
{
    return qobject_cast<MacOSApplication*>(QApplication::instance());
}

void MacOSApplication::initialize()
{
    setupApplicationIcon();
    setupGlobalMenuBar();
    setupFileAssociations();
    loadRecentFiles();
    
    // 設定應用程式目錄
    m_applicationSupportDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_preferencesDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    
    // 確保目錄存在
    QDir().mkpath(m_applicationSupportDir);
    QDir().mkpath(m_preferencesDir);
}

void MacOSApplication::setupApplicationIcon()
{
    // 設定應用程式圖示
    // setWindowIcon(QIcon(":/icons/app_icon.png"));
}

void MacOSApplication::setupGlobalMenuBar()
{
    m_globalMenuBar = new QMenuBar(nullptr);
    
    // 檔案選單
    m_fileMenu = m_globalMenuBar->addMenu("檔案");
    
    m_newAction = new QAction("新建", this);
    m_newAction->setShortcut(QKeySequence::New);
    connect(m_newAction, &QAction::triggered, this, &MacOSApplication::newScene);
    m_fileMenu->addAction(m_newAction);
    
    m_openAction = new QAction("開啟...", this);
    m_openAction->setShortcut(QKeySequence::Open);
    connect(m_openAction, &QAction::triggered, this, &MacOSApplication::openScene);
    m_fileMenu->addAction(m_openAction);
    
    // 最近檔案選單
    m_recentFilesMenu = m_fileMenu->addMenu("最近檔案");
    updateRecentFilesMenu();
    
    m_fileMenu->addSeparator();
    
    m_saveAction = new QAction("儲存", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_fileMenu->addAction(m_saveAction);
    
    m_saveAsAction = new QAction("另存新檔...", this);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    m_fileMenu->addAction(m_saveAsAction);
    
    m_fileMenu->addSeparator();
    
    m_closeAction = new QAction("關閉", this);
    m_closeAction->setShortcut(QKeySequence::Close);
    m_fileMenu->addAction(m_closeAction);
    
    // 編輯選單
    m_editMenu = m_globalMenuBar->addMenu("編輯");
    
    QAction* undoAction = new QAction("復原", this);
    undoAction->setShortcut(QKeySequence::Undo);
    m_editMenu->addAction(undoAction);
    
    QAction* redoAction = new QAction("重做", this);
    redoAction->setShortcut(QKeySequence::Redo);
    m_editMenu->addAction(redoAction);
    
    m_editMenu->addSeparator();
    
    QAction* cutAction = new QAction("剪下", this);
    cutAction->setShortcut(QKeySequence::Cut);
    m_editMenu->addAction(cutAction);
    
    QAction* copyAction = new QAction("複製", this);
    copyAction->setShortcut(QKeySequence::Copy);
    m_editMenu->addAction(copyAction);
    
    QAction* pasteAction = new QAction("貼上", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    m_editMenu->addAction(pasteAction);
    
    m_editMenu->addSeparator();
    
    m_preferencesAction = new QAction("偏好設定...", this);
    m_preferencesAction->setShortcut(QKeySequence::Preferences);
    connect(m_preferencesAction, &QAction::triggered, this, &MacOSApplication::showPreferences);
    m_editMenu->addAction(m_preferencesAction);
    
    // 檢視選單
    m_viewMenu = m_globalMenuBar->addMenu("檢視");
    
    QAction* toggleSceneTreeAction = new QAction("場景樹狀檢視", this);
    toggleSceneTreeAction->setCheckable(true);
    toggleSceneTreeAction->setChecked(true);
    m_viewMenu->addAction(toggleSceneTreeAction);
    
    QAction* togglePropertyAction = new QAction("屬性面板", this);
    togglePropertyAction->setCheckable(true);
    togglePropertyAction->setChecked(true);
    m_viewMenu->addAction(togglePropertyAction);
    
    QAction* toggleOutputAction = new QAction("輸出面板", this);
    toggleOutputAction->setCheckable(true);
    toggleOutputAction->setChecked(true);
    m_viewMenu->addAction(toggleOutputAction);
    
    m_viewMenu->addSeparator();
    
    QAction* fullScreenAction = new QAction("進入全螢幕", this);
    fullScreenAction->setShortcut(Qt::Key_F11);
    m_viewMenu->addAction(fullScreenAction);
    
    // 模擬選單
    m_simulationMenu = m_globalMenuBar->addMenu("模擬");
    
    QAction* playAction = new QAction("播放", this);
    playAction->setShortcut(Qt::Key_Space);
    m_simulationMenu->addAction(playAction);
    
    QAction* pauseAction = new QAction("暫停", this);
    m_simulationMenu->addAction(pauseAction);
    
    QAction* stopAction = new QAction("停止", this);
    m_simulationMenu->addAction(stopAction);
    
    QAction* resetAction = new QAction("重設", this);
    m_simulationMenu->addAction(resetAction);
    
    m_simulationMenu->addSeparator();
    
    QAction* physicsSettingsAction = new QAction("物理設定...", this);
    m_simulationMenu->addAction(physicsSettingsAction);
    
    // 說明選單
    m_helpMenu = m_globalMenuBar->addMenu("說明");
    
    QAction* userGuideAction = new QAction("使用者指南", this);
    m_helpMenu->addAction(userGuideAction);
    
    QAction* keyboardShortcutsAction = new QAction("鍵盤快速鍵", this);
    m_helpMenu->addAction(keyboardShortcutsAction);
    
    m_helpMenu->addSeparator();
    
    m_aboutAction = new QAction("關於 Physics Scene Editor", this);
    connect(m_aboutAction, &QAction::triggered, this, &MacOSApplication::showAbout);
    m_helpMenu->addAction(m_aboutAction);
    
    // 在 Linux 環境中，全域選單列可能不會顯示，但保留以維持相容性
}

void MacOSApplication::setupFileAssociations()
{
    // 在 Linux 環境中，檔案關聯需要透過 .desktop 檔案設定
    // 這裡只是保留介面以維持相容性
}

bool MacOSApplication::event(QEvent* event)
{
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent* openEvent = static_cast<QFileOpenEvent*>(event);
        QString fileName = openEvent->file();
        
        // 延遲處理檔案開啟，確保主視窗已經建立
        QTimer::singleShot(100, [this, fileName]() {
            openFile(fileName);
        });
        
        return true;
    }
    
    return QApplication::event(event);
}

bool MacOSApplication::openFile(const QString& fileName)
{
    if (fileName.isEmpty()) {
        return false;
    }
    
    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        QMessageBox::warning(nullptr, "錯誤", "無法開啟檔案: " + fileName);
        return false;
    }
    
    // 新增到最近檔案列表
    addToRecentFiles(fileName);
    
    // 發送檔案開啟信號
    Q_EMIT fileOpened(fileName);
    
    return true;
}

QString MacOSApplication::getApplicationSupportDirectory() const
{
    return m_applicationSupportDir;
}

QString MacOSApplication::getPreferencesDirectory() const
{
    return m_preferencesDir;
}

void MacOSApplication::loadRecentFiles()
{
    QSettings settings;
    m_recentFiles = settings.value("recentFiles").toStringList();
    
    // 移除不存在的檔案
    QStringList validFiles;
    for (const QString& file : m_recentFiles) {
        if (QFileInfo::exists(file)) {
            validFiles.append(file);
        }
    }
    m_recentFiles = validFiles;
    
    // 限制數量
    while (m_recentFiles.size() > MaxRecentFiles) {
        m_recentFiles.removeLast();
    }
}

void MacOSApplication::saveRecentFiles()
{
    QSettings settings;
    settings.setValue("recentFiles", m_recentFiles);
}

void MacOSApplication::addToRecentFiles(const QString& fileName)
{
    m_recentFiles.removeAll(fileName);
    m_recentFiles.prepend(fileName);
    
    while (m_recentFiles.size() > MaxRecentFiles) {
        m_recentFiles.removeLast();
    }
    
    updateRecentFilesMenu();
}

void MacOSApplication::updateRecentFilesMenu()
{
    if (!m_recentFilesMenu) {
        return;
    }
    
    m_recentFilesMenu->clear();
    
    for (int i = 0; i < m_recentFiles.size(); ++i) {
        const QString& fileName = m_recentFiles.at(i);
        QFileInfo fileInfo(fileName);
        
        QAction* action = new QAction(QString("%1. %2").arg(i + 1).arg(fileInfo.baseName()), this);
        action->setData(fileName);
        action->setStatusTip(fileName);
        
        connect(action, &QAction::triggered, this, &MacOSApplication::openRecentFile);
        m_recentFilesMenu->addAction(action);
    }
    
    if (!m_recentFiles.isEmpty()) {
        m_recentFilesMenu->addSeparator();
        QAction* clearAction = new QAction("清除選單", this);
        connect(clearAction, &QAction::triggered, this, &MacOSApplication::clearRecentFiles);
        m_recentFilesMenu->addAction(clearAction);
    }
}

void MacOSApplication::showAbout()
{
    QMessageBox::about(nullptr, "關於 Physics Scene Editor",
                      "<h3>Physics Scene Editor</h3>"
                      "<p>版本 1.0.0</p>"
                      "<p>專業的物理場景編輯器，支援 OGC 和 Bullet Physics 引擎。</p>"
                      "<p>Copyright © 2025 Physics Scene Editor Team</p>"
                      "<p>採用 MIT 授權條款</p>");
}

void MacOSApplication::showPreferences()
{
    QMessageBox::information(nullptr, "偏好設定", "偏好設定對話框尚未實現。");
}

void MacOSApplication::newScene()
{
    Q_EMIT newSceneRequested();
}

void MacOSApplication::openScene()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "開啟場景", "", "場景檔案 (*.pscene)");
    if (!fileName.isEmpty()) {
        openFile(fileName);
    }
}

void MacOSApplication::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString fileName = action->data().toString();
        openFile(fileName);
    }
}

void MacOSApplication::clearRecentFiles()
{
    m_recentFiles.clear();
    updateRecentFilesMenu();
    saveRecentFiles();
}
