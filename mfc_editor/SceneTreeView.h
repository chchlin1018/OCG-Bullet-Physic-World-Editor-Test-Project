#pragma once

#include "../scene_format/physics_scene_format.h"

/**
 * @file SceneTreeView.h
 * @brief 場景樹狀檢視類別
 * 
 * 提供階層式的場景物件檢視，允許使用者瀏覽、選擇和管理場景中的所有物件。
 * 支援拖放操作、右鍵選單、物件重命名等功能。
 */

class CSceneTreeView : public CDockablePane
{
	DECLARE_DYNAMIC(CSceneTreeView)

public:
	CSceneTreeView() noexcept;
	virtual ~CSceneTreeView();

// 屬性
protected:
	CTreeCtrl m_wndTree;
	CImageList m_imageList;

	// 樹狀節點類型
	enum NodeType {
		NODE_SCENE_ROOT,
		NODE_RIGID_BODIES,
		NODE_RIGID_BODY,
		NODE_CONSTRAINTS,
		NODE_CONSTRAINT,
		NODE_FORCE_FIELDS,
		NODE_FORCE_FIELD,
		NODE_LIGHTS,
		NODE_LIGHT,
		NODE_CAMERAS,
		NODE_CAMERA,
		NODE_MATERIALS,
		NODE_PHYSICS_MATERIALS,
		NODE_PHYSICS_MATERIAL,
		NODE_VISUAL_MATERIALS,
		NODE_VISUAL_MATERIAL
	};

	// 節點資料結構
	struct NodeData {
		NodeType type;
		CString objectName;
		BOOL isExpanded;
		
		NodeData(NodeType t, const CString& name = _T("")) 
			: type(t), objectName(name), isExpanded(FALSE) {}
	};

	// 根節點
	HTREEITEM m_hRootItem;
	HTREEITEM m_hRigidBodiesItem;
	HTREEITEM m_hConstraintsItem;
	HTREEITEM m_hForceFieldsItem;
	HTREEITEM m_hLightsItem;
	HTREEITEM m_hCamerasItem;
	HTREEITEM m_hMaterialsItem;
	HTREEITEM m_hPhysicsMaterialsItem;
	HTREEITEM m_hVisualMaterialsItem;

	// 拖放支援
	BOOL m_bDragging;
	HTREEITEM m_hDragItem;
	CImageList* m_pDragImage;
	HTREEITEM m_hDropTarget;

// 作業
public:
	void RefreshTree();
	void RefreshTree(const PhysicsScene::PhysicsScene& scene);
	void SelectObject(const CString& objectName);
	void ExpandAll();
	void CollapseAll();
	void SetScene(const PhysicsScene::PhysicsScene& scene);

protected:
	void InitializeImageList();
	void CreateRootNodes();
	void PopulateRigidBodies(const PhysicsScene::PhysicsScene& scene);
	void PopulateConstraints(const PhysicsScene::PhysicsScene& scene);
	void PopulateForceFields(const PhysicsScene::PhysicsScene& scene);
	void PopulateLights(const PhysicsScene::PhysicsScene& scene);
	void PopulateCameras(const PhysicsScene::PhysicsScene& scene);
	void PopulateMaterials(const PhysicsScene::PhysicsScene& scene);

	HTREEITEM AddTreeItem(HTREEITEM hParent, const CString& text, NodeType type, 
						  const CString& objectName = _T(""), int imageIndex = 0);
	NodeData* GetNodeData(HTREEITEM hItem);
	HTREEITEM FindObjectItem(const CString& objectName, NodeType type);
	void DeleteAllChildren(HTREEITEM hParent);

	// 拖放操作
	void BeginDrag(HTREEITEM hItem, CPoint point);
	void DragMove(CPoint point);
	void EndDrag(CPoint point);
	BOOL CanDropOn(HTREEITEM hDropTarget, HTREEITEM hDragItem);
	void PerformDrop(HTREEITEM hDropTarget, HTREEITEM hDragItem);

	// 右鍵選單
	void ShowContextMenu(CPoint point, HTREEITEM hItem);
	CMenu* CreateContextMenu(NodeType nodeType);

// 覆寫
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 實作
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	// 樹狀控制項通知
	afx_msg void OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeKeyDown(NMHDR* pNMHDR, LRESULT* pResult);

	// 滑鼠訊息（用於拖放）
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);

	// 右鍵選單命令
	afx_msg void OnCreateRigidBody();
	afx_msg void OnCreateConstraint();
	afx_msg void OnCreateForceField();
	afx_msg void OnCreateLight();
	afx_msg void OnCreateCamera();
	afx_msg void OnCreatePhysicsMaterial();
	afx_msg void OnCreateVisualMaterial();
	afx_msg void OnRename();
	afx_msg void OnDelete();
	afx_msg void OnDuplicate();
	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnProperties();
	afx_msg void OnExpandAll();
	afx_msg void OnCollapseAll();

	// 命令更新
	afx_msg void OnUpdateRename(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDuplicate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);

private:
	// 圖示索引
	enum IconIndex {
		ICON_SCENE = 0,
		ICON_FOLDER,
		ICON_RIGID_BODY,
		ICON_CONSTRAINT,
		ICON_FORCE_FIELD,
		ICON_LIGHT,
		ICON_CAMERA,
		ICON_MATERIAL,
		ICON_PHYSICS_MATERIAL,
		ICON_VISUAL_MATERIAL,
		ICON_BOX,
		ICON_SPHERE,
		ICON_CYLINDER,
		ICON_CAPSULE,
		ICON_PLANE,
		ICON_MESH
	};

	// 輔助函數
	CString GetObjectDisplayName(const CString& objectName, NodeType type);
	int GetIconIndex(NodeType type, const CString& objectName = _T(""));
	BOOL IsObjectNode(NodeType type);
	BOOL CanRename(NodeType type);
	BOOL CanDelete(NodeType type);
	BOOL CanDuplicate(NodeType type);

	// 剪貼簿支援
	struct ClipboardData {
		NodeType type;
		CString objectName;
		CString jsonData;
	};
	static ClipboardData s_clipboardData;
	static BOOL s_hasClipboardData;

	void CopyToClipboard(HTREEITEM hItem);
	BOOL CanPasteFrom(NodeType targetType);
	void PasteFromClipboard(HTREEITEM hTargetItem);

	// 物件建立輔助
	void CreateNewRigidBody(HTREEITEM hParent);
	void CreateNewConstraint(HTREEITEM hParent);
	void CreateNewForceField(HTREEITEM hParent);
	void CreateNewLight(HTREEITEM hParent);
	void CreateNewCamera(HTREEITEM hParent);
	void CreateNewPhysicsMaterial(HTREEITEM hParent);
	void CreateNewVisualMaterial(HTREEITEM hParent);

	// 物件操作輔助
	BOOL RenameObject(HTREEITEM hItem, const CString& newName);
	BOOL DeleteObject(HTREEITEM hItem);
	BOOL DuplicateObject(HTREEITEM hItem);

	// 通知主框架
	void NotifySelectionChanged(const CString& objectName, NodeType type);
	void NotifyObjectRenamed(const CString& oldName, const CString& newName, NodeType type);
	void NotifyObjectDeleted(const CString& objectName, NodeType type);
	void NotifyObjectCreated(const CString& objectName, NodeType type);

public:
	// 公用介面
	void AddRigidBody(const PhysicsScene::RigidBody& rigidBody);
	void AddConstraint(const PhysicsScene::Constraint& constraint);
	void AddForceField(const PhysicsScene::ForceField& forceField);
	void AddLight(const PhysicsScene::Light& light);
	void AddCamera(const PhysicsScene::Camera& camera);
	void AddPhysicsMaterial(const PhysicsScene::PhysicsMaterial& material);
	void AddVisualMaterial(const PhysicsScene::VisualMaterial& material);

	void RemoveObject(const CString& objectName);
	void RenameObject(const CString& oldName, const CString& newName);
	void UpdateObject(const CString& objectName);

	// 選擇管理
	CStringArray GetSelectedObjects();
	void SetSelectedObjects(const CStringArray& objects);
	void ClearSelection();

	// 展開/摺疊狀態管理
	void SaveExpansionState();
	void RestoreExpansionState();
};
