#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>
#include <QtCore/QTimer>
#include <QtGui/QIcon>

#include "../scene_format/physics_scene_format.h"

/**
 * @file SceneTreeWidget.h
 * @brief 場景樹狀檢視 Widget
 * 
 * 顯示場景中所有物件的階層結構，支援拖拽、重新命名、
 * 分組等操作。提供與 MFC 版本相同的功能。
 */

class SceneTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SceneTreeWidget(QWidget *parent = nullptr);
    ~SceneTreeWidget();

    // 場景管理
    void SetScene(const PhysicsScene::PhysicsScene& scene);
    void RefreshScene();
    void ClearScene();

    // 物件管理
    void AddObject(const QString& objectId, const QString& objectType);
    void RemoveObject(const QString& objectId, const QString& objectType);
    void UpdateObject(const QString& objectId, const QString& objectType);
    void RenameObject(const QString& objectId, const QString& newName);

    // 選擇管理
    void SelectObject(const QString& objectId, const QString& objectType);
    void ClearSelection();
    QString GetSelectedObjectId() const;
    QString GetSelectedObjectType() const;
    QStringList GetSelectedObjectIds() const;

    // 可見性管理
    void SetObjectVisible(const QString& objectId, bool visible);
    bool IsObjectVisible(const QString& objectId) const;

    // 分組管理
    void GroupObjects(const QStringList& objectIds, const QString& groupName);
    void UngroupObjects(const QString& groupId);
    void AddToGroup(const QString& objectId, const QString& groupId);
    void RemoveFromGroup(const QString& objectId);

    // 搜尋功能
    void SetSearchFilter(const QString& filter);
    void ClearSearchFilter();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    // 樹狀檢視事件
    void OnItemSelectionChanged();
    void OnItemDoubleClicked(QTreeWidgetItem* item, int column);
    void OnItemChanged(QTreeWidgetItem* item, int column);
    void OnItemExpanded(QTreeWidgetItem* item);
    void OnItemCollapsed(QTreeWidgetItem* item);

    // 工具列動作
    void OnAddRigidBody();
    void OnAddConstraint();
    void OnAddForceField();
    void OnAddLight();
    void OnAddCamera();
    void OnDeleteSelected();
    void OnDuplicateSelected();
    void OnGroupSelected();
    void OnUngroupSelected();

    // 右鍵選單動作
    void OnRename();
    void OnCopy();
    void OnPaste();
    void OnCut();
    void OnDelete();
    void OnDuplicate();
    void OnProperties();
    void OnToggleVisibility();
    void OnIsolate();
    void OnFocusInViewport();

    // 搜尋
    void OnSearchTextChanged(const QString& text);
    void OnSearchTimer();

private:
    // UI 建立
    void CreateUI();
    void CreateToolBar();
    void CreateTreeWidget();
    void CreateContextMenu();

    // 樹狀結構建立
    void BuildSceneTree();
    QTreeWidgetItem* CreateCategoryItem(const QString& name, const QIcon& icon);
    QTreeWidgetItem* CreateObjectItem(const QString& objectId, const QString& objectType, 
                                     const QString& name, QTreeWidgetItem* parent);

    // 物件類型處理
    QIcon GetObjectIcon(const QString& objectType) const;
    QString GetObjectDisplayName(const QString& objectId, const QString& objectType) const;
    QTreeWidgetItem* FindObjectItem(const QString& objectId, const QString& objectType) const;
    QTreeWidgetItem* FindCategoryItem(const QString& objectType) const;

    // 拖拽支援
    void SetupDragDrop();
    bool CanDropOn(QTreeWidgetItem* target, QTreeWidgetItem* source) const;
    void PerformDrop(QTreeWidgetItem* target, QTreeWidgetItem* source);

    // 搜尋功能
    void ApplySearchFilter();
    bool MatchesFilter(QTreeWidgetItem* item, const QString& filter) const;
    void SetItemVisible(QTreeWidgetItem* item, bool visible);

    // 狀態管理
    void SaveExpandedState();
    void RestoreExpandedState();
    void UpdateItemAppearance(QTreeWidgetItem* item);

    // 輔助方法
    QString GetItemObjectId(QTreeWidgetItem* item) const;
    QString GetItemObjectType(QTreeWidgetItem* item) const;
    void SetItemData(QTreeWidgetItem* item, const QString& objectId, const QString& objectType);

private:
    // UI 組件
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_toolBarLayout;
    QHBoxLayout* m_searchLayout;
    
    QTreeWidget* m_treeWidget;
    QLineEdit* m_searchEdit;
    
    // 工具按鈕
    QToolButton* m_addRigidBodyButton;
    QToolButton* m_addConstraintButton;
    QToolButton* m_addForceFieldButton;
    QToolButton* m_addLightButton;
    QToolButton* m_addCameraButton;
    QToolButton* m_deleteButton;
    QToolButton* m_duplicateButton;
    QToolButton* m_groupButton;
    QToolButton* m_ungroupButton;

    // 右鍵選單
    QMenu* m_contextMenu;
    QAction* m_renameAction;
    QAction* m_copyAction;
    QAction* m_pasteAction;
    QAction* m_cutAction;
    QAction* m_deleteAction;
    QAction* m_duplicateAction;
    QAction* m_propertiesAction;
    QAction* m_toggleVisibilityAction;
    QAction* m_isolateAction;
    QAction* m_focusAction;

    // 資料
    const PhysicsScene::PhysicsScene* m_scene;
    
    // 類別項目
    QTreeWidgetItem* m_rigidBodiesItem;
    QTreeWidgetItem* m_constraintsItem;
    QTreeWidgetItem* m_forceFieldsItem;
    QTreeWidgetItem* m_lightsItem;
    QTreeWidgetItem* m_camerasItem;
    QTreeWidgetItem* m_groupsItem;

    // 搜尋
    QString m_searchFilter;
    QTimer* m_searchTimer;

    // 狀態
    QMap<QString, bool> m_expandedState;
    QMap<QString, bool> m_visibilityState;
    bool m_updatingSelection;

    // 圖示
    QIcon m_rigidBodyIcon;
    QIcon m_constraintIcon;
    QIcon m_forceFieldIcon;
    QIcon m_lightIcon;
    QIcon m_cameraIcon;
    QIcon m_groupIcon;
    QIcon m_visibleIcon;
    QIcon m_hiddenIcon;

signals:
    // 選擇信號
    void ObjectSelected(const QString& objectId, const QString& objectType);
    void ObjectDoubleClicked(const QString& objectId, const QString& objectType);
    void SelectionChanged();

    // 物件操作信號
    void ObjectRenamed(const QString& objectId, const QString& newName);
    void ObjectsGrouped(const QStringList& objectIds, const QString& groupName);
    void ObjectsUngrouped(const QString& groupId);
    void ObjectVisibilityChanged(const QString& objectId, bool visible);

    // 編輯信號
    void AddRigidBodyRequested();
    void AddConstraintRequested();
    void AddForceFieldRequested();
    void AddLightRequested();
    void AddCameraRequested();
    void DeleteObjectsRequested(const QStringList& objectIds);
    void DuplicateObjectsRequested(const QStringList& objectIds);
    void CopyObjectsRequested(const QStringList& objectIds);
    void PasteObjectsRequested();
    void CutObjectsRequested(const QStringList& objectIds);

    // 視圖信號
    void FocusObjectRequested(const QString& objectId, const QString& objectType);
    void IsolateObjectRequested(const QString& objectId, const QString& objectType);
};
