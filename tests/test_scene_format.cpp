/**
 * @file test_scene_format.cpp
 * @brief 物理場景格式單元測試
 * 
 * 測試場景描述檔案格式的載入、儲存、驗證等功能。
 * 使用 Google Test 框架進行單元測試。
 */

#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "../scene_format/physics_scene_format.h"
#include "../cross_platform_runner/scene_loader.h"

namespace fs = std::filesystem;

class SceneFormatTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 建立測試目錄
        testDir = fs::temp_directory_path() / "physics_scene_test";
        fs::create_directories(testDir);
        
        // 初始化場景載入器
        sceneLoader = std::make_unique<SceneLoader>();
    }

    void TearDown() override {
        // 清理測試檔案
        if (fs::exists(testDir)) {
            fs::remove_all(testDir);
        }
    }

    // 建立基本測試場景
    PhysicsScene::PhysicsScene CreateBasicTestScene() {
        PhysicsScene::PhysicsScene scene;
        
        // 基本設定
        scene.metadata.name = "Test Scene";
        scene.metadata.description = "A test scene for unit testing";
        scene.metadata.author = "Unit Test";
        scene.metadata.version = "1.0.0";
        
        // 模擬設定
        scene.simulationSettings.timeStep = 0.016666667f;
        scene.simulationSettings.gravity = {0.0f, -9.81f, 0.0f};
        scene.simulationSettings.solverIterations = 10;
        scene.simulationSettings.useOGCContact = true;
        scene.simulationSettings.ogcContactRadius = 0.1f;
        scene.simulationSettings.hybridMode = true;
        
        // 渲染設定
        scene.renderSettings.backgroundColor = {0.2f, 0.3f, 0.4f, 1.0f};
        scene.renderSettings.ambientLight = {0.3f, 0.3f, 0.3f};
        scene.renderSettings.enableShadows = true;
        scene.renderSettings.enableAntiAliasing = true;
        scene.renderSettings.shadowMapSize = 1024;
        
        // 物理材質
        PhysicsScene::PhysicsMaterial defaultMaterial;
        defaultMaterial.name = "Default";
        defaultMaterial.density = 1000.0f;
        defaultMaterial.friction = 0.5f;
        defaultMaterial.restitution = 0.3f;
        defaultMaterial.rollingFriction = 0.1f;
        scene.physicsMaterials["default"] = defaultMaterial;
        
        // 視覺材質
        PhysicsScene::VisualMaterial redMaterial;
        redMaterial.name = "Red";
        redMaterial.diffuseColor = {0.8f, 0.2f, 0.2f, 1.0f};
        redMaterial.specularColor = {1.0f, 1.0f, 1.0f};
        redMaterial.shininess = 32.0f;
        scene.visualMaterials["red"] = redMaterial;
        
        // 剛體
        PhysicsScene::RigidBody box;
        box.name = "Test Box";
        box.transform.position = {0.0f, 5.0f, 0.0f};
        box.transform.rotation = {1.0f, 0.0f, 0.0f, 0.0f};
        box.transform.scale = {1.0f, 1.0f, 1.0f};
        box.shapeType = PhysicsScene::ShapeType::Box;
        box.shapeParameters.halfExtents = {0.5f, 0.5f, 0.5f};
        box.mass = 1.0f;
        box.physicsMaterial = "default";
        box.visualMaterial = "red";
        scene.rigidBodies["box"] = box;
        
        // 光源
        PhysicsScene::Light light;
        light.name = "Main Light";
        light.transform.position = {5.0f, 10.0f, 5.0f};
        light.lightType = PhysicsScene::LightType::Directional;
        light.color = {1.0f, 0.95f, 0.8f};
        light.intensity = 1.0f;
        light.castShadows = true;
        scene.lights["main"] = light;
        
        // 相機
        PhysicsScene::Camera camera;
        camera.name = "Main Camera";
        camera.transform.position = {0.0f, 5.0f, 10.0f};
        camera.fov = 45.0f;
        camera.nearPlane = 0.1f;
        camera.farPlane = 100.0f;
        camera.aspectRatio = 16.0f / 9.0f;
        scene.cameras["main"] = camera;
        
        return scene;
    }

    fs::path testDir;
    std::unique_ptr<SceneLoader> sceneLoader;
};

// 測試基本場景建立
TEST_F(SceneFormatTest, CreateBasicScene) {
    auto scene = CreateBasicTestScene();
    
    EXPECT_EQ(scene.metadata.name, "Test Scene");
    EXPECT_EQ(scene.rigidBodies.size(), 1);
    EXPECT_EQ(scene.lights.size(), 1);
    EXPECT_EQ(scene.cameras.size(), 1);
    EXPECT_EQ(scene.physicsMaterials.size(), 1);
    EXPECT_EQ(scene.visualMaterials.size(), 1);
}

// 測試場景儲存和載入
TEST_F(SceneFormatTest, SaveAndLoadScene) {
    auto originalScene = CreateBasicTestScene();
    std::string filename = (testDir / "test_scene.pscene").string();
    
    // 儲存場景
    ASSERT_TRUE(sceneLoader->SaveScene(filename, originalScene));
    ASSERT_TRUE(fs::exists(filename));
    
    // 載入場景
    PhysicsScene::PhysicsScene loadedScene;
    ASSERT_TRUE(sceneLoader->LoadScene(filename, loadedScene));
    
    // 驗證基本資訊
    EXPECT_EQ(loadedScene.metadata.name, originalScene.metadata.name);
    EXPECT_EQ(loadedScene.metadata.description, originalScene.metadata.description);
    EXPECT_EQ(loadedScene.rigidBodies.size(), originalScene.rigidBodies.size());
    EXPECT_EQ(loadedScene.lights.size(), originalScene.lights.size());
    EXPECT_EQ(loadedScene.cameras.size(), originalScene.cameras.size());
    
    // 驗證模擬設定
    EXPECT_FLOAT_EQ(loadedScene.simulationSettings.timeStep, originalScene.simulationSettings.timeStep);
    EXPECT_FLOAT_EQ(loadedScene.simulationSettings.gravity.y, originalScene.simulationSettings.gravity.y);
    EXPECT_EQ(loadedScene.simulationSettings.solverIterations, originalScene.simulationSettings.solverIterations);
    EXPECT_EQ(loadedScene.simulationSettings.useOGCContact, originalScene.simulationSettings.useOGCContact);
    
    // 驗證剛體資料
    auto& originalBox = originalScene.rigidBodies.at("box");
    auto& loadedBox = loadedScene.rigidBodies.at("box");
    EXPECT_EQ(loadedBox.name, originalBox.name);
    EXPECT_FLOAT_EQ(loadedBox.mass, originalBox.mass);
    EXPECT_EQ(loadedBox.shapeType, originalBox.shapeType);
    EXPECT_FLOAT_EQ(loadedBox.transform.position.y, originalBox.transform.position.y);
}

// 測試場景驗證
TEST_F(SceneFormatTest, ValidateScene) {
    auto scene = CreateBasicTestScene();
    
    // 驗證正常場景
    auto result = sceneLoader->ValidateScene(scene);
    EXPECT_TRUE(result.isValid);
    EXPECT_TRUE(result.errors.empty());
    
    // 測試無效場景 - 負質量
    scene.rigidBodies["box"].mass = -1.0f;
    result = sceneLoader->ValidateScene(scene);
    EXPECT_FALSE(result.isValid);
    EXPECT_FALSE(result.errors.empty());
    
    // 修復並重新驗證
    scene.rigidBodies["box"].mass = 1.0f;
    result = sceneLoader->ValidateScene(scene);
    EXPECT_TRUE(result.isValid);
}

// 測試不存在的檔案
TEST_F(SceneFormatTest, LoadNonExistentFile) {
    PhysicsScene::PhysicsScene scene;
    std::string filename = (testDir / "nonexistent.pscene").string();
    
    EXPECT_FALSE(sceneLoader->LoadScene(filename, scene));
    EXPECT_FALSE(sceneLoader->GetLastError().empty());
}

// 測試無效的 JSON 檔案
TEST_F(SceneFormatTest, LoadInvalidJSON) {
    std::string filename = (testDir / "invalid.pscene").string();
    
    // 建立無效的 JSON 檔案
    std::ofstream file(filename);
    file << "{ invalid json content }";
    file.close();
    
    PhysicsScene::PhysicsScene scene;
    EXPECT_FALSE(sceneLoader->LoadScene(filename, scene));
    EXPECT_FALSE(sceneLoader->GetLastError().empty());
}

// 測試場景資訊提取
TEST_F(SceneFormatTest, GetSceneInfo) {
    auto scene = CreateBasicTestScene();
    std::string filename = (testDir / "info_test.pscene").string();
    
    ASSERT_TRUE(sceneLoader->SaveScene(filename, scene));
    
    auto info = sceneLoader->GetSceneInfo(filename);
    EXPECT_EQ(info.filename, filename);
    EXPECT_EQ(info.rigidBodyCount, 1);
    EXPECT_EQ(info.lightCount, 1);
    EXPECT_EQ(info.cameraCount, 1);
    EXPECT_GT(info.fileSize, 0);
}

// 測試複雜場景
TEST_F(SceneFormatTest, ComplexScene) {
    PhysicsScene::PhysicsScene scene = CreateBasicTestScene();
    
    // 新增約束
    PhysicsScene::Constraint hinge;
    hinge.name = "Test Hinge";
    hinge.constraintType = PhysicsScene::ConstraintType::Hinge;
    hinge.bodyA = "box";
    hinge.bodyB = ""; // 固定到世界
    hinge.frameA.position = {0.0f, 0.5f, 0.0f};
    hinge.breakingThreshold = 1000.0f;
    scene.constraints["hinge"] = hinge;
    
    // 新增力場
    PhysicsScene::ForceField wind;
    wind.name = "Wind";
    wind.transform.position = {-5.0f, 0.0f, 0.0f};
    wind.forceFieldType = PhysicsScene::ForceFieldType::Directional;
    wind.direction = {1.0f, 0.0f, 0.0f};
    wind.strength = 10.0f;
    wind.radius = 5.0f;
    scene.forceFields["wind"] = wind;
    
    std::string filename = (testDir / "complex_scene.pscene").string();
    
    // 儲存和載入複雜場景
    ASSERT_TRUE(sceneLoader->SaveScene(filename, scene));
    
    PhysicsScene::PhysicsScene loadedScene;
    ASSERT_TRUE(sceneLoader->LoadScene(filename, loadedScene));
    
    EXPECT_EQ(loadedScene.constraints.size(), 1);
    EXPECT_EQ(loadedScene.forceFields.size(), 1);
    
    auto& loadedHinge = loadedScene.constraints.at("hinge");
    EXPECT_EQ(loadedHinge.name, "Test Hinge");
    EXPECT_EQ(loadedHinge.constraintType, PhysicsScene::ConstraintType::Hinge);
    
    auto& loadedWind = loadedScene.forceFields.at("wind");
    EXPECT_EQ(loadedWind.name, "Wind");
    EXPECT_EQ(loadedWind.forceFieldType, PhysicsScene::ForceFieldType::Directional);
}

// 測試載入選項
TEST_F(SceneFormatTest, LoadOptions) {
    auto scene = CreateBasicTestScene();
    std::string filename = (testDir / "options_test.pscene").string();
    
    ASSERT_TRUE(sceneLoader->SaveScene(filename, scene));
    
    // 設定載入選項 - 不載入光源
    SceneLoader::LoadOptions options;
    options.loadLights = false;
    sceneLoader->SetLoadOptions(options);
    
    PhysicsScene::PhysicsScene loadedScene;
    ASSERT_TRUE(sceneLoader->LoadScene(filename, loadedScene));
    
    // 驗證光源未被載入
    EXPECT_EQ(loadedScene.lights.size(), 0);
    EXPECT_EQ(loadedScene.rigidBodies.size(), 1); // 其他物件應該正常載入
}

// 測試儲存選項
TEST_F(SceneFormatTest, SaveOptions) {
    auto scene = CreateBasicTestScene();
    std::string filename = (testDir / "save_options_test.pscene").string();
    
    // 設定儲存選項 - 不儲存相機
    SceneLoader::SaveOptions options;
    options.saveCameras = false;
    options.prettyFormat = false; // 測試壓縮格式
    sceneLoader->SetSaveOptions(options);
    
    ASSERT_TRUE(sceneLoader->SaveScene(filename, scene));
    
    // 載入並驗證
    PhysicsScene::PhysicsScene loadedScene;
    ASSERT_TRUE(sceneLoader->LoadScene(filename, loadedScene));
    
    EXPECT_EQ(loadedScene.cameras.size(), 0); // 相機未被儲存
    EXPECT_EQ(loadedScene.rigidBodies.size(), 1); // 其他物件正常
}

// 測試場景統計
TEST_F(SceneFormatTest, SceneStatistics) {
    auto scene = CreateBasicTestScene();
    
    auto stats = sceneLoader->AnalyzeScene(scene);
    EXPECT_EQ(stats.totalObjects, 3); // 1 剛體 + 1 光源 + 1 相機
    EXPECT_EQ(stats.rigidBodies, 1);
    EXPECT_EQ(stats.lights, 1);
    EXPECT_EQ(stats.cameras, 1);
    EXPECT_FLOAT_EQ(stats.totalMass, 1.0f);
}

// 測試批次驗證
TEST_F(SceneFormatTest, BatchValidation) {
    // 建立多個測試檔案
    std::vector<std::string> filenames;
    
    for (int i = 0; i < 3; ++i) {
        auto scene = CreateBasicTestScene();
        scene.metadata.name = "Test Scene " + std::to_string(i);
        
        std::string filename = (testDir / ("batch_test_" + std::to_string(i) + ".pscene")).string();
        ASSERT_TRUE(sceneLoader->SaveScene(filename, scene));
        filenames.push_back(filename);
    }
    
    // 新增一個無效檔案
    std::string invalidFile = (testDir / "invalid_batch.pscene").string();
    std::ofstream file(invalidFile);
    file << "{ invalid }";
    file.close();
    filenames.push_back(invalidFile);
    
    // 批次驗證
    auto result = sceneLoader->ValidateSceneFiles(filenames);
    EXPECT_EQ(result.totalFiles, 4);
    EXPECT_EQ(result.successFiles.size(), 3);
    EXPECT_EQ(result.failedFiles.size(), 1);
}

// 效能測試
TEST_F(SceneFormatTest, PerformanceTest) {
    auto scene = CreateBasicTestScene();
    
    // 新增大量物件
    for (int i = 0; i < 100; ++i) {
        PhysicsScene::RigidBody box;
        box.name = "Box " + std::to_string(i);
        box.transform.position = {static_cast<float>(i % 10), static_cast<float>(i / 10), 0.0f};
        box.shapeType = PhysicsScene::ShapeType::Box;
        box.mass = 1.0f;
        scene.rigidBodies["box_" + std::to_string(i)] = box;
    }
    
    std::string filename = (testDir / "performance_test.pscene").string();
    
    // 測量儲存時間
    auto start = std::chrono::high_resolution_clock::now();
    ASSERT_TRUE(sceneLoader->SaveScene(filename, scene));
    auto saveTime = std::chrono::high_resolution_clock::now() - start;
    
    // 測量載入時間
    PhysicsScene::PhysicsScene loadedScene;
    start = std::chrono::high_resolution_clock::now();
    ASSERT_TRUE(sceneLoader->LoadScene(filename, loadedScene));
    auto loadTime = std::chrono::high_resolution_clock::now() - start;
    
    // 驗證結果
    EXPECT_EQ(loadedScene.rigidBodies.size(), scene.rigidBodies.size());
    
    // 輸出效能資訊（僅供參考）
    std::cout << "Save time: " << std::chrono::duration_cast<std::chrono::milliseconds>(saveTime).count() << "ms" << std::endl;
    std::cout << "Load time: " << std::chrono::duration_cast<std::chrono::milliseconds>(loadTime).count() << "ms" << std::endl;
    std::cout << "File size: " << fs::file_size(filename) << " bytes" << std::endl;
}

// 主函數
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
