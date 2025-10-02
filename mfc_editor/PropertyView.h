#pragma once

#include "../scene_format/physics_scene_format.h"

/**
 * @file PropertyView.h
 * @brief 屬性檢視類別
 * 
 * 提供所選物件的詳細屬性編輯功能，支援即時修改和驗證。
 * 使用屬性網格控制項來顯示和編輯各種類型的屬性。
 */

class CPropertyView : public CDockablePane
{
	DECLARE_DYNAMIC(CPropertyView)

public:
	CPropertyView() noexcept;
	virtual ~CPropertyView();

// 屬性
protected:
	CMFCPropertyGridCtrl m_wndPropList;
	CFont m_fntPropList;

	// 當前編輯的物件
	enum ObjectType {
		OBJECT_NONE,
		OBJECT_RIGID_BODY,
		OBJECT_CONSTRAINT,
		OBJECT_FORCE_FIELD,
		OBJECT_LIGHT,
		OBJECT_CAMERA,
		OBJECT_PHYSICS_MATERIAL,
		OBJECT_VISUAL_MATERIAL,
		OBJECT_SIMULATION_SETTINGS,
		OBJECT_RENDER_SETTINGS
	};

	ObjectType m_currentObjectType;
	CString m_currentObjectName;

	// 屬性群組
	CMFCPropertyGridProperty* m_pTransformGroup;
	CMFCPropertyGridProperty* m_pPhysicsGroup;
	CMFCPropertyGridProperty* m_pRenderGroup;
	CMFCPropertyGridProperty* m_pMaterialGroup;
	CMFCPropertyGridProperty* m_pConstraintGroup;
	CMFCPropertyGridProperty* m_pLightGroup;
	CMFCPropertyGridProperty* m_pCameraGroup;
	CMFCPropertyGridProperty* m_pSimulationGroup;

// 作業
public:
	void ShowProperties(const CString& objectName, const CString& objectType);
	void ShowRigidBodyProperties(const PhysicsScene::RigidBody& rigidBody);
	void ShowConstraintProperties(const PhysicsScene::Constraint& constraint);
	void ShowForceFieldProperties(const PhysicsScene::ForceField& forceField);
	void ShowLightProperties(const PhysicsScene::Light& light);
	void ShowCameraProperties(const PhysicsScene::Camera& camera);
	void ShowPhysicsMaterialProperties(const PhysicsScene::PhysicsMaterial& material);
	void ShowVisualMaterialProperties(const PhysicsScene::VisualMaterial& material);
	void ShowSimulationSettings(const PhysicsScene::SimulationSettings& settings);
	void ShowRenderSettings(const PhysicsScene::RenderSettings& settings);
	void ClearProperties();
	void RefreshProperties();

protected:
	void InitPropList();
	void SetPropListFont();

	// 屬性建立輔助函數
	CMFCPropertyGridProperty* CreateTransformGroup(const PhysicsScene::Transform& transform);
	CMFCPropertyGridProperty* CreateVector3Property(const CString& name, const PhysicsScene::Vector3& vector, BOOL editable = TRUE);
	CMFCPropertyGridProperty* CreateQuaternionProperty(const CString& name, const PhysicsScene::Quaternion& quaternion, BOOL editable = TRUE);
	CMFCPropertyGridProperty* CreateColorProperty(const CString& name, const PhysicsScene::Color& color, BOOL editable = TRUE);
	CMFCPropertyGridProperty* CreateFloatProperty(const CString& name, float value, BOOL editable = TRUE, float minVal = -FLT_MAX, float maxVal = FLT_MAX);
	CMFCPropertyGridProperty* CreateIntProperty(const CString& name, int value, BOOL editable = TRUE, int minVal = INT_MIN, int maxVal = INT_MAX);
	CMFCPropertyGridProperty* CreateBoolProperty(const CString& name, BOOL value, BOOL editable = TRUE);
	CMFCPropertyGridProperty* CreateStringProperty(const CString& name, const CString& value, BOOL editable = TRUE);
	CMFCPropertyGridProperty* CreateComboProperty(const CString& name, const CString& value, const CStringArray& options, BOOL editable = TRUE);
	CMFCPropertyGridProperty* CreateFileProperty(const CString& name, const CString& value, const CString& filter, BOOL editable = TRUE);

	// 特殊屬性建立
	CMFCPropertyGridProperty* CreateShapeTypeProperty(PhysicsScene::ShapeType shapeType);
	CMFCPropertyGridProperty* CreateConstraintTypeProperty(PhysicsScene::ConstraintType constraintType);
	CMFCPropertyGridProperty* CreateForceFieldTypeProperty(PhysicsScene::ForceFieldType forceFieldType);
	CMFCPropertyGridProperty* CreateLightTypeProperty(PhysicsScene::LightType lightType);

	// 屬性更新
	void UpdateRigidBodyFromProperties();
	void UpdateConstraintFromProperties();
	void UpdateForceFieldFromProperties();
	void UpdateLightFromProperties();
	void UpdateCameraFromProperties();
	void UpdatePhysicsMaterialFromProperties();
	void UpdateVisualMaterialFromProperties();
	void UpdateSimulationSettingsFromProperties();
	void UpdateRenderSettingsFromProperties();

	// 屬性驗證
	BOOL ValidateProperty(CMFCPropertyGridProperty* pProp, const COleVariant& newValue);
	BOOL ValidateFloatRange(float value, float minVal, float maxVal, const CString& propertyName);
	BOOL ValidateIntRange(int value, int minVal, int maxVal, const CString& propertyName);
	BOOL ValidateObjectName(const CString& name, const CString& currentName);
	BOOL ValidateFilePath(const CString& path);

	// 值轉換輔助
	PhysicsScene::Vector3 VariantToVector3(const COleVariant& var);
	PhysicsScene::Quaternion VariantToQuaternion(const COleVariant& var);
	PhysicsScene::Color VariantToColor(const COleVariant& var);
	COleVariant Vector3ToVariant(const PhysicsScene::Vector3& vector);
	COleVariant QuaternionToVariant(const PhysicsScene::Quaternion& quaternion);
	COleVariant ColorToVariant(const PhysicsScene::Color& color);

	// 屬性查找
	CMFCPropertyGridProperty* FindProperty(const CString& name, CMFCPropertyGridProperty* pParent = nullptr);
	CMFCPropertyGridProperty* FindPropertyByData(DWORD_PTR data);

// 覆寫
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

// 實作
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnProperties2();
	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	// 屬性網格通知
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);

private:
	// 屬性 ID 定義
	enum PropertyID {
		// 變換屬性
		PROP_POSITION_X = 1000,
		PROP_POSITION_Y,
		PROP_POSITION_Z,
		PROP_ROTATION_W,
		PROP_ROTATION_X,
		PROP_ROTATION_Y,
		PROP_ROTATION_Z,
		PROP_SCALE_X,
		PROP_SCALE_Y,
		PROP_SCALE_Z,

		// 物理屬性
		PROP_MASS = 2000,
		PROP_DENSITY,
		PROP_FRICTION,
		PROP_RESTITUTION,
		PROP_LINEAR_DAMPING,
		PROP_ANGULAR_DAMPING,
		PROP_LINEAR_VELOCITY_X,
		PROP_LINEAR_VELOCITY_Y,
		PROP_LINEAR_VELOCITY_Z,
		PROP_ANGULAR_VELOCITY_X,
		PROP_ANGULAR_VELOCITY_Y,
		PROP_ANGULAR_VELOCITY_Z,
		PROP_IS_KINEMATIC,
		PROP_IS_STATIC,
		PROP_COLLISION_GROUP,
		PROP_COLLISION_MASK,
		PROP_IS_TRIGGER,

		// 形狀屬性
		PROP_SHAPE_TYPE = 3000,
		PROP_SHAPE_WIDTH,
		PROP_SHAPE_HEIGHT,
		PROP_SHAPE_DEPTH,
		PROP_SHAPE_RADIUS,
		PROP_MESH_FILE,

		// 約束屬性
		PROP_CONSTRAINT_TYPE = 4000,
		PROP_BODY_A,
		PROP_BODY_B,
		PROP_FRAME_A_POS_X,
		PROP_FRAME_A_POS_Y,
		PROP_FRAME_A_POS_Z,
		PROP_FRAME_B_POS_X,
		PROP_FRAME_B_POS_Y,
		PROP_FRAME_B_POS_Z,
		PROP_LINEAR_LOWER_LIMIT_X,
		PROP_LINEAR_LOWER_LIMIT_Y,
		PROP_LINEAR_LOWER_LIMIT_Z,
		PROP_LINEAR_UPPER_LIMIT_X,
		PROP_LINEAR_UPPER_LIMIT_Y,
		PROP_LINEAR_UPPER_LIMIT_Z,
		PROP_BREAKING_THRESHOLD,

		// 力場屬性
		PROP_FORCE_FIELD_TYPE = 5000,
		PROP_FORCE_DIRECTION_X,
		PROP_FORCE_DIRECTION_Y,
		PROP_FORCE_DIRECTION_Z,
		PROP_FORCE_STRENGTH,
		PROP_FORCE_RADIUS,
		PROP_FORCE_FALLOFF,
		PROP_AFFECTED_GROUPS,

		// 光源屬性
		PROP_LIGHT_TYPE = 6000,
		PROP_LIGHT_COLOR_R,
		PROP_LIGHT_COLOR_G,
		PROP_LIGHT_COLOR_B,
		PROP_LIGHT_INTENSITY,
		PROP_LIGHT_RANGE,
		PROP_SPOT_ANGLE,
		PROP_SPOT_EXPONENT,
		PROP_CAST_SHADOWS,

		// 相機屬性
		PROP_CAMERA_FOV = 7000,
		PROP_CAMERA_NEAR_PLANE,
		PROP_CAMERA_FAR_PLANE,
		PROP_CAMERA_ASPECT_RATIO,
		PROP_IS_ORTHOGRAPHIC,
		PROP_ORTHOGRAPHIC_SIZE,

		// 材質屬性
		PROP_MATERIAL_NAME = 8000,
		PROP_DIFFUSE_COLOR_R,
		PROP_DIFFUSE_COLOR_G,
		PROP_DIFFUSE_COLOR_B,
		PROP_DIFFUSE_COLOR_A,
		PROP_SPECULAR_COLOR_R,
		PROP_SPECULAR_COLOR_G,
		PROP_SPECULAR_COLOR_B,
		PROP_SHININESS,
		PROP_METALLIC,
		PROP_ROUGHNESS,
		PROP_TRANSPARENCY,
		PROP_DIFFUSE_TEXTURE,
		PROP_NORMAL_TEXTURE,

		// 模擬設定
		PROP_TIME_STEP = 9000,
		PROP_GRAVITY_X,
		PROP_GRAVITY_Y,
		PROP_GRAVITY_Z,
		PROP_SOLVER_ITERATIONS,
		PROP_USE_OGC_CONTACT,
		PROP_OGC_CONTACT_RADIUS,
		PROP_HYBRID_MODE,
		PROP_ENABLE_CCD,
		PROP_ENABLE_SLEEPING,

		// 渲染設定
		PROP_BACKGROUND_COLOR_R = 10000,
		PROP_BACKGROUND_COLOR_G,
		PROP_BACKGROUND_COLOR_B,
		PROP_AMBIENT_LIGHT_R,
		PROP_AMBIENT_LIGHT_G,
		PROP_AMBIENT_LIGHT_B,
		PROP_ENABLE_SHADOWS,
		PROP_ENABLE_ANTIALIASING,
		PROP_SHADOW_MAP_SIZE,
		PROP_EXPOSURE,
		PROP_GAMMA
	};

	// 屬性資料結構
	struct PropertyData {
		PropertyID id;
		ObjectType objectType;
		CString objectName;
		
		PropertyData(PropertyID propId, ObjectType objType, const CString& objName = _T(""))
			: id(propId), objectType(objType), objectName(objName) {}
	};

	// 輔助函數
	void SetPropertyData(CMFCPropertyGridProperty* pProp, PropertyID id);
	PropertyData* GetPropertyData(CMFCPropertyGridProperty* pProp);
	void NotifyPropertyChanged(const CString& objectName, const CString& propertyName);
	void ShowErrorMessage(const CString& message);
	void ShowWarningMessage(const CString& message);

	// 預設值管理
	void SetDefaultRigidBodyProperties();
	void SetDefaultConstraintProperties();
	void SetDefaultForceFieldProperties();
	void SetDefaultLightProperties();
	void SetDefaultCameraProperties();
	void SetDefaultMaterialProperties();

public:
	// 公用介面
	void SetObject(const CString& objectName, const CString& objectType);
	void UpdateProperty(const CString& propertyName, const COleVariant& value);
	void EnableProperty(const CString& propertyName, BOOL enable);
	void SetPropertyDescription(const CString& propertyName, const CString& description);
	
	// 屬性存取
	COleVariant GetPropertyValue(const CString& propertyName);
	BOOL SetPropertyValue(const CString& propertyName, const COleVariant& value);
	
	// 群組管理
	void ExpandGroup(const CString& groupName, BOOL expand = TRUE);
	void ShowGroup(const CString& groupName, BOOL show = TRUE);
	
	// 驗證和錯誤處理
	BOOL ValidateAllProperties();
	void ResetToDefaults();
	void UndoChanges();
};
