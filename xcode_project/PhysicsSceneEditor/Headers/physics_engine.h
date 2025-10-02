#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

// Bullet Physics
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

// OGC 整合
#include "../ogc_integration/ogc_contact_solver.h"

// 場景格式
#include "../scene_format/physics_scene_format.h"

/**
 * @file physics_engine.h
 * @brief 跨平台物理引擎類別
 * 
 * 整合 Bullet Physics 和 OGC Contact Model，提供完整的物理模擬功能。
 * 支援混合模式，可以根據場景需求自動選擇最適合的物理演算法。
 */

class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine();

    // 初始化和清理
    bool Initialize();
    void Cleanup();
    bool InitializeScene(const PhysicsScene::PhysicsScene& scene);
    void ResetScene();

    // 模擬控制
    void StepSimulation(float deltaTime);
    void SetTimeStep(float timeStep);
    void SetGravity(const PhysicsScene::Vector3& gravity);
    void SetSolverIterations(int iterations);

    // OGC 設定
    void EnableOGCContact(bool enable);
    void SetOGCContactRadius(float radius);
    void SetHybridMode(bool enable);
    bool IsOGCEnabled() const { return m_useOGCContact; }
    bool IsHybridModeEnabled() const { return m_hybridMode; }

    // 物件管理
    void AddRigidBody(const std::string& name, const PhysicsScene::RigidBody& rigidBody);
    void RemoveRigidBody(const std::string& name);
    void UpdateRigidBody(const std::string& name, const PhysicsScene::RigidBody& rigidBody);
    
    void AddConstraint(const std::string& name, const PhysicsScene::Constraint& constraint);
    void RemoveConstraint(const std::string& name);
    
    void AddForceField(const std::string& name, const PhysicsScene::ForceField& forceField);
    void RemoveForceField(const std::string& name);

    // 查詢功能
    PhysicsScene::Transform GetRigidBodyTransform(const std::string& name) const;
    PhysicsScene::Vector3 GetRigidBodyLinearVelocity(const std::string& name) const;
    PhysicsScene::Vector3 GetRigidBodyAngularVelocity(const std::string& name) const;
    bool IsRigidBodyActive(const std::string& name) const;

    // 射線檢測
    struct RaycastResult {
        bool hit = false;
        std::string objectName;
        PhysicsScene::Vector3 point;
        PhysicsScene::Vector3 normal;
        float distance = 0.0f;
    };
    RaycastResult Raycast(const PhysicsScene::Vector3& from, const PhysicsScene::Vector3& to) const;

    // 碰撞檢測
    std::vector<std::string> GetCollidingObjects(const std::string& objectName) const;
    bool IsColliding(const std::string& objectA, const std::string& objectB) const;

    // 統計資訊
    struct Statistics {
        int rigidBodyCount = 0;
        int constraintCount = 0;
        int contactPointCount = 0;
        int activeBodyCount = 0;
        float simulationTime = 0.0f;
        float ogcSolveTime = 0.0f;
        float bulletSolveTime = 0.0f;
        int ogcIterations = 0;
        int bulletIterations = 0;
    };
    const Statistics& GetStatistics() const { return m_statistics; }

    // 除錯功能
    void SetDebugDrawer(btIDebugDraw* debugDrawer);
    void DebugDrawWorld();
    void EnableDebugDraw(bool enable);

private:
    // Bullet Physics 組件
    std::unique_ptr<btDefaultCollisionConfiguration> m_collisionConfig;
    std::unique_ptr<btCollisionDispatcher> m_dispatcher;
    std::unique_ptr<btDbvtBroadphase> m_broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::unique_ptr<btDiscreteDynamicsWorld> m_dynamicsWorld;

    // OGC 整合
    std::unique_ptr<OGCContactSolver> m_ogcSolver;
    bool m_useOGCContact;
    float m_ogcContactRadius;
    bool m_hybridMode;

    // 物件管理
    struct RigidBodyData {
        std::unique_ptr<btRigidBody> bulletBody;
        std::unique_ptr<btCollisionShape> shape;
        std::unique_ptr<btMotionState> motionState;
        PhysicsScene::RigidBody sceneData;
    };
    std::unordered_map<std::string, std::unique_ptr<RigidBodyData>> m_rigidBodies;

    struct ConstraintData {
        std::unique_ptr<btTypedConstraint> bulletConstraint;
        PhysicsScene::Constraint sceneData;
    };
    std::unordered_map<std::string, std::unique_ptr<ConstraintData>> m_constraints;

    struct ForceFieldData {
        PhysicsScene::ForceField sceneData;
        std::vector<std::string> affectedBodies;
    };
    std::unordered_map<std::string, std::unique_ptr<ForceFieldData>> m_forceFields;

    // 材質管理
    std::unordered_map<std::string, PhysicsScene::PhysicsMaterial> m_physicsMaterials;

    // 模擬設定
    float m_timeStep;
    PhysicsScene::Vector3 m_gravity;
    int m_solverIterations;
    float m_simulationTime;

    // 統計資訊
    mutable Statistics m_statistics;

    // 除錯繪製
    btIDebugDraw* m_debugDrawer;
    bool m_debugDrawEnabled;

    // 初始化輔助函數
    void InitializeBulletPhysics();
    void InitializeOGCIntegration();
    void SetupCollisionFiltering();

    // 物件建立輔助函數
    btCollisionShape* CreateCollisionShape(const PhysicsScene::RigidBody& rigidBody);
    btRigidBody* CreateBulletRigidBody(const PhysicsScene::RigidBody& rigidBody, btCollisionShape* shape);
    btTypedConstraint* CreateBulletConstraint(const PhysicsScene::Constraint& constraint);

    // 形狀建立函數
    btCollisionShape* CreateBoxShape(const PhysicsScene::Vector3& halfExtents);
    btCollisionShape* CreateSphereShape(float radius);
    btCollisionShape* CreateCylinderShape(float radius, float height);
    btCollisionShape* CreateCapsuleShape(float radius, float height);
    btCollisionShape* CreateConeShape(float radius, float height);
    btCollisionShape* CreatePlaneShape(const PhysicsScene::Vector3& normal, float distance);
    btCollisionShape* CreateMeshShape(const std::vector<PhysicsScene::Vector3>& vertices,
                                      const std::vector<std::array<int, 3>>& triangles);

    // 約束建立函數
    btTypedConstraint* CreateHingeConstraint(const PhysicsScene::Constraint& constraint);
    btTypedConstraint* CreateSliderConstraint(const PhysicsScene::Constraint& constraint);
    btTypedConstraint* CreateFixedConstraint(const PhysicsScene::Constraint& constraint);
    btTypedConstraint* CreatePointToPointConstraint(const PhysicsScene::Constraint& constraint);
    btTypedConstraint* CreateGeneric6DofConstraint(const PhysicsScene::Constraint& constraint);

    // 力場處理
    void UpdateForceFields(float deltaTime);
    void ApplyForceField(const ForceFieldData& forceField, float deltaTime);
    PhysicsScene::Vector3 CalculateForceFieldForce(const PhysicsScene::ForceField& forceField,
                                                    const PhysicsScene::Vector3& position) const;

    // OGC 整合函數
    void UpdateOGCContacts();
    void SolveOGCContacts(float deltaTime);
    std::vector<OGCContactSolver::ContactPoint> ExtractContactPoints() const;
    void ApplyOGCImpulses(const std::vector<OGCContactSolver::ContactImpulse>& impulses);

    // 混合模式處理
    void UpdateHybridMode(float deltaTime);
    bool ShouldUseOGCForContact(const btPersistentManifold* manifold) const;

    // 輔助函數
    btVector3 ToBulletVector3(const PhysicsScene::Vector3& v) const;
    PhysicsScene::Vector3 FromBulletVector3(const btVector3& v) const;
    btQuaternion ToBulletQuaternion(const PhysicsScene::Quaternion& q) const;
    PhysicsScene::Quaternion FromBulletQuaternion(const btQuaternion& q) const;
    btTransform ToBulletTransform(const PhysicsScene::Transform& t) const;
    PhysicsScene::Transform FromBulletTransform(const btTransform& t) const;

    // 統計更新
    void UpdateStatistics();
    void ResetStatistics();

    // 錯誤處理
    void HandlePhysicsError(const std::string& message);

public:
    // 回調介面
    class CollisionCallback {
    public:
        virtual ~CollisionCallback() = default;
        virtual void OnCollisionEnter(const std::string& objectA, const std::string& objectB) = 0;
        virtual void OnCollisionExit(const std::string& objectA, const std::string& objectB) = 0;
        virtual void OnCollisionStay(const std::string& objectA, const std::string& objectB) = 0;
    };

    void SetCollisionCallback(CollisionCallback* callback);

private:
    CollisionCallback* m_collisionCallback;
    std::unordered_map<std::pair<std::string, std::string>, bool, 
                       std::hash<std::string>> m_previousCollisions;

    void ProcessCollisionCallbacks();

    // 自訂哈希函數
    struct PairHash {
        std::size_t operator()(const std::pair<std::string, std::string>& p) const {
            return std::hash<std::string>{}(p.first) ^ (std::hash<std::string>{}(p.second) << 1);
        }
    };
};

/**
 * @class PhysicsDebugDrawer
 * @brief Bullet Physics 除錯繪製器
 * 
 * 實現 btIDebugDraw 介面，用於在 OpenGL 中繪製物理除錯資訊。
 */
class PhysicsDebugDrawer : public btIDebugDraw {
public:
    PhysicsDebugDrawer();
    virtual ~PhysicsDebugDrawer();

    // btIDebugDraw 介面實現
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, 
                                  btScalar distance, int lifeTime, const btVector3& color) override;
    virtual void reportErrorWarning(const char* warningString) override;
    virtual void draw3dText(const btVector3& location, const char* textString) override;
    virtual void setDebugMode(int debugMode) override;
    virtual int getDebugMode() const override;

    // 額外功能
    void SetLineWidth(float width) { m_lineWidth = width; }
    void SetPointSize(float size) { m_pointSize = size; }
    void EnableDepthTest(bool enable) { m_depthTestEnabled = enable; }

private:
    int m_debugMode;
    float m_lineWidth;
    float m_pointSize;
    bool m_depthTestEnabled;

    void SetupRenderState();
    void RestoreRenderState();
};
