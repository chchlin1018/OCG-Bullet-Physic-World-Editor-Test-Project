#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QColorDialog>
#include <QtCore/QVariant>
#include <QtCore/QMap>

#include "../scene_format/physics_scene_format.h"

class Vector3Widget;
class QuaternionWidget;
class ColorWidget;
class MaterialSelector;

/**
 * @file PropertyWidget.h
 * @brief 屬性編輯 Widget
 * 
 * 根據選中物件類型動態顯示相關屬性，支援即時編輯和驗證。
 * 提供與 MFC 版本相同的屬性編輯功能。
 */

class PropertyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyWidget(QWidget *parent = nullptr);
    ~PropertyWidget();

    // 物件設定
    void SetObject(const QString& objectId, const QString& objectType, 
                   const PhysicsScene::PhysicsScene& scene);
    void ClearObject();
    void RefreshProperties();

    // 屬性存取
    QVariant GetProperty(const QString& propertyName) const;
    void SetProperty(const QString& propertyName, const QVariant& value);
    bool HasProperty(const QString& propertyName) const;

    // 驗證
    bool ValidateProperties() const;
    QStringList GetValidationErrors() const;

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    // 屬性變更事件
    void OnPropertyChanged();
    void OnVector3Changed();
    void OnQuaternionChanged();
    void OnColorChanged();
    void OnMaterialChanged();
    void OnFilePathChanged();

    // 按鈕事件
    void OnResetToDefault();
    void OnCopyProperties();
    void OnPasteProperties();
    void OnLoadPreset();
    void OnSavePreset();

private:
    // UI 建立
    void CreateUI();
    void CreateToolBar();
    void CreateScrollArea();

    // 屬性界面建立
    void BuildRigidBodyProperties();
    void BuildConstraintProperties();
    void BuildForceFieldProperties();
    void BuildLightProperties();
    void BuildCameraProperties();
    void BuildMaterialProperties();

    // 通用屬性組
    QGroupBox* CreateTransformGroup();
    QGroupBox* CreatePhysicsGroup();
    QGroupBox* CreateShapeGroup();
    QGroupBox* CreateMaterialGroup();
    QGroupBox* CreateRenderGroup();
    QGroupBox* CreateConstraintGroup();
    QGroupBox* CreateLightGroup();
    QGroupBox* CreateCameraGroup();

    // 控制項建立
    QLineEdit* CreateLineEdit(const QString& propertyName, const QString& defaultValue = QString());
    QSpinBox* CreateSpinBox(const QString& propertyName, int min = 0, int max = 1000, int defaultValue = 0);
    QDoubleSpinBox* CreateDoubleSpinBox(const QString& propertyName, double min = 0.0, double max = 1000.0, 
                                       double defaultValue = 0.0, int decimals = 3);
    QComboBox* CreateComboBox(const QString& propertyName, const QStringList& items, 
                             const QString& defaultValue = QString());
    QCheckBox* CreateCheckBox(const QString& propertyName, bool defaultValue = false);
    QSlider* CreateSlider(const QString& propertyName, int min = 0, int max = 100, int defaultValue = 50);
    Vector3Widget* CreateVector3Widget(const QString& propertyName, const PhysicsScene::Vector3& defaultValue = {0,0,0});
    QuaternionWidget* CreateQuaternionWidget(const QString& propertyName, const PhysicsScene::Quaternion& defaultValue = {1,0,0,0});
    ColorWidget* CreateColorWidget(const QString& propertyName, const PhysicsScene::Color& defaultValue = {1,1,1,1});
    MaterialSelector* CreateMaterialSelector(const QString& propertyName, const QString& materialType);

    // 屬性載入/儲存
    void LoadRigidBodyProperties(const PhysicsScene::RigidBody& rigidBody);
    void LoadConstraintProperties(const PhysicsScene::Constraint& constraint);
    void LoadForceFieldProperties(const PhysicsScene::ForceField& forceField);
    void LoadLightProperties(const PhysicsScene::Light& light);
    void LoadCameraProperties(const PhysicsScene::Camera& camera);

    void SaveRigidBodyProperties(PhysicsScene::RigidBody& rigidBody) const;
    void SaveConstraintProperties(PhysicsScene::Constraint& constraint) const;
    void SaveForceFieldProperties(PhysicsScene::ForceField& forceField) const;
    void SaveLightProperties(PhysicsScene::Light& light) const;
    void SaveCameraProperties(PhysicsScene::Camera& camera) const;

    // 動態屬性處理
    void UpdateShapeParameters();
    void UpdateConstraintParameters();
    void UpdateForceFieldParameters();
    void UpdateLightParameters();

    // 驗證
    bool ValidateNumericRange(QWidget* widget, double min, double max) const;
    bool ValidateRequired(QWidget* widget) const;
    void SetValidationError(QWidget* widget, const QString& error);
    void ClearValidationError(QWidget* widget);

    // 工具方法
    void ConnectPropertySignals();
    void DisconnectPropertySignals();
    void SetWidgetValue(QWidget* widget, const QVariant& value);
    QVariant GetWidgetValue(QWidget* widget) const;
    void SetWidgetEnabled(const QString& propertyName, bool enabled);

private:
    // UI 組件
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_toolBarLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollWidget;
    QVBoxLayout* m_scrollLayout;

    // 工具按鈕
    QPushButton* m_resetButton;
    QPushButton* m_copyButton;
    QPushButton* m_pasteButton;
    QPushButton* m_presetButton;

    // 屬性組
    QGroupBox* m_transformGroup;
    QGroupBox* m_physicsGroup;
    QGroupBox* m_shapeGroup;
    QGroupBox* m_materialGroup;
    QGroupBox* m_renderGroup;
    QGroupBox* m_constraintGroup;
    QGroupBox* m_lightGroup;
    QGroupBox* m_cameraGroup;

    // 屬性控制項映射
    QMap<QString, QWidget*> m_propertyWidgets;
    QMap<QWidget*, QString> m_widgetProperties;

    // 資料
    QString m_currentObjectId;
    QString m_currentObjectType;
    const PhysicsScene::PhysicsScene* m_scene;

    // 狀態
    bool m_updatingProperties;
    QMap<QString, QVariant> m_clipboardProperties;

    // 預設值
    QMap<QString, QVariant> m_defaultValues;

signals:
    // 屬性變更信號
    void PropertyChanged(const QString& objectId, const QString& propertyName, const QVariant& value);
    void PropertiesChanged(const QString& objectId, const QMap<QString, QVariant>& properties);
    
    // 驗證信號
    void ValidationError(const QString& propertyName, const QString& error);
    void ValidationCleared();

    // 操作信號
    void ResetRequested(const QString& objectId);
    void CopyRequested(const QString& objectId);
    void PasteRequested(const QString& objectId);
};

/**
 * @class Vector3Widget
 * @brief 三維向量編輯控制項
 */
class Vector3Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Vector3Widget(QWidget* parent = nullptr);
    
    PhysicsScene::Vector3 GetValue() const;
    void SetValue(const PhysicsScene::Vector3& value);
    
    void SetRange(double min, double max);
    void SetDecimals(int decimals);
    void SetSingleStep(double step);

private slots:
    void OnValueChanged();

private:
    QHBoxLayout* m_layout;
    QDoubleSpinBox* m_xSpinBox;
    QDoubleSpinBox* m_ySpinBox;
    QDoubleSpinBox* m_zSpinBox;
    QLabel* m_xLabel;
    QLabel* m_yLabel;
    QLabel* m_zLabel;

signals:
    void ValueChanged(const PhysicsScene::Vector3& value);
};

/**
 * @class QuaternionWidget
 * @brief 四元數編輯控制項
 */
class QuaternionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QuaternionWidget(QWidget* parent = nullptr);
    
    PhysicsScene::Quaternion GetValue() const;
    void SetValue(const PhysicsScene::Quaternion& value);
    
    void SetEulerMode(bool euler);
    bool IsEulerMode() const { return m_eulerMode; }

private slots:
    void OnValueChanged();
    void OnModeToggled();

private:
    void UpdateFromQuaternion();
    void UpdateFromEuler();
    PhysicsScene::Vector3 QuaternionToEuler(const PhysicsScene::Quaternion& q) const;
    PhysicsScene::Quaternion EulerToQuaternion(const PhysicsScene::Vector3& euler) const;

private:
    QVBoxLayout* m_layout;
    QHBoxLayout* m_modeLayout;
    QHBoxLayout* m_valueLayout;
    
    QPushButton* m_modeButton;
    QDoubleSpinBox* m_wSpinBox;
    QDoubleSpinBox* m_xSpinBox;
    QDoubleSpinBox* m_ySpinBox;
    QDoubleSpinBox* m_zSpinBox;
    
    QLabel* m_wLabel;
    QLabel* m_xLabel;
    QLabel* m_yLabel;
    QLabel* m_zLabel;
    
    bool m_eulerMode;
    bool m_updating;

signals:
    void ValueChanged(const PhysicsScene::Quaternion& value);
};

/**
 * @class ColorWidget
 * @brief 顏色編輯控制項
 */
class ColorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColorWidget(QWidget* parent = nullptr);
    
    PhysicsScene::Color GetValue() const;
    void SetValue(const PhysicsScene::Color& value);
    
    void SetAlphaEnabled(bool enabled);

private slots:
    void OnColorButtonClicked();
    void OnSpinBoxChanged();

private:
    void UpdateColorButton();
    void UpdateSpinBoxes();

private:
    QHBoxLayout* m_layout;
    QPushButton* m_colorButton;
    QDoubleSpinBox* m_rSpinBox;
    QDoubleSpinBox* m_gSpinBox;
    QDoubleSpinBox* m_bSpinBox;
    QDoubleSpinBox* m_aSpinBox;
    
    PhysicsScene::Color m_color;
    bool m_alphaEnabled;
    bool m_updating;

signals:
    void ValueChanged(const PhysicsScene::Color& value);
};

/**
 * @class MaterialSelector
 * @brief 材質選擇控制項
 */
class MaterialSelector : public QWidget
{
    Q_OBJECT

public:
    explicit MaterialSelector(const QString& materialType, QWidget* parent = nullptr);
    
    QString GetSelectedMaterial() const;
    void SetSelectedMaterial(const QString& materialName);
    
    void SetMaterials(const QStringList& materials);
    void SetScene(const PhysicsScene::PhysicsScene* scene);

private slots:
    void OnMaterialChanged();
    void OnEditMaterial();
    void OnNewMaterial();

private:
    QHBoxLayout* m_layout;
    QComboBox* m_comboBox;
    QPushButton* m_editButton;
    QPushButton* m_newButton;
    
    QString m_materialType;
    const PhysicsScene::PhysicsScene* m_scene;

signals:
    void MaterialChanged(const QString& materialName);
    void EditMaterialRequested(const QString& materialName);
    void NewMaterialRequested();
};
