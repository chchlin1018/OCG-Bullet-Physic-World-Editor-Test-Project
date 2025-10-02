#pragma once

#include "../scene_format/physics_scene_format.h"
#include <gl/GL.h>
#include <gl/GLU.h>

/**
 * @file ViewportView.h
 * @brief 3D 視口檢視類別
 * 
 * 提供即時的 3D 場景預覽和編輯功能，支援 OpenGL 渲染、
 * 相機控制、物件選擇、變換操作等。
 */

class CViewportView : public CDockablePane
{
	DECLARE_DYNAMIC(CViewportView)

public:
	CViewportView() noexcept;
	virtual ~CViewportView();

// 屬性
protected:
	// OpenGL 相關
	CDC* m_pDC;
	HGLRC m_hRC;
	BOOL m_bOpenGLInitialized;

	// 視口設定
	int m_viewportWidth;
	int m_viewportHeight;
	float m_aspectRatio;

	// 相機設定
	struct Camera {
		PhysicsScene::Vector3 position;
		PhysicsScene::Vector3 target;
		PhysicsScene::Vector3 up;
		float fov;
		float nearPlane;
		float farPlane;
		BOOL isOrthographic;
		float orthographicSize;
		
		Camera() : position(0.0f, 5.0f, 10.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f),
				   fov(45.0f), nearPlane(0.1f), farPlane(1000.0f), isOrthographic(FALSE), orthographicSize(10.0f) {}
	} m_camera;

	// 相機控制
	BOOL m_bCameraDragging;
	CPoint m_lastMousePos;
	float m_cameraDistance;
	float m_cameraYaw;
	float m_cameraPitch;
	float m_cameraSpeed;
	float m_mouseSensitivity;

	// 渲染設定
	enum RenderMode {
		RENDER_WIREFRAME,
		RENDER_SOLID,
		RENDER_TEXTURED
	};
	RenderMode m_renderMode;

	BOOL m_bShowGrid;
	BOOL m_bShowAxes;
	BOOL m_bShowBoundingBoxes;
	BOOL m_bShowContactPoints;
	BOOL m_bShowConstraints;
	BOOL m_bEnableLighting;
	BOOL m_bEnableTextures;
	BOOL m_bEnableShadows;

	// 選擇和變換
	CStringArray m_selectedObjects;
	CString m_activeObject;
	BOOL m_bSelectionMode;
	
	enum TransformMode {
		TRANSFORM_NONE,
		TRANSFORM_TRANSLATE,
		TRANSFORM_ROTATE,
		TRANSFORM_SCALE
	};
	TransformMode m_transformMode;

	// 變換 Gizmo
	struct Gizmo {
		BOOL visible;
		PhysicsScene::Vector3 position;
		float size;
		int selectedAxis; // 0=無, 1=X, 2=Y, 3=Z
		
		Gizmo() : visible(FALSE), position(0.0f, 0.0f, 0.0f), size(1.0f), selectedAxis(0) {}
	} m_gizmo;

	// 網格設定
	struct Grid {
		BOOL visible;
		float size;
		int divisions;
		PhysicsScene::Color color;
		
		Grid() : visible(TRUE), size(10.0f), divisions(20), color(0.5f, 0.5f, 0.5f, 1.0f) {}
	} m_grid;

	// 效能監控
	DWORD m_lastFrameTime;
	float m_fps;
	int m_frameCount;
	DWORD m_fpsUpdateTime;

// 作業
public:
	void SetScene(const PhysicsScene::PhysicsScene& scene);
	void RefreshView();
	void ResetCamera();
	void FocusOnObject(const CString& objectName);
	void FocusOnSelection();
	void SetRenderMode(RenderMode mode);
	void SetSelectedObjects(const CStringArray& objects);
	void SetActiveObject(const CString& objectName);

	// 相機控制
	void SetCameraPosition(const PhysicsScene::Vector3& position);
	void SetCameraTarget(const PhysicsScene::Vector3& target);
	void SetCameraFOV(float fov);
	void SetOrthographicMode(BOOL orthographic);

	// 顯示選項
	void ShowGrid(BOOL show);
	void ShowAxes(BOOL show);
	void ShowBoundingBoxes(BOOL show);
	void ShowContactPoints(BOOL show);
	void ShowConstraints(BOOL show);
	void EnableLighting(BOOL enable);
	void EnableTextures(BOOL enable);
	void EnableShadows(BOOL enable);

protected:
	// OpenGL 初始化和清理
	BOOL InitializeOpenGL();
	void CleanupOpenGL();
	BOOL SetupPixelFormat();
	void InitializeRenderState();

	// 渲染函數
	void RenderScene();
	void RenderGrid();
	void RenderAxes();
	void RenderRigidBodies();
	void RenderConstraints();
	void RenderForceFields();
	void RenderLights();
	void RenderCameras();
	void RenderGizmo();
	void RenderSelectionOutline();
	void RenderBoundingBoxes();
	void RenderContactPoints();

	// 幾何渲染
	void RenderBox(const PhysicsScene::Vector3& halfExtents);
	void RenderSphere(float radius, int segments = 16);
	void RenderCylinder(float radius, float height, int segments = 16);
	void RenderCapsule(float radius, float height, int segments = 16);
	void RenderCone(float radius, float height, int segments = 16);
	void RenderPlane(float width, float depth);
	void RenderMesh(const std::vector<PhysicsScene::Vector3>& vertices, 
					const std::vector<std::array<int, 3>>& triangles);

	// 材質和光照
	void SetMaterial(const PhysicsScene::VisualMaterial& material);
	void SetupLighting();
	void SetupLight(const PhysicsScene::Light& light, int lightIndex);

	// 相機和投影
	void UpdateCamera();
	void SetupProjection();
	void SetupModelView();

	// 選擇和拾取
	void HandleMousePicking(CPoint point);
	CString PickObject(CPoint point);
	BOOL IsPointInObject(CPoint screenPoint, const CString& objectName);
	PhysicsScene::Vector3 ScreenToWorld(CPoint screenPoint, float depth = 0.0f);
	CPoint WorldToScreen(const PhysicsScene::Vector3& worldPoint);

	// 變換操作
	void UpdateGizmo();
	void RenderTranslateGizmo();
	void RenderRotateGizmo();
	void RenderScaleGizmo();
	int PickGizmoAxis(CPoint point);
	void HandleGizmoTransform(CPoint currentPos, CPoint lastPos);

	// 相機控制
	void HandleCameraRotation(CPoint delta);
	void HandleCameraPanning(CPoint delta);
	void HandleCameraZoom(float delta);
	void UpdateCameraFromSpherical();

	// 輔助函數
	void SetColor(const PhysicsScene::Color& color);
	void DrawString(const CString& text, const PhysicsScene::Vector3& position);
	void CalculateFPS();
	PhysicsScene::Vector3 GetObjectCenter(const CString& objectName);
	PhysicsScene::Vector3 GetSelectionCenter();

// 覆寫
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 實作
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	// 滑鼠和鍵盤輸入
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	// 右鍵選單
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnViewWireframe();
	afx_msg void OnViewSolid();
	afx_msg void OnViewTextured();
	afx_msg void OnViewGrid();
	afx_msg void OnViewAxes();
	afx_msg void OnViewBoundingBoxes();
	afx_msg void OnViewContactPoints();
	afx_msg void OnViewConstraints();
	afx_msg void OnViewLighting();
	afx_msg void OnViewTextures();
	afx_msg void OnViewShadows();
	afx_msg void OnViewResetCamera();
	afx_msg void OnViewFocusSelection();
	afx_msg void OnViewOrthographic();
	afx_msg void OnViewPerspective();

	// 變換工具
	afx_msg void OnTransformSelect();
	afx_msg void OnTransformTranslate();
	afx_msg void OnTransformRotate();
	afx_msg void OnTransformScale();

	// 命令更新
	afx_msg void OnUpdateViewWireframe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSolid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTextured(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewGrid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewAxes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewBoundingBoxes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewContactPoints(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewConstraints(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewLighting(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTextures(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewShadows(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewOrthographic(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewPerspective(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransformSelect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransformTranslate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransformRotate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTransformScale(CCmdUI* pCmdUI);

private:
	// 常數定義
	static const UINT TIMER_RENDER = 1;
	static const UINT TIMER_FPS = 2;
	static const int RENDER_INTERVAL = 16; // ~60 FPS
	static const int FPS_UPDATE_INTERVAL = 1000; // 1 秒

	// 顏色定義
	static const PhysicsScene::Color COLOR_GRID;
	static const PhysicsScene::Color COLOR_AXES_X;
	static const PhysicsScene::Color COLOR_AXES_Y;
	static const PhysicsScene::Color COLOR_AXES_Z;
	static const PhysicsScene::Color COLOR_SELECTION;
	static const PhysicsScene::Color COLOR_GIZMO_X;
	static const PhysicsScene::Color COLOR_GIZMO_Y;
	static const PhysicsScene::Color COLOR_GIZMO_Z;

	// 場景資料快取
	const PhysicsScene::PhysicsScene* m_pScene;
	BOOL m_bSceneChanged;

	// 渲染快取
	struct RenderCache {
		GLuint gridDisplayList;
		GLuint axesDisplayList;
		GLuint sphereDisplayList;
		GLuint cylinderDisplayList;
		BOOL valid;
		
		RenderCache() : gridDisplayList(0), axesDisplayList(0), 
						sphereDisplayList(0), cylinderDisplayList(0), valid(FALSE) {}
	} m_renderCache;

	// 輔助函數
	void InvalidateRenderCache();
	void UpdateRenderCache();
	void CreateGridDisplayList();
	void CreateAxesDisplayList();
	void CreateSphereDisplayList();
	void CreateCylinderDisplayList();

	// 通知系統
	void NotifySelectionChanged();
	void NotifyObjectTransformed(const CString& objectName);
	void NotifyCameraChanged();

public:
	// 公用介面
	RenderMode GetRenderMode() const { return m_renderMode; }
	BOOL IsGridVisible() const { return m_bShowGrid; }
	BOOL IsAxesVisible() const { return m_bShowAxes; }
	BOOL IsLightingEnabled() const { return m_bEnableLighting; }
	
	const Camera& GetCamera() const { return m_camera; }
	void SetCamera(const Camera& camera);
	
	TransformMode GetTransformMode() const { return m_transformMode; }
	void SetTransformMode(TransformMode mode);
	
	const CStringArray& GetSelectedObjects() const { return m_selectedObjects; }
	const CString& GetActiveObject() const { return m_activeObject; }
	
	// 截圖功能
	BOOL SaveScreenshot(const CString& filename, int width = 0, int height = 0);
	
	// 效能資訊
	float GetFPS() const { return m_fps; }
	int GetTriangleCount() const;
	int GetVertexCount() const;
};
