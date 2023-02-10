#ifndef CT2_CT2MAINFRM_H
#define CT2_CT2MAINFRM_H
// CT2MainFrm.h : CT2MainFrm 类的接口
//
#include "CT2StatusBar.h"
//#include "CT2Palette.h"
//#include "CT2Favorite.h"
#include "FileCompDlg.h"

//#include "ExplorerBar.h"
//#include "OptionBar.h"
#include "ToolBarEx.h"
#include "CT2TreeBar.h"

#pragma once
class CT2MainFrm : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CT2MainFrm)
public:
	CT2MainFrm();

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CT2MainFrm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CT2StatusBar m_wndStatusBar;
	CToolBar m_wndToolBar;
	CToolBarEx m_wndToolBarEx;
	//CT2Palette m_wndPalette;
	//CT2Favorites m_wndFavorites;
	CFileCompDlg m_FileComp;

	//CExplorerBar m_ExplorerBar;
	//COptionBar m_OptionBar;
	CT2TreeBar m_TreeBar;

	CStringArray m_ToolList;

	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnToolsLz77comp();
	afx_msg void OnToolsFilecomp();
	afx_msg void OnToolsTextcpchange();
	afx_msg void OnSetDefCp();
	afx_msg void OnSetDefPal();
	afx_msg void OnSetDefBk();
	afx_msg void OnSetDefOffset();
	afx_msg void OnUpdateFavoritesList(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFavoritesAdd(CCmdUI *pCmdUI);
	afx_msg void OnFavoritesList(UINT nID);
	afx_msg void OnFavoritesAdd();
	afx_msg void OnGotoWindow(UINT nID);
	afx_msg void OnToolsTools(UINT nID);
	void LoadToolList();
	void SaveToolList();
	afx_msg void OnUpdateToolsTools(CCmdUI *pCmdUI);
	void LoadToolBarEx();
	afx_msg void OnViewToolex();
	afx_msg void OnUpdateViewToolex(CCmdUI *pCmdUI);
	afx_msg void OnViewOptionbar();
	afx_msg void OnUpdateViewOptionbar(CCmdUI *pCmdUI);
	afx_msg void OnBreak();
	afx_msg void OnToolsNdsFirmware();
};

#endif // CT2_CT2MAINFRM_H