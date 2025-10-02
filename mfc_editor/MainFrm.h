#pragma once

#include "SceneTreeView.h"
#include "PropertyView.h"
#include "ViewportView.h"
#include "MaterialView.h"
#include "OutputView.h"
#include "../scene_format/physics_scene_format.h"

/**
 * @file MainFrm.h
 * @brief 主框架視窗類別
 * 
 * 管理整個應用程式的主視窗，包含功能表列、工具列、狀態列
 * 以及多個停靠視窗面板。
 */

class CMainFrame : public CFrameWndEx
{
protected: // 僅從序列化建立
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 屬性
public:

// 作業
public:

// 覆寫
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

// 實作
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控制列內嵌成員
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

	// 停靠視窗
	CSceneTreeView*   m_pSceneTreeView;
	CPropertyView*    m_pPropertyView;
	CViewportView*    m_pViewportView;
	CMaterialView*    m_pMaterialView;
	COutputView*      m_pOutputView;

	// 停靠窗格
	CDockablePane     m_wndSceneTree;
	CDockablePane     m_wndProperties;
	CDockablePane     m_wndViewport;
	CDockablePane     m_wndMaterials;
	CDockablePane     m_wndOutput;

	// 場景資料
	PhysicsScene::PhysicsScene m_scene;
	CString m_currentFilePath;
	BOOL m_bModified;

// 產生的訊息對應函式
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnClose();
	afx_msg void OnDestroy();

	// 檔案操作
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileImport();
	afx_msg void OnFileExport();
	afx_msg void OnFileRecentFile(UINT nID);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);

	// 編輯操作
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditDelete();
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);

	// 物件建立
	afx_msg void OnCreateRigidBody();
	afx_msg void OnCreateConstraint();
	afx_msg void OnCreateForceField();
	afx_msg void OnCreateLight();
	afx_msg void OnCreateCamera();
	afx_msg void OnCreateBoxShape();
	afx_msg void OnCreateSphereShape();
	afx_msg void OnCreateCylinderShape();
	afx_msg void OnCreateCapsuleShape();
	afx_msg void OnCreatePlaneShape();

	// 視圖操作
	afx_msg void OnViewSceneTree();
	afx_msg void OnViewProperties();
	afx_msg void OnViewMaterials();
	afx_msg void OnViewOutput();
	afx_msg void OnViewGrid();
	afx_msg void OnViewAxes();
	afx_msg void OnViewWireframe();
	afx_msg void OnViewSolid();
	afx_msg void OnViewTextured();
	afx_msg void OnViewResetCamera();
	afx_msg void OnUpdateViewSceneTree(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewMaterials(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewGrid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewAxes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewWireframe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSolid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTextured(CCmdUI* pCmdUI);

	// 模擬操作
	afx_msg void OnSimulationPlay();
	afx_msg void OnSimulationPause();
	afx_msg void OnSimulationStop();
	afx_msg void OnSimulationStep();
	afx_msg void OnSimulationReset();
	afx_msg void OnSimulationSettings();
	afx_msg void OnUpdateSimulationPlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSimulationPause(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSimulationStop(CCmdUI* pCmdUI);

	// 工具操作
	afx_msg void OnToolsOptions();
	afx_msg void OnToolsValidateScene();
	afx_msg void OnToolsOptimizeScene();
	afx_msg void OnToolsStatistics();

	DECLARE_MESSAGE_MAP()

private:
	// 初始化函數
	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateSceneTreePane();
	BOOL CreatePropertiesPane();
	BOOL CreateViewportPane();
	BOOL CreateMaterialsPane();
	BOOL CreateOutputPane();

	// 場景管理
	void NewScene();
	BOOL OpenScene(const CString& filePath);
	BOOL SaveScene(const CString& filePath);
	BOOL SaveSceneAs();
	void UpdateTitle();
	void SetModified(BOOL bModified = TRUE);
	BOOL PromptSaveChanges();

	// UI 更新
	void UpdateAllViews();
	void UpdateStatusBar();
	void RefreshViewport();

	// 物件操作
	void SelectObject(const CString& objectName);
	void DeleteSelectedObjects();
	CString GenerateUniqueObjectName(const CString& baseName);

	// 模擬狀態
	enum SimulationState {
		SIMULATION_STOPPED,
		SIMULATION_PLAYING,
		SIMULATION_PAUSED
	};
	SimulationState m_simulationState;
	UINT_PTR m_simulationTimer;

	// 選擇管理
	CStringArray m_selectedObjects;
	CString m_activeObject;

	// 視圖模式
	enum ViewMode {
		VIEW_WIREFRAME,
		VIEW_SOLID,
		VIEW_TEXTURED
	};
	ViewMode m_viewMode;

	// 顯示選項
	BOOL m_bShowGrid;
	BOOL m_bShowAxes;
	BOOL m_bShowBoundingBoxes;
	BOOL m_bShowContactPoints;

public:
	// 公用介面
	PhysicsScene::PhysicsScene& GetScene() { return m_scene; }
	const PhysicsScene::PhysicsScene& GetScene() const { return m_scene; }
	
	void SetScene(const PhysicsScene::PhysicsScene& scene);
	BOOL IsModified() const { return m_bModified; }
	const CString& GetCurrentFilePath() const { return m_currentFilePath; }
	
	// 選擇管理
	void SetSelectedObjects(const CStringArray& objects);
	const CStringArray& GetSelectedObjects() const { return m_selectedObjects; }
	void SetActiveObject(const CString& objectName);
	const CString& GetActiveObject() const { return m_activeObject; }
	
	// 視圖設定
	ViewMode GetViewMode() const { return m_viewMode; }
	void SetViewMode(ViewMode mode);
	BOOL IsGridVisible() const { return m_bShowGrid; }
	BOOL IsAxesVisible() const { return m_bShowAxes; }
	
	// 模擬控制
	SimulationState GetSimulationState() const { return m_simulationState; }
	void StartSimulation();
	void PauseSimulation();
	void StopSimulation();
	void StepSimulation();
	void ResetSimulation();
	
	// 通知系統
	void NotifySceneChanged();
	void NotifyObjectSelected(const CString& objectName);
	void NotifyObjectModified(const CString& objectName);
	void NotifyMaterialChanged(const CString& materialName);
	
	// 視窗存取
	CSceneTreeView* GetSceneTreeView() const { return m_pSceneTreeView; }
	CPropertyView* GetPropertyView() const { return m_pPropertyView; }
	CViewportView* GetViewportView() const { return m_pViewportView; }
	CMaterialView* GetMaterialView() const { return m_pMaterialView; }
	COutputView* GetOutputView() const { return m_pOutputView; }
};
