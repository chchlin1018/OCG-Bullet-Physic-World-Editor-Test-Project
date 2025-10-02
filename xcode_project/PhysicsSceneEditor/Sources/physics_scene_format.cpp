#include "physics_scene_format.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>

// JSON 處理 (使用 nlohmann/json 或簡單的手動實現)
#ifdef USE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace PhysicsScene {

// GeometryShape 便利建構函數實現
GeometryShape GeometryShape::createBox(float width, float height, float depth) {
    GeometryShape shape(ShapeType::Box);
    shape.parameters["width"] = width;
    shape.parameters["height"] = height;
    shape.parameters["depth"] = depth;
    return shape;
}

GeometryShape GeometryShape::createSphere(float radius) {
    GeometryShape shape(ShapeType::Sphere);
    shape.parameters["radius"] = radius;
    return shape;
}

GeometryShape GeometryShape::createCylinder(float radius, float height) {
    GeometryShape shape(ShapeType::Cylinder);
    shape.parameters["radius"] = radius;
    shape.parameters["height"] = height;
    return shape;
}

GeometryShape GeometryShape::createCapsule(float radius, float height) {
    GeometryShape shape(ShapeType::Capsule);
    shape.parameters["radius"] = radius;
    shape.parameters["height"] = height;
    return shape;
}

GeometryShape GeometryShape::createCone(float radius, float height) {
    GeometryShape shape(ShapeType::Cone);
    shape.parameters["radius"] = radius;
    shape.parameters["height"] = height;
    return shape;
}

GeometryShape GeometryShape::createPlane(float width, float depth) {
    GeometryShape shape(ShapeType::Plane);
    shape.parameters["width"] = width;
    shape.parameters["depth"] = depth;
    return shape;
}

// PhysicsScene 實現
PhysicsScene::PhysicsScene() {
    initializeDefaultMaterials();
    initializeDefaultObjects();
    
    // 設定建立時間
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    metadata.createdDate = ss.str();
    metadata.modifiedDate = ss.str();
}

void PhysicsScene::clear() {
    rigidBodies.clear();
    constraints.clear();
    forceFields.clear();
    lights.clear();
    cameras.clear();
    physicsMaterials.clear();
    visualMaterials.clear();
    
    initializeDefaultMaterials();
    initializeDefaultObjects();
}

bool PhysicsScene::isEmpty() const {
    return rigidBodies.empty() && 
           constraints.empty() && 
           forceFields.empty() && 
           lights.empty() && 
           cameras.empty();
}

// 物件查找實現
RigidBody* PhysicsScene::findRigidBody(const std::string& name) {
    auto it = std::find_if(rigidBodies.begin(), rigidBodies.end(),
        [&name](const RigidBody& body) { return body.name == name; });
    return it != rigidBodies.end() ? &(*it) : nullptr;
}

const RigidBody* PhysicsScene::findRigidBody(const std::string& name) const {
    auto it = std::find_if(rigidBodies.begin(), rigidBodies.end(),
        [&name](const RigidBody& body) { return body.name == name; });
    return it != rigidBodies.end() ? &(*it) : nullptr;
}

bool PhysicsScene::removeRigidBody(const std::string& name) {
    auto it = std::find_if(rigidBodies.begin(), rigidBodies.end(),
        [&name](const RigidBody& body) { return body.name == name; });
    if (it != rigidBodies.end()) {
        rigidBodies.erase(it);
        return true;
    }
    return false;
}

Constraint* PhysicsScene::findConstraint(const std::string& name) {
    auto it = std::find_if(constraints.begin(), constraints.end(),
        [&name](const Constraint& constraint) { return constraint.name == name; });
    return it != constraints.end() ? &(*it) : nullptr;
}

const Constraint* PhysicsScene::findConstraint(const std::string& name) const {
    auto it = std::find_if(constraints.begin(), constraints.end(),
        [&name](const Constraint& constraint) { return constraint.name == name; });
    return it != constraints.end() ? &(*it) : nullptr;
}

bool PhysicsScene::removeConstraint(const std::string& name) {
    auto it = std::find_if(constraints.begin(), constraints.end(),
        [&name](const Constraint& constraint) { return constraint.name == name; });
    if (it != constraints.end()) {
        constraints.erase(it);
        return true;
    }
    return false;
}

ForceField* PhysicsScene::findForceField(const std::string& name) {
    auto it = std::find_if(forceFields.begin(), forceFields.end(),
        [&name](const ForceField& field) { return field.name == name; });
    return it != forceFields.end() ? &(*it) : nullptr;
}

const ForceField* PhysicsScene::findForceField(const std::string& name) const {
    auto it = std::find_if(forceFields.begin(), forceFields.end(),
        [&name](const ForceField& field) { return field.name == name; });
    return it != forceFields.end() ? &(*it) : nullptr;
}

bool PhysicsScene::removeForceField(const std::string& name) {
    auto it = std::find_if(forceFields.begin(), forceFields.end(),
        [&name](const ForceField& field) { return field.name == name; });
    if (it != forceFields.end()) {
        forceFields.erase(it);
        return true;
    }
    return false;
}

Light* PhysicsScene::findLight(const std::string& name) {
    auto it = std::find_if(lights.begin(), lights.end(),
        [&name](const Light& light) { return light.name == name; });
    return it != lights.end() ? &(*it) : nullptr;
}

const Light* PhysicsScene::findLight(const std::string& name) const {
    auto it = std::find_if(lights.begin(), lights.end(),
        [&name](const Light& light) { return light.name == name; });
    return it != lights.end() ? &(*it) : nullptr;
}

bool PhysicsScene::removeLight(const std::string& name) {
    auto it = std::find_if(lights.begin(), lights.end(),
        [&name](const Light& light) { return light.name == name; });
    if (it != lights.end()) {
        lights.erase(it);
        return true;
    }
    return false;
}

Camera* PhysicsScene::findCamera(const std::string& name) {
    auto it = std::find_if(cameras.begin(), cameras.end(),
        [&name](const Camera& camera) { return camera.name == name; });
    return it != cameras.end() ? &(*it) : nullptr;
}

const Camera* PhysicsScene::findCamera(const std::string& name) const {
    auto it = std::find_if(cameras.begin(), cameras.end(),
        [&name](const Camera& camera) { return camera.name == name; });
    return it != cameras.end() ? &(*it) : nullptr;
}

bool PhysicsScene::removeCamera(const std::string& name) {
    auto it = std::find_if(cameras.begin(), cameras.end(),
        [&name](const Camera& camera) { return camera.name == name; });
    if (it != cameras.end()) {
        cameras.erase(it);
        return true;
    }
    return false;
}

// 材質查找實現
PhysicsMaterial* PhysicsScene::findPhysicsMaterial(const std::string& name) {
    auto it = physicsMaterials.find(name);
    return it != physicsMaterials.end() ? &it->second : nullptr;
}

const PhysicsMaterial* PhysicsScene::findPhysicsMaterial(const std::string& name) const {
    auto it = physicsMaterials.find(name);
    return it != physicsMaterials.end() ? &it->second : nullptr;
}

bool PhysicsScene::removePhysicsMaterial(const std::string& name) {
    return physicsMaterials.erase(name) > 0;
}

VisualMaterial* PhysicsScene::findVisualMaterial(const std::string& name) {
    auto it = visualMaterials.find(name);
    return it != visualMaterials.end() ? &it->second : nullptr;
}

const VisualMaterial* PhysicsScene::findVisualMaterial(const std::string& name) const {
    auto it = visualMaterials.find(name);
    return it != visualMaterials.end() ? &it->second : nullptr;
}

bool PhysicsScene::removeVisualMaterial(const std::string& name) {
    return visualMaterials.erase(name) > 0;
}

// 驗證實現
bool PhysicsScene::validate(std::vector<std::string>& errors) const {
    errors.clear();
    
    // 檢查物件名稱唯一性
    std::vector<std::string> rigidBodyNames;
    for (const auto& body : rigidBodies) {
        if (std::find(rigidBodyNames.begin(), rigidBodyNames.end(), body.name) != rigidBodyNames.end()) {
            errors.push_back("重複的剛體名稱: " + body.name);
        }
        rigidBodyNames.push_back(body.name);
        
        // 檢查材質引用
        if (physicsMaterials.find(body.physicsMaterial) == physicsMaterials.end()) {
            errors.push_back("剛體 '" + body.name + "' 引用了不存在的物理材質: " + body.physicsMaterial);
        }
        if (visualMaterials.find(body.visualMaterial) == visualMaterials.end()) {
            errors.push_back("剛體 '" + body.name + "' 引用了不存在的視覺材質: " + body.visualMaterial);
        }
        
        // 檢查物理屬性
        if (body.mass < 0.0f) {
            errors.push_back("剛體 '" + body.name + "' 的質量不能為負數");
        }
    }
    
    // 檢查約束
    for (const auto& constraint : constraints) {
        if (!constraint.bodyA.empty() && !findRigidBody(constraint.bodyA)) {
            errors.push_back("約束 '" + constraint.name + "' 引用了不存在的剛體A: " + constraint.bodyA);
        }
        if (!constraint.bodyB.empty() && !findRigidBody(constraint.bodyB)) {
            errors.push_back("約束 '" + constraint.name + "' 引用了不存在的剛體B: " + constraint.bodyB);
        }
    }
    
    // 檢查活動相機
    if (!activeCamera.empty() && !findCamera(activeCamera)) {
        errors.push_back("活動相機不存在: " + activeCamera);
    }
    
    return errors.empty();
}

// 統計資訊實現
PhysicsScene::Statistics PhysicsScene::getStatistics() const {
    Statistics stats;
    
    stats.rigidBodyCount = rigidBodies.size();
    stats.constraintCount = constraints.size();
    stats.forceFieldCount = forceFields.size();
    stats.lightCount = lights.size();
    stats.cameraCount = cameras.size();
    stats.physicsMaterialCount = physicsMaterials.size();
    stats.visualMaterialCount = visualMaterials.size();
    
    // 計算三角形和頂點數量
    for (const auto& body : rigidBodies) {
        stats.totalVertices += body.collisionShape.vertices.size();
        stats.totalTriangles += body.collisionShape.triangles.size();
        
        for (const auto& child : body.compoundChildren) {
            stats.totalVertices += child.shape.vertices.size();
            stats.totalTriangles += child.shape.triangles.size();
        }
    }
    
    return stats;
}

// 檔案 I/O 實現
bool PhysicsScene::saveToFile(const std::string& filename) const {
    auto format = Utils::detectFileFormat(filename);
    
    switch (format) {
        case Utils::FileFormat::JSON:
            return saveToJSON(filename);
        case Utils::FileFormat::Binary:
            return saveToBinary(filename);
        default:
            // 預設使用 JSON 格式
            return saveToJSON(filename + ".json");
    }
}

bool PhysicsScene::loadFromFile(const std::string& filename) {
    auto format = Utils::detectFileFormat(filename);
    
    switch (format) {
        case Utils::FileFormat::JSON:
            return loadFromJSON(filename);
        case Utils::FileFormat::Binary:
            return loadFromBinary(filename);
        default:
            return false;
    }
}

// JSON 序列化實現（簡化版本）
bool PhysicsScene::saveToJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << toJSONString();
    return file.good();
}

bool PhysicsScene::loadFromJSON(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return fromJSONString(buffer.str());
}

std::string PhysicsScene::toJSONString() const {
    std::stringstream ss;
    ss << "{\n";
    
    // 格式版本
    ss << "  \"formatVersion\": {\n";
    ss << "    \"major\": " << formatVersionMajor << ",\n";
    ss << "    \"minor\": " << formatVersionMinor << ",\n";
    ss << "    \"patch\": " << formatVersionPatch << "\n";
    ss << "  },\n";
    
    // 元資料
    ss << "  \"metadata\": {\n";
    ss << "    \"name\": \"" << metadata.name << "\",\n";
    ss << "    \"description\": \"" << metadata.description << "\",\n";
    ss << "    \"author\": \"" << metadata.author << "\",\n";
    ss << "    \"version\": \"" << metadata.version << "\",\n";
    ss << "    \"createdDate\": \"" << metadata.createdDate << "\",\n";
    ss << "    \"modifiedDate\": \"" << metadata.modifiedDate << "\"\n";
    ss << "  },\n";
    
    // 物理材質
    ss << "  \"physicsMaterials\": {\n";
    bool first = true;
    for (const auto& [name, material] : physicsMaterials) {
        if (!first) ss << ",\n";
        ss << "    \"" << name << "\": {\n";
        ss << "      \"density\": " << material.density << ",\n";
        ss << "      \"friction\": " << material.friction << ",\n";
        ss << "      \"restitution\": " << material.restitution << ",\n";
        ss << "      \"rollingFriction\": " << material.rollingFriction << ",\n";
        ss << "      \"isKinematic\": " << (material.isKinematic ? "true" : "false") << ",\n";
        ss << "      \"isStatic\": " << (material.isStatic ? "true" : "false") << "\n";
        ss << "    }";
        first = false;
    }
    ss << "\n  },\n";
    
    // 視覺材質
    ss << "  \"visualMaterials\": {\n";
    first = true;
    for (const auto& [name, material] : visualMaterials) {
        if (!first) ss << ",\n";
        ss << "    \"" << name << "\": {\n";
        ss << "      \"diffuseColor\": [" << material.diffuseColor.r << ", " 
           << material.diffuseColor.g << ", " << material.diffuseColor.b << ", " 
           << material.diffuseColor.a << "],\n";
        ss << "      \"shininess\": " << material.shininess << ",\n";
        ss << "      \"metallic\": " << material.metallic << ",\n";
        ss << "      \"roughness\": " << material.roughness << "\n";
        ss << "    }";
        first = false;
    }
    ss << "\n  },\n";
    
    // 剛體
    ss << "  \"rigidBodies\": [\n";
    first = true;
    for (const auto& body : rigidBodies) {
        if (!first) ss << ",\n";
        ss << "    {\n";
        ss << "      \"name\": \"" << body.name << "\",\n";
        ss << "      \"transform\": {\n";
        ss << "        \"position\": [" << body.transform.position.x << ", " 
           << body.transform.position.y << ", " << body.transform.position.z << "],\n";
        ss << "        \"rotation\": [" << body.transform.rotation.w << ", " 
           << body.transform.rotation.x << ", " << body.transform.rotation.y << ", " 
           << body.transform.rotation.z << "],\n";
        ss << "        \"scale\": [" << body.transform.scale.x << ", " 
           << body.transform.scale.y << ", " << body.transform.scale.z << "]\n";
        ss << "      },\n";
        ss << "      \"mass\": " << body.mass << ",\n";
        ss << "      \"physicsMaterial\": \"" << body.physicsMaterial << "\",\n";
        ss << "      \"visualMaterial\": \"" << body.visualMaterial << "\",\n";
        ss << "      \"collisionShape\": {\n";
        ss << "        \"type\": " << static_cast<int>(body.collisionShape.type) << "\n";
        // 這裡可以加入更多形狀參數的序列化
        ss << "      }\n";
        ss << "    }";
        first = false;
    }
    ss << "\n  ],\n";
    
    // 模擬設定
    ss << "  \"simulationSettings\": {\n";
    ss << "    \"timeStep\": " << simulationSettings.timeStep << ",\n";
    ss << "    \"gravity\": [" << simulationSettings.gravity.x << ", " 
       << simulationSettings.gravity.y << ", " << simulationSettings.gravity.z << "],\n";
    ss << "    \"solverIterations\": " << simulationSettings.solverIterations << ",\n";
    ss << "    \"useOGCContact\": " << (simulationSettings.useOGCContact ? "true" : "false") << ",\n";
    ss << "    \"ogcContactRadius\": " << simulationSettings.ogcContactRadius << "\n";
    ss << "  }\n";
    
    ss << "}\n";
    
    return ss.str();
}

bool PhysicsScene::fromJSONString(const std::string& jsonStr) {
    // 這裡應該實現 JSON 解析
    // 為了簡化，這裡只提供框架
    // 實際實現需要使用 JSON 解析庫如 nlohmann/json
    
    // 清空當前場景
    clear();
    
    // TODO: 實現 JSON 解析邏輯
    // 解析格式版本、元資料、材質、物件等
    
    return true;  // 暫時返回 true
}

// 二進制序列化實現
bool PhysicsScene::saveToBinary(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // 寫入檔案頭
    const char magic[] = "OGCSCENE";
    file.write(magic, 8);
    file.write(reinterpret_cast<const char*>(&formatVersionMajor), sizeof(int));
    file.write(reinterpret_cast<const char*>(&formatVersionMinor), sizeof(int));
    file.write(reinterpret_cast<const char*>(&formatVersionPatch), sizeof(int));
    
    // 寫入物件數量
    size_t count = rigidBodies.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(size_t));
    
    // 寫入剛體資料
    for (const auto& body : rigidBodies) {
        // 寫入名稱長度和名稱
        size_t nameLen = body.name.length();
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(size_t));
        file.write(body.name.c_str(), nameLen);
        
        // 寫入變換
        file.write(reinterpret_cast<const char*>(&body.transform), sizeof(Transform));
        
        // 寫入物理屬性
        file.write(reinterpret_cast<const char*>(&body.mass), sizeof(float));
        
        // 寫入材質名稱
        size_t matLen = body.physicsMaterial.length();
        file.write(reinterpret_cast<const char*>(&matLen), sizeof(size_t));
        file.write(body.physicsMaterial.c_str(), matLen);
    }
    
    return file.good();
}

bool PhysicsScene::loadFromBinary(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // 讀取檔案頭
    char magic[9] = {0};
    file.read(magic, 8);
    if (std::string(magic) != "OGCSCENE") {
        return false;
    }
    
    int major, minor, patch;
    file.read(reinterpret_cast<char*>(&major), sizeof(int));
    file.read(reinterpret_cast<char*>(&minor), sizeof(int));
    file.read(reinterpret_cast<char*>(&patch), sizeof(int));
    
    // 檢查版本相容性
    if (major != formatVersionMajor) {
        return false;  // 主版本不相容
    }
    
    // 清空當前場景
    clear();
    
    // 讀取物件數量
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(size_t));
    
    // 讀取剛體資料
    for (size_t i = 0; i < count; ++i) {
        RigidBody body;
        
        // 讀取名稱
        size_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(size_t));
        body.name.resize(nameLen);
        file.read(&body.name[0], nameLen);
        
        // 讀取變換
        file.read(reinterpret_cast<char*>(&body.transform), sizeof(Transform));
        
        // 讀取物理屬性
        file.read(reinterpret_cast<char*>(&body.mass), sizeof(float));
        
        // 讀取材質名稱
        size_t matLen;
        file.read(reinterpret_cast<char*>(&matLen), sizeof(size_t));
        body.physicsMaterial.resize(matLen);
        file.read(&body.physicsMaterial[0], matLen);
        
        rigidBodies.push_back(body);
    }
    
    return file.good();
}

// 私有方法實現
void PhysicsScene::initializeDefaultMaterials() {
    // 預設物理材質
    PhysicsMaterial defaultPhysics("Default");
    physicsMaterials["Default"] = defaultPhysics;
    
    PhysicsMaterial metal("Metal");
    metal.density = 7.8f;
    metal.friction = 0.7f;
    metal.restitution = 0.1f;
    physicsMaterials["Metal"] = metal;
    
    PhysicsMaterial wood("Wood");
    wood.density = 0.6f;
    wood.friction = 0.6f;
    wood.restitution = 0.3f;
    physicsMaterials["Wood"] = wood;
    
    PhysicsMaterial rubber("Rubber");
    rubber.density = 1.2f;
    rubber.friction = 0.9f;
    rubber.restitution = 0.8f;
    physicsMaterials["Rubber"] = rubber;
    
    PhysicsMaterial ice("Ice");
    ice.density = 0.9f;
    ice.friction = 0.1f;
    ice.restitution = 0.1f;
    physicsMaterials["Ice"] = ice;
    
    // 預設視覺材質
    VisualMaterial defaultVisual("Default");
    visualMaterials["Default"] = defaultVisual;
    
    VisualMaterial red("Red");
    red.diffuseColor = Color(0.8f, 0.2f, 0.2f, 1.0f);
    visualMaterials["Red"] = red;
    
    VisualMaterial green("Green");
    green.diffuseColor = Color(0.2f, 0.8f, 0.2f, 1.0f);
    visualMaterials["Green"] = green;
    
    VisualMaterial blue("Blue");
    blue.diffuseColor = Color(0.2f, 0.2f, 0.8f, 1.0f);
    visualMaterials["Blue"] = blue;
}

void PhysicsScene::initializeDefaultObjects() {
    // 建立預設相機
    Camera mainCamera("MainCamera");
    mainCamera.transform.position = Vector3(0.0f, 5.0f, 10.0f);
    cameras.push_back(mainCamera);
    activeCamera = "MainCamera";
    
    // 建立預設光源
    Light mainLight("MainLight");
    mainLight.type = LightType::Directional;
    mainLight.transform.rotation = Quaternion(0.707f, -0.707f, 0.0f, 0.0f);  // 45度向下
    lights.push_back(mainLight);
    
    // 建立預設重力場
    ForceField gravity("Gravity");
    gravity.type = ForceFieldType::Gravity;
    gravity.direction = Vector3(0.0f, -9.81f, 0.0f);
    forceFields.push_back(gravity);
}

std::string PhysicsScene::generateUniqueObjectName(const std::string& baseName, 
                                                 const std::vector<std::string>& existingNames) const {
    std::string name = baseName;
    int counter = 1;
    
    while (std::find(existingNames.begin(), existingNames.end(), name) != existingNames.end()) {
        name = baseName + "_" + std::to_string(counter);
        counter++;
    }
    
    return name;
}

// Utils 命名空間實現
namespace Utils {

FileFormat detectFileFormat(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) {
        return FileFormat::Unknown;
    }
    
    std::string ext = filename.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return FileFormat::JSON;
    } else if (ext == "bin" || ext == "ogc") {
        return FileFormat::Binary;
    }
    
    return FileFormat::Unknown;
}

std::string getFileFormatExtension(FileFormat format) {
    switch (format) {
        case FileFormat::JSON:
            return ".json";
        case FileFormat::Binary:
            return ".ogc";
        default:
            return "";
    }
}

Vector3 calculateBoxInertia(float mass, float width, float height, float depth) {
    float factor = mass / 12.0f;
    return Vector3(
        factor * (height * height + depth * depth),
        factor * (width * width + depth * depth),
        factor * (width * width + height * height)
    );
}

Vector3 calculateSphereInertia(float mass, float radius) {
    float inertia = 0.4f * mass * radius * radius;
    return Vector3(inertia, inertia, inertia);
}

Vector3 calculateCylinderInertia(float mass, float radius, float height) {
    float radialInertia = 0.5f * mass * radius * radius;
    float axialInertia = mass * (3.0f * radius * radius + height * height) / 12.0f;
    return Vector3(axialInertia, radialInertia, axialInertia);
}

Transform interpolateTransform(const Transform& a, const Transform& b, float t) {
    Transform result;
    
    // 線性插值位置
    result.position.x = a.position.x + t * (b.position.x - a.position.x);
    result.position.y = a.position.y + t * (b.position.y - a.position.y);
    result.position.z = a.position.z + t * (b.position.z - a.position.z);
    
    // 球面線性插值旋轉（簡化版本）
    result.rotation = a.rotation;  // TODO: 實現 slerp
    
    // 線性插值縮放
    result.scale.x = a.scale.x + t * (b.scale.x - a.scale.x);
    result.scale.y = a.scale.y + t * (b.scale.y - a.scale.y);
    result.scale.z = a.scale.z + t * (b.scale.z - a.scale.z);
    
    return result;
}

Transform combineTransforms(const Transform& parent, const Transform& child) {
    // TODO: 實現變換組合
    return child;  // 簡化實現
}

Transform invertTransform(const Transform& transform) {
    // TODO: 實現變換反轉
    return transform;  // 簡化實現
}

Color hexToColor(const std::string& hex) {
    if (hex.length() != 7 || hex[0] != '#') {
        return Color();
    }
    
    unsigned int r, g, b;
    sscanf(hex.c_str() + 1, "%02x%02x%02x", &r, &g, &b);
    
    return Color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}

std::string colorToHex(const Color& color) {
    int r = static_cast<int>(color.r * 255.0f);
    int g = static_cast<int>(color.g * 255.0f);
    int b = static_cast<int>(color.b * 255.0f);
    
    char hex[8];
    sprintf(hex, "#%02x%02x%02x", r, g, b);
    
    return std::string(hex);
}

bool isValidObjectName(const std::string& name) {
    if (name.empty() || name.length() > 128) {
        return false;
    }
    
    // 檢查是否包含無效字元
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != ' ') {
            return false;
        }
    }
    
    return true;
}

bool isValidFilePath(const std::string& path) {
    // 簡化的檔案路徑驗證
    return !path.empty() && path.length() < 260;
}

float degreesToRadians(float degrees) {
    return degrees * 3.14159265359f / 180.0f;
}

float radiansToDegrees(float radians) {
    return radians * 180.0f / 3.14159265359f;
}

} // namespace Utils

} // namespace PhysicsScene
