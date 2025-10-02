#include "physics/ClothSimulation.h"
#include <cmath>
#include <algorithm>
#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLContext>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

namespace Physics {

// ============================================================================
// ClothParticle Implementation
// ============================================================================

ClothParticle::ClothParticle(const QVector3D& pos, float m)
    : position(pos)
    , velocity(0, 0, 0)
    , acceleration(0, 0, 0)
    , force(0, 0, 0)
    , mass(m)
    , invMass(m > 0 ? 1.0f / m : 0.0f)
    , pinned(false)
    , normal(0, 1, 0)
    , texCoord(0, 0)
{
}

void ClothParticle::update(float deltaTime) {
    if (pinned) return;
    
    // Verlet integration
    acceleration = force * invMass;
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
    
    clearForces();
}

void ClothParticle::addForce(const QVector3D& f) {
    force += f;
}

void ClothParticle::clearForces() {
    force = QVector3D(0, 0, 0);
}

// ============================================================================
// ClothConstraint Implementation
// ============================================================================

ClothConstraint::ClothConstraint(ClothParticle* p1, ClothParticle* p2, float restLen)
    : particle1(p1)
    , particle2(p2)
    , stiffness(0.8f)
    , damping(0.1f)
{
    if (restLen < 0) {
        restLength = (p1->position - p2->position).length();
    } else {
        restLength = restLen;
    }
}

void ClothConstraint::satisfy() {
    QVector3D delta = particle2->position - particle1->position;
    float currentLength = delta.length();
    
    if (currentLength < 1e-6f) return;
    
    float difference = (currentLength - restLength) / currentLength;
    QVector3D correction = delta * difference * 0.5f * stiffness;
    
    if (!particle1->pinned) {
        particle1->position += correction;
    }
    if (!particle2->pinned) {
        particle2->position -= correction;
    }
    
    // 阻尼
    QVector3D relativeVelocity = particle2->velocity - particle1->velocity;
    QVector3D dampingForce = relativeVelocity * damping;
    
    if (!particle1->pinned) {
        particle1->velocity += dampingForce * particle1->invMass;
    }
    if (!particle2->pinned) {
        particle2->velocity -= dampingForce * particle2->invMass;
    }
}

// ============================================================================
// CylinderCollider Implementation
// ============================================================================

CylinderCollider::CylinderCollider(const QVector3D& center, float r, float h)
    : center(center)
    , radius(r)
    , height(h)
{
    transform.setToIdentity();
    transform.translate(center);
}

bool CylinderCollider::checkCollision(ClothParticle* particle, QVector3D& contactPoint, QVector3D& contactNormal) {
    QVector3D localPos = particle->position - center;
    
    // 檢查高度範圍
    if (localPos.y() < -height * 0.5f || localPos.y() > height * 0.5f) {
        return false;
    }
    
    // 檢查徑向距離
    float radialDistance = sqrt(localPos.x() * localPos.x() + localPos.z() * localPos.z());
    
    if (radialDistance < radius) {
        // 發生碰撞
        if (radialDistance < 1e-6f) {
            // 粒子在圓柱軸上，使用預設法線
            contactNormal = QVector3D(1, 0, 0);
        } else {
            contactNormal = QVector3D(localPos.x() / radialDistance, 0, localPos.z() / radialDistance);
        }
        
        contactPoint = center + QVector3D(contactNormal.x() * radius, localPos.y(), contactNormal.z() * radius);
        return true;
    }
    
    return false;
}

// ============================================================================
// OGCContactModel Implementation
// ============================================================================

OGCContactModel::OGCContactModel(float contactRadius)
    : m_contactRadius(contactRadius)
    , m_stiffness(1000.0f)
    , m_damping(50.0f)
{
}

void OGCContactModel::processContacts(const std::vector<ContactInfo>& contacts, float deltaTime) {
    for (const auto& contact : contacts) {
        applyOGCForce(contact, deltaTime);
    }
}

void OGCContactModel::applyOGCForce(const ContactInfo& contact, float deltaTime) {
    // OGC 核心演算法：偏移幾何接觸
    QVector3D offsetGeometry = calculateOffsetGeometry(contact);
    
    // 計算接觸力
    float penetration = std::max(0.0f, contact.penetrationDepth);
    QVector3D contactForce = contact.contactNormal * (m_stiffness * penetration);
    
    // 添加阻尼
    float normalVelocity = QVector3D::dotProduct(contact.particle->velocity, contact.contactNormal);
    if (normalVelocity < 0) {  // 只在接近時添加阻尼
        QVector3D dampingForce = contact.contactNormal * (m_damping * normalVelocity);
        contactForce += dampingForce;
    }
    
    // 應用力到粒子
    contact.particle->addForce(contactForce);
    
    // OGC 特有的位置修正
    if (penetration > 0) {
        QVector3D positionCorrection = contact.contactNormal * (penetration * 0.8f);
        if (!contact.particle->pinned) {
            contact.particle->position += positionCorrection;
        }
    }
}

QVector3D OGCContactModel::calculateOffsetGeometry(const ContactInfo& contact) {
    // OGC 偏移幾何計算
    // 這是 OGC 模型的核心：使用接觸半徑創建偏移幾何
    QVector3D offset = contact.contactNormal * m_contactRadius;
    return contact.contactPoint + offset;
}

// ============================================================================
// ClothSimulation Implementation
// ============================================================================

ClothSimulation::ClothSimulation(int width, int height, float spacing)
    : m_width(width)
    , m_height(height)
    , m_spacing(spacing)
    , m_useOGC(true)
    , m_gravity(0, -9.81f, 0)
    , m_wind(0, 0, 0)
    , m_damping(0.99f)
    , m_timeStep(1.0f / 60.0f)
    , m_constraintIterations(3)
    , m_paused(false)
    , m_simulationTime(0.0f)
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_renderDataDirty(true)
{
    m_ogcModel = std::make_unique<OGCContactModel>(0.05f);
}

ClothSimulation::~ClothSimulation() {
    // OpenGL 資源清理
    if (QOpenGLContext::currentContext()) {
        auto* gl = QOpenGLContext::currentContext()->functions();
        if (m_VAO) gl->glDeleteVertexArrays(1, &m_VAO);
        if (m_VBO) gl->glDeleteBuffers(1, &m_VBO);
        if (m_EBO) gl->glDeleteBuffers(1, &m_EBO);
    }
}

void ClothSimulation::initialize() {
    qDebug() << "初始化布料模擬...";
    
    // 清理現有數據
    m_particles.clear();
    m_constraints.clear();
    m_cylinders.clear();
    
    // 創建布料網格
    createClothMesh();
    createConstraints();
    
    // 添加預設圓柱體
    addCylinder(QVector3D(0, -2, 0), 1.5f, 0.5f);
    
    // 固定布料頂部
    for (int x = 0; x < m_width; ++x) {
        if (x % 4 == 0) {  // 每隔4個點固定一個
            getParticle(x, 0)->pinned = true;
        }
    }
    
    m_simulationTime = 0.0f;
    m_renderDataDirty = true;
    
    qDebug() << QString("布料模擬初始化完成：%1 個粒子，%2 個約束")
                .arg(m_particles.size())
                .arg(m_constraints.size());
}

void ClothSimulation::initialize(int width, int height, float spacing) {
    // 更新布料參數
    m_width = width;
    m_height = height;
    m_spacing = spacing;
    
    // 調用標準初始化
    initialize();
}

void ClothSimulation::update(float deltaTime) {
    if (m_paused) return;
    
    float dt = std::min(deltaTime, m_timeStep);
    
    // 應用外力
    applyForces();
    
    // 處理碰撞
    handleCollisions();
    
    // 更新粒子
    updateParticles(dt);
    
    // 滿足約束（多次迭代）
    for (int i = 0; i < m_constraintIterations; ++i) {
        satisfyConstraints();
    }
    
    // 計算法線
    calculateNormals();
    
    m_simulationTime += dt;
    m_renderDataDirty = true;
}

void ClothSimulation::reset() {
    initialize();
}

void ClothSimulation::addCylinder(const QVector3D& center, float radius, float height) {
    auto cylinder = std::make_unique<CylinderCollider>(center, radius, height);
    m_cylinders.push_back(std::move(cylinder));
    
    qDebug() << QString("添加圓柱體：中心(%1, %2, %3)，半徑 %4，高度 %5")
                .arg(center.x()).arg(center.y()).arg(center.z())
                .arg(radius).arg(height);
}

void ClothSimulation::setOGCContactRadius(float radius) {
    if (m_ogcModel) {
        m_ogcModel->setContactRadius(radius);
    }
}

float ClothSimulation::getOGCContactRadius() const {
    return m_ogcModel ? m_ogcModel->getContactRadius() : 0.05f;
}

void ClothSimulation::createClothMesh() {
    // 創建粒子網格
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            QVector3D pos(
                (x - m_width * 0.5f) * m_spacing,
                2.0f,  // 起始高度
                (y - m_height * 0.5f) * m_spacing
            );
            
            auto particle = std::make_unique<ClothParticle>(pos);
            particle->texCoord = QVector2D(float(x) / (m_width - 1), float(y) / (m_height - 1));
            m_particles.push_back(std::move(particle));
        }
    }
}

void ClothSimulation::createConstraints() {
    // 結構約束（水平和垂直）
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            ClothParticle* current = getParticle(x, y);
            
            // 右邊的約束
            if (x < m_width - 1) {
                ClothParticle* right = getParticle(x + 1, y);
                m_constraints.push_back(std::make_unique<ClothConstraint>(current, right));
            }
            
            // 下面的約束
            if (y < m_height - 1) {
                ClothParticle* down = getParticle(x, y + 1);
                m_constraints.push_back(std::make_unique<ClothConstraint>(current, down));
            }
        }
    }
    
    // 剪切約束（對角線）
    for (int y = 0; y < m_height - 1; ++y) {
        for (int x = 0; x < m_width - 1; ++x) {
            ClothParticle* current = getParticle(x, y);
            ClothParticle* diag1 = getParticle(x + 1, y + 1);
            ClothParticle* diag2 = getParticle(x + 1, y);
            ClothParticle* diag3 = getParticle(x, y + 1);
            
            m_constraints.push_back(std::make_unique<ClothConstraint>(current, diag1));
            m_constraints.push_back(std::make_unique<ClothConstraint>(diag2, diag3));
        }
    }
    
    // 彎曲約束（每隔一個粒子）
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width - 2; ++x) {
            ClothParticle* p1 = getParticle(x, y);
            ClothParticle* p2 = getParticle(x + 2, y);
            m_constraints.push_back(std::make_unique<ClothConstraint>(p1, p2));
        }
    }
    
    for (int y = 0; y < m_height - 2; ++y) {
        for (int x = 0; x < m_width; ++x) {
            ClothParticle* p1 = getParticle(x, y);
            ClothParticle* p2 = getParticle(x, y + 2);
            m_constraints.push_back(std::make_unique<ClothConstraint>(p1, p2));
        }
    }
}

void ClothSimulation::applyForces() {
    for (auto& particle : m_particles) {
        // 重力
        particle->addForce(m_gravity * particle->mass);
        
        // 風力
        if (m_wind.length() > 0) {
            particle->addForce(m_wind * particle->mass * 0.1f);
        }
        
        // 阻尼
        particle->velocity *= m_damping;
    }
}

void ClothSimulation::satisfyConstraints() {
    for (auto& constraint : m_constraints) {
        constraint->satisfy();
    }
}

void ClothSimulation::handleCollisions() {
    if (!m_useOGC || m_cylinders.empty()) return;
    
    std::vector<OGCContactModel::ContactInfo> contacts;
    
    for (auto& particle : m_particles) {
        for (auto& cylinder : m_cylinders) {
            QVector3D contactPoint, contactNormal;
            
            if (cylinder->checkCollision(particle.get(), contactPoint, contactNormal)) {
                OGCContactModel::ContactInfo contact;
                contact.particle = particle.get();
                contact.contactPoint = contactPoint;
                contact.contactNormal = contactNormal;
                contact.penetrationDepth = (contactPoint - particle->position).length();
                contact.contactRadius = m_ogcModel->getContactRadius();
                
                contacts.push_back(contact);
            }
        }
    }
    
    // 使用 OGC 模型處理接觸
    if (!contacts.empty()) {
        m_ogcModel->processContacts(contacts, m_timeStep);
    }
}

void ClothSimulation::updateParticles(float deltaTime) {
    for (auto& particle : m_particles) {
        particle->update(deltaTime);
    }
}

ClothParticle* ClothSimulation::getParticle(int x, int y) {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return nullptr;
    }
    return m_particles[getParticleIndex(x, y)].get();
}

int ClothSimulation::getParticleIndex(int x, int y) const {
    return y * m_width + x;
}

void ClothSimulation::calculateNormals() {
    // 重置法線
    for (auto& particle : m_particles) {
        particle->normal = QVector3D(0, 0, 0);
    }
    
    // 計算面法線並累加到頂點
    for (int y = 0; y < m_height - 1; ++y) {
        for (int x = 0; x < m_width - 1; ++x) {
            ClothParticle* p1 = getParticle(x, y);
            ClothParticle* p2 = getParticle(x + 1, y);
            ClothParticle* p3 = getParticle(x, y + 1);
            ClothParticle* p4 = getParticle(x + 1, y + 1);
            
            if (p1 && p2 && p3 && p4) {
                // 第一個三角形
                QVector3D v1 = p2->position - p1->position;
                QVector3D v2 = p3->position - p1->position;
                QVector3D normal1 = QVector3D::crossProduct(v1, v2).normalized();
                
                p1->normal += normal1;
                p2->normal += normal1;
                p3->normal += normal1;
                
                // 第二個三角形
                QVector3D v3 = p4->position - p2->position;
                QVector3D v4 = p3->position - p2->position;
                QVector3D normal2 = QVector3D::crossProduct(v3, v4).normalized();
                
                p2->normal += normal2;
                p3->normal += normal2;
                p4->normal += normal2;
            }
        }
    }
    
    // 正規化法線
    for (auto& particle : m_particles) {
        if (particle->normal.length() > 0) {
            particle->normal.normalize();
        } else {
            particle->normal = QVector3D(0, 1, 0);
        }
    }
}

void ClothSimulation::render() {
    if (m_renderDataDirty) {
        setupRenderData();
        m_renderDataDirty = false;
    }
    
    // 簡化渲染：暫時不使用 OpenGL 立即模式
    // 在實際應用中應該使用現代 OpenGL (VBO/VAO)
    qDebug() << "ClothSimulation::render() - 布料粒子數量:" << m_particles.size();
    
    // 暫時註解掉立即模式渲染
    /*
    // 渲染布料網格
    glColor3f(0.8f, 0.6f, 0.4f);  // 布料顏色
    
    glBegin(GL_TRIANGLES);
    for (int y = 0; y < m_height - 1; ++y) {
        for (int x = 0; x < m_width - 1; ++x) {
            ClothParticle* p1 = getParticle(x, y);
            ClothParticle* p2 = getParticle(x + 1, y);
            ClothParticle* p3 = getParticle(x, y + 1);
            ClothParticle* p4 = getParticle(x + 1, y + 1);
            
            if (p1 && p2 && p3 && p4) {
                // 第一個三角形
                glNormal3f(p1->normal.x(), p1->normal.y(), p1->normal.z());
                glVertex3f(p1->position.x(), p1->position.y(), p1->position.z());
                
                glNormal3f(p2->normal.x(), p2->normal.y(), p2->normal.z());
                glVertex3f(p2->position.x(), p2->position.y(), p2->position.z());
                
                glNormal3f(p3->normal.x(), p3->normal.y(), p3->normal.z());
                glVertex3f(p3->position.x(), p3->position.y(), p3->position.z());
                
                // 第二個三角形
                glNormal3f(p2->normal.x(), p2->normal.y(), p2->normal.z());
                glVertex3f(p2->position.x(), p2->position.y(), p2->position.z());
                
                glNormal3f(p4->normal.x(), p4->normal.y(), p4->normal.z());
                // glVertex3f(p4->position.x(), p4->position.y(), p4->position.z());
                
                // glNormal3f(p3->normal.x(), p3->normal.y(), p3->normal.z());
                // glVertex3f(p3->position.x(), p3->position.y(), p3->position.z());
            }
        }
    }
    // glEnd();
    */
}

void ClothSimulation::renderWireframe() {
    // 暫時註解掉線框渲染
    /*
    glColor3f(0.2f, 0.2f, 0.8f);  // 線框顏色
    glBegin(GL_LINES);
    
    for (auto& constraint : m_constraints) {
        // 這裡需要訪問約束的粒子，可能需要修改 ClothConstraint 類別
        // 暫時省略線框渲染
    }
    
    glEnd();
    */
}

void ClothSimulation::renderParticles() {
    // 暫時註解掉粒子渲染
    /*
    glColor3f(1.0f, 0.0f, 0.0f);  // 粒子顏色
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    
    for (auto& particle : m_particles) {
        glVertex3f(particle->position.x(), particle->position.y(), particle->position.z());
    }
    
    glEnd();
    */
}

void ClothSimulation::renderColliders() {
    // 暫時註解掉碰撞體渲染
    /*
    glColor3f(0.5f, 0.5f, 0.5f);  // 碰撞體顏色
    
    for (auto& cylinder : m_cylinders) {
        // 渲染圓柱體
        glPushMatrix();
        glTranslatef(cylinder->center.x(), cylinder->center.y(), cylinder->center.z());
        
        // 簡化的圓柱體渲染
        const int segments = 16;
        const float angleStep = 2.0f * M_PI / segments;
        
        // 側面
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segments; ++i) {
            float angle = i * angleStep;
            float x = cylinder->radius * cos(angle);
            float z = cylinder->radius * sin(angle);
            
            glVertex3f(x, -cylinder->height * 0.5f, z);
            glVertex3f(x, cylinder->height * 0.5f, z);
        }
        glEnd();
        
        // 頂面和底面
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, cylinder->height * 0.5f, 0);
        for (int i = 0; i <= segments; ++i) {
            float angle = i * angleStep;
            float x = cylinder->radius * cos(angle);
            float z = cylinder->radius * sin(angle);
            glVertex3f(x, cylinder->height * 0.5f, z);
        }
        glEnd();
        
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, -cylinder->height * 0.5f, 0);
        for (int i = segments; i >= 0; --i) {
            float angle = i * angleStep;
            float x = cylinder->radius * cos(angle);
            float z = cylinder->radius * sin(angle);
            glVertex3f(x, -cylinder->height * 0.5f, z);
        }
        glEnd();
        
        glPopMatrix();
    }
    */
}

void ClothSimulation::setupRenderData() {
    // 準備頂點數據（如果需要使用現代 OpenGL）
    m_vertices.clear();
    m_indices.clear();
    
    // 這裡可以準備 VBO/VAO 數據
    // 目前使用立即模式渲染，所以暫時省略
}

} // namespace Physics
