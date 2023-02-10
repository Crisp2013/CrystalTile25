// CrystalTile2.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include ".\CrystalTile2.h"
#include "CT2MainFrm.h"

#include "CT2ChildFrm.h"
#include "CT2Doc.h"
#include "CT2TileView.h"
#include "OpenFileDialog.h"
#include "TextFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

TCHAR CT2Settings[]			= _T("Settings");
TCHAR CT2Favorites[]		= _T("Favorites");
TCHAR CT2ScaleList[]		= _T("ScaleList");
TCHAR CT2OffsetList[]		= _T("OffsetList");
TCHAR CT2DefOffset[]		= _T("DefOffset");
TCHAR CT2DefPal[]			= _T("DefPal");
TCHAR CT2DefBk[]			= _T("DefBk");
TCHAR CT2PrivateBuild[]		= _T("PrivateBuild");
TCHAR CT2ToolList[]			= _T("ToolList");

BYTE m_nOpenMapFile=0;
CList<FAVORITES, FAVORITES&> g_Favorites;
HIMAGELIST g_hSysImageList;

// CT2App

BEGIN_MESSAGE_MAP(CT2App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// 基于文件的标准文档命令
	// ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND_RANGE(ID_FILE_MRU_FIRST, ID_FILE_MRU_LAST, OnOpenRecentFile)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_FIRSTDSEMUTASK, OnUpdateFileFirstdsemutask)
	ON_COMMAND_RANGE(ID_FILE_FIRSTDSEMUTASK, ID_FILE_FIRSTDSEMUTASK+31, OnFirstdsemutask)
	ON_COMMAND(ID_TOOLS_TBL2CMAP, OnToolsTbl2cmap)
END_MESSAGE_MAP()

// CT2App 构造

CT2App::CT2App()
//: m_hSJisTbl(NULL)
: m_pCopyData(NULL)
, m_nCopyDataSize(0)
, m_pCodePage(NULL)
, m_nCPCount(0)
, m_nTreeID(-1)
{
	m_hHexMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_VIEW_HEXVIEW));
	m_hHexAccelTable = LoadAccelerators(NULL, MAKEINTRESOURCE(IDR_VIEW_HEXVIEW));

	m_hAsmMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_VIEW_ASMVIEW));
	m_hAsmAccelTable = LoadAccelerators(NULL, MAKEINTRESOURCE(IDR_VIEW_ASMVIEW));

	m_hTileEditorMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_VIEW_TILEEDITORVIEW));
	m_hTileEditorAccelTable = LoadAccelerators(NULL, MAKEINTRESOURCE(IDR_VIEW_TILEEDITORVIEW));

	m_hScriptMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_VIEW_SCRIPTVIEW));
	m_hScriptAccelTable = LoadAccelerators(NULL, MAKEINTRESOURCE(IDR_VIEW_SCRIPTVIEW));

	LoadSystemImageList(g_hSysImageList, SHGFI_SMALLICON);
}

CT2App::~CT2App()
{
	if(m_pRecentFileList) delete[] m_pRecentStatList;

	if(m_hHexMenu!=NULL) DestroyMenu(m_hHexMenu);
	if(m_hHexAccelTable!=NULL) ::FreeResource((HGLOBAL)m_hHexAccelTable);

	if(m_hAsmMenu!=NULL) DestroyMenu(m_hAsmMenu);
	if(m_hAsmAccelTable!=NULL) ::FreeResource((HGLOBAL)m_hAsmAccelTable);

	if(m_hTileEditorMenu!=NULL) DestroyMenu(m_hTileEditorMenu);
	if(m_hTileEditorAccelTable!=NULL) ::FreeResource((HGLOBAL)m_hTileEditorAccelTable);

	if(m_hScriptMenu!=NULL) DestroyMenu(m_hScriptMenu);
	if(m_hScriptAccelTable!=NULL) ::FreeResource((HGLOBAL)m_hScriptAccelTable);

	if(m_pCopyData) delete[] m_pCopyData;
	if(m_pCodePage) delete[] m_pCodePage;

	int c = (int)g_Favorites.GetCount();
	CString strItem = GetProfileString(CT2Favorites, _T(""));
	LPTSTR lpFav = strItem.GetBuffer(2048);
	GetPrivateProfileString(CT2Favorites, NULL, NULL, lpFav, 2048, m_pszProfileName);
	while(*lpFav)
	{
		WriteProfileString(CT2Favorites, lpFav, NULL);
		lpFav+=lstrlen(lpFav)+1;
	}
	strItem.ReleaseBuffer();
	for(int i=0; i<c; i++)
	{
		FAVORITES fav = g_Favorites.GetAt(g_Favorites.FindIndex(i));
		strItem.Format(_T("Fav%d"), i);
		WriteProfileBinary(CT2Favorites,
			strItem, (BYTE*)&fav, sizeof(fav));
	}

	if(g_hCTImageList)
		ImageList_Destroy(g_hCTImageList);
}

// 唯一的一个 CT2App 对象

CT2App theApp;

// CT2App 初始化

BOOL CT2App::InitInstance()
{
	CWinApp::InitInstance();

	if(m_pszProfileName) delete[] m_pszProfileName;
	m_pszProfileName = new TCHAR[_MAX_PATH];
	DWORD dwLen = GetModuleFileName(NULL, (LPTSTR)m_pszProfileName, _MAX_PATH);
	lstrcpy((LPTSTR)(m_pszProfileName+(dwLen-3)), _T("ini"));

	// 防止不同版本的CrystalTile配置文件冲突
	CVerInfo ver;
	CString PrivateBuild = GetProfileString(CT2Settings, CT2PrivateBuild);
	if(PrivateBuild.Compare(ver.PrivateBuild)!=0)
	{
		DeleteFile(m_pszProfileName);
		WriteProfileString(CT2Settings, CT2PrivateBuild, ver.PrivateBuild);
	}

	if(IsRunning()) return FALSE;

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_CrystalTile2TYPE,
		RUNTIME_CLASS(CT2Doc),
		RUNTIME_CLASS(CT2ChildFrm), // 自定义 MDI 子框架
		RUNTIME_CLASS(CT2TileView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	m_hTileMenu = pDocTemplate->m_hMenuShared;
	m_hTileAccelTable = pDocTemplate->m_hAccelTable;

	// 创建主 MDI 框架窗口
	CT2MainFrm* pMainFrame = new CT2MainFrm;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	LoadStdProfileSettings2(9);

	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生
	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if(cmdInfo.m_nShellCommand!=CCommandLineInfo::FileNew)
	{
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}

	return TRUE;
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	CHyperlink m_Mail;
	CHyperlink m_HomePage;
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CVerInfo ver;
	CString strText;
	strText.Format(_T("%s %s\n%s\n%s"),
		ver.ProductName, ver.FileVersion, ver.LegalCopyright, ver.PrivateBuild);
	SetDlgItemText(IDC_VERINFO, strText);

	m_Mail.SubclassDlgItem(IDC_MAILTO, this);
	m_HomePage.SubclassDlgItem(IDC_HOMEPAGE, this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 用于运行对话框的应用程序命令
void CT2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// 程序是否已运行
BOOL CT2App::IsRunning(void)
{
	CString strClass;
	strClass.LoadString(IDR_MAINFRAME);
	HWND hWnd = FindWindow(strClass, 0);

	if(hWnd)
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);

		if(cmdInfo.m_nShellCommand==CCommandLineInfo::FileOpen)
		{
			SetWindowText(hWnd, cmdInfo.m_strFileName);
			SendMessage(hWnd, WM_OPENFILE, 0, 0);
		}
		SetForegroundWindow(hWnd);
		return TRUE;
	}
	return FALSE;
}

void CT2App::LoadStdProfileSettings2(UINT nMaxMRU)
{
	DWORD dwPolicies = m_dwPolicies;
	m_dwPolicies=0;
	LoadStdProfileSettings(nMaxMRU);
	m_dwPolicies = dwPolicies;

	m_pRecentStatList = new CRecentStatList(nMaxMRU);
	m_pRecentStatList->ReadList();

	BYTE* p;
	UINT nSize;
	CString strItem;
	int i=0, i2=0;
	CListCtrl *pList = (CListCtrl*)GetFavorite().GetDlgItem(IDC_FAVORITES_LIST);
	while(TRUE)
	{
		strItem.Format(_T("Fav%d"), i++);
		if(!GetProfileBinary(CT2Favorites, strItem, &p, &nSize)) break;
		if(nSize==sizeof(FAVORITES))
		{
			g_Favorites.AddTail(*(FAVORITES*)p);
			pList->InsertItem(i2++, ((FAVORITES*)p)->Desc);
		}
		delete[] p;
	}

	if(m_hScriptMenu)
	{
		EnumSystemCodePages(&m_pCodePage, m_nCPCount);
		HMENU hScriptView = GetSubMenu(m_hScriptMenu, 4);
		HMENU hHexView = GetSubMenu(m_hHexMenu, 3);
		HMENU hSP = GetSubMenu(hScriptView, 5);
		HMENU hSP2 = GetSubMenu(hHexView, 5);
		UINT nCPID = ID_CODEPAGE_ACP+1;
		CString strItem;
		CComboBox *pCB = (CComboBox*)GetOptionBox().GetDlgItem(IDC_OPTION_CPLIST);
		int nDefSel=0;
		for(UINT i=0; i<m_nCPCount; i++, nCPID++)
		{
			strItem = GetCodePageDescription(m_pCodePage[i]);

			AppendMenu(hSP, MF_STRING|MF_BYCOMMAND, nCPID, strItem);
			AppendMenu(hSP2, MF_STRING|MF_BYCOMMAND, nCPID, strItem);

			pCB->InsertString(i, strItem);
			if(m_pCodePage[i]==GetACP())
				nDefSel = i;
		}

		pCB->SetCurSel(GetProfileInt(CT2Settings, _T("DefCp"), nDefSel));
	}

	BYTE* pData;
	if(GetProfileBinary(CT2Settings, _T("MainFrmWP"),
		&pData, &nSize))
	{
		if(nSize==sizeof(MAINFRMPOS))
		{
			WINDOWPLACEMENT wp;
			m_pMainWnd->GetWindowPlacement(&wp);
			wp.rcNormalPosition = ((MAINFRMPOS*)pData)->rcMainFrm;
			m_pMainWnd->SetWindowPlacement(&wp);
			m_pMainWnd->ShowWindow(((MAINFRMPOS*)pData)->bShowMax?SW_MAXIMIZE:m_nCmdShow);
		}else
			m_pMainWnd->ShowWindow(m_nCmdShow);
		delete[] pData;
	}else
		m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
}

int CT2App::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	int nRET = CWinApp::ExitInstance();
	if(m_pRecentStatList)
		m_pRecentStatList->WriteList();
	return nRET;
}

void CT2App::AddToRecentFileList(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类
	// m_pRecentStatList->Add(lpszPathName);

	// if (m_pRecentFileList != NULL) m_pRecentFileList->Add(lpszPathName);
	// fully qualify the path name
	if ( lpszPathName == NULL || lstrlen(lpszPathName) >= _MAX_PATH )
	{
		ASSERT(FALSE);
		// MFC requires paths with length < _MAX_PATH
		// No other way to handle the error from a void function
		AfxThrowFileException(CFileException::badPath);
	}

	if( AfxFullPath(g_szBuffer, lpszPathName) == FALSE )
	{
		ASSERT(FALSE);
		// MFC requires paths with length < _MAX_PATH
		// No other way to handle the error from a void function
		AfxThrowFileException(CFileException::badPath);
	}
	
	// update the MRU list, if an existing MRU string matches file name
	int iMRU;
	for (iMRU = 0; iMRU < m_pRecentFileList->m_nSize-1; iMRU++)
	{
		if (AfxComparePath(m_pRecentFileList->m_arrNames[iMRU], g_szBuffer))
			break;      // iMRU will point to matching entry
	}
	// move MRU strings before this one down
	UINT_PTR nTemp = m_pRecentStatList->m_pRecentStatList[iMRU];
	for (int iMRU2=iMRU; iMRU2 > 0; iMRU2--)
	{
		ASSERT(iMRU2 > 0);
		ASSERT(iMRU2 < m_pRecentFileList->m_nSize);
		m_pRecentFileList->m_arrNames[iMRU2] =
			m_pRecentFileList->m_arrNames[iMRU2-1];

		m_pRecentStatList->m_pRecentStatList[iMRU2] =
			m_pRecentStatList->m_pRecentStatList[iMRU2-1];

	}
	if(iMRU)
		m_pRecentStatList->m_pRecentStatList[0] = nTemp;
	if(!m_pRecentStatList->m_pRecentStatList[0])
	{
		m_pRecentStatList->m_pRecentStatList[0] = (UINT_PTR)new STATEDATA;
		memset((BYTE*)m_pRecentStatList->m_pRecentStatList[0], 0, sizeof(STATEDATA));
	}

	// place this one at the beginning
	m_pRecentFileList->m_arrNames[0] = g_szBuffer;
}

void CT2App::OnOpenRecentFile(UINT nID)
{
	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList != NULL);

	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);

	TRACE(traceAppMsg, 0, _T("MRU: open file (%d) '%s'.\n"), (nIndex) + 1,
			(LPCTSTR)(*m_pRecentFileList)[nIndex]);

	if (OpenDocumentFile((*m_pRecentFileList)[nIndex]) == NULL)
	{
		m_pRecentFileList->Remove(nIndex);
		m_pRecentStatList->Remove(nIndex);
	}
}
UINT CT2App::GetMRUIndex(CString strPathName)
{
	if(!strPathName.IsEmpty() && m_pRecentFileList)
	for(int iMRU=0; iMRU<m_pRecentFileList->m_nSize; iMRU++)
	{
		if(strPathName.CompareNoCase(m_pRecentFileList->m_arrNames[iMRU])==0)
			return iMRU;
	}
	return -1;
}

CT2View* CT2App::GetCurView(UINT* pType)
{
	CT2ChildFrm* pCf = (CT2ChildFrm*)
		((CT2MainFrm*)GetMainWnd())->MDIGetActive();
	if(pCf==NULL) return NULL;
	return pCf->GetCurView(pType);
}

CT2View* CT2App::GetView(WORD nView)
{
	CT2MainFrm* pMf = (CT2MainFrm*)m_pMainWnd;
	CT2ChildFrm* pCf = (CT2ChildFrm*)pMf->MDIGetActive();
	if(pCf==NULL) return NULL;
	switch(nView)
	{
	case ID_VIEW_TILEVIEW:
		return pCf->m_pTileView;
	case ID_VIEW_HEXVIEW:
		return pCf->m_pHexView;
	case ID_VIEW_ASMVIEW:
		return pCf->m_pAsmView;
	case ID_VIEW_TILEEDITORVIEW:
		return pCf->m_pTileEditorView;
	case ID_VIEW_SCRIPTVIEW:
		return pCf->m_pScriptView;
	}
	return NULL;
}

CT2StatusBar* CT2App::GetStatusBar(void)
{
	return &((CT2MainFrm*)m_pMainWnd)->m_wndStatusBar;
}

CToolBar* CT2App::GetToolBar(void)
{
	return &((CT2MainFrm*)m_pMainWnd)->m_wndToolBar;
}
/*
CCrystalBar* CT2App::GetExplorerBox()
{
	return &((CT2MainFrm*)m_pMainWnd)->m_ExplorerBar;
}
*/
CDialog& CT2App::GetOptionBox()
{
	return ((CT2MainFrm*)m_pMainWnd)->m_TreeBar.m_wndDefOption;
}

CT2Palette& CT2App::GetPalDlg(void)
{
	return ((CT2MainFrm*)m_pMainWnd)->m_TreeBar.m_wndPal;// m_wndPalette;
}

CT2Favorite& CT2App::GetFavorite(void)
{
	return ((CT2MainFrm*)m_pMainWnd)->m_TreeBar.m_wndFav;// m_wndFavorites;
}

void CT2App::OnFileOpen()
{
	static COpenFileDialog fd(TRUE);
	CString strOpenRomFilter;strOpenRomFilter.LoadString(m_hInstance, IDS_OPENROMFILTER, 0);
	CString strInitialDir;
	strOpenRomFilter.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter=strOpenRomFilter;
	CView *pView = GetCurView();
	if(pView)
	{
		strInitialDir = pView->GetDocument()->GetPathName();
		strInitialDir = strInitialDir.Left(strInitialDir.ReverseFind(_T('\\')));
		fd.m_pOFN->lpstrInitialDir=strInitialDir;
	}
	if(fd.DoModal()!=IDOK) return;

	m_nOpenMapFile = fd.m_bMapFile|0x02;
	if(fd.m_pOFN->Flags&OFN_READONLY)
		m_nOpenMapFile|=4;
	CString n;
	AfxExtractSubString(n, g_strMFile, 0);
	OpenDocumentFile(n);
}

CString CT2App::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	LPCTSTR lpszDefault)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);

	ASSERT(m_pszProfileName != NULL);

	if (lpszDefault == NULL)
		lpszDefault = _T("");	// don't pass in NULL
	TCHAR szT[0x10000];
	DWORD dw = ::GetPrivateProfileString(lpszSection, lpszEntry,
		lpszDefault, szT, _countof(szT), m_pszProfileName);
	ASSERT(dw < 0x10000-2);
	return szT;
}

BOOL CT2App::GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	BYTE** ppData, UINT* pBytes)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
	ASSERT(ppData != NULL);
	ASSERT(pBytes != NULL);
	*ppData = NULL;
	*pBytes = 0;

	ASSERT(m_pszProfileName != NULL);

	CString str = GetProfileString(lpszSection, lpszEntry, NULL);
	if (str.IsEmpty())
		return FALSE;
	ASSERT(str.GetLength()%2 == 0);
	INT_PTR nLen = str.GetLength();
	*pBytes = UINT(nLen)/2;
	*ppData = new BYTE[*pBytes];
	for (int i=0;i<nLen;i+=2)
	{
		(*ppData)[i/2] = (BYTE)
			(((str[i+1] - 'A') << 4) + (str[i] - 'A'));
	}
	return TRUE;
}

//CStringList g_AsmList;
//CStringList g_ThumbList;
int g_nLine;

CString GetLine(LPTSTR &pAsm)
{
	CString Line;

	LPTSTR pEnd = pAsm;
	int nLen;
	if( (*pEnd==_T('\r') || *pEnd==_T('\n')) && *pEnd )
	{
		pEnd++;
		if(*pEnd == _T('\n'))
			pEnd++;
		nLen = (int)(pEnd-pAsm);
	}else
	{
		while( *pEnd!=_T('\r') && *pEnd!=_T('\n') && *pEnd )
			pEnd++;
		nLen = (int)(pEnd-pAsm);
		if( *pEnd )
		{
			pEnd++;
			if(*pEnd == _T('\n'))
				pEnd++;
		}
	}

	/*
	while( (*pEnd==_T('\r') || *pEnd==_T('\n')) && *pEnd )
		pEnd++;
	*/

	Line.Append(pAsm, nLen);
	pAsm = pEnd;

	//Line.MakeLower();
	//Line.TrimLeft();
	//Line.TrimRight();

	g_nLine++;
	return Line;
}

BOOL CALLBACK DsEmu(HWND hWnd, LPARAM lParam)
{
	CCrystalMemory mem;
	HMENU hMenu = (HMENU)lParam;
	GetClassName(hWnd, g_szBuffer, _MAX_PATH);
	sMakeLower(g_szBuffer);

	#define DSSTYLE (WS_VISIBLE|WS_SYSMENU|WS_OVERLAPPED|WS_CAPTION)
	if( !GetParent(hWnd) && theApp.GetMainWnd()->m_hWnd!=hWnd &&
        (GetWindowLong(hWnd, GWL_STYLE)&DSSTYLE)==DSSTYLE &&
		lstrcmp(g_szBuffer, _T("cabinetwclass"))!=0 &&
		lstrcmp(g_szBuffer, _T("explorewclass"))!=0 /*&&
		(sFindStr(g_szBuffer, _T("ds")) || sFindStr(g_szBuffer, _T("no$")) || sFindStr(g_szBuffer, _T("desmume")))*/
		)
	{
		int i=GetMenuItemCount(hMenu);
		GetWindowText(hWnd, g_szBuffer, _MAX_PATH);
		if(i>32 || !*g_szBuffer) return FALSE;
		CString h;
		h.Format(_T("%08X-%s"), hWnd, g_szBuffer);
		AppendMenu(hMenu, MF_BYCOMMAND|MF_STRING, ID_FILE_FIRSTDSEMUTASK+i, h);
	}

	return TRUE;
}

void CT2App::OnUpdateFileFirstdsemutask(CCmdUI *pCmdUI)
{
	int c = 0;
	CT2View *p = theApp.GetCurView();
	if(p && p->GetParent()->IsZoomed()) c = 1;
	HMENU hMenu = GetSubMenu(GetMainWnd()->GetMenu()->m_hMenu,  c);
	hMenu = GetSubMenu(hMenu, GetMenuItemIndex(hMenu, ID_FILE_OPEN)+1);

	c = GetMenuItemCount(hMenu);
	for(int i=0; i<c; i++)
		RemoveMenu(hMenu, ID_FILE_FIRSTDSEMUTASK+i, MF_REMOVE);

	::EnumTaskWindows((HANDLE)NULL, DsEmu, (LPARAM)hMenu);

	c = GetMenuItemCount(hMenu);
	if(c==0)
	{
		CString t; t.LoadString(IDS_NOPROCESS);
		AppendMenu(hMenu, MF_BYCOMMAND|MF_STRING|MF_DISABLED,
			ID_FILE_FIRSTDSEMUTASK, t);
		pCmdUI->Enable(FALSE);
	}
}

void CT2App::OnFirstdsemutask(UINT nID)
{
	int i = nID-ID_FILE_FIRSTDSEMUTASK;

	HMENU hMenu = GetSubMenu(GetMainWnd()->GetMenu()->m_hMenu, 0);
	hMenu = GetSubMenu(hMenu, GetMenuItemIndex(hMenu, ID_FILE_OPEN)+1);

	CString t;
	GetMenuString(hMenu, i, (LPTSTR)t.GetBuffer(_MAX_PATH), _MAX_PATH, MF_BYPOSITION);
	t.ReleaseBuffer();
	HWND hWnd = (HWND)(UINT_PTR)StrToIntEX(t.Left(8), TRUE);

	GetClassName(hWnd, g_szBuffer, _MAX_PATH);
	t.Format(_T("=>%s"), g_szBuffer);

	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	if(!m_pDocManager->GetNextDocTemplate(pos)->
		OpenDocumentFile(t))
		Hint(IDS_PCMEM_ERROR, MB_ICONERROR|MB_OK);
}

void CT2App::OnToolsTbl2cmap()
{
	static CTextFileDialog fd(TRUE, IDS_TBLFILTER);
	if(fd.DoModal()!=IDOK) return;

	CFileDialog fd2(FALSE);
	if(fd2.DoModal()!=IDOK) return;

	CFile file;
	if(!file.Open(fd2.GetPathName(), CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	LPWSTR lpTbl = GetUText(fd.GetPathName(), fd.m_nCodePage);

	if(!lpTbl)
	{
		ShowError();
		return;
	}

	CString strLine;
	LPWSTR lpText = lpTbl;

	unsigned char cmap[] = {0x50,0x41,0x4D,0x43,	// flags
		0x00,0x00,0x00,0x00,	// size
		0x00,0x00,0xFF,0xFF,	// begin-end
		0x02,0x00,0x00,0x00,	// nType / unknown
		0x00,0x00,0x00,0x00,	// offset
		0x00,0x00				// count
	};
	file.Write(cmap, sizeof(cmap));
	UINT wCount = 0;
	while(*lpText)
	{
		strLine = GetLine(lpText);
		if(strLine.GetAt(0)==_T(';') || strLine.IsEmpty()) continue;

		int pos = strLine.Find(_T('='));
		if(pos==-1) continue;
		UINT nCode = StrToIntEX(strLine.Left(pos));
		if(nCode==-1 || strLine.GetLength()<(pos+2)) continue;

		if(pos<=2)
			file.Write(&nCode, 2);
		else
		{
			file.Write(&nCode, 2);
		}
		file.Write(&wCount, 2);

		wCount++;
		if(wCount>0xFFFF) break;
	}
	WORD wEnd = 0;
	file.Write(&wEnd, 2);

	file.Seek(0x14, SEEK_SET);
	file.Write(&wCount, 2);

	UINT nSize = (UINT)file.GetLength();
	file.Seek(4, SEEK_SET);
	file.Write(&nSize, 4);

	delete[] lpTbl;
}

void CT2App::SetCT2Tree(WORD nID)
{
	nID=IDR_TD_TILEVIEW+(nID-ID_VIEW_TILEVIEW);
	if(nID==m_nTreeID) return;

	CT2MainFrm *pMF=(CT2MainFrm*)GetMainWnd();
	CCrystalTreeCtrl &tc = pMF->m_TreeBar.m_Tree;

	// 标准
	HRSRC hRsrc;
	HANDLE hHandle=NULL;
	if(nID!=IDR_TD_SCRIPTVIEW)
	{
		hRsrc=FindResource(NULL, MAKEINTRESOURCE( IDR_TD_ASMVIEW ), _T("CRYSTAL"));
		if(hRsrc)
		{
			HANDLE hRes=LoadResource(NULL, hRsrc);
			CTC *pCtc = (CTC*)LockResource(hRes);
			tc.LoadTree(pCtc);
			UnlockResource(hRes);
			FreeResource(hRes);

			m_nTreeID = nID;
		}
	}else
		tc.DeleteAllItems();
	if(nID==IDR_TD_TILEVIEW || nID==IDR_TD_EDITORVIEW)
	{
		int nTreeID=IDR_TD_TILEVIEW;
		while(TRUE)
		{
			hRsrc=FindResource(NULL, MAKEINTRESOURCE( nTreeID ), _T("CRYSTAL"));
			if(hRsrc)
			{
				HANDLE hRes=LoadResource(NULL, hRsrc);
				CTC *pCtc = (CTC*)LockResource(hRes);
				tc.LoadTree(pCtc, CTC_ACT_EXPANDALL);
				UnlockResource(hRes);
				FreeResource(hRes);
				m_nTreeID = nID;
			}
			if(nTreeID == IDR_TD_EDITORVIEW)
				break;
			nTreeID = IDR_TD_EDITORVIEW;
		}
	}else
	if(nID!=IDR_TD_ASMVIEW)
	{
		hRsrc=FindResource(NULL, MAKEINTRESOURCE( nID ), _T("CRYSTAL"));
		if(hRsrc)
		{
			HANDLE hRes=LoadResource(NULL, hRsrc);
			CTC *pCtc = (CTC*)LockResource(hRes);
			tc.LoadTree(pCtc, CTC_ACT_EXPANDALL);
			UnlockResource(hRes);
			FreeResource(hRes);

			m_nTreeID = nID;
		}
	}
}
