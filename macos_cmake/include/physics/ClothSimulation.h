#pragma once

#include <vector>
#include <memory>
#include <QVector3D>
#include <QMatrix4x4>

namespace Physics {

/**
 * @brief 布料粒子類別
 */
class ClothParticle {
public:
    ClothParticle(const QVector3D& position, float mass = 1.0f);
    
    // 物理屬性
    QVector3D position;
    QVector3D velocity;
    QVector3D acceleration;
    QVector3D force;
    float mass;
    float invMass;
    bool pinned;  // 是否固定
    
    // 渲染屬性
    QVector3D normal;
    QVector2D texCoord;
    
    void update(float deltaTime);
    void addForce(const QVector3D& f);
    void clearForces();
};

/**
 * @brief 布料約束類別（彈簧約束）
 */
class ClothConstraint {
public:
    ClothConstraint(ClothParticle* p1, ClothParticle* p2, float restLength = -1.0f);
    
    void satisfy();
    void render();
    
private:
    ClothParticle* particle1;
    ClothParticle* particle2;
    float restLength;
    float stiffness;
    float damping;
};

/**
 * @brief 圓柱體碰撞體類別
 */
class CylinderCollider {
public:
    CylinderCollider(const QVector3D& center, float radius, float height);
    
    bool checkCollision(ClothParticle* particle, QVector3D& contactPoint, QVector3D& contactNormal);
    void render();
    
    QVector3D center;
    float radius;
    float height;
    QMatrix4x4 transform;
};

/**
 * @brief OGC 接觸模型實現
 */
class OGCContactModel {
public:
    struct ContactInfo {
        ClothParticle* particle;
        QVector3D contactPoint;
        QVector3D contactNormal;
        float penetrationDepth;
        float contactRadius;
    };
    
    OGCContactModel(float contactRadius = 0.1f);
    
    void processContacts(const std::vector<ContactInfo>& contacts, float deltaTime);
    void setContactRadius(float radius) { m_contactRadius = radius; }
    float getContactRadius() const { return m_contactRadius; }
    
private:
    float m_contactRadius;
    float m_stiffness;
    float m_damping;
    
    void applyOGCForce(const ContactInfo& contact, float deltaTime);
    QVector3D calculateOffsetGeometry(const ContactInfo& contact);
};

/**
 * @brief 布料模擬主類別
 */
class ClothSimulation {
public:
    ClothSimulation(int width = 20, int height = 20, float spacing = 0.2f);
    ~ClothSimulation();
    
    // 模擬控制
    void initialize();
    void initialize(int width, int height, float spacing);  // 帶參數的初始化
    void update(float deltaTime);
    void reset();
    void pause() { m_paused = true; }
    void resume() { m_paused = false; }
    bool isPaused() const { return m_paused; }
    
    // 場景設定
    void addCylinder(const QVector3D& center, float radius, float height);
    void setGravity(const QVector3D& gravity) { m_gravity = gravity; }
    void setWind(const QVector3D& wind) { m_wind = wind; }
    void setDamping(float damping) { m_damping = damping; }
    
    // OGC 設定
    void enableOGC(bool enable) { m_useOGC = enable; }
    void setUseOGC(bool enable) { m_useOGC = enable; }  // 別名方法
    void setOGCContactRadius(float radius);
    
    // 渲染
    void render();
    void renderWireframe();
    void renderParticles();
    void renderConstraints();
    void renderColliders();
    
    // 統計資訊
    int getParticleCount() const { return m_particles.size(); }
    int getConstraintCount() const { return m_constraints.size(); }
    float getSimulationTime() const { return m_simulationTime; }
    
    // OGC 狀態查詢
    bool getUseOGC() const { return m_useOGC; }
    float getOGCContactRadius() const;
    
    // 時間步長設定
    void setTimeStep(float timeStep) { m_timeStep = timeStep; }
    
private:
    // 布料網格
    int m_width, m_height;
    float m_spacing;
    std::vector<std::unique_ptr<ClothParticle>> m_particles;
    std::vector<std::unique_ptr<ClothConstraint>> m_constraints;
    
    // 碰撞體
    std::vector<std::unique_ptr<CylinderCollider>> m_cylinders;
    
    // OGC 接觸模型
    std::unique_ptr<OGCContactModel> m_ogcModel;
    bool m_useOGC;
    
    // 物理參數
    QVector3D m_gravity;
    QVector3D m_wind;
    float m_damping;
    float m_timeStep;
    int m_constraintIterations;
    
    // 模擬狀態
    bool m_paused;
    float m_simulationTime;
    
    // 私有方法
    void createClothMesh();
    void createConstraints();
    void applyForces();
    void satisfyConstraints();
    void handleCollisions();
    void updateParticles(float deltaTime);
    
    // 輔助方法
    ClothParticle* getParticle(int x, int y);
    int getParticleIndex(int x, int y) const;
    void calculateNormals();
    
    // 渲染輔助
    void setupRenderData();
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
    unsigned int m_VAO = 0, m_VBO = 0, m_EBO = 0;  // 初始化為 0
    bool m_renderDataDirty;
};

} // namespace Physics
