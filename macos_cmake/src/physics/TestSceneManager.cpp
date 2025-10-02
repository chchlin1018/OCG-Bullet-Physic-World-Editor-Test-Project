#include "physics/TestSceneManager.h"
#include <QDebug>
#include <QApplication>
#include <cmath>

namespace Physics {

// ============================================================================
// TestSceneManager Implementation
// ============================================================================

TestSceneManager::TestSceneManager(QObject* parent)
    : QObject(parent)
    , m_updateTimer(new QTimer(this))
    , m_currentSceneType(TestSceneType::ClothDropOnCylinder)
    , m_isRunning(false)
    , m_simulationSpeed(1.0f)
    , m_showWireframe(false)
    , m_showParticles(false)
    , m_showColliders(true)
    , m_fps(0.0f)
    , m_frameTime(0.0f)
    , m_frameCount(0)
    , m_fpsUpdateTime(0.0f)
{
    // 設定更新計時器
    m_updateTimer->setInterval(16);  // ~60 FPS
    connect(m_updateTimer, &QTimer::timeout, this, &TestSceneManager::onUpdateTimer);
    
    // 初始化布料模擬
    initializeClothSimulation();
    
    // 載入預設場景
    loadScene(TestSceneType::ClothDropOnCylinder);
    
    qDebug() << "測試場景管理器初始化完成";
}

TestSceneManager::~TestSceneManager() {
    stopSimulation();
}

void TestSceneManager::loadScene(TestSceneType sceneType) {
    qDebug() << "載入測試場景：" << static_cast<int>(sceneType);
    
    stopSimulation();
    m_currentSceneType = sceneType;
    
    setupScene(sceneType);
    
    emit sceneLoaded(sceneType);
    
    // 如果配置為自動開始，則啟動模擬
    if (m_currentConfig.autoStart) {
        startSimulation();
    }
}

void TestSceneManager::loadCustomScene(const SceneConfig& config) {
    qDebug() << "載入自訂場景";
    
    stopSimulation();
    m_currentConfig = config;
    
    applySceneConfig(config);
    
    if (config.autoStart) {
        startSimulation();
    }
}

void TestSceneManager::resetCurrentScene() {
    qDebug() << "重置當前場景";
    
    bool wasRunning = m_isRunning;
    stopSimulation();
    
    setupScene(m_currentSceneType);
    
    if (wasRunning && m_currentConfig.autoStart) {
        startSimulation();
    }
}

void TestSceneManager::startSimulation() {
    if (m_isRunning) return;
    
    qDebug() << "開始物理模擬";
    
    m_isRunning = true;
    m_frameCount = 0;
    m_fpsUpdateTime = 0.0f;
    
    m_updateTimer->start();
    emit simulationStarted();
}

void TestSceneManager::stopSimulation() {
    if (!m_isRunning) return;
    
    qDebug() << "停止物理模擬";
    
    m_isRunning = false;
    m_updateTimer->stop();
    emit simulationStopped();
}

void TestSceneManager::pauseSimulation() {
    if (!m_isRunning) return;
    
    qDebug() << "暫停物理模擬";
    
    m_updateTimer->stop();
    emit simulationPaused();
}

void TestSceneManager::resumeSimulation() {
    if (!m_isRunning) return;
    
    qDebug() << "恢復物理模擬";
    
    m_updateTimer->start();
    emit simulationResumed();
}

void TestSceneManager::setSceneConfig(const SceneConfig& config) {
    m_currentConfig = config;
    applySceneConfig(config);
}

void TestSceneManager::setSimulationSpeed(float speed) {
    m_simulationSpeed = std::max(0.1f, std::min(5.0f, speed));
    
    if (m_clothSim) {
        // 調整時間步長以反映速度變化
        float baseTimeStep = 1.0f / 60.0f;
        m_clothSim->setTimeStep(baseTimeStep * m_simulationSpeed);
    }
    
    qDebug() << "設定模擬速度：" << m_simulationSpeed;
}

void TestSceneManager::setTimeStep(float timeStep) {
    if (m_clothSim) {
        m_clothSim->setTimeStep(timeStep);
    }
}

void TestSceneManager::enableOGC(bool enable) {
    if (m_clothSim) {
        m_clothSim->setUseOGC(enable);
        m_currentConfig.useOGC = enable;
    }
    
    qDebug() << "OGC 接觸模型：" << (enable ? "啟用" : "停用");
}

void TestSceneManager::setOGCContactRadius(float radius) {
    if (m_clothSim) {
        m_clothSim->setOGCContactRadius(radius);
        m_currentConfig.ogcContactRadius = radius;
    }
    
    qDebug() << "OGC 接觸半徑：" << radius;
}

bool TestSceneManager::isOGCEnabled() const {
    return m_clothSim ? m_clothSim->getUseOGC() : false;
}

float TestSceneManager::getOGCContactRadius() const {
    return m_clothSim ? m_clothSim->getOGCContactRadius() : 0.05f;
}

bool TestSceneManager::isSimulationPaused() const {
    return m_isRunning && !m_updateTimer->isActive();
}

int TestSceneManager::getParticleCount() const {
    return m_clothSim ? m_clothSim->getParticleCount() : 0;
}

int TestSceneManager::getConstraintCount() const {
    return m_clothSim ? m_clothSim->getConstraintCount() : 0;
}

float TestSceneManager::getSimulationTime() const {
    return m_clothSim ? m_clothSim->getSimulationTime() : 0.0f;
}

void TestSceneManager::render() {
    if (!m_clothSim) return;
    
    // 渲染布料
    m_clothSim->render();
    
    // 渲染線框
    if (m_showWireframe) {
        m_clothSim->renderWireframe();
    }
    
    // 渲染粒子
    if (m_showParticles) {
        m_clothSim->renderParticles();
    }
    
    // 渲染碰撞體
    if (m_showColliders) {
        m_clothSim->renderColliders();
    }
}

void TestSceneManager::update(float deltaTime) {
    if (!m_clothSim || !m_isRunning) return;
    
    // 應用模擬速度
    float adjustedDeltaTime = deltaTime * m_simulationSpeed;
    
    // 更新物理模擬
    m_clothSim->update(adjustedDeltaTime);
    
    // 更新統計資訊
    updateStatistics(deltaTime);
}

void TestSceneManager::onUpdateTimer() {
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;
    
    update(deltaTime);
}

void TestSceneManager::initializeClothSimulation() {
    m_clothSim = std::make_unique<ClothSimulation>();
    qDebug() << "布料模擬系統初始化完成";
}

void TestSceneManager::setupScene(TestSceneType sceneType) {
    switch (sceneType) {
        case TestSceneType::ClothDropOnCylinder:
            setupClothDropOnCylinder();
            break;
        case TestSceneType::ClothDropOnSphere:
            setupClothDropOnSphere();
            break;
        case TestSceneType::ClothDropOnBox:
            setupClothDropOnBox();
            break;
        case TestSceneType::ClothWind:
            setupClothWind();
            break;
        case TestSceneType::ClothTearing:
            setupClothTearing();
            break;
        case TestSceneType::MultiClothInteraction:
            setupMultiClothInteraction();
            break;
    }
}

void TestSceneManager::updateStatistics(float deltaTime) {
    m_frameCount++;
    m_fpsUpdateTime += deltaTime;
    
    // 每秒更新一次 FPS
    if (m_fpsUpdateTime >= 1.0f) {
        m_fps = m_frameCount / m_fpsUpdateTime;
        m_frameTime = m_fpsUpdateTime / m_frameCount * 1000.0f;  // 毫秒
        
        emit statisticsUpdated(
            getParticleCount(),
            getConstraintCount(),
            getSimulationTime(),
            m_fps
        );
        
        m_frameCount = 0;
        m_fpsUpdateTime = 0.0f;
    }
}

void TestSceneManager::applySceneConfig(const SceneConfig& config) {
    if (!m_clothSim) return;
    
    // 重新初始化布料
    m_clothSim->initialize(config.clothWidth, config.clothHeight, config.clothSpacing);
    
    // 設定物理參數
    m_clothSim->setGravity(config.gravity);
    m_clothSim->setWind(config.wind);
    m_clothSim->setDamping(config.damping);
    
    // 設定 OGC 參數
    m_clothSim->setUseOGC(config.useOGC);
    m_clothSim->setOGCContactRadius(config.ogcContactRadius);
    
    // 添加碰撞體
    m_clothSim->addCylinder(
        config.cylinderCenter,
        config.cylinderRadius,
        config.cylinderHeight
    );
    
    // 設定模擬速度
    setSimulationSpeed(config.simulationSpeed);
    
    // 設定渲染選項
    m_showWireframe = config.showWireframe;
    m_showParticles = config.showParticles;
    m_showColliders = config.showColliders;
}

void TestSceneManager::setupClothDropOnCylinder() {
    qDebug() << "設定布料掉落到圓柱體場景";
    
    m_currentConfig = getDefaultClothDropConfig();
    applySceneConfig(m_currentConfig);
}

void TestSceneManager::setupClothDropOnSphere() {
    qDebug() << "設定布料掉落到球體場景";
    
    SceneConfig config = getDefaultClothDropConfig();
    config.cylinderRadius = 1.2f;  // 較小的半徑模擬球體
    config.cylinderHeight = 0.1f;  // 很小的高度
    
    m_currentConfig = config;
    applySceneConfig(config);
}

void TestSceneManager::setupClothDropOnBox() {
    qDebug() << "設定布料掉落到盒子場景";
    
    SceneConfig config = getDefaultClothDropConfig();
    config.cylinderRadius = 1.0f;
    config.cylinderHeight = 1.0f;  // 較高的圓柱體模擬盒子
    
    m_currentConfig = config;
    applySceneConfig(config);
}

void TestSceneManager::setupClothWind() {
    qDebug() << "設定布料風力測試場景";
    
    SceneConfig config = getWindTestConfig();
    m_currentConfig = config;
    applySceneConfig(config);
}

void TestSceneManager::setupClothTearing() {
    qDebug() << "設定布料撕裂測試場景";
    
    SceneConfig config = getHighResolutionConfig();
    config.wind = QVector3D(5.0f, 0, 0);  // 強風力
    
    m_currentConfig = config;
    applySceneConfig(config);
}

void TestSceneManager::setupMultiClothInteraction() {
    qDebug() << "設定多布料互動場景";
    
    SceneConfig config = getDefaultClothDropConfig();
    config.clothWidth = 15;
    config.clothHeight = 15;
    
    m_currentConfig = config;
    applySceneConfig(config);
}

// 靜態方法實現
SceneConfig TestSceneManager::getDefaultClothDropConfig() {
    SceneConfig config;
    config.clothWidth = 20;
    config.clothHeight = 20;
    config.clothSpacing = 0.2f;
    config.clothPosition = QVector3D(0, 3, 0);
    config.gravity = QVector3D(0, -9.81f, 0);
    config.wind = QVector3D(0, 0, 0);
    config.damping = 0.99f;
    config.useOGC = true;
    config.ogcContactRadius = 0.05f;
    config.cylinderCenter = QVector3D(0, -1, 0);
    config.cylinderRadius = 1.5f;
    config.cylinderHeight = 0.5f;
    config.autoStart = true;
    config.simulationSpeed = 1.0f;
    config.showWireframe = false;
    config.showParticles = false;
    config.showColliders = true;
    return config;
}

SceneConfig TestSceneManager::getWindTestConfig() {
    SceneConfig config = getDefaultClothDropConfig();
    config.wind = QVector3D(2.0f, 0, 1.0f);
    config.cylinderCenter = QVector3D(0, -2, 0);
    config.showWireframe = true;
    return config;
}

SceneConfig TestSceneManager::getHighResolutionConfig() {
    SceneConfig config = getDefaultClothDropConfig();
    config.clothWidth = 30;
    config.clothHeight = 30;
    config.clothSpacing = 0.15f;
    config.ogcContactRadius = 0.03f;
    return config;
}

// ============================================================================
// AutoTestRunner Implementation
// ============================================================================

AutoTestRunner::AutoTestRunner(TestSceneManager* sceneManager, QObject* parent)
    : QObject(parent)
    , m_sceneManager(sceneManager)
    , m_testTimer(new QTimer(this))
    , m_autoTestRunning(false)
    , m_testDuration(10.0f)
    , m_currentTestTime(0.0f)
    , m_currentSceneIndex(0)
{
    connect(m_testTimer, &QTimer::timeout, this, &AutoTestRunner::onTestTimer);
    m_testTimer->setInterval(100);  // 每 100ms 更新一次
    
    // 設定預設測試序列
    m_sceneSequence = {
        TestSceneType::ClothDropOnCylinder,
        TestSceneType::ClothDropOnSphere,
        TestSceneType::ClothWind,
        TestSceneType::ClothDropOnBox
    };
    
    qDebug() << "自動測試執行器初始化完成";
}

void AutoTestRunner::startAutoTest() {
    if (m_autoTestRunning || !m_sceneManager) return;
    
    qDebug() << "開始自動測試序列";
    
    m_autoTestRunning = true;
    m_currentSceneIndex = 0;
    m_currentTestTime = 0.0f;
    
    // 載入第一個測試場景
    if (!m_sceneSequence.empty()) {
        m_sceneManager->loadScene(m_sceneSequence[0]);
        emit testSceneChanged(m_sceneSequence[0]);
    }
    
    m_testTimer->start();
    emit autoTestStarted();
}

void AutoTestRunner::stopAutoTest() {
    if (!m_autoTestRunning) return;
    
    qDebug() << "停止自動測試序列";
    
    m_autoTestRunning = false;
    m_testTimer->stop();
    
    emit autoTestFinished();
}

void AutoTestRunner::setSceneSequence(const std::vector<TestSceneType>& sequence) {
    m_sceneSequence = sequence;
    qDebug() << "設定測試場景序列，共" << sequence.size() << "個場景";
}

TestSceneType AutoTestRunner::getCurrentTestScene() const {
    if (m_currentSceneIndex < m_sceneSequence.size()) {
        return m_sceneSequence[m_currentSceneIndex];
    }
    return TestSceneType::ClothDropOnCylinder;
}

float AutoTestRunner::getCurrentTestProgress() const {
    if (m_testDuration <= 0) return 1.0f;
    return std::min(1.0f, m_currentTestTime / m_testDuration);
}

void AutoTestRunner::onTestTimer() {
    if (!m_autoTestRunning) return;
    
    m_currentTestTime += 0.1f;  // 100ms
    
    emit testProgressUpdated(getCurrentTestProgress());
    
    // 檢查是否需要切換到下一個場景
    if (m_currentTestTime >= m_testDuration) {
        nextTestScene();
    }
}

void AutoTestRunner::nextTestScene() {
    m_currentSceneIndex++;
    m_currentTestTime = 0.0f;
    
    if (m_currentSceneIndex >= m_sceneSequence.size()) {
        // 所有測試完成
        stopAutoTest();
        return;
    }
    
    // 載入下一個測試場景
    TestSceneType nextScene = m_sceneSequence[m_currentSceneIndex];
    m_sceneManager->loadScene(nextScene);
    emit testSceneChanged(nextScene);
    
    qDebug() << "切換到測試場景：" << static_cast<int>(nextScene);
}

} // namespace Physics

#include "TestSceneManager.moc"
