/**
 * @file integration_test.cpp
 * @brief 整合測試程式
 * 
 * 測試 MFC 編輯器產生的場景檔案能否正確在跨平台執行器中執行。
 * 包含完整的工作流程測試：編輯器 -> 場景檔案 -> 執行器。
 */

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <memory>

#include "../scene_format/physics_scene_format.h"
#include "../cross_platform_runner/scene_loader.h"
#include "../cross_platform_runner/physics_engine.h"
#include "../cross_platform_runner/renderer.h"

/**
 * @class IntegrationTest
 * @brief 整合測試類別
 * 
 * 提供完整的端到端測試功能，模擬從場景編輯到執行的完整流程。
 */
class IntegrationTest {
public:
    IntegrationTest() : 
        sceneLoader(std::make_unique<SceneLoader>()),
        physicsEngine(std::make_unique<PhysicsEngine>()),
        testsPassed(0),
        testsFailed(0) {
    }

    ~IntegrationTest() = default;

    /**
     * @brief 執行所有整合測試
     * @return 是否所有測試都通過
     */
    bool RunAllTests() {
        std::cout << "=== Physics Scene Editor Integration Tests ===" << std::endl;
        std::cout << std::endl;

        // 基本功能測試
        RunTest("Basic Scene Loading", [this]() { return TestBasicSceneLoading(); });
        RunTest("Physics Engine Integration", [this]() { return TestPhysicsEngineIntegration(); });
        RunTest("Complex Scene Simulation", [this]() { return TestComplexSceneSimulation(); });
        RunTest("Error Handling", [this]() { return TestErrorHandling(); });
        RunTest("Performance Benchmarks", [this]() { return TestPerformanceBenchmarks(); });
        RunTest("Cross-Platform Compatibility", [this]() { return TestCrossPlatformCompatibility(); });
        RunTest("Memory Management", [this]() { return TestMemoryManagement(); });
        RunTest("Concurrent Access", [this]() { return TestConcurrentAccess(); });

        // 輸出測試結果
        std::cout << std::endl;
        std::cout << "=== Test Results ===" << std::endl;
        std::cout << "Tests Passed: " << testsPassed << std::endl;
        std::cout << "Tests Failed: " << testsFailed << std::endl;
        std::cout << "Success Rate: " << (testsPassed * 100.0 / (testsPassed + testsFailed)) << "%" << std::endl;

        return testsFailed == 0;
    }

private:
    std::unique_ptr<SceneLoader> sceneLoader;
    std::unique_ptr<PhysicsEngine> physicsEngine;
    int testsPassed;
    int testsFailed;

    /**
     * @brief 執行單一測試
     */
    void RunTest(const std::string& testName, std::function<bool()> testFunc) {
        std::cout << "Running: " << testName << "... ";
        std::cout.flush();

        auto start = std::chrono::high_resolution_clock::now();
        bool result = false;
        
        try {
            result = testFunc();
        } catch (const std::exception& e) {
            std::cout << "EXCEPTION: " << e.what() << std::endl;
            result = false;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        if (result) {
            std::cout << "PASS (" << duration.count() << "ms)" << std::endl;
            testsPassed++;
        } else {
            std::cout << "FAIL (" << duration.count() << "ms)" << std::endl;
            testsFailed++;
        }
    }

    /**
     * @brief 建立測試場景
     */
    PhysicsScene::PhysicsScene CreateTestScene() {
        PhysicsScene::PhysicsScene scene;

        // 基本設定
        scene.metadata.name = "Integration Test Scene";
        scene.metadata.description = "Test scene for integration testing";
        scene.metadata.author = "Integration Test";
        scene.metadata.version = "1.0.0";

        // 模擬設定
        scene.simulationSettings.timeStep = 0.016666667f;
        scene.simulationSettings.gravity = {0.0f, -9.81f, 0.0f};
        scene.simulationSettings.solverIterations = 10;
        scene.simulationSettings.useOGCContact = true;
        scene.simulationSettings.ogcContactRadius = 0.1f;
        scene.simulationSettings.hybridMode = true;

        // 物理材質
        PhysicsScene::PhysicsMaterial defaultMaterial;
        defaultMaterial.name = "Default";
        defaultMaterial.density = 1000.0f;
        defaultMaterial.friction = 0.5f;
        defaultMaterial.restitution = 0.3f;
        scene.physicsMaterials["default"] = defaultMaterial;

        // 地面
        PhysicsScene::RigidBody ground;
        ground.name = "Ground";
        ground.transform.position = {0.0f, 0.0f, 0.0f};
        ground.shapeType = PhysicsScene::ShapeType::Plane;
        ground.shapeParameters.normal = {0.0f, 1.0f, 0.0f};
        ground.shapeParameters.distance = 0.0f;
        ground.mass = 0.0f; // 靜態物體
        ground.physicsMaterial = "default";
        scene.rigidBodies["ground"] = ground;

        // 掉落的盒子
        PhysicsScene::RigidBody box;
        box.name = "Falling Box";
        box.transform.position = {0.0f, 5.0f, 0.0f};
        box.shapeType = PhysicsScene::ShapeType::Box;
        box.shapeParameters.halfExtents = {0.5f, 0.5f, 0.5f};
        box.mass = 1.0f;
        box.physicsMaterial = "default";
        scene.rigidBodies["box"] = box;

        return scene;
    }

    /**
     * @brief 測試基本場景載入
     */
    bool TestBasicSceneLoading() {
        auto scene = CreateTestScene();
        
        // 儲存場景
        std::string filename = "test_basic_scene.pscene";
        if (!sceneLoader->SaveScene(filename, scene)) {
            std::cerr << "Failed to save scene: " << sceneLoader->GetLastError() << std::endl;
            return false;
        }

        // 載入場景
        PhysicsScene::PhysicsScene loadedScene;
        if (!sceneLoader->LoadScene(filename, loadedScene)) {
            std::cerr << "Failed to load scene: " << sceneLoader->GetLastError() << std::endl;
            return false;
        }

        // 驗證場景內容
        if (loadedScene.rigidBodies.size() != scene.rigidBodies.size()) {
            std::cerr << "RigidBody count mismatch" << std::endl;
            return false;
        }

        if (loadedScene.metadata.name != scene.metadata.name) {
            std::cerr << "Scene name mismatch" << std::endl;
            return false;
        }

        // 清理
        std::remove(filename.c_str());
        return true;
    }

    /**
     * @brief 測試物理引擎整合
     */
    bool TestPhysicsEngineIntegration() {
        auto scene = CreateTestScene();

        // 初始化物理引擎
        if (!physicsEngine->Initialize()) {
            std::cerr << "Failed to initialize physics engine" << std::endl;
            return false;
        }

        // 載入場景到物理引擎
        if (!physicsEngine->InitializeScene(scene)) {
            std::cerr << "Failed to initialize scene in physics engine" << std::endl;
            return false;
        }

        // 執行幾個時間步
        for (int i = 0; i < 60; ++i) { // 1 秒的模擬
            physicsEngine->StepSimulation(0.016666667f);
        }

        // 檢查盒子是否下降
        auto boxTransform = physicsEngine->GetRigidBodyTransform("box");
        if (boxTransform.position.y >= 4.9f) { // 應該已經下降
            std::cerr << "Box did not fall as expected" << std::endl;
            return false;
        }

        // 檢查統計資訊
        auto stats = physicsEngine->GetStatistics();
        if (stats.rigidBodyCount != 2) { // 地面 + 盒子
            std::cerr << "Incorrect rigid body count in statistics" << std::endl;
            return false;
        }

        physicsEngine->Cleanup();
        return true;
    }

    /**
     * @brief 測試複雜場景模擬
     */
    bool TestComplexSceneSimulation() {
        // 載入複雜測試場景
        PhysicsScene::PhysicsScene scene;
        if (!sceneLoader->LoadScene("../examples/complex_constraint_test.pscene", scene)) {
            // 如果範例檔案不存在，建立一個簡化版本
            scene = CreateComplexTestScene();
        }

        // 初始化物理引擎
        if (!physicsEngine->Initialize()) {
            return false;
        }

        if (!physicsEngine->InitializeScene(scene)) {
            return false;
        }

        // 執行較長時間的模擬
        for (int i = 0; i < 300; ++i) { // 5 秒的模擬
            physicsEngine->StepSimulation(0.016666667f);
            
            // 每秒檢查一次統計資訊
            if (i % 60 == 0) {
                auto stats = physicsEngine->GetStatistics();
                if (stats.simulationTime < 0.0f) {
                    std::cerr << "Invalid simulation time" << std::endl;
                    return false;
                }
            }
        }

        physicsEngine->Cleanup();
        return true;
    }

    /**
     * @brief 建立複雜測試場景
     */
    PhysicsScene::PhysicsScene CreateComplexTestScene() {
        auto scene = CreateTestScene();

        // 新增更多物件
        for (int i = 0; i < 5; ++i) {
            PhysicsScene::RigidBody box;
            box.name = "Box " + std::to_string(i);
            box.transform.position = {static_cast<float>(i - 2), 3.0f + i * 0.5f, 0.0f};
            box.shapeType = PhysicsScene::ShapeType::Box;
            box.shapeParameters.halfExtents = {0.3f, 0.3f, 0.3f};
            box.mass = 0.5f + i * 0.1f;
            box.physicsMaterial = "default";
            scene.rigidBodies["box_" + std::to_string(i)] = box;
        }

        // 新增約束
        PhysicsScene::Constraint hinge;
        hinge.name = "Test Hinge";
        hinge.constraintType = PhysicsScene::ConstraintType::Hinge;
        hinge.bodyA = "box_0";
        hinge.bodyB = "box_1";
        hinge.frameA.position = {0.3f, 0.0f, 0.0f};
        hinge.frameB.position = {-0.3f, 0.0f, 0.0f};
        scene.constraints["hinge"] = hinge;

        return scene;
    }

    /**
     * @brief 測試錯誤處理
     */
    bool TestErrorHandling() {
        // 測試載入不存在的檔案
        PhysicsScene::PhysicsScene scene;
        if (sceneLoader->LoadScene("nonexistent_file.pscene", scene)) {
            std::cerr << "Should have failed to load nonexistent file" << std::endl;
            return false;
        }

        // 測試載入無效的 JSON
        std::ofstream invalidFile("invalid_test.pscene");
        invalidFile << "{ invalid json }";
        invalidFile.close();

        if (sceneLoader->LoadScene("invalid_test.pscene", scene)) {
            std::cerr << "Should have failed to load invalid JSON" << std::endl;
            return false;
        }

        // 測試物理引擎錯誤處理
        PhysicsScene::PhysicsScene invalidScene;
        invalidScene.rigidBodies["invalid"].mass = -1.0f; // 無效質量

        if (!physicsEngine->Initialize()) {
            return false;
        }

        // 這應該會失敗或產生警告
        physicsEngine->InitializeScene(invalidScene);

        // 清理
        std::remove("invalid_test.pscene");
        physicsEngine->Cleanup();
        return true;
    }

    /**
     * @brief 測試效能基準
     */
    bool TestPerformanceBenchmarks() {
        auto scene = CreateTestScene();

        // 新增大量物件進行效能測試
        for (int i = 0; i < 50; ++i) {
            PhysicsScene::RigidBody box;
            box.name = "PerfBox " + std::to_string(i);
            box.transform.position = {
                static_cast<float>(i % 10 - 5), 
                5.0f + (i / 10) * 2.0f, 
                static_cast<float>((i / 5) % 10 - 5)
            };
            box.shapeType = PhysicsScene::ShapeType::Box;
            box.shapeParameters.halfExtents = {0.2f, 0.2f, 0.2f};
            box.mass = 0.5f;
            box.physicsMaterial = "default";
            scene.rigidBodies["perfbox_" + std::to_string(i)] = box;
        }

        // 測量載入時間
        auto start = std::chrono::high_resolution_clock::now();
        if (!physicsEngine->Initialize() || !physicsEngine->InitializeScene(scene)) {
            return false;
        }
        auto loadTime = std::chrono::high_resolution_clock::now() - start;

        // 測量模擬時間
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 60; ++i) {
            physicsEngine->StepSimulation(0.016666667f);
        }
        auto simTime = std::chrono::high_resolution_clock::now() - start;

        // 檢查效能指標
        auto stats = physicsEngine->GetStatistics();
        
        std::cout << std::endl;
        std::cout << "  Load time: " << std::chrono::duration_cast<std::chrono::milliseconds>(loadTime).count() << "ms" << std::endl;
        std::cout << "  Simulation time (60 steps): " << std::chrono::duration_cast<std::chrono::milliseconds>(simTime).count() << "ms" << std::endl;
        std::cout << "  Average step time: " << (stats.simulationTime / 60.0f * 1000.0f) << "ms" << std::endl;
        std::cout << "  Active bodies: " << stats.activeBodyCount << std::endl;

        physicsEngine->Cleanup();

        // 基本效能要求（可根據需要調整）
        if (std::chrono::duration_cast<std::chrono::milliseconds>(loadTime).count() > 1000) {
            std::cerr << "Load time too slow" << std::endl;
            return false;
        }

        if (stats.simulationTime / 60.0f > 0.020f) { // 平均每步超過 20ms
            std::cerr << "Simulation too slow" << std::endl;
            return false;
        }

        return true;
    }

    /**
     * @brief 測試跨平台相容性
     */
    bool TestCrossPlatformCompatibility() {
        auto scene = CreateTestScene();
        
        // 測試檔案路徑處理
        std::vector<std::string> testPaths = {
            "test_scene.pscene",
            "./test_scene.pscene",
            "../test_scene.pscene"
        };

        for (const auto& path : testPaths) {
            if (!sceneLoader->SaveScene(path, scene)) {
                continue; // 某些路徑可能無效，這是正常的
            }

            PhysicsScene::PhysicsScene loadedScene;
            if (!sceneLoader->LoadScene(path, loadedScene)) {
                std::cerr << "Failed to load from path: " << path << std::endl;
                return false;
            }

            std::remove(path.c_str());
        }

        // 測試不同的檔案格式設定
        SceneLoader::SaveOptions options;
        options.prettyFormat = false;
        sceneLoader->SetSaveOptions(options);

        if (!sceneLoader->SaveScene("compact_test.pscene", scene)) {
            return false;
        }

        PhysicsScene::PhysicsScene loadedScene;
        if (!sceneLoader->LoadScene("compact_test.pscene", loadedScene)) {
            return false;
        }

        std::remove("compact_test.pscene");
        return true;
    }

    /**
     * @brief 測試記憶體管理
     */
    bool TestMemoryManagement() {
        // 測試多次初始化和清理
        for (int cycle = 0; cycle < 5; ++cycle) {
            auto scene = CreateTestScene();

            if (!physicsEngine->Initialize()) {
                return false;
            }

            if (!physicsEngine->InitializeScene(scene)) {
                return false;
            }

            // 執行一些模擬
            for (int i = 0; i < 30; ++i) {
                physicsEngine->StepSimulation(0.016666667f);
            }

            physicsEngine->Cleanup();
        }

        // 測試場景重置
        auto scene = CreateTestScene();
        if (!physicsEngine->Initialize()) {
            return false;
        }

        for (int reset = 0; reset < 3; ++reset) {
            if (!physicsEngine->InitializeScene(scene)) {
                return false;
            }

            for (int i = 0; i < 20; ++i) {
                physicsEngine->StepSimulation(0.016666667f);
            }

            physicsEngine->ResetScene();
        }

        physicsEngine->Cleanup();
        return true;
    }

    /**
     * @brief 測試併發存取
     */
    bool TestConcurrentAccess() {
        auto scene = CreateTestScene();

        // 測試多執行緒場景載入（雖然不建議，但應該要能處理）
        std::vector<std::thread> threads;
        std::vector<bool> results(4, false);

        for (int i = 0; i < 4; ++i) {
            threads.emplace_back([this, &scene, &results, i]() {
                try {
                    std::string filename = "concurrent_test_" + std::to_string(i) + ".pscene";
                    SceneLoader loader;
                    
                    if (!loader.SaveScene(filename, scene)) {
                        return;
                    }

                    PhysicsScene::PhysicsScene loadedScene;
                    if (!loader.LoadScene(filename, loadedScene)) {
                        return;
                    }

                    results[i] = (loadedScene.rigidBodies.size() == scene.rigidBodies.size());
                    std::remove(filename.c_str());
                } catch (...) {
                    results[i] = false;
                }
            });
        }

        // 等待所有執行緒完成
        for (auto& thread : threads) {
            thread.join();
        }

        // 檢查結果
        for (bool result : results) {
            if (!result) {
                return false;
            }
        }

        return true;
    }
};

/**
 * @brief 主函數
 */
int main(int argc, char* argv[]) {
    std::cout << "Physics Scene Editor Integration Test Suite" << std::endl;
    std::cout << "Version 1.0.0" << std::endl;
    std::cout << std::endl;

    IntegrationTest tester;
    bool success = tester.RunAllTests();

    if (success) {
        std::cout << std::endl;
        std::cout << "All integration tests passed successfully!" << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "Some integration tests failed. Please check the output above." << std::endl;
        return 1;
    }
}
