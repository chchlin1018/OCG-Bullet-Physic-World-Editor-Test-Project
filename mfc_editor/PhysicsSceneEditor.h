#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此檔案前，請先包含 'pch.h' 以產生 PCH"
#endif

#include "resource.h"       // 主要符號
#include "../scene_format/physics_scene_format.h"

/**
 * @file PhysicsSceneEditor.h
 * @brief 物理場景編輯器主應用程式類別
 * 
 * 這是一個基於 MFC 的物理場景編輯器應用程式，提供直觀的圖形化界面
 * 用於建立、編輯和管理物理場景。
 * 
 * 主要功能：
 * - 場景物件的建立、編輯、刪除
 * - 材質管理和編輯
 * - 3D 預覽和視覺化
 * - 場景檔案的載入和儲存
 * - 物理屬性的即時調整
 */

// CPhysicsSceneEditorApp:
// 如需此類別的實作，請參閱 PhysicsSceneEditor.cpp
//

class CPhysicsSceneEditorApp : public CWinApp
{
public:
	CPhysicsSceneEditorApp() noexcept;

// 覆寫
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 實作
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	// 應用程式初始化
	BOOL InitializeOpenGL();
	void InitializePhysicsEngine();
	void LoadApplicationSettings();
	void SaveApplicationSettings();

	// 全域設定
	struct AppSettings {
		CString lastOpenPath;
		CString lastSavePath;
		BOOL autoSave = TRUE;
		int autoSaveInterval = 300; // 秒
		BOOL showGrid = TRUE;
		BOOL showAxes = TRUE;
		BOOL enableAntiAliasing = TRUE;
		float cameraSpeed = 1.0f;
		float mouseSensitivity = 1.0f;
		int maxRecentFiles = 10;
		CStringArray recentFiles;
	} m_settings;

public:
	const AppSettings& GetSettings() const { return m_settings; }
	void UpdateSettings(const AppSettings& settings);
	
	// 最近開啟的檔案管理
	void AddToRecentFiles(const CString& filePath);
	void UpdateRecentFileMenu();
};

extern CPhysicsSceneEditorApp theApp;
