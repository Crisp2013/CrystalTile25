// CT2MainFrm.cpp : CT2MainFrm 类的实现
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include ".\CT2MainFrm.h"
#include "CT2Doc.h"
#include "TextFileDialog.h"
#include "ToolsDlg.h"
#include "DSFirmWareSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CT2MainFrm

IMPLEMENT_DYNAMIC(CT2MainFrm, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CT2MainFrm, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_TOOLS_LZ77COMP, OnToolsLz77comp)
	ON_COMMAND(ID_TOOLS_FILECOMP, OnToolsFilecomp)
	ON_COMMAND(ID_TOOLS_TEXTCPCHANGE, OnToolsTextcpchange)
	ON_CBN_SELCHANGE(IDC_OPTION_CPLIST, OnSetDefCp)
	ON_CBN_SELCHANGE(IDC_OPTION_PALLIST, OnSetDefPal)
	ON_CBN_SELCHANGE(IDC_OPTION_BKLIST, OnSetDefBk)
	ON_CBN_SELCHANGE(IDC_OPTION_OFFSETLIST, OnSetDefOffset)
	ON_UPDATE_COMMAND_UI(IDC_FAVORITES_LIST, OnUpdateFavoritesList)
	ON_UPDATE_COMMAND_UI(IDC_FAVORITES_DESC, OnUpdateFavoritesList)
	ON_UPDATE_COMMAND_UI(ID_FAVORITES_ADD, OnUpdateFavoritesAdd)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FAVORITES_ADD+1, ID_FAVORITES_ADD+0xFF, OnUpdateFavoritesList)
	ON_COMMAND_RANGE(ID_FAVORITES_ADD+1, ID_FAVORITES_ADD+0xFF, OnFavoritesList)
	ON_COMMAND(ID_FAVORITES_ADD, OnFavoritesAdd)
	ON_COMMAND_RANGE(ID_WINDOW_OPTIONTREE, ID_WINDOW_DEFOPTION, OnGotoWindow)
	ON_COMMAND_RANGE(ID_TOOLS_TOOLS, ID_TOOLS_TOOLS+32, OnToolsTools)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TOOLS, OnUpdateToolsTools)
	ON_COMMAND(ID_VIEW_TOOLEX, OnViewToolex)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLEX, OnUpdateViewToolex)
	ON_COMMAND(ID_VIEW_OPTIONBAR, OnViewOptionbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIONBAR, OnUpdateViewOptionbar)
	ON_COMMAND(ID_ACCEL_BREAK, OnBreak)
	ON_COMMAND(ID_TOOLS_NDS_FIRMWARE, OnToolsNdsFirmware)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_PROGRESSBAR,
	IDC_HEXVIEW_BITS,
	IDS_LINECOL,
	ID_TILEEDITOR_OFFSET,
	IDS_OPENMODE,
	ID_ROMINFO
};

// CT2MainFrm 构造/析构

CT2MainFrm::CT2MainFrm()
{

}

CT2MainFrm::~CT2MainFrm()
{
	if(m_FileComp.GetSafeHwnd())
		m_FileComp.DestroyWindow();
	SaveToolList();
}

int CT2MainFrm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this,
		TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME) )
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	m_wndToolBar.SetWindowText(LoadStr(IDS_TOOLBAR));

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	/*m_OptionBar.Create(this, COptionBar::IDD,
		WS_CHILD|CBRS_ALIGN_BOTTOM|CBRS_TOOLTIPS,
		AFX_IDW_CONTROLBAR_LAST-4);

	m_ExplorerBar.Create(this, CExplorerBar::IDD,
		WS_CHILD|CBRS_ALIGN_RIGHT|CBRS_TOOLTIPS,
		AFX_IDW_CONTROLBAR_LAST-3);*/

	m_TreeBar.Create(this, WS_CHILD|CBRS_ALIGN_LEFT|CBRS_TOOLTIPS|WS_VISIBLE,
		AFX_IDW_CONTROLBAR_LAST-2);

	// TODO: 如果不需要工具栏可停靠，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_TreeBar.EnableDocking(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
	m_TreeBar.SetWindowText(LoadStr(IDS_OPTIONTREE));

	CImageList img; // ??????
	img.Create(IDR_MAINFRAME, 16, 0, RGB(192, 192, 192));
	m_wndToolBar.GetToolBarCtrl().SetImageList(&img);
	img.Detach();
	
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_TreeBar);

	
	LoadToolList();

	LoadBarState(_T("CT2_BAR"));

	return 0;
}

BOOL CT2MainFrm::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	cs.dwExStyle|=WS_EX_ACCEPTFILES;
	return TRUE;
}

// CT2MainFrm 诊断

#ifdef _DEBUG
void CT2MainFrm::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CT2MainFrm::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CT2MainFrm 消息处理程序

LRESULT CT2MainFrm::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_OPENFILE:
		{
			CString FileName;
			CWnd::FromHandle(m_hWnd)->GetWindowText(FileName);
			AfxGetApp()->OpenDocumentFile(FileName);
			return 0;
		}break;
	case WM_SYSCOMMAND:
		switch(wParam)
		{
		case SC_CLOSE:
			{
				POSITION pos = theApp.GetFirstDocTemplatePosition();
				CDocTemplate *pDt = theApp.GetNextDocTemplate(pos);
				pos = pDt->GetFirstDocPosition();
				while(pos)
				{
					CT2Doc* pDoc = (CT2Doc*)pDt->GetNextDoc(pos);
					if(pDoc->m_hMapFile)
						if(!pDoc->CanCloseFrame(this))
							return 0;
				}
				SaveBarState(_T("CT2_BAR"));
				WINDOWPLACEMENT wp;
				GetWindowPlacement(&wp);
				MAINFRMPOS mp;
				mp.rcMainFrm = wp.rcNormalPosition;
				mp.bShowMax = GetStyle()&WS_MAXIMIZE?1:0;
				theApp.WriteProfileBinary(CT2Settings, _T("MainFrmWP"), (BYTE*)&mp, sizeof(MAINFRMPOS));
			}
		}break;
	}

	return CMDIFrameWnd::DefWindowProc(message, wParam, lParam);
}

BOOL CT2MainFrm::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle , CWnd* pParentWnd , CCreateContext* pContext)
{
	m_nIDHelp = nIDResource;    // ID for help context (+HID_BASE_RESOURCE)

	/*CString strFullString;
	if (strFullString.LoadString(nIDResource))
		AfxExtractSubString(m_strTitle, strFullString, 0);    // first sub-string
	*/
	m_strTitle.LoadString(nIDResource);

	// VERIFY(AfxDeferRegisterClass(AFX_WNDFRAMEORVIEW_REG));

	// attempt to create the window
	HINSTANCE hInst = AfxFindResourceHandle(
		MAKEINTRESOURCE(nIDResource), RT_GROUP_ICON);
	HICON hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(nIDResource));

	if (hIcon != NULL)
	{
		CREATESTRUCT cs;
		memset(&cs, 0, sizeof(CREATESTRUCT));
		cs.style = dwDefaultStyle;
		PreCreateWindow(cs);
			// will fill lpszClassName with default WNDCLASS name
			// ignore instance handle from PreCreateWindow.

		WNDCLASS wndcls;
		if (cs.lpszClass != NULL &&
			GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndcls) &&
			wndcls.hIcon != hIcon)
		{
			// register a very similar WNDCLASS
			// WNDCLASS wndcls;
			if (!::GetClassInfo(hInst, m_strTitle, &wndcls))
			{
				wndcls.lpfnWndProc = ::DefWindowProc;
				// wndcls.cbClsExtra = 0;
				wndcls.hInstance = hInst;
				wndcls.hIcon = hIcon;
				// wndcls.hCursor = hCursor;
				// wndcls.hbrBackground = hbrBackground;
				// wndcls.lpszMenuName = NULL;
				wndcls.lpszClassName = m_strTitle;
				if (!AfxRegisterClass(&wndcls))
					AfxThrowResourceException();
			}
		}
	}

	if (!Create(m_strTitle, m_strTitle, dwDefaultStyle, rectDefault,
	  pParentWnd, MAKEINTRESOURCE(nIDResource), 0L, pContext))
	{
		return FALSE;   // will self destruct on failure normally
	}

	// save the default menu handle
	ASSERT(m_hWnd != NULL);
	m_hMenuDefault = ::GetMenu(m_hWnd);

	// load accelerator resource
	LoadAccelTable(MAKEINTRESOURCE(nIDResource));

	#define WM_INITIALUPDATE    0x0364  // (params unused) - sent to children
	if (pContext == NULL)   // send initial update
		SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

	return TRUE;
	// return CMDIFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
}

void CT2MainFrm::OnDropFiles(HDROP hDropInfo)
{
	UINT nFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	// SetFocus();
	for(UINT i=0; i<nFileCount; i++)
	{
		::DragQueryFile(hDropInfo, i, g_szBuffer, _MAX_PATH);
		theApp.OpenDocumentFile(g_szBuffer);
	}

	CMDIFrameWnd::OnDropFiles(hDropInfo);
}

void CT2MainFrm::OnToolsLz77comp()
{
	static CFileDialog fd(TRUE);
	CFile AgbComp;
	CString hint, Entry=_T("AgbComp"),
		strAgbComp = theApp.GetProfileString(CT2Settings, Entry);

	if(!AgbComp.Open(strAgbComp,CFile::modeRead|CFile::typeBinary))
	{
		if(Hint(IDS_AGBCOMP, MB_YESNO|MB_ICONQUESTION)==IDYES)
		{
SelAgbComp:
			CString filter;filter.LoadString(IDS_EXEC);
			filter.AppendChar(_T('\0'));
			fd.m_ofn.lpstrFilter = filter;
			do
			{
				if(fd.DoModal()!=IDOK) return;
				strAgbComp = fd.GetPathName();
				hint = PathFindExtension(strAgbComp);
				hint.MakeLower();
			}while(hint!=_T(".exe"));
		}else
		{
			HRSRC hRsrc=FindResource(NULL,
				MAKEINTRESOURCE(IDR_AGBCOMP), _T("CRYSTAL"));
			if(hRsrc)
			{
				HGLOBAL hAgbcomp=LoadResource(NULL, hRsrc);
				BYTE* pAgbcomp=(BYTE*)LockResource(hAgbcomp);
				BYTE *pAgb; int nLzSize, nBinSize;
				Lz77uncomp(pAgbcomp, &pAgb, nLzSize, nBinSize);

				LPTSTR p = strAgbComp.GetBuffer(_MAX_PATH);
				GetModuleFileName(NULL, p, _MAX_PATH);
				int pos = strAgbComp.ReverseFind(_T('\\'));
				lstrcpy(p+pos, _T("\\Agbcomp.exe"));
				strAgbComp.ReleaseBuffer();
				CFile file;
				BOOL bOK;
				if(bOK=file.Open(strAgbComp, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
					file.Write(pAgb, nBinSize);
				else
					ShowError();
	
				delete[] pAgb;
				UnlockResource(hAgbcomp);
				FreeResource(hAgbcomp);
				if(!bOK) goto SelAgbComp;
			}
		}
		fd.m_pOFN->lpstrFile[0]=0;
		theApp.WriteProfileString(CT2Settings, Entry, strAgbComp);
	}else
		AgbComp.Close();

	hint.LoadString(IDS_ALLFILE);
	hint.AppendChar(_T('\0'));
	fd.m_ofn.lpstrFilter = hint;
	if(fd.DoModal()!=IDOK) return;
	hint = fd.GetPathName();
	CString line;
	line.Format(_T("-b -l 2 \"%s\""), hint);
	if(!exeProgram(strAgbComp, line))
		return;

	hint = fd.GetFileName();
	int nPos = hint.ReverseFind(_T('.'));
	CString Append = _T("_LZ.bin");
	if(nPos!=-1)
	{
		hint = fd.GetPathName();
		nPos = hint.ReverseFind(_T('.'));
		hint = hint.Left(nPos)+Append;
	}else
		hint.Append(Append);

	CFile file;
	if(file.Open(hint,CFile::modeRead|CFile::typeBinary))
	{
		int nFileSize = (int)file.GetLength();
		BYTE *pRes = NULL, *pDst=NULL;
		pRes = new BYTE[nFileSize];
		if(pRes)
		{
			file.Read(pRes, nFileSize);
			file.Close();
			int nLzSize,nBinSize;
			if(Lz77uncomp(pRes, &pDst, nLzSize, nBinSize))
			{
				if(nFileSize>nLzSize)
				{
					if(file.Open(hint, CFile::modeWrite|CFile::modeCreate|CFile::typeBinary))
					{
						file.Write(pRes, nLzSize);
						file.Close();
					}
				}
			}
			if(pDst) {delete[] pDst;pDst=NULL;}
			delete[] pRes;
		}else file.Close();
	}
	if(Hint(IDS_COMP, MB_YESNO|MB_ICONINFORMATION)==IDYES)
		theApp.OpenDocumentFile(hint);
}

void CT2MainFrm::OnToolsFilecomp()
{
	if(m_FileComp.GetSafeHwnd()==NULL)
		m_FileComp.Create(CFileCompDlg::IDD, this);
	m_FileComp.ShowWindow(SW_NORMAL);
	m_FileComp.SetFocus();
}

void CT2MainFrm::OnToolsTextcpchange()
{
	static CTextFileDialog fd(TRUE, IDS_TBLFILTER);
	CString filter;
	if(fd.DoModal()!=IDOK) return;

	static CTextFileDialog fds(FALSE, IDS_TBLFILTER);
	if(fds.DoModal()!=IDOK) return;

	filter=fds.GetPathName();
	PathAddExtension(filter.GetBuffer(_MAX_PATH),  _T(".txt"));
	filter.ReleaseBuffer();

	CStdioFile file1, file2;
	if( !file1.Open(fd.GetPathName(),
		CStdioFile::modeRead|CStdioFile::typeBinary) ||
		!file2.Open(filter,
		CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::typeBinary) )
	{
		ShowError();
		return;
	}

	UINT nfile1Size = (UINT)file1.GetLength();
	BYTE *pfile1 = new BYTE[nfile1Size];
	file1.Read(pfile1, nfile1Size);
	BYTE *pFile1 = pfile1;

	UINT nfile2Size=nfile1Size;
	if(fds.m_nCodePage>=1200 || fd.m_nCodePage<1200)
		nfile2Size<<=1;

	BYTE *pfile2 = new BYTE[nfile2Size];
	if(*(WORD*)pFile1!=0xFEFF && *(WORD*)pFile1!=0xFFFE)
	{
		nfile2Size = MultiByteToWideChar(fd.m_nCodePage, 0,
			(LPCSTR)pFile1, nfile1Size,
			(LPWSTR)pfile2, nfile2Size);
		nfile2Size<<=1;
		if(fds.m_nCodePage>=1200)
		{
			pFile1=pfile2;
			nfile1Size=nfile2Size;
			goto wchar;
		}
	}else
	{
		if(nfile1Size<=2) goto End;
		pFile1+=2;
		nfile1Size-=2;
		wchar:
		if(fds.m_nCodePage==1200)
		{
			UINT nHeader=0xFEFF;
			file2.Write(&nHeader, 2);
			file2.Write(pFile1, nfile1Size);
			goto End;
		}else
		if(fds.m_nCodePage==1201)
		{
			UINT nHeader=0xFFFE;
			file2.Write(&nHeader, 2);
			nfile1Size&=~1;
			WORD *pw = (WORD*)pFile1;
			for(UINT i=0; i<nfile1Size; i+=2)
			{
				*pw++ = (*pw>>8|*pw<<8);
			}
			file2.Write(pFile1, nfile1Size);
			goto End;
		}
		memcpy(pfile2, pFile1, nfile1Size);
		nfile2Size = nfile1Size;
	}

	CPINFO cpi;
	GetCPInfo(fds.m_nCodePage, &cpi);
	BOOL bUsed=FALSE;
	nfile1Size = WideCharToMultiByte(fds.m_nCodePage, 0,
		(LPCWSTR)pfile2, nfile2Size/2,
		(LPSTR)pFile1, nfile1Size,
		fds.m_nCodePage==65001?NULL:(LPCSTR)cpi.DefaultChar, fds.m_nCodePage==65001?NULL:&bUsed);
	if(bUsed)
		Hint(IDS_INVALIDCHAR, MB_OK|MB_ICONERROR);
	file2.Write(pFile1, nfile1Size);

End:
	delete[] pfile1;
	delete[] pfile2;
}

void CT2MainFrm::OnSetDefCp()
{
	CComboBox *pCB = (CComboBox*)theApp.GetOptionBox().GetDlgItem(IDC_OPTION_CPLIST);
	theApp.WriteProfileInt(CT2Settings, _T("DefCp"), pCB->GetCurSel());
}

void CT2MainFrm::OnSetDefPal()
{
	CComboBox *pCB = (CComboBox*)theApp.GetOptionBox().GetDlgItem(IDC_OPTION_PALLIST);
	theApp.WriteProfileInt(CT2Settings, CT2DefPal, pCB->GetCurSel());
}

void CT2MainFrm::OnSetDefBk()
{
	CComboBox *pCB = (CComboBox*)theApp.GetOptionBox().GetDlgItem(IDC_OPTION_BKLIST);
	theApp.WriteProfileInt(CT2Settings, CT2DefBk, pCB->GetCurSel());
}

void CT2MainFrm::OnSetDefOffset()
{
	CComboBox *pCB = (CComboBox*)theApp.GetOptionBox().GetDlgItem(IDC_OPTION_OFFSETLIST);
	theApp.WriteProfileInt(CT2Settings, CT2DefOffset, pCB->GetCurSel());
}

void CT2MainFrm::OnUpdateFavoritesList(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
	int nItem = pCmdUI->m_nID-ID_FAVORITES_ADD-1;
	POSITION pos = g_Favorites.FindIndex(nItem);
	if(nItem>=0 && pos!=NULL)
	{
		FAVORITES &fav = g_Favorites.GetAt(pos);
		pCmdUI->Enable(PathFileExists(fav.Doc));
	}
}

void CT2MainFrm::OnUpdateFavoritesAdd(CCmdUI *pCmdUI)
{
	UINT nType=0;
	CT2View *pView = theApp.GetCurView(&nType);
	char nMax = pView && pView->GetDocument()->GetLastView()->GetParent()->IsZoomed()?1:0;
	HMENU hFav;
	switch(nType)
	{
	case ID_VIEW_TILEVIEW:
		hFav = GetSubMenu(theApp.m_hTileMenu, 4+nMax);
		break;
	case ID_VIEW_HEXVIEW:
		hFav = GetSubMenu(theApp.m_hHexMenu, 6+nMax);
		break;
	case ID_VIEW_ASMVIEW:
		hFav = GetSubMenu(theApp.m_hAsmMenu, 4+nMax);
		break;
	case ID_VIEW_TILEEDITORVIEW:
		hFav = GetSubMenu(theApp.m_hTileEditorMenu, 4+nMax);
		break;
	default:
		if(pView!=NULL) return;
		hFav = GetSubMenu(m_hMenuDefault, 3);
		break;
	}
	int c = (int)g_Favorites.GetCount();
	int c2 = GetMenuItemCount(hFav)-2;
	if(c2>c) c = c2;
	for(int i=0; i<c; i++)
		RemoveMenu(hFav, ID_FAVORITES_ADD+1+i, MF_REMOVE|MF_BYCOMMAND);
	RemoveMenu(hFav, ID_FAVORITES_ADD+1, MF_REMOVE|MF_BYCOMMAND);	// SEP

	CString strFav;
	c2 = (int)g_Favorites.GetCount();

	if(c2!=0)	// SEP
		AppendMenu(hFav, MF_SEPARATOR|MF_BYCOMMAND, ID_FAVORITES_ADD+1, _T("-"));

	for(int i=0; i<c2; i++)
	{
		FAVORITES &fav = g_Favorites.GetAt(g_Favorites.FindIndex(i));
		if(i<10)
			strFav.Format(_T("&%d - %s"), i, fav.Desc);
		else
		if(i<(10+26))
			strFav.Format(_T("&%c - %s"), 0x37+i, fav.Desc);
		else
			strFav.Format(_T("%s"), fav.Desc);
		AppendMenu(hFav, MF_STRING|MF_BYCOMMAND,
			ID_FAVORITES_ADD+1+i, strFav);
	}
}

void CT2MainFrm::OnFavoritesList(UINT nID)
{
	m_TreeBar.m_wndFav.Goto(nID-(ID_FAVORITES_ADD+1));
}

void CT2MainFrm::OnFavoritesAdd()
{
	m_TreeBar.m_Tab.SetCurFocus(2);
	theApp.GetFavorite().GetDlgItem(IDC_FAVORITES_DESC)->SetFocus();
}

void CT2MainFrm::OnGotoWindow(UINT nID)
{
	switch(nID)
	{
	case ID_WINDOW_OPTIONTREE:
		m_TreeBar.m_Tab.SetCurFocus(0);
		m_TreeBar.m_Tree.SetFocus();
		break;

	case ID_WINDOW_PALOPTION:
		m_TreeBar.m_Tab.SetCurFocus(1);
		//m_TreeBar.m_wndPal.GetDlgItem(IDC_DEFPALBTN)->SetFocus();
		break;
	case ID_WINDOW_DEFOPTION:
		m_TreeBar.m_Tab.SetCurFocus(3);
		//m_TreeBar.m_wndDefOption.GetDlgItem(IDC_OPTION_CPLIST)->SetFocus();
		break;
	}
	if(!m_TreeBar.IsVisible())
		ShowControlBar(&m_TreeBar, TRUE, FALSE);
}

void CT2MainFrm::OnToolsTools(UINT nID)
{
	if(nID==ID_TOOLS_TOOLS)
	{
		CToolsDlg td;
		td.DoModal();
		return;
	}
	int nSel = nID-ID_TOOLS_TOOLS-1;
	CString tmp = m_ToolList.GetAt(nSel);
	CString command, r;
	AfxExtractSubString(command, tmp, 1);
	AfxExtractSubString(r, tmp, 2);

	::ShellExecute(0, _T("open"), command, r, 0, SW_SHOWNORMAL);
}

void CT2MainFrm::LoadToolList()
{
	CString t;
	int i=0;
	while(TRUE)
	{
		t.Format(_T("Tool%d"), i++);
		CString tool = theApp.GetProfileString(CT2ToolList, t);
		if(tool.IsEmpty()) break;

		tool.Replace(_T('\t'), _T('\n'));
		m_ToolList.Add(tool);
	}
	LoadToolBarEx();
}

void CT2MainFrm::SaveToolList()
{
	CString t, t2;
	for(int i=0; i<m_ToolList.GetCount(); i++)
	{
		t.Format(_T("Tool%d"), i);
		t2 = m_ToolList.GetAt(i);
		t2.Replace(_T('\n'), _T('\t'));
		theApp.WriteProfileString(CT2ToolList, t, t2);
	}
	//侨胶
	/*while (TRUE)
	{
		t.Format(_T("Tool%d"), i);
		CString tool = theApp.GetProfileString(CT2ToolList, t);
		if(tool.IsEmpty()) break;
		theApp.WriteProfileString(CT2ToolList, t, NULL);
		i++;
	}*/
}

void CT2MainFrm::OnUpdateToolsTools(CCmdUI *pCmdUI)
{
	if(pCmdUI->m_pMenu)
	{
		int i = GetMenuItemIndex(pCmdUI->m_pMenu->GetSafeHmenu(), ID_TOOLS_TOOLS)+1;

		UINT c = pCmdUI->m_pMenu->GetMenuItemCount();
		for(UINT i2=i; i2<c; i2++)
		{
			pCmdUI->m_pMenu->RemoveMenu(i, MF_BYPOSITION);
		}
		for(int i3=1; i3<=m_ToolList.GetCount(); i3++)
		{
			CString text = m_ToolList.GetAt(i3-1);
			CString title;
			AfxExtractSubString(title, text, 0);
			pCmdUI->m_pMenu->AppendMenu(MF_STRING|MF_BYCOMMAND, ID_TOOLS_TOOLS+i3, title);
		}
	}
}

void CT2MainFrm::LoadToolBarEx()
{
	CString t;
	if(!m_wndToolBarEx.GetSafeHwnd())
	{
		m_wndToolBarEx.CreateEx(this,
		TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_LEFT
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
		CRect(0,0,0,0), ID_TOOLS_TOOLS);

		t.LoadString(IDS_TOOLBAREX);
		m_wndToolBarEx.SetWindowText(t);
		m_wndToolBarEx.EnableDocking(CBRS_ALIGN_ANY);
		DockControlBar(&m_wndToolBarEx);
	}

	int i = (int)m_ToolList.GetCount();
	if(i==0)
	{
		int nCount = (int)m_wndToolBarEx.SendMessage(TB_BUTTONCOUNT, 0, 0);
		while (nCount--)
			VERIFY(m_wndToolBarEx.SendMessage(TB_DELETEBUTTON, 0, 0));
		ShowControlBar(&m_wndToolBarEx, FALSE, FALSE);
		return;
	}

	UINT *btn = new UINT[i];
	for(int b=0; b<i; b++)
	{
		btn[b]=b+ID_TOOLS_TOOLS+1;
	}
	m_wndToolBarEx.SetButtons(btn, i);
	delete[] btn;

	HBITMAP hImageWell = CreateBitmap(i*16, 16, 1, 24, NULL);

	HDC hDC = ::CreateCompatibleDC(NULL);
	SelectObject(hDC, hImageWell);
	CString p;
	for(int b=0; b<i; b++)
	{
		t = m_ToolList.GetAt(b);
		AfxExtractSubString(p, t, 1);
		int x = GetIconIndex(p, SHGFI_SMALLICON);
		ImageList_DrawEx(g_hSysImageList, x, hDC,
			b*16, 0, 16, 16, ::GetSysColor(COLOR_BTNFACE), 0, 0);
	}
	DeleteDC(hDC);
	m_wndToolBarEx.SetSizes(CSize(16+7, 16+7), CSize(16, 16));
	m_wndToolBarEx.AddReplaceBitmap(hImageWell);

	m_wndToolBarEx.GetParentFrame()->RecalcLayout();
}

void CT2MainFrm::OnViewToolex()
{
	ShowControlBar(&m_wndToolBarEx, ~m_wndToolBarEx.IsVisible(), FALSE);
}

void CT2MainFrm::OnUpdateViewToolex(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndToolBarEx.IsVisible());
}

void CT2MainFrm::OnViewOptionbar()
{
	ShowControlBar(&m_TreeBar, ~m_TreeBar.IsVisible(), FALSE);
}

void CT2MainFrm::OnUpdateViewOptionbar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_TreeBar.IsVisible());
}

void CT2MainFrm::OnBreak()
{
	g_bBreak = TRUE;
}

void CT2MainFrm::OnToolsNdsFirmware()
{
	CDSFirmWareSettings fw;
	fw.DoModal();
}
