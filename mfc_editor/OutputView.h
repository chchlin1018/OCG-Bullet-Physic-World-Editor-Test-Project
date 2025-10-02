#pragma once

/**
 * @file OutputView.h
 * @brief 輸出檢視類別
 * 
 * 提供應用程式的日誌輸出、錯誤訊息、警告和除錯資訊的顯示。
 * 支援不同類型的訊息分類、過濾和搜尋功能。
 */

class COutputView : public CDockablePane
{
	DECLARE_DYNAMIC(COutputView)

public:
	COutputView() noexcept;
	virtual ~COutputView();

// 屬性
protected:
	CMFCTabCtrl m_tabCtrl;
	
	// 輸出視窗
	CListCtrl m_outputList;
	CEdit m_outputEdit;
	
	// 工具列
	CMFCToolBar m_toolBar;
	
	// 訊息類型
	enum MessageType {
		MSG_INFO = 0,
		MSG_WARNING,
		MSG_ERROR,
		MSG_DEBUG,
		MSG_PHYSICS,
		MSG_RENDER,
		MSG_IO
	};
	
	// 訊息結構
	struct OutputMessage {
		MessageType type;
		CString timestamp;
		CString category;
		CString message;
		CString details;
		DWORD threadId;
		
		OutputMessage() : type(MSG_INFO), threadId(0) {}
		OutputMessage(MessageType t, const CString& cat, const CString& msg, const CString& det = _T(""))
			: type(t), category(cat), message(msg), details(det) {
			threadId = GetCurrentThreadId();
			
			// 生成時間戳記
			SYSTEMTIME st;
			GetLocalTime(&st);
			timestamp.Format(_T("%02d:%02d:%02d.%03d"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		}
	};
	
	std::vector<OutputMessage> m_messages;
	CImageList m_imageList;
	
	// 過濾設定
	BOOL m_showInfo;
	BOOL m_showWarnings;
	BOOL m_showErrors;
	BOOL m_showDebug;
	BOOL m_showPhysics;
	BOOL m_showRender;
	BOOL m_showIO;
	
	// 搜尋功能
	CString m_searchText;
	BOOL m_caseSensitive;
	BOOL m_wholeWord;
	
	// 自動捲動
	BOOL m_autoScroll;
	
	// 最大訊息數量
	int m_maxMessages;
	
	// 字型
	CFont m_font;

// 作業
public:
	// 訊息輸出
	void LogInfo(const CString& category, const CString& message, const CString& details = _T(""));
	void LogWarning(const CString& category, const CString& message, const CString& details = _T(""));
	void LogError(const CString& category, const CString& message, const CString& details = _T(""));
	void LogDebug(const CString& category, const CString& message, const CString& details = _T(""));
	void LogPhysics(const CString& message, const CString& details = _T(""));
	void LogRender(const CString& message, const CString& details = _T(""));
	void LogIO(const CString& message, const CString& details = _T(""));
	
	// 通用日誌函數
	void LogMessage(MessageType type, const CString& category, const CString& message, const CString& details = _T(""));
	
	// 清理和管理
	void ClearOutput();
	void ClearCategory(const CString& category);
	void ClearMessageType(MessageType type);
	
	// 過濾和搜尋
	void SetFilter(MessageType type, BOOL show);
	void SetSearchText(const CString& searchText, BOOL caseSensitive = FALSE, BOOL wholeWord = FALSE);
	void ClearSearch();
	
	// 設定
	void SetMaxMessages(int maxMessages);
	void SetAutoScroll(BOOL autoScroll);
	void SetFont(const CFont& font);
	
	// 匯出功能
	BOOL ExportToFile(const CString& filename);
	BOOL ExportToClipboard();

protected:
	void InitializeControls();
	void InitializeToolBar();
	void InitializeImageList();
	void SetupColumns();
	
	// 訊息管理
	void AddMessage(const OutputMessage& message);
	void RefreshMessageList();
	void UpdateMessageCount();
	BOOL PassesFilter(const OutputMessage& message);
	BOOL MatchesSearch(const OutputMessage& message);
	
	// UI 更新
	void UpdateToolBarButtons();
	void ScrollToBottom();
	int GetMessageIcon(MessageType type);
	COLORREF GetMessageColor(MessageType type);
	CString GetMessageTypeString(MessageType type);
	
	// 輔助函數
	void TrimMessages();
	CString FormatMessageForExport(const OutputMessage& message);

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

	// 工具列按鈕
	afx_msg void OnClearOutput();
	afx_msg void OnToggleInfo();
	afx_msg void OnToggleWarnings();
	afx_msg void OnToggleErrors();
	afx_msg void OnToggleDebug();
	afx_msg void OnTogglePhysics();
	afx_msg void OnToggleRender();
	afx_msg void OnToggleIO();
	afx_msg void OnToggleAutoScroll();
	afx_msg void OnFind();
	afx_msg void OnExportToFile();
	afx_msg void OnCopyToClipboard();
	
	// 命令更新
	afx_msg void OnUpdateToggleInfo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToggleWarnings(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToggleErrors(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToggleDebug(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTogglePhysics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToggleRender(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToggleIO(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToggleAutoScroll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClearOutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFind(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportToFile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopyToClipboard(CCmdUI* pCmdUI);

	// 列表控制項事件
	afx_msg void OnOutputListDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOutputListRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOutputListKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOutputListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);

	// 右鍵選單
	afx_msg void OnContextClearAll();
	afx_msg void OnContextClearInfo();
	afx_msg void OnContextClearWarnings();
	afx_msg void OnContextClearErrors();
	afx_msg void OnContextClearDebug();
	afx_msg void OnContextCopyMessage();
	afx_msg void OnContextCopyAllMessages();
	afx_msg void OnContextShowDetails();
	afx_msg void OnContextGotoSource();

private:
	// 常數定義
	static const int MAX_MESSAGES_DEFAULT = 10000;
	static const int COLUMN_ICON = 0;
	static const int COLUMN_TIME = 1;
	static const int COLUMN_TYPE = 2;
	static const int COLUMN_CATEGORY = 3;
	static const int COLUMN_MESSAGE = 4;
	
	// 圖示索引
	enum IconIndex {
		ICON_INFO = 0,
		ICON_WARNING,
		ICON_ERROR,
		ICON_DEBUG,
		ICON_PHYSICS,
		ICON_RENDER,
		ICON_IO
	};
	
	// 工具列命令 ID
	enum ToolBarCommands {
		ID_CLEAR_OUTPUT = 32000,
		ID_TOGGLE_INFO,
		ID_TOGGLE_WARNINGS,
		ID_TOGGLE_ERRORS,
		ID_TOGGLE_DEBUG,
		ID_TOGGLE_PHYSICS,
		ID_TOGGLE_RENDER,
		ID_TOGGLE_IO,
		ID_TOGGLE_AUTO_SCROLL,
		ID_FIND_OUTPUT,
		ID_EXPORT_OUTPUT,
		ID_COPY_OUTPUT
	};
	
	// 統計資訊
	struct Statistics {
		int totalMessages;
		int infoCount;
		int warningCount;
		int errorCount;
		int debugCount;
		int physicsCount;
		int renderCount;
		int ioCount;
		
		Statistics() : totalMessages(0), infoCount(0), warningCount(0), errorCount(0),
					   debugCount(0), physicsCount(0), renderCount(0), ioCount(0) {}
	} m_statistics;
	
	// 輔助函數
	void UpdateStatistics();
	CString GetStatisticsString();
	void ShowMessageDetails(int messageIndex);
	BOOL FindNextMessage(const CString& searchText, int startIndex, BOOL forward = TRUE);
	void HighlightMessage(int messageIndex);

public:
	// 公用介面
	int GetMessageCount() const { return static_cast<int>(m_messages.size()); }
	int GetMessageCount(MessageType type) const;
	
	// 過濾狀態
	BOOL IsFilterEnabled(MessageType type) const;
	void EnableAllFilters(BOOL enable = TRUE);
	void DisableAllFilters();
	
	// 搜尋功能
	const CString& GetSearchText() const { return m_searchText; }
	BOOL IsCaseSensitive() const { return m_caseSensitive; }
	BOOL IsWholeWord() const { return m_wholeWord; }
	
	// 設定存取
	int GetMaxMessages() const { return m_maxMessages; }
	BOOL IsAutoScrollEnabled() const { return m_autoScroll; }
	
	// 統計資訊
	const Statistics& GetStatistics() const { return m_statistics; }
	
	// 訊息存取
	const OutputMessage& GetMessage(int index) const { return m_messages[index]; }
	std::vector<OutputMessage> GetFilteredMessages() const;
	
	// 批次操作
	void BeginBatch();
	void EndBatch();
	
private:
	BOOL m_batchMode;
	int m_batchCount;
};

// 全域日誌巨集
#define LOG_INFO(category, message) \
	if (CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd())) { \
		if (COutputView* pOutput = pFrame->GetOutputView()) { \
			pOutput->LogInfo(category, message); \
		} \
	}

#define LOG_WARNING(category, message) \
	if (CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd())) { \
		if (COutputView* pOutput = pFrame->GetOutputView()) { \
			pOutput->LogWarning(category, message); \
		} \
	}

#define LOG_ERROR(category, message) \
	if (CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd())) { \
		if (COutputView* pOutput = pFrame->GetOutputView()) { \
			pOutput->LogError(category, message); \
		} \
	}

#define LOG_DEBUG(category, message) \
	if (CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd())) { \
		if (COutputView* pOutput = pFrame->GetOutputView()) { \
			pOutput->LogDebug(category, message); \
		} \
	}

#define LOG_PHYSICS(message) \
	if (CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd())) { \
		if (COutputView* pOutput = pFrame->GetOutputView()) { \
			pOutput->LogPhysics(message); \
		} \
	}

#define LOG_RENDER(message) \
	if (CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd())) { \
		if (COutputView* pOutput = pFrame->GetOutputView()) { \
			pOutput->LogRender(message); \
		} \
	}

#define LOG_IO(message) \
	if (CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd())) { \
		if (COutputView* pOutput = pFrame->GetOutputView()) { \
			pOutput->LogIO(message); \
		} \
	}
