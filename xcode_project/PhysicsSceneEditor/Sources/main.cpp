/**
 * @file main.cpp
 * @brief 跨平台物理場景執行器主程式
 * 
 * 這是一個跨平台的物理場景執行器，能夠載入由 MFC 編輯器產生的場景描述檔案，
 * 並使用 OGC 和 Bullet Physics 進行即時物理模擬和 OpenGL 視覺化。
 * 
 * 支援平台：
 * - Windows (Visual Studio, MinGW)
 * - macOS (Xcode, Clang)
 * - Linux (GCC, Clang)
 * 
 * 主要功能：
 * - 載入 .pscene 物理場景檔案
 * - 即時物理模擬（OGC + Bullet Physics）
 * - OpenGL 3D 視覺化
 * - 鍵盤和滑鼠互動控制
 * - 效能監控和統計
 */

#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

// 跨平台標頭檔
#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define getcwd _getcwd
#else
    #include <unistd.h>
    #include <sys/stat.h>
#endif

// OpenGL 和視窗管理
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 專案標頭檔
#include "physics_engine.h"
#include "renderer.h"
#include "input_manager.h"
#include "scene_loader.h"
#include "performance_monitor.h"
#include "../scene_format/physics_scene_format.h"

/**
 * @class PhysicsSceneRunner
 * @brief 物理場景執行器主類別
 * 
 * 負責整合所有子系統，包括物理引擎、渲染器、輸入管理等，
 * 提供完整的場景執行環境。
 */
class PhysicsSceneRunner {
public:
    PhysicsSceneRunner();
    ~PhysicsSceneRunner();

    // 初始化和清理
    bool Initialize(int argc, char* argv[]);
    void Cleanup();

    // 主執行迴圈
    int Run();

    // 場景管理
    bool LoadScene(const std::string& filename);
    void ResetScene();
    void SaveScreenshot(const std::string& filename);

    // 模擬控制
    void PlaySimulation();
    void PauseSimulation();
    void StopSimulation();
    void StepSimulation();
    bool IsSimulationRunning() const { return m_simulationState == SimulationState::Playing; }

private:
    // 模擬狀態
    enum class SimulationState {
        Stopped,
        Playing,
        Paused
    };

    // 核心子系統
    std::unique_ptr<PhysicsEngine> m_physicsEngine;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<SceneLoader> m_sceneLoader;
    std::unique_ptr<PerformanceMonitor> m_performanceMonitor;

    // GLFW 視窗
    GLFWwindow* m_window;
    int m_windowWidth;
    int m_windowHeight;
    std::string m_windowTitle;

    // 場景資料
    PhysicsScene::PhysicsScene m_scene;
    std::string m_currentSceneFile;
    bool m_sceneLoaded;

    // 模擬狀態
    SimulationState m_simulationState;
    double m_simulationTime;
    double m_lastFrameTime;
    float m_timeScale;

    // 效能統計
    double m_frameTime;
    double m_physicsTime;
    double m_renderTime;
    int m_frameCount;
    double m_fpsUpdateTime;
    float m_currentFPS;

    // 設定
    struct Settings {
        bool vsync = true;
        bool fullscreen = false;
        int multisamples = 4;
        bool showUI = true;
        bool showStats = true;
        bool showHelp = false;
        float mouseSensitivity = 1.0f;
        float cameraSpeed = 5.0f;
    } m_settings;

    // 初始化函數
    bool InitializeGLFW();
    bool InitializeOpenGL();
    bool InitializeSubsystems();
    void SetupCallbacks();

    // 主迴圈函數
    void Update(double deltaTime);
    void Render();
    void UpdateUI();
    void UpdateStatistics(double deltaTime);

    // 事件處理
    void ProcessInput();
    void HandleKeyboard(double deltaTime);
    void HandleMouse();

    // 回調函數
    static void ErrorCallback(int error, const char* description);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void WindowSizeCallback(GLFWwindow* window, int width, int height);
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    // 輔助函數
    void PrintUsage(const char* programName);
    void PrintSystemInfo();
    void PrintControls();
    std::string GetExecutablePath();
    std::string GetResourcePath(const std::string& relativePath);

    // 錯誤處理
    void HandleError(const std::string& message);
    bool CheckGLError(const std::string& operation);

public:
    // 靜態存取器（用於回調函數）
    static PhysicsSceneRunner* GetInstance() { return s_instance; }

private:
    static PhysicsSceneRunner* s_instance;
};

// 靜態成員定義
PhysicsSceneRunner* PhysicsSceneRunner::s_instance = nullptr;

/**
 * @brief 建構函數
 */
PhysicsSceneRunner::PhysicsSceneRunner()
    : m_window(nullptr)
    , m_windowWidth(1280)
    , m_windowHeight(720)
    , m_windowTitle("Physics Scene Runner")
    , m_sceneLoaded(false)
    , m_simulationState(SimulationState::Stopped)
    , m_simulationTime(0.0)
    , m_lastFrameTime(0.0)
    , m_timeScale(1.0f)
    , m_frameTime(0.0)
    , m_physicsTime(0.0)
    , m_renderTime(0.0)
    , m_frameCount(0)
    , m_fpsUpdateTime(0.0)
    , m_currentFPS(0.0f)
{
    s_instance = this;
}

/**
 * @brief 解構函數
 */
PhysicsSceneRunner::~PhysicsSceneRunner() {
    Cleanup();
    s_instance = nullptr;
}

/**
 * @brief 初始化執行器
 */
bool PhysicsSceneRunner::Initialize(int argc, char* argv[]) {
    std::cout << "Initializing Physics Scene Runner..." << std::endl;

    // 解析命令列參數
    std::string sceneFile;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            PrintUsage(argv[0]);
            return false;
        } else if (arg == "--fullscreen" || arg == "-f") {
            m_settings.fullscreen = true;
        } else if (arg == "--no-vsync") {
            m_settings.vsync = false;
        } else if (arg == "--width" && i + 1 < argc) {
            m_windowWidth = std::atoi(argv[++i]);
        } else if (arg == "--height" && i + 1 < argc) {
            m_windowHeight = std::atoi(argv[++i]);
        } else if (arg.find(".pscene") != std::string::npos) {
            sceneFile = arg;
        }
    }

    // 初始化 GLFW
    if (!InitializeGLFW()) {
        return false;
    }

    // 初始化 OpenGL
    if (!InitializeOpenGL()) {
        return false;
    }

    // 初始化子系統
    if (!InitializeSubsystems()) {
        return false;
    }

    // 設定回調函數
    SetupCallbacks();

    // 載入場景（如果指定）
    if (!sceneFile.empty()) {
        if (!LoadScene(sceneFile)) {
            std::cerr << "Warning: Failed to load scene file: " << sceneFile << std::endl;
        }
    }

    // 列印系統資訊
    PrintSystemInfo();
    PrintControls();

    std::cout << "Physics Scene Runner initialized successfully!" << std::endl;
    return true;
}

/**
 * @brief 清理資源
 */
void PhysicsSceneRunner::Cleanup() {
    std::cout << "Cleaning up Physics Scene Runner..." << std::endl;

    // 清理子系統
    m_performanceMonitor.reset();
    m_sceneLoader.reset();
    m_inputManager.reset();
    m_renderer.reset();
    m_physicsEngine.reset();

    // 清理 GLFW
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

/**
 * @brief 主執行迴圈
 */
int PhysicsSceneRunner::Run() {
    std::cout << "Starting main loop..." << std::endl;

    m_lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;

        // 限制最大時間步長
        deltaTime = std::min(deltaTime, 1.0 / 30.0); // 最低 30 FPS

        // 處理輸入
        ProcessInput();

        // 更新模擬
        Update(deltaTime);

        // 渲染場景
        Render();

        // 更新統計
        UpdateStatistics(deltaTime);

        // 交換緩衝區和處理事件
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    std::cout << "Main loop ended." << std::endl;
    return 0;
}

/**
 * @brief 載入物理場景
 */
bool PhysicsSceneRunner::LoadScene(const std::string& filename) {
    std::cout << "Loading scene: " << filename << std::endl;

    if (!m_sceneLoader->LoadScene(filename, m_scene)) {
        std::cerr << "Failed to load scene: " << filename << std::endl;
        return false;
    }

    // 初始化物理引擎
    if (!m_physicsEngine->InitializeScene(m_scene)) {
        std::cerr << "Failed to initialize physics engine with scene" << std::endl;
        return false;
    }

    // 初始化渲染器
    if (!m_renderer->InitializeScene(m_scene)) {
        std::cerr << "Failed to initialize renderer with scene" << std::endl;
        return false;
    }

    m_currentSceneFile = filename;
    m_sceneLoaded = true;
    m_simulationTime = 0.0;

    // 更新視窗標題
    std::string title = m_windowTitle + " - " + filename;
    glfwSetWindowTitle(m_window, title.c_str());

    std::cout << "Scene loaded successfully!" << std::endl;
    return true;
}

/**
 * @brief 重置場景
 */
void PhysicsSceneRunner::ResetScene() {
    if (!m_sceneLoaded) return;

    std::cout << "Resetting scene..." << std::endl;

    StopSimulation();
    m_physicsEngine->ResetScene();
    m_simulationTime = 0.0;

    std::cout << "Scene reset complete." << std::endl;
}

/**
 * @brief 主程式進入點
 */
int main(int argc, char* argv[]) {
    try {
        PhysicsSceneRunner runner;

        if (!runner.Initialize(argc, argv)) {
            return -1;
        }

        return runner.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return -1;
    }
}
