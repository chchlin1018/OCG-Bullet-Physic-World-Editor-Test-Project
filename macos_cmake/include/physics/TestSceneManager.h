#pragma once

#include "ClothSimulation.h"
#include <QTimer>
#include <QObject>
#include <memory>

namespace Physics {

/**
 * @brief 測試場景類型枚舉
 */
enum class TestSceneType {
    ClothDropOnCylinder,    // 布料掉落到圓柱體
    ClothDropOnSphere,      // 布料掉落到球體
    ClothDropOnBox,         // 布料掉落到盒子
    ClothWind,              // 布料風力測試
    ClothTearing,           // 布料撕裂測試
    MultiClothInteraction   // 多布料互動
};

/**
 * @brief 場景配置結構
 */
struct SceneConfig {
    // 布料參數
    int clothWidth = 20;
    int clothHeight = 20;
    float clothSpacing = 0.2f;
    QVector3D clothPosition = QVector3D(0, 3, 0);
    
    // 物理參數
    QVector3D gravity = QVector3D(0, -9.81f, 0);
    QVector3D wind = QVector3D(0, 0, 0);
    float damping = 0.99f;
    
    // OGC 參數
    bool useOGC = true;
    float ogcContactRadius = 0.05f;
    
    // 碰撞體參數
    QVector3D cylinderCenter = QVector3D(0, -1, 0);
    float cylinderRadius = 1.5f;
    float cylinderHeight = 0.5f;
    
    // 模擬參數
    bool autoStart = true;
    float simulationSpeed = 1.0f;
    bool showWireframe = false;
    bool showParticles = false;
    bool showColliders = true;
};

/**
 * @brief 測試場景管理器
 */
class TestSceneManager : public QObject {
    Q_OBJECT

public:
    explicit TestSceneManager(QObject* parent = nullptr);
    ~TestSceneManager();
    
    // 場景管理
    void loadScene(TestSceneType sceneType);
    void loadCustomScene(const SceneConfig& config);
    void resetCurrentScene();
    void startSimulation();
    void stopSimulation();
    void pauseSimulation();
    void resumeSimulation();
    
    // 場景配置
    void setSceneConfig(const SceneConfig& config);
    SceneConfig getSceneConfig() const { return m_currentConfig; }
    
    // 模擬控制
    void setSimulationSpeed(float speed);
    float getSimulationSpeed() const { return m_simulationSpeed; }
    void setTimeStep(float timeStep);
    
    // 渲染控制
    void setShowWireframe(bool show) { m_showWireframe = show; }
    void setShowParticles(bool show) { m_showParticles = show; }
    void setShowColliders(bool show) { m_showColliders = show; }
    bool getShowWireframe() const { return m_showWireframe; }
    bool getShowParticles() const { return m_showParticles; }
    bool getShowColliders() const { return m_showColliders; }
    
    // OGC 控制
    void enableOGC(bool enable);
    void setOGCContactRadius(float radius);
    bool isOGCEnabled() const;
    float getOGCContactRadius() const;
    
    // 狀態查詢
    bool isSimulationRunning() const { return m_isRunning; }
    bool isSimulationPaused() const;
    TestSceneType getCurrentSceneType() const { return m_currentSceneType; }
    
    // 統計資訊
    int getParticleCount() const;
    int getConstraintCount() const;
    float getSimulationTime() const;
    float getFPS() const { return m_fps; }
    
    // 渲染
    void render();
    void update(float deltaTime);
    
    // 預設場景配置
    static SceneConfig getDefaultClothDropConfig();
    static SceneConfig getWindTestConfig();
    static SceneConfig getHighResolutionConfig();

signals:
    void simulationStarted();
    void simulationStopped();
    void simulationPaused();
    void simulationResumed();
    void sceneLoaded(TestSceneType sceneType);
    void statisticsUpdated(int particles, int constraints, float simTime, float fps);

private slots:
    void onUpdateTimer();

private:
    // 核心組件
    std::unique_ptr<ClothSimulation> m_clothSim;
    QTimer* m_updateTimer;
    
    // 場景狀態
    TestSceneType m_currentSceneType;
    SceneConfig m_currentConfig;
    bool m_isRunning;
    float m_simulationSpeed;
    
    // 渲染選項
    bool m_showWireframe;
    bool m_showParticles;
    bool m_showColliders;
    
    // 效能統計
    float m_fps;
    float m_frameTime;
    int m_frameCount;
    float m_fpsUpdateTime;
    
    // 私有方法
    void initializeClothSimulation();
    void setupScene(TestSceneType sceneType);
    void updateStatistics(float deltaTime);
    void applySceneConfig(const SceneConfig& config);
    
    // 場景設定方法
    void setupClothDropOnCylinder();
    void setupClothDropOnSphere();
    void setupClothDropOnBox();
    void setupClothWind();
    void setupClothTearing();
    void setupMultiClothInteraction();
};

/**
 * @brief 自動測試場景執行器
 */
class AutoTestRunner : public QObject {
    Q_OBJECT

public:
    explicit AutoTestRunner(TestSceneManager* sceneManager, QObject* parent = nullptr);
    
    // 自動測試控制
    void startAutoTest();
    void stopAutoTest();
    void setTestDuration(float seconds) { m_testDuration = seconds; }
    void setSceneSequence(const std::vector<TestSceneType>& sequence);
    
    // 狀態查詢
    bool isAutoTestRunning() const { return m_autoTestRunning; }
    TestSceneType getCurrentTestScene() const;
    float getCurrentTestProgress() const;

signals:
    void autoTestStarted();
    void autoTestFinished();
    void testSceneChanged(TestSceneType sceneType);
    void testProgressUpdated(float progress);

private slots:
    void onTestTimer();

private:
    TestSceneManager* m_sceneManager;
    QTimer* m_testTimer;
    
    bool m_autoTestRunning;
    float m_testDuration;
    float m_currentTestTime;
    int m_currentSceneIndex;
    std::vector<TestSceneType> m_sceneSequence;
    
    void nextTestScene();
};

} // namespace Physics
