#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include <QtWidgets/QSplashScreen>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QTranslator>
#include <QtCore/QLocale>
#include <QtCore/QCommandLineParser>
#include <QtCore/QLoggingCategory>

#include "../include/MainWindow.h"

/**
 * @file main.cpp
 * @brief Qt 版本物理場景編輯器主程式
 * 
 * 跨平台的物理場景編輯器，支援 Windows、macOS 和 Linux。
 * 提供與 MFC 版本相同的功能，但具有更好的跨平台相容性。
 */

// 日誌類別
Q_LOGGING_CATEGORY(app, "app")
Q_LOGGING_CATEGORY(physics, "physics")
Q_LOGGING_CATEGORY(renderer, "renderer")
Q_LOGGING_CATEGORY(ui, "ui")

// 應用程式資訊
const QString APP_NAME = "Physics Scene Editor";
const QString APP_VERSION = "1.0.0";
const QString APP_ORGANIZATION = "Physics Scene Editor Team";
const QString APP_DOMAIN = "physics-scene-editor.com";

/**
 * @brief 設定應用程式樣式
 */
void SetupApplicationStyle(QApplication& app)
{
    // 設定應用程式樣式
#ifdef Q_OS_WIN
    app.setStyle(QStyleFactory::create("WindowsVista"));
#elif defined(Q_OS_MAC)
    app.setStyle(QStyleFactory::create("macintosh"));
#else
    app.setStyle(QStyleFactory::create("Fusion"));
#endif

    // 設定深色主題（可選）
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    // 可以通過命令列參數啟用深色主題
    // app.setPalette(darkPalette);
}

/**
 * @brief 設定應用程式圖示和資源
 */
void SetupApplicationResources(QApplication& app)
{
    // 設定應用程式圖示
    QIcon appIcon;
    appIcon.addFile(":/icons/app_16.png", QSize(16, 16));
    appIcon.addFile(":/icons/app_32.png", QSize(32, 32));
    appIcon.addFile(":/icons/app_48.png", QSize(48, 48));
    appIcon.addFile(":/icons/app_64.png", QSize(64, 64));
    appIcon.addFile(":/icons/app_128.png", QSize(128, 128));
    appIcon.addFile(":/icons/app_256.png", QSize(256, 256));
    app.setWindowIcon(appIcon);

    // 設定資源搜尋路徑
    QDir::addSearchPath("icons", ":/icons");
    QDir::addSearchPath("shaders", ":/shaders");
    QDir::addSearchPath("textures", ":/textures");
}

/**
 * @brief 設定國際化
 */
void SetupInternationalization(QApplication& app)
{
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    
    for (const QString &locale : uiLanguages) {
        const QString baseName = "physics_scene_editor_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
}

/**
 * @brief 設定日誌系統
 */
void SetupLogging()
{
    // 設定日誌格式
    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz t} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");

    // 設定日誌檔案
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);
    
    // 可以在這裡設定檔案日誌輸出
    // 例如使用 QLoggingCategory 和自訂的日誌處理器
}

/**
 * @brief 檢查系統需求
 */
bool CheckSystemRequirements()
{
    // 檢查 OpenGL 支援
    QOpenGLContext context;
    if (!context.create()) {
        qCritical() << "Failed to create OpenGL context";
        return false;
    }

    context.makeCurrent(new QOffscreenSurface());
    QOpenGLFunctions* functions = context.functions();
    
    QString glVersion = reinterpret_cast<const char*>(functions->glGetString(GL_VERSION));
    QString glRenderer = reinterpret_cast<const char*>(functions->glGetString(GL_RENDERER));
    QString glVendor = reinterpret_cast<const char*>(functions->glGetString(GL_VENDOR));
    
    qInfo() << "OpenGL Version:" << glVersion;
    qInfo() << "OpenGL Renderer:" << glRenderer;
    qInfo() << "OpenGL Vendor:" << glVendor;

    // 檢查最低 OpenGL 版本需求 (3.3)
    QStringList versionParts = glVersion.split('.');
    if (versionParts.size() >= 2) {
        int major = versionParts[0].toInt();
        int minor = versionParts[1].toInt();
        if (major < 3 || (major == 3 && minor < 3)) {
            qCritical() << "OpenGL 3.3 or higher is required";
            return false;
        }
    }

    return true;
}

/**
 * @brief 顯示啟動畫面
 */
QSplashScreen* ShowSplashScreen()
{
    QPixmap pixmap(":/images/splash.png");
    if (pixmap.isNull()) {
        // 如果沒有啟動畫面圖片，建立一個簡單的
        pixmap = QPixmap(400, 300);
        pixmap.fill(QColor(53, 53, 53));
        
        QPainter painter(&pixmap);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 24, QFont::Bold));
        painter.drawText(pixmap.rect(), Qt::AlignCenter, APP_NAME);
        painter.setFont(QFont("Arial", 12));
        painter.drawText(QRect(0, 250, 400, 50), Qt::AlignCenter, "Version " + APP_VERSION);
    }

    QSplashScreen* splash = new QSplashScreen(pixmap);
    splash->show();
    splash->showMessage("正在初始化...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    
    return splash;
}

/**
 * @brief 處理命令列參數
 */
void ProcessCommandLineArguments(QApplication& app, MainWindow& mainWindow)
{
    QCommandLineParser parser;
    parser.setApplicationDescription("跨平台物理場景編輯器");
    parser.addHelpOption();
    parser.addVersionOption();

    // 檔案參數
    QCommandLineOption fileOption(QStringList() << "f" << "file",
                                 "開啟指定的場景檔案", "filename");
    parser.addOption(fileOption);

    // 樣式參數
    QCommandLineOption darkThemeOption("dark-theme", "使用深色主題");
    parser.addOption(darkThemeOption);

    // 除錯參數
    QCommandLineOption debugOption("debug", "啟用除錯模式");
    parser.addOption(debugOption);

    // 無 GPU 模式
    QCommandLineOption noGpuOption("no-gpu", "停用 GPU 加速");
    parser.addOption(noGpuOption);

    // 解析參數
    parser.process(app);

    // 處理參數
    if (parser.isSet(darkThemeOption)) {
        // 啟用深色主題
        QPalette darkPalette;
        // ... 設定深色主題調色盤
        app.setPalette(darkPalette);
    }

    if (parser.isSet(debugOption)) {
        QLoggingCategory::setFilterRules("*.debug=true");
    }

    if (parser.isSet(fileOption)) {
        QString filename = parser.value(fileOption);
        QTimer::singleShot(100, [&mainWindow, filename]() {
            mainWindow.OpenScene(filename);
        });
    }
}

/**
 * @brief 主程式進入點
 */
int main(int argc, char *argv[])
{
    // 設定 Qt 應用程式屬性
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    // 建立應用程式
    QApplication app(argc, argv);

    // 設定應用程式資訊
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName(APP_ORGANIZATION);
    app.setOrganizationDomain(APP_DOMAIN);

    // 初始化日誌系統
    SetupLogging();

    qInfo() << "Starting" << APP_NAME << "version" << APP_VERSION;
    qInfo() << "Qt version:" << QT_VERSION_STR;
    qInfo() << "Platform:" << QGuiApplication::platformName();

    // 檢查系統需求
    if (!CheckSystemRequirements()) {
        QMessageBox::critical(nullptr, "系統需求不符", 
                             "您的系統不符合最低需求。請確保您的顯示卡支援 OpenGL 3.3 或更高版本。");
        return -1;
    }

    // 顯示啟動畫面
    QSplashScreen* splash = ShowSplashScreen();
    app.processEvents();

    // 設定應用程式樣式和資源
    SetupApplicationStyle(app);
    SetupApplicationResources(app);
    SetupInternationalization(app);

    splash->showMessage("正在載入主視窗...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();

    try {
        // 建立主視窗
        MainWindow mainWindow;
        
        splash->showMessage("正在初始化編輯器...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
        app.processEvents();

        // 處理命令列參數
        ProcessCommandLineArguments(app, mainWindow);

        // 顯示主視窗
        mainWindow.show();
        
        // 關閉啟動畫面
        splash->finish(&mainWindow);
        delete splash;

        qInfo() << "Application started successfully";

        // 進入事件循環
        int result = app.exec();

        qInfo() << "Application exiting with code" << result;
        return result;
    }
    catch (const std::exception& e) {
        qCritical() << "Unhandled exception:" << e.what();
        
        if (splash) {
            splash->close();
            delete splash;
        }

        QMessageBox::critical(nullptr, "嚴重錯誤", 
                             QString("應用程式遇到嚴重錯誤：\n%1").arg(e.what()));
        return -1;
    }
    catch (...) {
        qCritical() << "Unknown exception occurred";
        
        if (splash) {
            splash->close();
            delete splash;
        }

        QMessageBox::critical(nullptr, "嚴重錯誤", 
                             "應用程式遇到未知的嚴重錯誤。");
        return -1;
    }
}
