#pragma once

#include "../scene_format/physics_scene_format.h"

/**
 * @file MaterialView.h
 * @brief 材質檢視類別
 * 
 * 提供材質庫的管理功能，包括物理材質和視覺材質的建立、編輯、
 * 預覽和應用。支援材質預設、匯入匯出等進階功能。
 */

class CMaterialView : public CDockablePane
{
	DECLARE_DYNAMIC(CMaterialView)

public:
	CMaterialView() noexcept;
	virtual ~CMaterialView();

// 屬性
protected:
	CTabCtrl m_tabCtrl;
	
	// 物理材質頁面
	CListCtrl m_physicsMatList;
	CImageList m_physicsMatImageList;
	
	// 視覺材質頁面
	CListCtrl m_visualMatList;
	CImageList m_visualMatImageList;
	
	// 材質預覽
	CStatic m_previewCtrl;
	CDC m_previewDC;
	CBitmap m_previewBitmap;
	
	// 當前選擇
	enum MaterialType {
		MATERIAL_PHYSICS,
		MATERIAL_VISUAL
	};
	MaterialType m_currentMaterialType;
	CString m_selectedMaterialName;
	
	// 材質編輯控制項
	CEdit m_nameEdit;
	CButton m_applyBtn;
	CButton m_resetBtn;
	CButton m_deleteBtn;
	CButton m_duplicateBtn;
	CButton m_newBtn;
	
	// 物理材質編輯控制項
	CSliderCtrl m_densitySlider;
	CSliderCtrl m_frictionSlider;
	CSliderCtrl m_restitutionSlider;
	CSliderCtrl m_rollingFrictionSlider;
	CEdit m_densityEdit;
	CEdit m_frictionEdit;
	CEdit m_restitutionEdit;
	CEdit m_rollingFrictionEdit;
	CButton m_kinematicCheck;
	CButton m_staticCheck;
	
	// 視覺材質編輯控制項
	CMFCColorButton m_diffuseColorBtn;
	CMFCColorButton m_specularColorBtn;
	CMFCColorButton m_emissiveColorBtn;
	CSliderCtrl m_shininessSlider;
	CSliderCtrl m_metallicSlider;
	CSliderCtrl m_roughnessSlider;
	CSliderCtrl m_transparencySlider;
	CEdit m_shininessEdit;
	CEdit m_metallicEdit;
	CEdit m_roughnessEdit;
	CEdit m_transparencyEdit;
	
	// 紋理控制項
	CEdit m_diffuseTextureEdit;
	CEdit m_normalTextureEdit;
	CEdit m_specularTextureEdit;
	CEdit m_emissiveTextureEdit;
	CButton m_diffuseTextureBrowseBtn;
	CButton m_normalTextureBrowseBtn;
	CButton m_specularTextureBrowseBtn;
	CButton m_emissiveTextureBrowseBtn;
	
	// 預設材質庫
	std::map<CString, PhysicsScene::PhysicsMaterial> m_defaultPhysicsMaterials;
	std::map<CString, PhysicsScene::VisualMaterial> m_defaultVisualMaterials;

// 作業
public:
	void SetScene(const PhysicsScene::PhysicsScene& scene);
	void RefreshMaterialLists();
	void SelectMaterial(const CString& materialName, MaterialType type);
	void AddPhysicsMaterial(const PhysicsScene::PhysicsMaterial& material);
	void AddVisualMaterial(const PhysicsScene::VisualMaterial& material);
	void UpdateMaterial(const CString& materialName, MaterialType type);
	void RemoveMaterial(const CString& materialName, MaterialType type);

protected:
	void InitializeControls();
	void InitializeTabs();
	void InitializeMaterialLists();
	void InitializePreview();
	void InitializeDefaultMaterials();
	
	// 材質列表管理
	void PopulatePhysicsMaterialList();
	void PopulateVisualMaterialList();
	void AddPhysicsMaterialToList(const PhysicsScene::PhysicsMaterial& material);
	void AddVisualMaterialToList(const PhysicsScene::VisualMaterial& material);
	void UpdateMaterialListItem(const CString& materialName, MaterialType type);
	void RemoveMaterialFromList(const CString& materialName, MaterialType type);
	
	// 材質編輯
	void ShowPhysicsMaterialEditor(const PhysicsScene::PhysicsMaterial& material);
	void ShowVisualMaterialEditor(const PhysicsScene::VisualMaterial& material);
	void HideMaterialEditor();
	void UpdatePhysicsMaterialFromControls();
	void UpdateVisualMaterialFromControls();
	void ResetMaterialEditor();
	
	// 預覽功能
	void UpdateMaterialPreview();
	void RenderPhysicsMaterialPreview(const PhysicsScene::PhysicsMaterial& material);
	void RenderVisualMaterialPreview(const PhysicsScene::VisualMaterial& material);
	void ClearPreview();
	
	// 材質操作
	PhysicsScene::PhysicsMaterial CreateNewPhysicsMaterial();
	PhysicsScene::VisualMaterial CreateNewVisualMaterial();
	PhysicsScene::PhysicsMaterial DuplicatePhysicsMaterial(const CString& sourceName);
	PhysicsScene::VisualMaterial DuplicateVisualMaterial(const CString& sourceName);
	BOOL ValidateMaterialName(const CString& name, const CString& currentName = _T(""));
	CString GenerateUniqueMaterialName(const CString& baseName, MaterialType type);
	
	// 檔案操作
	BOOL ImportMaterials(const CString& filename);
	BOOL ExportMaterials(const CString& filename);
	BOOL LoadMaterialPresets(const CString& filename);
	BOOL SaveMaterialPresets(const CString& filename);
	
	// 紋理處理
	void BrowseTexture(CEdit& textureEdit);
	BOOL ValidateTexturePath(const CString& path);
	void LoadTexturePreview(const CString& texturePath);

// 覆寫
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 實作
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	// 標籤控制項
	afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	
	// 材質列表
	afx_msg void OnPhysicsMatListSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVisualMatListSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPhysicsMatListDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVisualMatListDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPhysicsMatListRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVisualMatListRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPhysicsMatListKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVisualMatListKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	
	// 按鈕事件
	afx_msg void OnNewMaterial();
	afx_msg void OnDuplicateMaterial();
	afx_msg void OnDeleteMaterial();
	afx_msg void OnApplyMaterial();
	afx_msg void OnResetMaterial();
	
	// 物理材質控制項
	afx_msg void OnDensitySliderChange();
	afx_msg void OnFrictionSliderChange();
	afx_msg void OnRestitutionSliderChange();
	afx_msg void OnRollingFrictionSliderChange();
	afx_msg void OnDensityEditChange();
	afx_msg void OnFrictionEditChange();
	afx_msg void OnRestitutionEditChange();
	afx_msg void OnRollingFrictionEditChange();
	afx_msg void OnKinematicCheck();
	afx_msg void OnStaticCheck();
	
	// 視覺材質控制項
	afx_msg void OnDiffuseColorChange();
	afx_msg void OnSpecularColorChange();
	afx_msg void OnEmissiveColorChange();
	afx_msg void OnShininessSliderChange();
	afx_msg void OnMetallicSliderChange();
	afx_msg void OnRoughnessSliderChange();
	afx_msg void OnTransparencySliderChange();
	afx_msg void OnShininessEditChange();
	afx_msg void OnMetallicEditChange();
	afx_msg void OnRoughnessEditChange();
	afx_msg void OnTransparencyEditChange();
	
	// 紋理按鈕
	afx_msg void OnBrowseDiffuseTexture();
	afx_msg void OnBrowseNormalTexture();
	afx_msg void OnBrowseSpecularTexture();
	afx_msg void OnBrowseEmissiveTexture();
	afx_msg void OnClearDiffuseTexture();
	afx_msg void OnClearNormalTexture();
	afx_msg void OnClearSpecularTexture();
	afx_msg void OnClearEmissiveTexture();
	
	// 右鍵選單
	afx_msg void OnContextNewMaterial();
	afx_msg void OnContextDuplicateMaterial();
	afx_msg void OnContextRenameMaterial();
	afx_msg void OnContextDeleteMaterial();
	afx_msg void OnContextImportMaterials();
	afx_msg void OnContextExportMaterials();
	afx_msg void OnContextLoadPresets();
	afx_msg void OnContextSavePresets();
	afx_msg void OnContextResetToDefaults();
	
	// 命令更新
	afx_msg void OnUpdateNewMaterial(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDuplicateMaterial(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteMaterial(CCmdUI* pCmdUI);
	afx_msg void OnUpdateApplyMaterial(CCmdUI* pCmdUI);
	afx_msg void OnUpdateResetMaterial(CCmdUI* pCmdUI);

private:
	// 常數定義
	static const int TAB_PHYSICS_MATERIALS = 0;
	static const int TAB_VISUAL_MATERIALS = 1;
	
	static const int PREVIEW_WIDTH = 128;
	static const int PREVIEW_HEIGHT = 128;
	
	// 列表欄位
	enum PhysicsMatColumns {
		PHYS_COL_NAME = 0,
		PHYS_COL_DENSITY,
		PHYS_COL_FRICTION,
		PHYS_COL_RESTITUTION,
		PHYS_COL_TYPE
	};
	
	enum VisualMatColumns {
		VIS_COL_NAME = 0,
		VIS_COL_DIFFUSE,
		VIS_COL_SHININESS,
		VIS_COL_METALLIC,
		VIS_COL_ROUGHNESS
	};
	
	// 輔助函數
	void LayoutControls();
	void ShowMaterialEditor(BOOL show);
	void EnableMaterialControls(BOOL enable);
	void SyncSliderAndEdit(CSliderCtrl& slider, CEdit& edit, float value, float minVal, float maxVal);
	void UpdateSliderFromEdit(CSliderCtrl& slider, CEdit& edit, float minVal, float maxVal);
	void UpdateEditFromSlider(CSliderCtrl& slider, CEdit& edit, float minVal, float maxVal);
	
	COLORREF ColorToColorRef(const PhysicsScene::Color& color);
	PhysicsScene::Color ColorRefToColor(COLORREF colorRef);
	CString FormatFloat(float value, int precision = 2);
	float ParseFloat(const CString& text, float defaultValue = 0.0f);
	
	// 材質查找
	PhysicsScene::PhysicsMaterial* FindPhysicsMaterial(const CString& name);
	PhysicsScene::VisualMaterial* FindVisualMaterial(const CString& name);
	int FindMaterialListIndex(const CString& name, MaterialType type);
	
	// 通知系統
	void NotifyMaterialChanged(const CString& materialName, MaterialType type);
	void NotifyMaterialCreated(const CString& materialName, MaterialType type);
	void NotifyMaterialDeleted(const CString& materialName, MaterialType type);
	void NotifyMaterialRenamed(const CString& oldName, const CString& newName, MaterialType type);

public:
	// 公用介面
	void CreateNewPhysicsMaterial(const CString& name = _T(""));
	void CreateNewVisualMaterial(const CString& name = _T(""));
	BOOL RenameMaterial(const CString& oldName, const CString& newName, MaterialType type);
	BOOL DeleteMaterial(const CString& materialName, MaterialType type);
	
	// 材質存取
	PhysicsScene::PhysicsMaterial GetPhysicsMaterial(const CString& name);
	PhysicsScene::VisualMaterial GetVisualMaterial(const CString& name);
	BOOL SetPhysicsMaterial(const CString& name, const PhysicsScene::PhysicsMaterial& material);
	BOOL SetVisualMaterial(const CString& name, const PhysicsScene::VisualMaterial& material);
	
	// 材質應用
	void ApplyMaterialToSelection(const CString& materialName, MaterialType type);
	CStringArray GetMaterialNames(MaterialType type);
	
	// 預設材質
	void LoadDefaultMaterials();
	void ResetToDefaultMaterials();
	PhysicsScene::PhysicsMaterial GetDefaultPhysicsMaterial(const CString& name);
	PhysicsScene::VisualMaterial GetDefaultVisualMaterial(const CString& name);
	
	// 選擇管理
	CString GetSelectedMaterialName() const { return m_selectedMaterialName; }
	MaterialType GetCurrentMaterialType() const { return m_currentMaterialType; }
	void SetSelectedMaterial(const CString& materialName, MaterialType type);
	void ClearSelection();
};
