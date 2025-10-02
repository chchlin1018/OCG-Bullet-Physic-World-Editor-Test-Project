#pragma once

#include <QtOpenGL/QOpenGLWidget>
#include <QtOpenGL/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QtOpenGL/QOpenGLTexture>
#include <QtOpenGL/QOpenGLFramebufferObject>
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QKeyEvent>
#include <QtCore/QTimer>
#include <QtCore/QElapsedTimer>
#include <memory>

#include "../scene_format/physics_scene_format.h"
#include "../cross_platform_runner/renderer.h"
#include "../cross_platform_runner/physics_engine.h"

class Camera;
class Grid;
class Gizmo;
class SelectionRenderer;

/**
 * @file ViewportWidget.h
 * @brief 3D 視口 Widget
 * 
 * 基於 OpenGL 的 3D 視口，支援場景渲染、物件選擇、
 * 變換操作等功能。整合物理模擬預覽。
 */

class ViewportWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    enum class ViewportMode {
        Solid,
        Wireframe,
        SolidWireframe,
        Debug
    };

    enum class TransformMode {
        Select,
        Move,
        Rotate,
        Scale
    };

    enum class ProjectionMode {
        Perspective,
        Orthographic
    };

    explicit ViewportWidget(QWidget *parent = nullptr);
    ~ViewportWidget();

    // 場景管理
    void SetScene(const PhysicsScene::PhysicsScene& scene);
    void RefreshScene();
    void ClearScene();

    // 選擇管理
    void SelectObject(const QString& objectId, const QString& objectType);
    void ClearSelection();
    QString GetSelectedObjectId() const { return m_selectedObjectId; }
    QStringList GetSelectedObjectIds() const { return m_selectedObjectIds; }

    // 視口設定
    void SetViewportMode(ViewportMode mode);
    ViewportMode GetViewportMode() const { return m_viewportMode; }

    void SetTransformMode(TransformMode mode);
    TransformMode GetTransformMode() const { return m_transformMode; }

    void SetProjectionMode(ProjectionMode mode);
    ProjectionMode GetProjectionMode() const { return m_projectionMode; }

    // 相機控制
    void SetCameraPosition(const QVector3D& position);
    void SetCameraTarget(const QVector3D& target);
    void SetCameraUp(const QVector3D& up);
    QVector3D GetCameraPosition() const;
    QVector3D GetCameraTarget() const;

    // 視圖操作
    void FrameSelected();
    void FrameAll();
    void ResetView();
    void SetViewDirection(const QVector3D& direction);

    // 顯示選項
    void SetShowGrid(bool show);
    void SetShowAxes(bool show);
    void SetShowBoundingBoxes(bool show);
    void SetShowConstraints(bool show);
    void SetShowContactPoints(bool show);
    void SetShowStatistics(bool show);

    bool GetShowGrid() const { return m_showGrid; }
    bool GetShowAxes() const { return m_showAxes; }
    bool GetShowBoundingBoxes() const { return m_showBoundingBoxes; }
    bool GetShowConstraints() const { return m_showConstraints; }
    bool GetShowContactPoints() const { return m_showContactPoints; }
    bool GetShowStatistics() const { return m_showStatistics; }

    // 模擬控制
    void SetSimulationMode(bool enabled);
    bool IsSimulationMode() const { return m_simulationMode; }
    void StepSimulation();
    void ResetSimulation();

    // 截圖
    QImage CaptureScreenshot();
    bool SaveScreenshot(const QString& filename);

protected:
    // OpenGL 事件
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

    // 滑鼠事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    // 鍵盤事件
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    // 其他事件
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private slots:
    void OnAnimationTimer();
    void OnSimulationTimer();

private:
    // 初始化
    void InitializeRenderer();
    void InitializePhysics();
    void InitializeCamera();
    void InitializeShaders();
    void InitializeBuffers();

    // 渲染
    void RenderScene();
    void RenderObjects();
    void RenderGrid();
    void RenderAxes();
    void RenderBoundingBoxes();
    void RenderConstraints();
    void RenderContactPoints();
    void RenderGizmos();
    void RenderSelection();
    void RenderStatistics();

    // 選擇
    void PerformSelection(const QPoint& point);
    void PerformBoxSelection(const QRect& rect);
    QString PickObject(const QPoint& point);
    QStringList PickObjects(const QRect& rect);

    // 變換操作
    void UpdateGizmo();
    void HandleGizmoInteraction(const QPoint& point);
    void ApplyTransform(const QVector3D& delta);

    // 相機控制
    void UpdateCamera();
    void OrbitCamera(float deltaX, float deltaY);
    void PanCamera(float deltaX, float deltaY);
    void ZoomCamera(float delta);
    void FlyCamera(const QVector3D& direction);

    // 工具方法
    QVector3D ScreenToWorld(const QPoint& screenPoint, float depth = 0.0f);
    QPoint WorldToScreen(const QVector3D& worldPoint);
    QVector3D GetWorldRay(const QPoint& screenPoint);
    float GetDepthAt(const QPoint& screenPoint);

    // 邊界框計算
    QVector3D GetSceneBounds();
    QVector3D GetObjectBounds(const QString& objectId);
    QVector3D GetSelectionBounds();

    // 統計資訊
    void UpdateStatistics();
    void RenderStatisticsText();

private:
    // 渲染器
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<PhysicsEngine> m_physicsEngine;

    // 場景資料
    const PhysicsScene::PhysicsScene* m_scene;

    // 選擇狀態
    QString m_selectedObjectId;
    QStringList m_selectedObjectIds;

    // 視口設定
    ViewportMode m_viewportMode;
    TransformMode m_transformMode;
    ProjectionMode m_projectionMode;

    // 相機
    std::unique_ptr<Camera> m_camera;
    QVector3D m_cameraPosition;
    QVector3D m_cameraTarget;
    QVector3D m_cameraUp;
    float m_cameraDistance;
    float m_cameraFov;
    float m_cameraNear;
    float m_cameraFar;

    // 變換矩陣
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewProjectionMatrix;

    // 滑鼠狀態
    QPoint m_lastMousePos;
    Qt::MouseButton m_mouseButton;
    bool m_mouseDragging;
    bool m_mouseOverGizmo;

    // 鍵盤狀態
    QSet<int> m_pressedKeys;

    // 顯示選項
    bool m_showGrid;
    bool m_showAxes;
    bool m_showBoundingBoxes;
    bool m_showConstraints;
    bool m_showContactPoints;
    bool m_showStatistics;

    // 模擬狀態
    bool m_simulationMode;
    QTimer* m_simulationTimer;
    QElapsedTimer m_frameTimer;

    // 動畫
    QTimer* m_animationTimer;
    float m_animationTime;

    // 效能統計
    int m_frameCount;
    float m_frameTime;
    float m_renderTime;
    int m_triangleCount;
    int m_objectCount;

    // OpenGL 資源
    QOpenGLShaderProgram* m_basicShader;
    QOpenGLShaderProgram* m_wireframeShader;
    QOpenGLShaderProgram* m_debugShader;
    
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_indexBuffer;
    QOpenGLVertexArrayObject m_vertexArray;

    // 輔助渲染器
    std::unique_ptr<Grid> m_grid;
    std::unique_ptr<Gizmo> m_gizmo;
    std::unique_ptr<SelectionRenderer> m_selectionRenderer;

    // 常數
    static const float DEFAULT_CAMERA_DISTANCE;
    static const float DEFAULT_CAMERA_FOV;
    static const float DEFAULT_CAMERA_NEAR;
    static const float DEFAULT_CAMERA_FAR;
    static const float CAMERA_MOVE_SPEED;
    static const float CAMERA_ROTATE_SPEED;
    static const float CAMERA_ZOOM_SPEED;

signals:
    // 選擇信號
    void ObjectSelected(const QString& objectId, const QString& objectType);
    void ObjectDoubleClicked(const QString& objectId, const QString& objectType);
    void SelectionChanged(const QStringList& objectIds);

    // 變換信號
    void ObjectTransformed(const QString& objectId, const QMatrix4x4& transform);
    void ObjectsTransformed(const QStringList& objectIds, const QMatrix4x4& transform);

    // 視圖信號
    void CameraChanged();
    void ViewportModeChanged(ViewportMode mode);
    void TransformModeChanged(TransformMode mode);

    // 互動信號
    void ViewportClicked(const QPoint& position);
    void ViewportDoubleClicked(const QPoint& position);
    void ViewportContextMenu(const QPoint& position);

    // 統計信號
    void StatisticsUpdated(int frameCount, float frameTime, int triangleCount, int objectCount);
};

/**
 * @class Camera
 * @brief 相機類別
 */
class Camera
{
public:
    Camera();
    
    void SetPosition(const QVector3D& position);
    void SetTarget(const QVector3D& target);
    void SetUp(const QVector3D& up);
    
    QVector3D GetPosition() const { return m_position; }
    QVector3D GetTarget() const { return m_target; }
    QVector3D GetUp() const { return m_up; }
    QVector3D GetForward() const;
    QVector3D GetRight() const;
    
    void SetFov(float fov);
    void SetAspectRatio(float aspectRatio);
    void SetNearFar(float nearPlane, float farPlane);
    
    QMatrix4x4 GetViewMatrix() const;
    QMatrix4x4 GetProjectionMatrix() const;
    
    void Orbit(float deltaX, float deltaY);
    void Pan(float deltaX, float deltaY);
    void Zoom(float delta);
    void Fly(const QVector3D& direction, float speed);

private:
    QVector3D m_position;
    QVector3D m_target;
    QVector3D m_up;
    
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
};

/**
 * @class Grid
 * @brief 網格渲染器
 */
class Grid
{
public:
    Grid();
    ~Grid();
    
    void Initialize();
    void Render(const QMatrix4x4& viewProjectionMatrix);
    
    void SetSize(float size);
    void SetSpacing(float spacing);
    void SetColor(const QVector3D& color);

private:
    void CreateGridMesh();

private:
    QOpenGLShaderProgram* m_shader;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLVertexArrayObject m_vertexArray;
    
    float m_size;
    float m_spacing;
    QVector3D m_color;
    int m_vertexCount;
};

/**
 * @class Gizmo
 * @brief 變換操作器
 */
class Gizmo
{
public:
    enum class Mode {
        Move,
        Rotate,
        Scale
    };

    Gizmo();
    ~Gizmo();
    
    void Initialize();
    void Render(const QMatrix4x4& viewProjectionMatrix, const QVector3D& position, Mode mode);
    
    bool HitTest(const QVector3D& rayOrigin, const QVector3D& rayDirection, int& axis);
    QVector3D GetAxisDirection(int axis, Mode mode) const;

private:
    void CreateGizmoMesh();
    void RenderMoveGizmo(const QMatrix4x4& mvpMatrix, const QVector3D& position);
    void RenderRotateGizmo(const QMatrix4x4& mvpMatrix, const QVector3D& position);
    void RenderScaleGizmo(const QMatrix4x4& mvpMatrix, const QVector3D& position);

private:
    QOpenGLShaderProgram* m_shader;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_indexBuffer;
    QOpenGLVertexArrayObject m_vertexArray;
    
    Mode m_currentMode;
    int m_highlightedAxis;
};
