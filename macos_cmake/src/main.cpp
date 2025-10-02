#include "MacOSApplication.h"
#include "MacOSMainWindow.h"

#include <QDir>
#include <QStandardPaths>
#include <QLoggingCategory>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QMessageBox>
#include <QStyleFactory>
#include <QScreen>
#include <QOperatingSystemVersion>
#include <QPalette>
#include <QTimer>

// 日誌類別
Q_LOGGING_CATEGORY(app, "app")
Q_LOGGING_CATEGORY(physics, "physics")
Q_LOGGING_CATEGORY(renderer, "renderer")

/**
 * @brief 設定應用程式資訊
 */
void setupApplicationInfo()
{
    QCoreApplication::setApplicationName("Physics Scene Editor");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("Physics Scene Editor Team");
    QCoreApplication::setOrganizationDomain("physicssceneeditor.com");
    
    // 設定應用程式屬性
    // 注意：Qt6 中 AA_EnableHighDpiScaling 和 AA_UseHighDpiPixmaps 已預設啟用
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
}

/**
 * @brief 設定日誌系統
 */
void setupLogging()
{
    // 設定日誌格式
    qSetMessagePattern("[%{time yyyy-MM-dd hh:mm:ss.zzz}] "
                      "[%{category}] [%{type}] "
                      "%{function}:%{line} - %{message}");
    
    // 根據建置類型設定日誌等級
#ifdef DEBUG_BUILD
    QLoggingCategory::setFilterRules("*.debug=true");
#else
    QLoggingCategory::setFilterRules("*.debug=false;*.info=true");
#endif
}

/**
 * @brief 檢查系統需求
 * @return 是否滿足系統需求
 */
bool checkSystemRequirements()
{
    // 檢查 macOS 版本
    QOperatingSystemVersion current = QOperatingSystemVersion::current();
    
    // 檢查是否為 macOS 且版本 >= 10.15
    if (current.type() == QOperatingSystemVersion::MacOS) {
        if (current.majorVersion() < 10 || 
            (current.majorVersion() == 10 && current.minorVersion() < 15)) {
            QMessageBox::critical(nullptr, 
                                QObject::tr("系統需求不符"),
                                QObject::tr("此應用程式需要 macOS 10.15 (Catalina) 或更高版本。\n"
                                          "您的系統版本：%1.%2")
                                .arg(current.majorVersion())
                                .arg(current.minorVersion()));
            return false;
        }
    }
    
    // 檢查 OpenGL 支援
    // 注意：這裡只是基本檢查，實際的 OpenGL 檢查會在視口初始化時進行
    qCInfo(app) << "系統版本：" << current.name();
    qCInfo(app) << "滿足最低系統需求";
    
    return true;
}

/**
 * @brief 設定應用程式樣式
 * @param app 應用程式實例
 */
void setupApplicationStyle(MacOSApplication* app)
{
    // 在 macOS 上使用原生樣式
    app->setStyle(QStyleFactory::create("macOS"));
    
    // 設定應用程式調色板以支援深色模式
    QPalette palette = app->palette();
    
    // 檢查是否為深色模式
    bool isDarkMode = palette.color(QPalette::Window).lightness() < 128;
    
    if (isDarkMode) {
        qCInfo(::app) << "檢測到深色模式";
        // 可以在這裡設定深色模式特定的調色板
    } else {
        qCInfo(::app) << "使用淺色模式";
    }
    
    app->setPalette(palette);
}

/**
 * @brief 處理命令列參數
 * @param app 應用程式實例
 * @return 要開啟的檔案路徑（如果有的話）
 */
QString handleCommandLineArguments(MacOSApplication* app)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("專業的物理場景編輯器，支援 OGC 和 Bullet Physics");
    parser.addHelpOption();
    parser.addVersionOption();
    
    // 新增命令列選項
    QCommandLineOption debugOption(QStringList() << "d" << "debug",
                                  "啟用除錯模式");
    parser.addOption(debugOption);
    
    QCommandLineOption verboseOption(QStringList() << "v" << "verbose",
                                    "啟用詳細輸出");
    parser.addOption(verboseOption);
    
    QCommandLineOption noGpuOption("no-gpu",
                                  "停用 GPU 加速");
    parser.addOption(noGpuOption);
    
    QCommandLineOption engineOption("engine",
                                   "指定物理引擎 (ogc, bullet, hybrid)",
                                   "engine", "hybrid");
    parser.addOption(engineOption);
    
    // 位置參數（檔案路徑）
    parser.addPositionalArgument("file", "要開啟的場景檔案", "[file]");
    
    // 解析命令列
    parser.process(*app);
    
    // 處理選項
    if (parser.isSet(debugOption)) {
        QLoggingCategory::setFilterRules("*.debug=true");
        qCInfo(app) << "除錯模式已啟用";
    }
    
    if (parser.isSet(verboseOption)) {
        QLoggingCategory::setFilterRules("*.debug=true;*.info=true");
        qCInfo(app) << "詳細輸出已啟用";
    }
    
    if (parser.isSet(noGpuOption)) {
        qCInfo(app) << "GPU 加速已停用";
        // 設定環境變數或全域標誌
        qputenv("DISABLE_GPU", "1");
    }
    
    QString engine = parser.value(engineOption);
    if (!engine.isEmpty()) {
        qCInfo(app) << "指定物理引擎：" << engine;
        qputenv("PHYSICS_ENGINE", engine.toUtf8());
    }
    
    // 取得檔案路徑
    const QStringList positionalArgs = parser.positionalArguments();
    if (!positionalArgs.isEmpty()) {
        QString filePath = positionalArgs.first();
        QFileInfo fileInfo(filePath);
        
        if (fileInfo.exists() && fileInfo.isReadable()) {
            qCInfo(app()) << "將開啟檔案：" << filePath;
            return fileInfo.absoluteFilePath();
        } else {
            qCWarning(app()) << "檔案不存在或無法讀取：" << filePath;
        }
    }
    
    return QString();
}

/**
 * @brief 主程式進入點
 * @param argc 命令列參數數量
 * @param argv 命令列參數陣列
 * @return 應用程式退出代碼
 */
int main(int argc, char* argv[])
{
    // 設定應用程式資訊
    setupApplicationInfo();
    
    // 建立應用程式實例
    MacOSApplication app(argc, argv);
    
    // 設定日誌系統
    setupLogging();
    
    qCInfo(::app) << "Physics Scene Editor 正在啟動...";
    qCInfo(::app) << "版本：" << QCoreApplication::applicationVersion();
    qCInfo(::app) << "Qt 版本：" << QT_VERSION_STR;
    
    // 檢查系統需求
    if (!checkSystemRequirements()) {
        return 1;
    }
    
    // 設定應用程式樣式
    setupApplicationStyle(&app);
    
    // 處理命令列參數
    QString fileToOpen = handleCommandLineArguments(&app);
    
    try {
        // 建立主視窗
        MacOSMainWindow mainWindow;
        
        // 顯示主視窗
        mainWindow.show();
        
        // 如果有指定檔案，則開啟它
        if (!fileToOpen.isEmpty()) {
            QTimer::singleShot(100, [&mainWindow, fileToOpen]() {
                mainWindow.loadScene(fileToOpen);
            });
        }
        
        qCInfo(::app) << "應用程式已啟動";
        
        // 進入事件循環
        int result = app.exec();
        
        qCInfo(::app) << "應用程式正在退出，退出代碼：" << result;
        return result;
        
    } catch (const std::exception& e) {
        qCCritical(::app) << "未處理的異常：" << e.what();
        
        QMessageBox::critical(nullptr,
                            QObject::tr("嚴重錯誤"),
                            QObject::tr("應用程式遇到嚴重錯誤：\n%1\n\n"
                                      "應用程式將會關閉。")
                            .arg(QString::fromStdString(e.what())));
        return 1;
        
    } catch (...) {
        qCCritical(::app) << "未知的嚴重錯誤";
        
        QMessageBox::critical(nullptr,
                            QObject::tr("嚴重錯誤"),
                            QObject::tr("應用程式遇到未知的嚴重錯誤。\n\n"
                                      "應用程式將會關閉。"));
        return 1;
    }
}
