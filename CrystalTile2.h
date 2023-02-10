#ifndef CT2_CRYSTALTILE2_H
#define CT2_CRYSTALTILE2_H
// CrystalTile2.h : CrystalTile2 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h” 
#endif

#include "resource.h"       // 主符号
#include "Common.h"
#include "RecentStatList.h"
#include "CT2StatusBar.h"
#include "CT2View.h"
#include "CT2MainFrm.h"

// CT2App:
// 有关此类的实现，请参阅 CrystalTile2.cpp
//

class CT2App : public CWinApp
{
public:
	CT2App();
	virtual ~CT2App();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:
	HMENU m_hTileMenu;
	HACCEL m_hTileAccelTable;

	HMENU m_hHexMenu;
	HACCEL m_hHexAccelTable;

	HMENU m_hAsmMenu;
	HACCEL m_hAsmAccelTable;

	HMENU m_hTileEditorMenu;
	HACCEL m_hTileEditorAccelTable;

	HMENU m_hScriptMenu;
	HACCEL m_hScriptAccelTable;

	CRecentStatList* m_pRecentStatList;

	BYTE *m_pCopyData;
	UINT m_nCopyDataSize;

	UINT *m_pCodePage;
	UINT m_nCPCount;

	WORD m_nTreeID;

	// 程序是否已运行
	BOOL IsRunning(void);
	void LoadStdProfileSettings2(UINT nMaxMRU=_AFX_MRU_COUNT);
	virtual int ExitInstance();
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	afx_msg void OnOpenRecentFile(UINT nID);
	UINT GetMRUIndex(CString strPathName);
	CT2View* GetCurView(UINT* pType=NULL);
	CT2View* GetView(WORD nView);
	CT2StatusBar* GetStatusBar(void);
	CToolBar* GetToolBar(void);
	CDialog& GetOptionBox();
	CT2Palette& GetPalDlg(void);
	CT2Favorite& GetFavorite(void);
	afx_msg void OnFileOpen();
	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
							LPCTSTR lpszDefault = NULL);
	BOOL GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
							BYTE** ppData, UINT* pBytes);
	afx_msg void OnUpdateFileFirstdsemutask(CCmdUI *pCmdUI);
	afx_msg void OnFirstdsemutask(UINT nID);
	afx_msg void OnToolsTbl2cmap();
	void SetCT2Tree(WORD nID);
};

extern CT2App theApp;
extern BYTE m_nOpenMapFile;
extern TCHAR CT2Settings[];
extern TCHAR CT2ScaleList[];
extern TCHAR CT2OffsetList[];
extern TCHAR CT2DefOffset[];
extern TCHAR CT2DefPal[];
extern TCHAR CT2DefBk[];
extern TCHAR CT2ToolList[];

extern CList<FAVORITES, FAVORITES&> g_Favorites;

extern HIMAGELIST g_hSysImageList;

extern CString GetLine(LPTSTR &pAsm);
#endif // CT2_CRYSTALTILE2_H