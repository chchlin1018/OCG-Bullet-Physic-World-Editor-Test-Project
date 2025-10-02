#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

// OpenGL
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 場景格式
#include "../scene_format/physics_scene_format.h"

/**
 * @file renderer.h
 * @brief 跨平台 OpenGL 渲染器類別
 * 
 * 提供現代化的 OpenGL 渲染功能，支援多種渲染模式、光照、陰影、
 * 材質系統等。專為物理場景視覺化設計。
 */

class Renderer {
public:
    Renderer();
    ~Renderer();

    // 初始化和清理
    bool Initialize(int windowWidth, int windowHeight);
    void Cleanup();
    bool InitializeScene(const PhysicsScene::PhysicsScene& scene);
    void ClearScene();

    // 渲染控制
    void BeginFrame();
    void EndFrame();
    void Render(const PhysicsScene::PhysicsScene& scene);
    void SetViewport(int x, int y, int width, int height);

    // 相機控制
    struct Camera {
        glm::vec3 position = glm::vec3(0.0f, 5.0f, 10.0f);
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        float fov = 45.0f;
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;
        bool orthographic = false;
        float orthographicSize = 10.0f;
        
        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix(float aspectRatio) const;
    };

    void SetCamera(const Camera& camera);
    const Camera& GetCamera() const { return m_camera; }
    Camera& GetCamera() { return m_camera; }

    // 渲染模式
    enum class RenderMode {
        Wireframe,
        Solid,
        Textured
    };

    void SetRenderMode(RenderMode mode);
    RenderMode GetRenderMode() const { return m_renderMode; }

    // 顯示選項
    void ShowGrid(bool show);
    void ShowAxes(bool show);
    void ShowBoundingBoxes(bool show);
    void ShowContactPoints(bool show);
    void ShowConstraints(bool show);
    void EnableLighting(bool enable);
    void EnableShadows(bool enable);
    void EnableAntiAliasing(bool enable);

    bool IsGridVisible() const { return m_showGrid; }
    bool IsAxesVisible() const { return m_showAxes; }
    bool IsLightingEnabled() const { return m_lightingEnabled; }
    bool AreShadowsEnabled() const { return m_shadowsEnabled; }

    // 材質和紋理
    void LoadTexture(const std::string& name, const std::string& filename);
    void UnloadTexture(const std::string& name);
    bool HasTexture(const std::string& name) const;

    // 除錯渲染
    void DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color = glm::vec3(1.0f));
    void DrawPoint(const glm::vec3& position, const glm::vec3& color = glm::vec3(1.0f), float size = 5.0f);
    void DrawSphere(const glm::vec3& center, float radius, const glm::vec3& color = glm::vec3(1.0f));
    void DrawBox(const glm::vec3& center, const glm::vec3& halfExtents, const glm::vec3& color = glm::vec3(1.0f));
    void DrawText(const std::string& text, const glm::vec2& position, const glm::vec3& color = glm::vec3(1.0f));

    // 統計資訊
    struct Statistics {
        int drawCalls = 0;
        int triangleCount = 0;
        int vertexCount = 0;
        float renderTime = 0.0f;
        float shadowMapTime = 0.0f;
        int textureMemoryMB = 0;
        int bufferMemoryMB = 0;
    };
    const Statistics& GetStatistics() const { return m_statistics; }

    // 截圖功能
    bool SaveScreenshot(const std::string& filename, int width = 0, int height = 0);

private:
    // 著色器管理
    class Shader {
    public:
        Shader();
        ~Shader();

        bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath,
                          const std::string& geometryPath = "");
        bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource,
                           const std::string& geometrySource = "");
        void Use();
        void Unuse();

        // Uniform 設定
        void SetBool(const std::string& name, bool value);
        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetVec2(const std::string& name, const glm::vec2& value);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetVec4(const std::string& name, const glm::vec4& value);
        void SetMat3(const std::string& name, const glm::mat3& value);
        void SetMat4(const std::string& name, const glm::mat4& value);

        GLuint GetProgram() const { return m_program; }
        bool IsValid() const { return m_program != 0; }

    private:
        GLuint m_program;
        std::unordered_map<std::string, GLint> m_uniformLocations;

        GLuint CompileShader(const std::string& source, GLenum type);
        GLint GetUniformLocation(const std::string& name);
    };

    // 網格資料
    struct Mesh {
        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;
        int indexCount = 0;
        int vertexCount = 0;
        GLenum primitiveType = GL_TRIANGLES;
        
        void Cleanup();
    };

    // 紋理資料
    struct Texture {
        GLuint id = 0;
        int width = 0;
        int height = 0;
        int channels = 0;
        std::string filename;
        
        void Cleanup();
    };

    // 材質資料
    struct Material {
        glm::vec4 diffuseColor = glm::vec4(1.0f);
        glm::vec3 specularColor = glm::vec3(1.0f);
        glm::vec3 emissiveColor = glm::vec3(0.0f);
        float shininess = 32.0f;
        float metallic = 0.0f;
        float roughness = 0.5f;
        float transparency = 1.0f;
        
        std::string diffuseTexture;
        std::string normalTexture;
        std::string specularTexture;
        std::string emissiveTexture;
    };

    // 光源資料
    struct Light {
        enum Type { Directional, Point, Spot };
        Type type = Point;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
        glm::vec3 color = glm::vec3(1.0f);
        float intensity = 1.0f;
        float range = 10.0f;
        float spotAngle = 45.0f;
        float spotExponent = 1.0f;
        bool castShadows = true;
    };

    // 陰影映射
    struct ShadowMap {
        GLuint framebuffer = 0;
        GLuint depthTexture = 0;
        int size = 1024;
        glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
        
        void Cleanup();
    };

    // 核心資料
    Camera m_camera;
    int m_windowWidth;
    int m_windowHeight;
    float m_aspectRatio;

    // 渲染狀態
    RenderMode m_renderMode;
    bool m_showGrid;
    bool m_showAxes;
    bool m_showBoundingBoxes;
    bool m_showContactPoints;
    bool m_showConstraints;
    bool m_lightingEnabled;
    bool m_shadowsEnabled;
    bool m_antiAliasingEnabled;

    // 著色器
    std::unique_ptr<Shader> m_basicShader;
    std::unique_ptr<Shader> m_litShader;
    std::unique_ptr<Shader> m_shadowMapShader;
    std::unique_ptr<Shader> m_wireframeShader;
    std::unique_ptr<Shader> m_debugShader;
    std::unique_ptr<Shader> m_textShader;

    // 幾何資料
    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_meshes;
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
    std::unordered_map<std::string, Material> m_materials;

    // 預建幾何
    std::unique_ptr<Mesh> m_boxMesh;
    std::unique_ptr<Mesh> m_sphereMesh;
    std::unique_ptr<Mesh> m_cylinderMesh;
    std::unique_ptr<Mesh> m_capsuleMesh;
    std::unique_ptr<Mesh> m_coneMesh;
    std::unique_ptr<Mesh> m_planeMesh;
    std::unique_ptr<Mesh> m_gridMesh;
    std::unique_ptr<Mesh> m_axesMesh;

    // 光照和陰影
    std::vector<Light> m_lights;
    std::vector<std::unique_ptr<ShadowMap>> m_shadowMaps;
    glm::vec3 m_ambientLight;

    // 統計資訊
    mutable Statistics m_statistics;

    // 除錯渲染
    std::vector<glm::vec3> m_debugLines;
    std::vector<glm::vec3> m_debugLineColors;
    std::vector<glm::vec3> m_debugPoints;
    std::vector<glm::vec3> m_debugPointColors;
    std::vector<float> m_debugPointSizes;

    // 初始化函數
    bool InitializeShaders();
    bool InitializeGeometry();
    bool InitializeShadowMapping();
    void InitializeDefaultMaterials();

    // 著色器載入
    std::string LoadShaderSource(const std::string& filename);
    std::string GetDefaultVertexShader();
    std::string GetDefaultFragmentShader();
    std::string GetLitVertexShader();
    std::string GetLitFragmentShader();
    std::string GetShadowMapVertexShader();
    std::string GetShadowMapFragmentShader();
    std::string GetWireframeVertexShader();
    std::string GetWireframeFragmentShader();
    std::string GetDebugVertexShader();
    std::string GetDebugFragmentShader();

    // 幾何建立
    std::unique_ptr<Mesh> CreateBoxMesh(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    std::unique_ptr<Mesh> CreateSphereMesh(float radius = 1.0f, int segments = 32);
    std::unique_ptr<Mesh> CreateCylinderMesh(float radius = 1.0f, float height = 2.0f, int segments = 32);
    std::unique_ptr<Mesh> CreateCapsuleMesh(float radius = 1.0f, float height = 2.0f, int segments = 16);
    std::unique_ptr<Mesh> CreateConeMesh(float radius = 1.0f, float height = 2.0f, int segments = 32);
    std::unique_ptr<Mesh> CreatePlaneMesh(float width = 10.0f, float depth = 10.0f);
    std::unique_ptr<Mesh> CreateGridMesh(float size = 10.0f, int divisions = 20);
    std::unique_ptr<Mesh> CreateAxesMesh(float length = 1.0f);

    // 網格操作
    std::unique_ptr<Mesh> CreateMeshFromVertices(const std::vector<glm::vec3>& vertices,
                                                const std::vector<glm::vec3>& normals,
                                                const std::vector<glm::vec2>& texCoords,
                                                const std::vector<unsigned int>& indices);
    void UpdateMesh(Mesh* mesh, const std::vector<glm::vec3>& vertices,
                   const std::vector<glm::vec3>& normals,
                   const std::vector<glm::vec2>& texCoords,
                   const std::vector<unsigned int>& indices);

    // 渲染函數
    void RenderScene(const PhysicsScene::PhysicsScene& scene);
    void RenderRigidBodies(const PhysicsScene::PhysicsScene& scene);
    void RenderRigidBody(const PhysicsScene::RigidBody& rigidBody, const std::string& materialName);
    void RenderConstraints(const PhysicsScene::PhysicsScene& scene);
    void RenderForceFields(const PhysicsScene::PhysicsScene& scene);
    void RenderLights(const PhysicsScene::PhysicsScene& scene);
    void RenderGrid();
    void RenderAxes();
    void RenderDebugElements();

    // 陰影渲染
    void RenderShadowMaps(const PhysicsScene::PhysicsScene& scene);
    void RenderShadowMap(const Light& light, ShadowMap& shadowMap, const PhysicsScene::PhysicsScene& scene);

    // 材質和光照
    void SetupLighting(const PhysicsScene::PhysicsScene& scene);
    void ApplyMaterial(const Material& material);
    void BindTexture(const std::string& textureName, int unit);

    // 輔助函數
    glm::vec3 ToGLMVec3(const PhysicsScene::Vector3& v) const;
    glm::vec4 ToGLMVec4(const PhysicsScene::Color& c) const;
    glm::mat4 ToGLMMatrix(const PhysicsScene::Transform& t) const;
    Material ConvertMaterial(const PhysicsScene::VisualMaterial& material) const;
    Light ConvertLight(const PhysicsScene::Light& light) const;

    // 統計更新
    void UpdateStatistics();
    void ResetStatistics();

    // 錯誤處理
    bool CheckGLError(const std::string& operation);
    void HandleRenderError(const std::string& message);

public:
    // 回調介面
    class RenderCallback {
    public:
        virtual ~RenderCallback() = default;
        virtual void OnPreRender() = 0;
        virtual void OnPostRender() = 0;
        virtual void OnRenderObject(const std::string& objectName) = 0;
    };

    void SetRenderCallback(RenderCallback* callback);

private:
    RenderCallback* m_renderCallback;
};
