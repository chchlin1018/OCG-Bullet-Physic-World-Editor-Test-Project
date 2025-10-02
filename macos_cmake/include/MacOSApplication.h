#pragma once

#include <QApplication>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>

/**
 * @brief macOS 特化的應用程式類別
 * 
 * 這個類別處理 macOS 特定的應用程式行為，包括：
 * - 全域選單列管理
 * - macOS 檔案關聯處理
 * - 系統整合功能
 * - 應用程式生命週期管理
 */
class MacOSApplication : public QApplication
{
    Q_OBJECT

public:
    /**
     * @brief 建構函式
     * @param argc 命令列參數數量
     * @param argv 命令列參數陣列
     */
    MacOSApplication(int& argc, char** argv);
    
    /**
     * @brief 解構函式
     */
    ~MacOSApplication() override;
    
    /**
     * @brief 取得應用程式單例
     * @return MacOSApplication 指標
     */
    static MacOSApplication* instance();
    
    /**
     * @brief 設定全域選單列
     */
    void setupGlobalMenuBar();
    
    /**
     * @brief 處理檔案開啟事件
     * @param fileName 要開啟的檔案路徑
     * @return 是否成功處理
     */
    bool openFile(const QString& fileName);
    
    /**
     * @brief 取得應用程式支援目錄
     * @return 支援目錄路徑
     */
    QString getApplicationSupportDirectory() const;
    
    /**
     * @brief 取得使用者偏好設定目錄
     * @return 偏好設定目錄路徑
     */
    QString getPreferencesDirectory() const;

protected:
    /**
     * @brief 處理檔案開啟事件（macOS 特定）
     * @param event 檔案開啟事件
     * @return 是否處理了事件
     */
    bool event(QEvent* event) override;

private Q_SLOTS:
    /**
     * @brief 顯示關於對話框
     */
    void showAbout();
    
    /**
     * @brief 顯示偏好設定
     */
    void showPreferences();
    
    /**
     * @brief 新建場景
     */
    void newScene();
    
    /**
     * @brief 開啟場景
     */
    void openScene();
    
    /**
     * @brief 開啟最近的檔案
     */
    void openRecentFile();
    
    /**
     * @brief 清除最近檔案列表
     */
    void clearRecentFiles();

private:
    /**
     * @brief 初始化應用程式
     */
    void initialize();
    
    /**
     * @brief 設定應用程式圖示
     */
    void setupApplicationIcon();
    
    /**
     * @brief 設定檔案關聯
     */
    void setupFileAssociations();
    
    /**
     * @brief 載入最近檔案列表
     */
    void loadRecentFiles();
    
    /**
     * @brief 儲存最近檔案列表
     */
    void saveRecentFiles();
    
    /**
     * @brief 新增檔案到最近檔案列表
     * @param fileName 檔案路徑
     */
    void addToRecentFiles(const QString& fileName);
    
    /**
     * @brief 更新最近檔案選單
     */
    void updateRecentFilesMenu();

Q_SIGNALS:
    /**
     * @brief 檔案開啟信號
     * @param fileName 檔案路徑
     */
    void fileOpened(const QString& fileName);
    
    /**
     * @brief 新場景信號
     */
    void newSceneRequested();
    
    /**
     * @brief 偏好設定變更信號
     */
    void preferencesChanged();

private:
    QMenuBar* m_globalMenuBar;          ///< 全域選單列
    QMenu* m_fileMenu;                  ///< 檔案選單
    QMenu* m_editMenu;                  ///< 編輯選單
    QMenu* m_viewMenu;                  ///< 檢視選單
    QMenu* m_simulationMenu;            ///< 模擬選單
    QMenu* m_helpMenu;                  ///< 說明選單
    QMenu* m_recentFilesMenu;           ///< 最近檔案選單
    
    QAction* m_newAction;               ///< 新建動作
    QAction* m_openAction;              ///< 開啟動作
    QAction* m_saveAction;              ///< 儲存動作
    QAction* m_saveAsAction;            ///< 另存新檔動作
    QAction* m_closeAction;             ///< 關閉動作
    QAction* m_quitAction;              ///< 退出動作
    QAction* m_preferencesAction;       ///< 偏好設定動作
    QAction* m_aboutAction;             ///< 關於動作
    
    QStringList m_recentFiles;          ///< 最近檔案列表
    static const int MaxRecentFiles = 10; ///< 最大最近檔案數量
    
    QString m_applicationSupportDir;    ///< 應用程式支援目錄
    QString m_preferencesDir;           ///< 偏好設定目錄
};

/**
 * @brief 取得 macOS 應用程式實例的便利函式
 * @return MacOSApplication 指標
 */
inline MacOSApplication* macOSApp()
{
    return qobject_cast<MacOSApplication*>(QApplication::instance());
}
