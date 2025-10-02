#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <array>

/**
 * @file physics_scene_format.h
 * @brief 物理場景描述檔案格式定義
 * 
 * 這個檔案定義了用於描述物理場景的資料結構和檔案格式。
 * 支援 JSON 和二進制格式的序列化/反序列化。
 * 
 * 版本: 1.0
 * 作者: OGC Simulator Team
 */

namespace PhysicsScene {

// 版本資訊
constexpr int SCENE_FORMAT_VERSION_MAJOR = 1;
constexpr int SCENE_FORMAT_VERSION_MINOR = 0;
constexpr int SCENE_FORMAT_VERSION_PATCH = 0;

// 基本數學類型
struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    Vector3() = default;
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    
    bool operator==(const Vector3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct Quaternion {
    float w = 1.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    
    Quaternion() = default;
    Quaternion(float w_, float x_, float y_, float z_) : w(w_), x(x_), y(y_), z(z_) {}
    
    bool operator==(const Quaternion& other) const {
        return w == other.w && x == other.x && y == other.y && z == other.z;
    }
};

struct Transform {
    Vector3 position;
    Quaternion rotation;
    Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);
    
    Transform() = default;
    Transform(const Vector3& pos, const Quaternion& rot = Quaternion(), const Vector3& scl = Vector3(1.0f, 1.0f, 1.0f))
        : position(pos), rotation(rot), scale(scl) {}
};

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
    
    Color() = default;
    Color(float r_, float g_, float b_, float a_ = 1.0f) : r(r_), g(g_), b(b_), a(a_) {}
};

// 物理材質屬性
struct PhysicsMaterial {
    std::string name = "Default";
    float density = 1.0f;              // 密度 (kg/m³)
    float friction = 0.5f;             // 摩擦係數
    float restitution = 0.0f;          // 彈性係數
    float rollingFriction = 0.0f;      // 滾動摩擦係數
    float spinningFriction = 0.0f;     // 旋轉摩擦係數
    float contactDamping = 0.0f;       // 接觸阻尼
    float contactStiffness = 0.0f;     // 接觸剛度
    bool isKinematic = false;          // 是否為運動學物體
    bool isStatic = false;             // 是否為靜態物體
    
    PhysicsMaterial() = default;
    PhysicsMaterial(const std::string& name_) : name(name_) {}
};

// 視覺材質屬性
struct VisualMaterial {
    std::string name = "Default";
    Color diffuseColor = Color(0.8f, 0.8f, 0.8f, 1.0f);
    Color specularColor = Color(0.2f, 0.2f, 0.2f, 1.0f);
    Color emissiveColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
    float shininess = 32.0f;
    float metallic = 0.0f;
    float roughness = 0.5f;
    float transparency = 1.0f;
    
    // 紋理路徑
    std::string diffuseTexture;
    std::string normalTexture;
    std::string specularTexture;
    std::string emissiveTexture;
    std::string metallicTexture;
    std::string roughnessTexture;
    
    VisualMaterial() = default;
    VisualMaterial(const std::string& name_) : name(name_) {}
};

// 幾何形狀類型
enum class ShapeType {
    Box,
    Sphere,
    Cylinder,
    Capsule,
    Cone,
    Plane,
    ConvexHull,
    TriangleMesh,
    Compound,
    HeightField
};

// 基本幾何形狀
struct GeometryShape {
    ShapeType type = ShapeType::Box;
    std::map<std::string, float> parameters;  // 形狀參數
    std::string meshFile;                     // 網格檔案路徑（用於 ConvexHull 和 TriangleMesh）
    std::vector<Vector3> vertices;            // 頂點資料（用於自訂形狀）
    std::vector<std::array<int, 3>> triangles; // 三角形索引（用於三角網格）
    
    GeometryShape() = default;
    GeometryShape(ShapeType t) : type(t) {}
    
    // 便利建構函數
    static GeometryShape createBox(float width, float height, float depth);
    static GeometryShape createSphere(float radius);
    static GeometryShape createCylinder(float radius, float height);
    static GeometryShape createCapsule(float radius, float height);
    static GeometryShape createCone(float radius, float height);
    static GeometryShape createPlane(float width, float depth);
};

// 複合形狀的子形狀
struct CompoundChild {
    GeometryShape shape;
    Transform localTransform;
    
    CompoundChild() = default;
    CompoundChild(const GeometryShape& s, const Transform& t) : shape(s), localTransform(t) {}
};

// 剛體定義
struct RigidBody {
    std::string name = "RigidBody";
    Transform transform;
    GeometryShape collisionShape;
    std::vector<CompoundChild> compoundChildren;  // 用於複合形狀
    
    // 物理屬性
    float mass = 1.0f;
    Vector3 centerOfMass = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 inertiaTensor = Vector3(1.0f, 1.0f, 1.0f);
    Vector3 linearVelocity = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 angularVelocity = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 linearFactor = Vector3(1.0f, 1.0f, 1.0f);   // 線性運動約束
    Vector3 angularFactor = Vector3(1.0f, 1.0f, 1.0f);  // 角運動約束
    
    // 阻尼
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;
    
    // 睡眠閾值
    float linearSleepingThreshold = 0.8f;
    float angularSleepingThreshold = 1.0f;
    
    // 材質引用
    std::string physicsMaterial = "Default";
    std::string visualMaterial = "Default";
    
    // 碰撞屬性
    int collisionGroup = 1;
    int collisionMask = -1;  // 與所有群組碰撞
    bool isTrigger = false;
    
    // 渲染屬性
    bool visible = true;
    bool castShadows = true;
    bool receiveShadows = true;
    
    RigidBody() = default;
    RigidBody(const std::string& name_) : name(name_) {}
};

// 約束類型
enum class ConstraintType {
    PointToPoint,
    Hinge,
    Slider,
    ConeTwist,
    Generic6DOF,
    Fixed
};

// 約束定義
struct Constraint {
    std::string name = "Constraint";
    ConstraintType type = ConstraintType::PointToPoint;
    
    std::string bodyA;  // 第一個剛體名稱
    std::string bodyB;  // 第二個剛體名稱（空字串表示連接到世界）
    
    Transform frameA;   // 在 bodyA 中的相對變換
    Transform frameB;   // 在 bodyB 中的相對變換
    
    // 約束參數
    std::map<std::string, float> parameters;
    
    // 約束限制
    Vector3 linearLowerLimit = Vector3(-1e30f, -1e30f, -1e30f);
    Vector3 linearUpperLimit = Vector3(1e30f, 1e30f, 1e30f);
    Vector3 angularLowerLimit = Vector3(-1e30f, -1e30f, -1e30f);
    Vector3 angularUpperLimit = Vector3(1e30f, 1e30f, 1e30f);
    
    // 約束力限制
    float breakingImpulseThreshold = 1e30f;
    
    bool enabled = true;
    
    Constraint() = default;
    Constraint(const std::string& name_) : name(name_) {}
};

// 力場類型
enum class ForceFieldType {
    Gravity,
    Uniform,
    Radial,
    Vortex,
    Drag,
    Spring
};

// 力場定義
struct ForceField {
    std::string name = "ForceField";
    ForceFieldType type = ForceFieldType::Gravity;
    
    Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 direction = Vector3(0.0f, -9.81f, 0.0f);
    float strength = 1.0f;
    float radius = 10.0f;
    float falloff = 1.0f;  // 衰減指數
    
    // 影響的物體群組
    int affectedGroups = -1;  // 影響所有群組
    
    bool enabled = true;
    
    ForceField() = default;
    ForceField(const std::string& name_) : name(name_) {}
};

// 光源類型
enum class LightType {
    Directional,
    Point,
    Spot,
    Area
};

// 光源定義
struct Light {
    std::string name = "Light";
    LightType type = LightType::Directional;
    
    Transform transform;
    Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
    float range = 10.0f;        // 點光源和聚光燈的範圍
    float spotAngle = 45.0f;    // 聚光燈的角度
    float spotExponent = 1.0f;  // 聚光燈的衰減指數
    
    bool castShadows = true;
    bool enabled = true;
    
    Light() = default;
    Light(const std::string& name_) : name(name_) {}
};

// 相機定義
struct Camera {
    std::string name = "Camera";
    Transform transform;
    
    float fov = 45.0f;          // 視野角度
    float nearPlane = 0.1f;     // 近裁剪面
    float farPlane = 1000.0f;   // 遠裁剪面
    float aspectRatio = 16.0f / 9.0f;
    
    bool isOrthographic = false;
    float orthographicSize = 10.0f;
    
    Camera() = default;
    Camera(const std::string& name_) : name(name_) {}
};

// 模擬設定
struct SimulationSettings {
    float timeStep = 1.0f / 60.0f;     // 時間步長
    int maxSubSteps = 10;              // 最大子步數
    float fixedTimeStep = 1.0f / 240.0f; // 固定時間步長
    
    Vector3 gravity = Vector3(0.0f, -9.81f, 0.0f);
    
    // 求解器設定
    int solverIterations = 10;
    int positionIterations = 3;
    float erp = 0.2f;              // Error Reduction Parameter
    float cfm = 0.0f;              // Constraint Force Mixing
    
    // OGC 設定
    bool useOGCContact = false;
    float ogcContactRadius = 0.01f;
    bool hybridMode = false;
    
    // 碰撞檢測設定
    float contactBreakingThreshold = 0.02f;
    float contactProcessingThreshold = 0.01f;
    
    // 效能設定
    bool enableCCD = false;        // 連續碰撞檢測
    bool enableSleeping = true;    // 物體休眠
    float sleepingLinearThreshold = 0.8f;
    float sleepingAngularThreshold = 1.0f;
    float sleepingTime = 2.0f;
    
    SimulationSettings() = default;
};

// 渲染設定
struct RenderSettings {
    Color backgroundColor = Color(0.2f, 0.3f, 0.4f, 1.0f);
    Color ambientLight = Color(0.2f, 0.2f, 0.2f, 1.0f);
    
    bool enableShadows = true;
    bool enableAntiAliasing = true;
    bool enableVSync = true;
    
    int shadowMapSize = 2048;
    float shadowBias = 0.001f;
    
    // 後處理效果
    bool enableBloom = false;
    bool enableSSAO = false;
    bool enableToneMapping = true;
    float exposure = 1.0f;
    float gamma = 2.2f;
    
    RenderSettings() = default;
};

// 場景元資料
struct SceneMetadata {
    std::string name = "Untitled Scene";
    std::string description;
    std::string author;
    std::string version = "1.0.0";
    std::string createdDate;
    std::string modifiedDate;
    std::map<std::string, std::string> customProperties;
    
    SceneMetadata() = default;
};

// 主要場景類別
class PhysicsScene {
public:
    // 場景元資料
    SceneMetadata metadata;
    
    // 格式版本
    int formatVersionMajor = SCENE_FORMAT_VERSION_MAJOR;
    int formatVersionMinor = SCENE_FORMAT_VERSION_MINOR;
    int formatVersionPatch = SCENE_FORMAT_VERSION_PATCH;
    
    // 材質庫
    std::map<std::string, PhysicsMaterial> physicsMaterials;
    std::map<std::string, VisualMaterial> visualMaterials;
    
    // 場景物件
    std::vector<RigidBody> rigidBodies;
    std::vector<Constraint> constraints;
    std::vector<ForceField> forceFields;
    std::vector<Light> lights;
    std::vector<Camera> cameras;
    
    // 設定
    SimulationSettings simulationSettings;
    RenderSettings renderSettings;
    
    // 活動相機
    std::string activeCamera = "MainCamera";
    
public:
    PhysicsScene();
    ~PhysicsScene() = default;
    
    // 場景管理
    void clear();
    bool isEmpty() const;
    
    // 物件管理
    RigidBody* findRigidBody(const std::string& name);
    const RigidBody* findRigidBody(const std::string& name) const;
    bool removeRigidBody(const std::string& name);
    
    Constraint* findConstraint(const std::string& name);
    const Constraint* findConstraint(const std::string& name) const;
    bool removeConstraint(const std::string& name);
    
    ForceField* findForceField(const std::string& name);
    const ForceField* findForceField(const std::string& name) const;
    bool removeForceField(const std::string& name);
    
    Light* findLight(const std::string& name);
    const Light* findLight(const std::string& name) const;
    bool removeLight(const std::string& name);
    
    Camera* findCamera(const std::string& name);
    const Camera* findCamera(const std::string& name) const;
    bool removeCamera(const std::string& name);
    
    // 材質管理
    PhysicsMaterial* findPhysicsMaterial(const std::string& name);
    const PhysicsMaterial* findPhysicsMaterial(const std::string& name) const;
    bool removePhysicsMaterial(const std::string& name);
    
    VisualMaterial* findVisualMaterial(const std::string& name);
    const VisualMaterial* findVisualMaterial(const std::string& name) const;
    bool removeVisualMaterial(const std::string& name);
    
    // 驗證
    bool validate(std::vector<std::string>& errors) const;
    
    // 統計資訊
    struct Statistics {
        size_t rigidBodyCount = 0;
        size_t constraintCount = 0;
        size_t forceFieldCount = 0;
        size_t lightCount = 0;
        size_t cameraCount = 0;
        size_t physicsMaterialCount = 0;
        size_t visualMaterialCount = 0;
        size_t totalTriangles = 0;
        size_t totalVertices = 0;
    };
    
    Statistics getStatistics() const;
    
    // 序列化/反序列化
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    
    bool saveToJSON(const std::string& filename) const;
    bool loadFromJSON(const std::string& filename);
    
    bool saveToBinary(const std::string& filename) const;
    bool loadFromBinary(const std::string& filename);
    
    std::string toJSONString() const;
    bool fromJSONString(const std::string& jsonStr);
    
private:
    void initializeDefaultMaterials();
    void initializeDefaultObjects();
    std::string generateUniqueObjectName(const std::string& baseName, 
                                       const std::vector<std::string>& existingNames) const;
};

// 便利函數
namespace Utils {
    // 檔案格式檢測
    enum class FileFormat {
        Unknown,
        JSON,
        Binary
    };
    
    FileFormat detectFileFormat(const std::string& filename);
    std::string getFileFormatExtension(FileFormat format);
    
    // 幾何工具
    Vector3 calculateBoxInertia(float mass, float width, float height, float depth);
    Vector3 calculateSphereInertia(float mass, float radius);
    Vector3 calculateCylinderInertia(float mass, float radius, float height);
    
    // 變換工具
    Transform interpolateTransform(const Transform& a, const Transform& b, float t);
    Transform combineTransforms(const Transform& parent, const Transform& child);
    Transform invertTransform(const Transform& transform);
    
    // 顏色工具
    Color hexToColor(const std::string& hex);
    std::string colorToHex(const Color& color);
    
    // 驗證工具
    bool isValidObjectName(const std::string& name);
    bool isValidFilePath(const std::string& path);
    
    // 單位轉換
    float degreesToRadians(float degrees);
    float radiansToDegrees(float radians);
}

} // namespace PhysicsScene
