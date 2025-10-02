#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

// JSON 解析
#include <nlohmann/json.hpp>

// 場景格式
#include "../scene_format/physics_scene_format.h"

/**
 * @file scene_loader.h
 * @brief 物理場景載入器類別
 * 
 * 負責載入和儲存物理場景描述檔案，支援 JSON 格式的 .pscene 檔案。
 * 提供完整的場景資料驗證、錯誤處理和進度回報功能。
 */

class SceneLoader {
public:
    SceneLoader();
    ~SceneLoader();

    // 載入和儲存
    bool LoadScene(const std::string& filename, PhysicsScene::PhysicsScene& scene);
    bool SaveScene(const std::string& filename, const PhysicsScene::PhysicsScene& scene);

    // 格式支援
    bool IsValidSceneFile(const std::string& filename);
    std::vector<std::string> GetSupportedExtensions() const;
    std::string GetFileFilter() const;

    // 驗證功能
    struct ValidationResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        void AddError(const std::string& message) {
            errors.push_back(message);
            isValid = false;
        }
        
        void AddWarning(const std::string& message) {
            warnings.push_back(message);
        }
        
        bool HasIssues() const {
            return !errors.empty() || !warnings.empty();
        }
    };

    ValidationResult ValidateScene(const PhysicsScene::PhysicsScene& scene);
    ValidationResult ValidateSceneFile(const std::string& filename);

    // 進度回報
    class ProgressCallback {
    public:
        virtual ~ProgressCallback() = default;
        virtual void OnProgress(float percentage, const std::string& message) = 0;
        virtual bool ShouldCancel() = 0;
    };

    void SetProgressCallback(ProgressCallback* callback);

    // 錯誤處理
    const std::string& GetLastError() const { return m_lastError; }
    const std::vector<std::string>& GetLoadWarnings() const { return m_loadWarnings; }

    // 場景資訊
    struct SceneInfo {
        std::string filename;
        std::string version;
        std::string createdBy;
        std::string createdDate;
        std::string modifiedDate;
        std::string description;
        int rigidBodyCount = 0;
        int constraintCount = 0;
        int forceFieldCount = 0;
        int lightCount = 0;
        int cameraCount = 0;
        int materialCount = 0;
        size_t fileSize = 0;
    };

    SceneInfo GetSceneInfo(const std::string& filename);

    // 匯入/匯出選項
    struct LoadOptions {
        bool loadRigidBodies = true;
        bool loadConstraints = true;
        bool loadForceFields = true;
        bool loadLights = true;
        bool loadCameras = true;
        bool loadMaterials = true;
        bool loadSimulationSettings = true;
        bool loadRenderSettings = true;
        bool validateOnLoad = true;
        bool repairOnLoad = false;
        float scaleFactor = 1.0f;
        std::string materialSearchPath;
        std::string textureSearchPath;
    };

    struct SaveOptions {
        bool saveRigidBodies = true;
        bool saveConstraints = true;
        bool saveForceFields = true;
        bool saveLights = true;
        bool saveCameras = true;
        bool saveMaterials = true;
        bool saveSimulationSettings = true;
        bool saveRenderSettings = true;
        bool prettyFormat = true;
        bool embedTextures = false;
        int compressionLevel = 0; // 0 = 無壓縮, 1-9 = 壓縮等級
    };

    void SetLoadOptions(const LoadOptions& options);
    void SetSaveOptions(const SaveOptions& options);
    const LoadOptions& GetLoadOptions() const { return m_loadOptions; }
    const SaveOptions& GetSaveOptions() const { return m_saveOptions; }

private:
    // JSON 轉換函數
    nlohmann::json SceneToJson(const PhysicsScene::PhysicsScene& scene);
    bool JsonToScene(const nlohmann::json& json, PhysicsScene::PhysicsScene& scene);

    // 個別物件轉換
    nlohmann::json RigidBodyToJson(const PhysicsScene::RigidBody& rigidBody);
    bool JsonToRigidBody(const nlohmann::json& json, PhysicsScene::RigidBody& rigidBody);

    nlohmann::json ConstraintToJson(const PhysicsScene::Constraint& constraint);
    bool JsonToConstraint(const nlohmann::json& json, PhysicsScene::Constraint& constraint);

    nlohmann::json ForceFieldToJson(const PhysicsScene::ForceField& forceField);
    bool JsonToForceField(const nlohmann::json& json, PhysicsScene::ForceField& forceField);

    nlohmann::json LightToJson(const PhysicsScene::Light& light);
    bool JsonToLight(const nlohmann::json& json, PhysicsScene::Light& light);

    nlohmann::json CameraToJson(const PhysicsScene::Camera& camera);
    bool JsonToCamera(const nlohmann::json& json, PhysicsScene::Camera& camera);

    nlohmann::json PhysicsMaterialToJson(const PhysicsScene::PhysicsMaterial& material);
    bool JsonToPhysicsMaterial(const nlohmann::json& json, PhysicsScene::PhysicsMaterial& material);

    nlohmann::json VisualMaterialToJson(const PhysicsScene::VisualMaterial& material);
    bool JsonToVisualMaterial(const nlohmann::json& json, PhysicsScene::VisualMaterial& material);

    nlohmann::json SimulationSettingsToJson(const PhysicsScene::SimulationSettings& settings);
    bool JsonToSimulationSettings(const nlohmann::json& json, PhysicsScene::SimulationSettings& settings);

    nlohmann::json RenderSettingsToJson(const PhysicsScene::RenderSettings& settings);
    bool JsonToRenderSettings(const nlohmann::json& json, PhysicsScene::RenderSettings& settings);

    // 基本類型轉換
    nlohmann::json Vector3ToJson(const PhysicsScene::Vector3& v);
    bool JsonToVector3(const nlohmann::json& json, PhysicsScene::Vector3& v);

    nlohmann::json QuaternionToJson(const PhysicsScene::Quaternion& q);
    bool JsonToQuaternion(const nlohmann::json& json, PhysicsScene::Quaternion& q);

    nlohmann::json ColorToJson(const PhysicsScene::Color& c);
    bool JsonToColor(const nlohmann::json& json, PhysicsScene::Color& c);

    nlohmann::json TransformToJson(const PhysicsScene::Transform& t);
    bool JsonToTransform(const nlohmann::json& json, PhysicsScene::Transform& t);

    // 驗證函數
    bool ValidateRigidBody(const PhysicsScene::RigidBody& rigidBody, ValidationResult& result);
    bool ValidateConstraint(const PhysicsScene::Constraint& constraint, ValidationResult& result);
    bool ValidateForceField(const PhysicsScene::ForceField& forceField, ValidationResult& result);
    bool ValidateLight(const PhysicsScene::Light& light, ValidationResult& result);
    bool ValidateCamera(const PhysicsScene::Camera& camera, ValidationResult& result);
    bool ValidatePhysicsMaterial(const PhysicsScene::PhysicsMaterial& material, ValidationResult& result);
    bool ValidateVisualMaterial(const PhysicsScene::VisualMaterial& material, ValidationResult& result);
    bool ValidateSimulationSettings(const PhysicsScene::SimulationSettings& settings, ValidationResult& result);
    bool ValidateRenderSettings(const PhysicsScene::RenderSettings& settings, ValidationResult& result);

    // 修復函數
    void RepairRigidBody(PhysicsScene::RigidBody& rigidBody);
    void RepairConstraint(PhysicsScene::Constraint& constraint);
    void RepairForceField(PhysicsScene::ForceField& forceField);
    void RepairLight(PhysicsScene::Light& light);
    void RepairCamera(PhysicsScene::Camera& camera);
    void RepairPhysicsMaterial(PhysicsScene::PhysicsMaterial& material);
    void RepairVisualMaterial(PhysicsScene::VisualMaterial& material);
    void RepairSimulationSettings(PhysicsScene::SimulationSettings& settings);
    void RepairRenderSettings(PhysicsScene::RenderSettings& settings);

    // 檔案操作
    bool ReadTextFile(const std::string& filename, std::string& content);
    bool WriteTextFile(const std::string& filename, const std::string& content);
    bool FileExists(const std::string& filename);
    size_t GetFileSize(const std::string& filename);
    std::string GetFileExtension(const std::string& filename);
    std::string GetFileName(const std::string& filepath);
    std::string GetDirectoryPath(const std::string& filepath);

    // 路徑處理
    std::string ResolvePath(const std::string& path, const std::string& basePath);
    std::string MakeRelativePath(const std::string& path, const std::string& basePath);
    bool IsAbsolutePath(const std::string& path);

    // 紋理處理
    bool ValidateTexturePath(const std::string& texturePath, const std::string& basePath);
    std::string FindTexture(const std::string& textureName, const std::string& searchPath);
    std::vector<std::string> GetTextureSearchPaths(const std::string& sceneFilePath);

    // 進度報告
    void ReportProgress(float percentage, const std::string& message);
    bool CheckCancellation();

    // 錯誤處理
    void SetError(const std::string& error);
    void AddWarning(const std::string& warning);
    void ClearErrors();

    // 版本處理
    bool IsVersionSupported(const std::string& version);
    void UpgradeScene(nlohmann::json& json, const std::string& fromVersion, const std::string& toVersion);

    // 成員變數
    LoadOptions m_loadOptions;
    SaveOptions m_saveOptions;
    ProgressCallback* m_progressCallback;
    std::string m_lastError;
    std::vector<std::string> m_loadWarnings;

    // 常數
    static const std::string CURRENT_VERSION;
    static const std::vector<std::string> SUPPORTED_VERSIONS;
    static const std::vector<std::string> SUPPORTED_EXTENSIONS;

public:
    // 靜態輔助函數
    static std::string GetDefaultSceneTemplate();
    static PhysicsScene::PhysicsScene CreateDefaultScene();
    static bool IsSceneFileExtension(const std::string& extension);

    // 批次處理
    struct BatchResult {
        std::vector<std::string> successFiles;
        std::vector<std::pair<std::string, std::string>> failedFiles; // filename, error
        int totalFiles = 0;
        int processedFiles = 0;
    };

    BatchResult ValidateSceneFiles(const std::vector<std::string>& filenames);
    BatchResult ConvertSceneFiles(const std::vector<std::string>& filenames, 
                                 const std::string& outputDirectory,
                                 const std::string& targetFormat = ".pscene");

    // 場景合併
    bool MergeScenes(const std::vector<std::string>& sceneFiles, 
                    const std::string& outputFile,
                    const std::string& mergeStrategy = "append");

    // 場景分割
    bool SplitScene(const std::string& inputFile,
                   const std::string& outputDirectory,
                   const std::string& splitCriteria = "by_type");

    // 場景統計
    struct SceneStatistics {
        int totalObjects = 0;
        int rigidBodies = 0;
        int staticBodies = 0;
        int kinematicBodies = 0;
        int constraints = 0;
        int forceFields = 0;
        int lights = 0;
        int cameras = 0;
        int materials = 0;
        int textures = 0;
        float totalMass = 0.0f;
        PhysicsScene::Vector3 boundingBoxMin;
        PhysicsScene::Vector3 boundingBoxMax;
    };

    SceneStatistics AnalyzeScene(const PhysicsScene::PhysicsScene& scene);
    SceneStatistics AnalyzeSceneFile(const std::string& filename);

    // 場景最佳化
    struct OptimizationOptions {
        bool removeDuplicateMaterials = true;
        bool mergeStaticBodies = false;
        bool removeUnusedMaterials = true;
        bool optimizeMeshes = false;
        bool compressTextures = false;
        float mergeTolerance = 0.001f;
    };

    bool OptimizeScene(PhysicsScene::PhysicsScene& scene, const OptimizationOptions& options);
    bool OptimizeSceneFile(const std::string& inputFile, const std::string& outputFile, 
                          const OptimizationOptions& options);
};
