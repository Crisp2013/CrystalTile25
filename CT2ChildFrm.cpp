// CT2ChildFrm.cpp : CT2ChildFrm 类的实现
//
#include "stdafx.h"
#include "CrystalTile2.h"

#include "CT2Doc.h"
#include ".\CT2ChildFrm.h"

#include "CT2TileView.h"
#include "CT2HexView.h"
#include "CT2AsmView.h"
#include "CT2TileEditorView.h"
#include "CT2ScriptView.h"

#include "CT2Palette.h"
#include "CT2MainFrm.h"

#include "SDatInfo.h"
#include "PacFileInfo.h"
#include "UtilityFileInfo.h"
#include "NARCFileInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CT2ChildFrm

IMPLEMENT_DYNCREATE(CT2ChildFrm, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CT2ChildFrm, CMDIChildWnd)
	ON_COMMAND_RANGE(CT2_ID_VIEW_FIRST, CT2_ID_VIEW_LAST, SetCurView)
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI_RANGE(CT2_ID_VIEW_FIRST, CT2_ID_VIEW_LAST, OnUpdateSetView)
	ON_WM_DESTROY()
	//ON_COMMAND(ID_TTB_PAL, SetPalette)
	ON_WM_ACTIVATE()
	ON_WM_KEYDOWN()
	ON_COMMAND_RANGE(ID_SETCOLOR_TEXTCOLOR, ID_SETCOLOR_SHADOWCOLOR, SetColor)
	ON_COMMAND(ID_TOOLS_NDSFSI, OnToolsNdsfsi)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_NDSFSI, OnUpdateToolsNdsfsi)
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_VIEW_BK1, ID_VIEW_BK7, OnViewBkBrush)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_BK1, ID_VIEW_BK7, OnUpdateViewBkBrush)
	ON_COMMAND(ID_TOOLS_SFI, OnSFI)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SFI, OnUpdateSFI)
	ON_COMMAND_RANGE(ID_FSI_GOARM9, ID_FSI_GOARM7, OnFsiGoPC)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FSI_GOARM9, ID_FSI_GOARM7, OnUpdateToolsNdsfsi)
	/*ON_COMMAND(ID_TOOLS_MACRO_START, OnToolsMacroStart)
	ON_COMMAND(ID_TOOLS_MACRO_PAUSE, OnToolsMacroPause)
	ON_COMMAND(ID_TOOLS_MACRO_CANCEL, OnToolsMacroCancel)
	ON_COMMAND_RANGE(ID_TOOLS_MACRO_SAVE, ID_TOOLS_MACRO_SAVEALL, OnToolsMacroSave)
	ON_COMMAND(ID_TOOLS_MACRO_RUN, OnToolsMacroRun)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MACRO_START, OnUpdateToolsMacroStart)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MACRO_PAUSE, OnUpdateToolsMacroPause)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MACRO_CANCEL, OnUpdateToolsMacroCancel)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TOOLS_MACRO_SAVE, ID_TOOLS_MACRO_SAVEALL, OnUpdateToolsMacroSave)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MACRO_RUN, OnUpdateToolsMacroRun)*/
END_MESSAGE_MAP()

// CT2ChildFrm 构造/析构

CT2ChildFrm::CT2ChildFrm()
: m_pTileView(NULL)
, m_pHexView(NULL)
, m_pAsmView(NULL)
, m_pScriptView(NULL)
, m_nCurView(ID_VIEW_TILEVIEW)
, m_nOldView(-1)
//, m_nMacroStatus(CT_MACRO_CLOSE)
, m_pSFI(NULL)
{
	// TODO: 在此添加成员初始化代码
}

CT2ChildFrm::~CT2ChildFrm()
{
	if(m_NdsFSI.GetSafeHwnd()!=NULL)
		m_NdsFSI.DestroyWindow();
	if(m_pSFI)
	{
		m_pSFI->DestroyWindow();
		delete m_pSFI;
	}
}

/*
BOOL CT2ChildFrm::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}
*/

// CT2ChildFrm 诊断

#ifdef _DEBUG
void CT2ChildFrm::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CT2ChildFrm::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


// CT2ChildFrm 消息处理程序

CT2View* CT2ChildFrm::GetCurView(UINT* pType)
{
	CT2View* pView=NULL;
	if(pType) *pType = m_nCurView;
	switch(m_nCurView)
	{
	default:
		if(pType) *pType = -1;
		break;
	case ID_VIEW_TILEVIEW:
		pView = m_pTileView;
		break;
	case ID_VIEW_HEXVIEW:
		pView = m_pHexView;
		break;
	case ID_VIEW_ASMVIEW:
		pView = m_pAsmView;
		break;
	case ID_VIEW_TILEEDITORVIEW:
		pView = m_pTileEditorView;
		break;
	case ID_VIEW_SCRIPTVIEW:
		pView = m_pScriptView;
		break;
	}
	return pView;
}

void CT2ChildFrm::SetCurView(UINT nView)
{
	if(nView<CT2_ID_VIEW_FIRST || nView>CT2_ID_VIEW_LAST)
		//nView = CT2_ID_VIEW_FIRST;
		return;
	if(m_nCurView==nView && m_nOldView!=-1)
	{
		if(nView==ID_VIEW_HEXVIEW)
			SetCurView(m_nOldView);
		return;
	}

	UINT nOld = m_nOldView;
	if(m_nCurView!=ID_VIEW_HEXVIEW)
		m_nOldView = m_nCurView;

	CT2View* pViewActive = GetCurView();

	if(!pViewActive) return;

	UINT nSwpOff = m_nCurView==ID_VIEW_TILEEDITORVIEW?
		m_pTileView->m_nOffset:pViewActive->m_nOffset;
	pViewActive->SetWindowPos(NULL,
		0, 0, 0, 0, SWP_HIDEWINDOW|SWP_NOSIZE|SWP_NOMOVE);

	// SwpView
	m_nCurView = nView;

	CRect rc;	GetClientRect(&rc);

	pViewActive = GetCurView();

	if(pViewActive)
	{
		pViewActive->SetWindowPos(NULL,
			rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);

		SetActiveView(pViewActive);

		CComboBox *pCB = (CComboBox*)theApp.GetOptionBox().GetDlgItem(IDC_OPTION_OFFSETLIST);
		if(pCB->GetCurSel()==0 && m_nOldView!=ID_VIEW_SCRIPTVIEW && nOld!=-1)
		{
			if(!( pViewActive->GetDocument()->m_nRomType==CT2_RT_NSCR &&
				m_nCurView==ID_VIEW_TILEVIEW ))
				pViewActive->ScrollTo(nSwpOff);
		}
	}
	HMENU hMenu=((CT2MainFrm*) theApp.m_pMainWnd)->m_hMenuDefault;
	HACCEL hAccel=((CT2MainFrm*) theApp.m_pMainWnd)->m_hAccelTable;
	switch(m_nCurView)
	{
	case ID_VIEW_TILEVIEW:
		hMenu=theApp.m_hTileMenu;
		hAccel=theApp.m_hTileAccelTable;
		break;
	case ID_VIEW_HEXVIEW:
		if(theApp.m_hHexMenu!=NULL)
			hMenu = theApp.m_hHexMenu;
		if(theApp.m_hHexAccelTable!=NULL)
			hAccel = theApp.m_hHexAccelTable;
		break;
	case ID_VIEW_ASMVIEW:
		if(theApp.m_hAsmMenu!=NULL)
			hMenu = theApp.m_hAsmMenu;
		if(theApp.m_hAsmAccelTable!=NULL)
			hAccel = theApp.m_hAsmAccelTable;
		break;
	case ID_VIEW_TILEEDITORVIEW:
		if(theApp.m_hTileEditorMenu!=NULL)
			hMenu = theApp.m_hTileEditorMenu;
		if(theApp.m_hTileEditorAccelTable!=NULL)
			hAccel = theApp.m_hTileEditorAccelTable;
		break;
	case ID_VIEW_SCRIPTVIEW:
		if(theApp.m_hScriptMenu!=NULL)
			hMenu = theApp.m_hScriptMenu;
		if(theApp.m_hScriptAccelTable!=NULL)
			hAccel = theApp.m_hScriptAccelTable;
		break;
	}
	SetHandles(hMenu, hAccel);
	GetMDIFrame()->OnUpdateFrameMenu(hMenu);
	GetMDIFrame()->DrawMenuBar();
}

void CT2ChildFrm::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);

	CT2View* pView = GetCurView();
	if(pView)
	{
		pView->SetWindowPos(NULL, 0, 0, cx, cy,
			SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
	}
}

BOOL CT2ChildFrm::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	BOOL bRET = CMDIChildWnd::OnCreateClient(lpcs, pContext);

	m_pTileView = (CT2View*)((CT2Doc*)pContext->m_pCurrentDoc)->GetLastView();

	m_pHexView = new CT2HexView;
	if (!m_pHexView->Create(NULL, NULL, WS_CHILD | WS_BORDER,
		CRect(0,0,0,0), this,
		AFX_IDW_PANE_FIRST+1, pContext))
		return FALSE;        // can't continue without a view

	m_pAsmView = new CT2AsmView;
	if (!m_pAsmView->Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS|WS_CLIPCHILDREN | WS_BORDER,
		CRect(0,0,0,0), this,
		AFX_IDW_PANE_FIRST+2, pContext))
		return FALSE;        // can't continue without a view

	m_pTileEditorView = new CT2TileEditorView;
	if (!m_pTileEditorView->Create(NULL, NULL, WS_CHILD | WS_BORDER,
		CRect(0,0,0,0), this,
		AFX_IDW_PANE_FIRST+3, pContext))
		return FALSE;        // can't continue without a view

	m_pScriptView = new CT2ScriptView;
	if (!m_pScriptView->Create(NULL, NULL, WS_CHILD | WS_BORDER,
		CRect(0,0,0,0), this,
		AFX_IDW_PANE_FIRST+4, pContext))
		return FALSE;        // can't continue without a view

	//pContext->m_pCurrentDoc->AddView(m_pHexView);
	//pContext->m_pCurrentDoc->AddView(m_pAsmView);
	//pContext->m_pCurrentDoc->AddView(m_pTileEditorView);

	return bRET;
}

void CT2ChildFrm::OnUpdateSetView(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_nCurView==pCmdUI->m_nID);
}

void CT2ChildFrm::OnDestroy()
{
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	UINT nIndex = theApp.GetMRUIndex(m_pTileView->GetDocument()->GetPathName());
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	if(pSD)
	{
		pSD->wpChildFrm.rcMainFrm = wp.rcNormalPosition;
		pSD->wpChildFrm.bShowMax = GetStyle()&WS_MAXIMIZE?1:0;
		pSD->nStateMapFile = ((CT2Doc*)m_pTileView->GetDocument())->m_hMapFile?1:0;
		pSD->nCurView = m_nCurView;
	}

	CMDIChildWnd::OnDestroy();
}

void CT2ChildFrm::ActivateFrame(int nCmdShow)
{
	UINT nIndex = theApp.GetMRUIndex(m_pTileView->GetDocument()->GetPathName());
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	if( pSD &&
		theApp.m_pRecentStatList->IsOK(nIndex))
	{
		WINDOWPLACEMENT wp;
		GetWindowPlacement(&wp);
		wp.rcNormalPosition = pSD->wpChildFrm.rcMainFrm;
		wp.showCmd=SW_HIDE;
		SetWindowPlacement(&wp);

		SetCurView(pSD->nCurView);

		nCmdShow = pSD->wpChildFrm.bShowMax?SW_MAXIMIZE:nCmdShow;
	}

	CMDIChildWnd::ActivateFrame(nCmdShow);
}
/*
void CT2ChildFrm::SetPalette(void)
{
	CT2Palette& PalDlg = theApp.GetPalDlg();
	PalDlg.DoModal();
}
*/
void CT2ChildFrm::SetColor(UINT nID)
{
	static CColorDialog cd(0, CC_RGBINIT);
	CT2TileEditorView *pEditor = (CT2TileEditorView*)m_pTileEditorView;
	char bWColor= ((CT2TileView*)m_pTileView)->m_nBitCount==16;
	switch(nID)
	{
	case ID_SETCOLOR_TEXTCOLOR:
		cd.m_cc.rgbResult = bWColor?WordToColor(pEditor->m_nTextColor):pEditor->m_nTextColor;
		break;
	case ID_SETCOLOR_BKCOLOR:
		cd.m_cc.rgbResult = bWColor?WordToColor(pEditor->m_nBkColor):pEditor->m_nBkColor;
		break;
	case ID_SETCOLOR_SHADOWCOLOR:
		cd.m_cc.rgbResult = bWColor?WordToColor(pEditor->m_nShadowColor):pEditor->m_nShadowColor;
		break;
	}
	if(cd.DoModal()==IDOK)
	{
		if(bWColor)
			cd.m_cc.rgbResult = ColorToWord(cd.m_cc.rgbResult);
		switch(nID)
		{
		case ID_SETCOLOR_TEXTCOLOR:
            pEditor->m_nTextColor = cd.GetColor();
			break;
		case ID_SETCOLOR_BKCOLOR:
            pEditor->m_nBkColor = cd.GetColor();
			break;
		case ID_SETCOLOR_SHADOWCOLOR:
            pEditor->m_nShadowColor = cd.GetColor();
			break;
		}
		pEditor->OnUpdateData();
	}
}

void CT2ChildFrm::OnToolsNdsfsi()
{
	m_NdsFSI.ShowWindow(SW_NORMAL);
	m_NdsFSI.SetFocus();
}

void CT2ChildFrm::OnUpdateToolsNdsfsi(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_pTileView->GetDocument()->m_nRomType==CT2_RT_NDS );
}

void CT2ChildFrm::OnSetFocus(CWnd* pOldWnd)
{
	CMDIChildWnd::OnSetFocus(pOldWnd);

	CT2Doc *pDoc = m_pTileView->GetDocument();
	CString PathName = pDoc->GetPathName();

	if(pDoc->m_hMapFile==NULL)
	{
		CFile file;
		if(file.Open(PathName, CFile::modeRead|CFile::typeBinary))
		{
			FILETIME ftTime;
			::GetFileTime(file.m_hFile, NULL, NULL, &ftTime);
			file.Close();
			if(*(ULONGLONG*)&pDoc->m_ftTime!=*(ULONGLONG*)&ftTime)
			{
				*(ULONGLONG*)&pDoc->m_ftTime=*(ULONGLONG*)&ftTime;
				if(Hint(IDS_FILEISMODIFTED, MB_YESNO|MB_ICONINFORMATION)==IDYES)
					pDoc->OpenRom(PathName);
			}
		}
	}
}

void CT2ChildFrm::OnViewBkBrush(UINT nID)
{
	WORD nBkPixType = IDR_BK1+nID-ID_VIEW_BK1;
	CT2TileView *pView = (CT2TileView*)m_pTileView;
	if(pView->m_nBkPixType!=nBkPixType)
	{
		pView->LoadBkBmp(nBkPixType);
		Invalidate(FALSE);
	}
}

void CT2ChildFrm::OnUpdateViewBkBrush(CCmdUI *pCmdUI)
{
	CT2TileView *pView = (CT2TileView*)m_pTileView;
	pCmdUI->SetRadio(pView->m_nBkPixType==IDR_BK1+pCmdUI->m_nID-ID_VIEW_BK1);
}

void CT2ChildFrm::OnSFI()
{
	if(m_pSFI)
	{
		m_pSFI->ShowWindow(SW_NORMAL);
		m_pSFI->SetFocus();
	}
}

void CT2ChildFrm::OnUpdateSFI(CCmdUI *pCmdUI)
{
	CString name = m_pTileView->GetDocument()->GetTitle();
	if(!m_pSFI)
	{
		OnCreateSFI(name);
		if(m_pSFI)
			((CSubFileInfo*)m_pSFI)->OnInit(name,
				this, 0, m_pTileView->GetDocument()->m_nRomSize);
	}

	pCmdUI->SetText(GetSubFileMenuText(name));
	pCmdUI->Enable(m_pSFI!=NULL);
}

void CT2ChildFrm::OnFsiGoPC(UINT nID)
{
	UINT nPC = -1;
	UINT nBase;
	if(nID==ID_FSI_GOARM9)
	{
		nPC = m_NdsFSI.m_pHeader->Arm9_Rom_Offset +
			(m_NdsFSI.m_pHeader->Arm9_Entry_Address-m_NdsFSI.m_pHeader->Arm9_Ram_Address);
		nBase = m_NdsFSI.m_pHeader->Arm9_Ram_Address-m_NdsFSI.m_pHeader->Arm9_Rom_Offset;
	}else
	{
		nPC = m_NdsFSI.m_pHeader->Arm7_Rom_Offset +
			(m_NdsFSI.m_pHeader->Arm7_Entry_Address-m_NdsFSI.m_pHeader->Arm7_Ram_Address);
		nBase = m_NdsFSI.m_pHeader->Arm7_Ram_Address-m_NdsFSI.m_pHeader->Arm7_Rom_Offset;
	}
	if(nPC!=-1)
	{
		UINT nType;
		CT2View *pView = GetCurView(&nType);
		pView->ScrollTo(nPC);
		if(nType==ID_VIEW_ASMVIEW)
		{
			CT2AsmView *pAsm = (CT2AsmView*)pView;
			//pAsm->m_nBaseAddress = nBase;
			CString strBaseAddress;
			strBaseAddress.Format(_T("%08X"), nBase);
			pAsm->SetDlgItemText(IDC_ARM_BASEADDRESS, strBaseAddress);

			if(nID==ID_FSI_GOARM9)
				pAsm->m_nCpuType = 0;
			else
				pAsm->m_nCpuType = 1;
			pAsm->m_nSize = 4;
		}
	}
}

// CrystalTile Macro
/*
void CT2ChildFrm::OnToolsMacroStart()
{
	switch(m_pTileView->GetDocument()->m_nRomType)
	{
	case CT2_RT_NDS:
		m_Macro.m_Header.nType = CT_MACRO_TYPE_NDS;
		break;
	}
	m_nMacroStatus = CT_MACRO_OPEN;
}

void CT2ChildFrm::OnUpdateToolsMacroStart(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nMacroStatus==CT_MACRO_CLOSE);
}

void CT2ChildFrm::OnToolsMacroPause()
{
	if(m_nMacroStatus == CT_MACRO_PAUSE)
		m_nMacroStatus = CT_MACRO_OPEN;
	else
		m_nMacroStatus = CT_MACRO_PAUSE;
}

void CT2ChildFrm::OnUpdateToolsMacroPause(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nMacroStatus!=CT_MACRO_CLOSE);
	pCmdUI->SetCheck(m_nMacroStatus==CT_MACRO_PAUSE);
}

void CT2ChildFrm::OnToolsMacroCancel()
{
	m_Macro.Clear();
	m_nMacroStatus = CT_MACRO_CLOSE;
}

void CT2ChildFrm::OnUpdateToolsMacroCancel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nMacroStatus!=CT_MACRO_CLOSE);
}

void CT2ChildFrm::OnToolsMacroSave(UINT nID)
{
	static CFileDialog fd(FALSE);
	CString Filter;
	Filter.LoadString(IDS_MACRO_FILTER);
	Filter.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter = Filter;
	if(fd.DoModal()!=IDOK) return;
	Filter = fd.GetPathName();
	CString ext = _T(".ctm");
	if(Filter.Right(4).CompareNoCase(ext)!=0)
		Filter.Append(ext);
	if(m_Macro.Save(Filter, nID==ID_TOOLS_MACRO_SAVEALL))
	{
		m_Macro.Clear();
		m_nMacroStatus = CT_MACRO_CLOSE;
	}
}

void CT2ChildFrm::OnUpdateToolsMacroSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_Macro.IsEmpty());
}

void CT2ChildFrm::OnToolsMacroRun()
{
	static CFileDialog fd(TRUE);
	CString Filter;
	Filter.LoadString(IDS_MACRO_FILTER);
	Filter.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter = Filter;
	if(fd.DoModal()!=IDOK) return;

	CFile f;
	if(!f.Open(fd.GetPathName(), CFile::modeRead|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	DWORD dwSize = (DWORD)f.GetLength();
	HANDLE hMapFile = ::CreateFileMapping(f.m_hFile, NULL, PAGE_READONLY, 0, dwSize, NULL);
	if(hMapFile==NULL)
	{
		ShowError();
		return;
	}

	BYTE *pMacroFile = (BYTE*)::MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, dwSize);
	if(!pMacroFile)
	{
		ShowError();
		CloseHandle(hMapFile);
		return;
	}

	CT_MACRO_HEADER *pHeader = (CT_MACRO_HEADER*) pMacroFile;
	if( pHeader->dwSize!=sizeof(CT_MACRO_HEADER) &&
		*(UINT*)pHeader->Flags != 0x204D5443 ||	// Flags = "CTM "(CrystalTile Macro)
		!pHeader->dwVer || pHeader->dwVer>CT_MACRO_VER )
		goto END;

	switch(pHeader->nType)
	{
	case CT_MACRO_TYPE_NDS:
		if(m_pTileView->GetDocument()->m_nRomType!=CT2_RT_NDS)
		{
			Hint(IDS_MACRO_NOTANDSROM);
			goto END;
		}break;
	}

	CT_MACRO *pMacro = (CT_MACRO*)(pMacroFile+pHeader->dwSize);
	BYTE *pEnd = pMacroFile+dwSize;
	CT2StatusBar *p = theApp.GetStatusBar();;
	p->BeginProgress(-1);
	while((BYTE*)pMacro<pEnd)
	{
		switch(pMacro->wActive)
		{
		case CT2PALETTE_LOADPAL:
			{
				CT_MACRO_1VAL_1FILE *m = (CT_MACRO_1VAL_1FILE*)pMacro;
				theApp.GetPalDlg().OnLoadPal(m->szFile);
			}break;

		case CT2PALETTE_LOADDEFPAL:
			{
				CT_MACRO_1VAL_1FILE *m = (CT_MACRO_1VAL_1FILE*)pMacro;
				theApp.GetPalDlg().OnLoadDefPal(m->nVal);
			}break;

		case CT2TILEVIEW_EDITIMPORT:
			{
				CT_MACRO_1VAL_1FILE *m = (CT_MACRO_1VAL_1FILE*)pMacro;
				((CT2TileView*)m_pTileView)->OnEditImport(m->szFile);
			}break;

		case CT2TILEVIEW_IMPROTBMP:
			{
				if(m_nCurView!=ID_VIEW_TILEVIEW) SetCurView(ID_VIEW_TILEVIEW);
				CT_MACRO_CT2TILEVIEW_IMPROTBMP *m = (CT_MACRO_CT2TILEVIEW_IMPROTBMP*)pMacro;
				CT2TileView *pTV = (CT2TileView*)m_pTileView;
				pTV->m_ptImport = m->ptImport;
				WORD c = pTV->m_nColCount;
				pTV->Load_StateData(&m->StateData);
				pTV->OnUpdateData();
				pTV->OnImportBmp();
				pTV->m_nColCount = c;
			}break;

		case CT2TILEEDITOR_TBL:
			{
				CT_MACRO_ONTBL *m = (CT_MACRO_ONTBL*)pMacro;
				CT2TileEditorView *pTEV = (CT2TileEditorView*)m_pTileEditorView;

				pTEV->m_nTextColor		= m->nTextColor;
				pTEV->m_nBkColor		= m->nBkColor;
				pTEV->m_nShadowColor	= m->nShadowColor;
				pTEV->m_nAlignLeft		= m->nAlignLeft;
				pTEV->m_nAlignTop		= m->nAlignTop;
				pTEV->m_rcText			= m->rcText;
				pTEV->m_nTransparent	= m->nTransparent;
				pTEV->m_nShadowType		= m->nShadowType;
				pTEV->m_lfFont			= m->lfFont;
				pTEV->OnSetFont();

				CT2TileView *pTV = (CT2TileView*)m_pTileView;

				pTV->Load_StateData(&m->StateData);
				pTV->m_ptCursorPos = m->ptPos;

				m_pTileView->OnUpdateData();

				pTEV->OnTbl(m->szFile, m->nVal);
			}break;

		case CT2TILEEDITOR_SETTILE:
			{
				CT_MACRO_ONTBL *m = (CT_MACRO_ONTBL*)pMacro;
				CT2TileEditorView *pTEV = (CT2TileEditorView*)m_pTileEditorView;

				pTEV->m_nTextColor		= m->nTextColor;
				pTEV->m_nBkColor		= m->nBkColor;
				pTEV->m_nShadowColor	= m->nShadowColor;
				pTEV->m_nAlignLeft		= m->nAlignLeft;
				pTEV->m_nAlignTop		= m->nAlignTop;
				pTEV->m_rcText			= m->rcText;
				pTEV->m_nTransparent	= m->nTransparent;
				pTEV->m_nShadowType		= m->nShadowType;
				pTEV->m_lfFont			= m->lfFont;
				pTEV->OnSetFont();
				pTEV->m_strText			= m->szFile;
				pTEV->OnUpdateData();

				CT2TileView *pTV = (CT2TileView*)m_pTileView;

				pTV->Load_StateData(&m->StateData);
				pTV->m_ptCursorPos = m->ptPos;
				m_pTileView->OnUpdateData();

				pTEV->SetTile();
			}break;

		case CT2HEXVIEW_SETDATA:
			{
				CT_MACRO_CT2HEXVIEW_SETDATA *m = (CT_MACRO_CT2HEXVIEW_SETDATA*)pMacro;
				CT2HexView *pHV = (CT2HexView*) m_pHexView;
				pHV->m_nByteLR = (m->nFlags>>2)&1;
				pHV->m_nAsciiMode = (m->nFlags>>1)&1;
				pHV->m_nSwpCode = m->nFlags&1;
				pHV->SetData(m->nOffset, m->nData, m->nSize);
			}break;

		case CT2HEXVIEW_IMPORTDATA:
			{
				CT_MACRO_1VAL_1FILE *m = (CT_MACRO_1VAL_1FILE*)pMacro;
				CT2HexView *pHV = (CT2HexView*) m_pHexView;

				pHV->OnImportData(m->szFile, m->nVal);
			}break;

		case CT2HEXVIEW_IMPORTTEXT:
			{
				CT_MACRO_1VAL_1FILE *m = (CT_MACRO_1VAL_1FILE*)pMacro;
				CT2HexView *pHV = (CT2HexView*) m_pHexView;

				pHV->OnImportText(m->szFile, m->nVal);
			}break;

		case CT2HEXVIEW_PAL2DATA:
			{
				CT_MACRO_CT2HEXVIEW_SETDATA *m = (CT_MACRO_CT2HEXVIEW_SETDATA*)pMacro;
				CT2HexView *pHV = (CT2HexView*) m_pHexView;

				pHV->m_nAlign = m->nSize;
				pHV->OnEditPal2data(m->nOffset, m->nData, m->nFlags);
			}break;

		case CT2HEXVIEW_DATA2PAL:
			{
				CT_MACRO_CT2HEXVIEW_SETDATA *m = (CT_MACRO_CT2HEXVIEW_SETDATA*)pMacro;
				CT2HexView *pHV = (CT2HexView*) m_pHexView;

				pHV->m_nAlign = m->nSize;
				pHV->OnEditData2pal(m->nOffset, m->nData, m->nFlags);
			}break;

		case CT2TILEVIEW_SETPIXEL:
			{
				CT2TileView *pTV = (CT2TileView*)m_pTileView;
				CT_MACRO_CT2TILEVIEW_SETPIXEL *m = (CT_MACRO_CT2TILEVIEW_SETPIXEL*)pMacro;

				pTV->m_ptCursorPos = m->ptPos;
				pTV->m_nOffset = m->nOffset;

				pTV->Load_StateData(&m->StateData);

				pTV->OnUpdateData();

				UINT nOffset = m->nOffset+m->ptPos.x*pTV->m_nTileSize;
				pTV->DoUndo(pTV->GetDocument()->m_pRom+nOffset, nOffset, pTV->m_nTileSize);

				pTV->SetPixel(m->nTilePixelNO, m->nPixel);
			}break;

		case CT2SCRIPTVIEW_OPENSCRIPT:
			{
				CT_MACRO_1VAL_1FILE *m = (CT_MACRO_1VAL_1FILE*)pMacro;

				((CT2ScriptView*)m_pScriptView)->OnOpenScript(m->szFile, m->nVal);
			}break;

		case CT2SCRIPTVIEW_IMPORT:
			{
				CT_MACRO_IMPORTTEXT *m = (CT_MACRO_IMPORTTEXT*)pMacro;
				CT2ScriptView *pSV = (CT2ScriptView*)m_pScriptView;

				pSV->m_nNewAddress	= m->nNewAddress;
				pSV->m_nSwpCode		= m->nSwpCode;
				pSV->m_nAlign2		= m->nAlign2;
				pSV->m_bNewAddress	= m->bNewAddress;
				pSV->m_bRelativePoint = m->bRelativePoint;

				pSV->OnImport(m->nNoDefHint, m->nNewCode, m->nTblMode, m->nCodePage, m->nSaveTbl);
			}break;

		case CT2DOC_LOADTBL:
			{
				CT_MACRO_1VAL_1FILE *m = (CT_MACRO_1VAL_1FILE*)pMacro;
				m_pTileView->GetDocument()->LoadTbl(m->szFile, m->nVal);
			}break;

		case CT2TILEVIEW_EDITCOPY:
			{
				if(m_nCurView!=ID_VIEW_TILEVIEW) SetCurView(ID_VIEW_TILEVIEW);
				CT_MACRO_CT2TILEVIEW_IMPROTBMP *m = (CT_MACRO_CT2TILEVIEW_IMPROTBMP*)pMacro;
				CT2TileView *pTV = (CT2TileView*)m_pTileView;
				pTV->m_ptCursorPos = m->ptImport;
				WORD c = pTV->m_nColCount;
				pTV->Load_StateData(&m->StateData);
				pTV->OnUpdateData();
				pTV->OnCopy();
				pTV->m_nColCount = c;
			}break;

		case CT2TILEVIEW_EDITPASTE:
			{
				CT2TileView *pTV = (CT2TileView*)m_pTileView;
				if(m_nCurView!=ID_VIEW_TILEVIEW) SetCurView(ID_VIEW_TILEVIEW);
				pTV->OnPaste();
			}break;

		case CT2NDSFSI_LOADPAL:
			{
				CT_MACRO_CT2HEXVIEW_SETDATA *m = (CT_MACRO_CT2HEXVIEW_SETDATA*)pMacro;
				m_NdsFSI.LoadPal((WORD*)(m_pTileView->GetDocument()->m_pRom + m->nOffset),
					m_pTileView->GetDocument()->m_pPal);
			}break;

		case CT2NDSFSI_SETNTFRWH:
			{
				CT_MACRO_CT2HEXVIEW_SETDATA *m = (CT_MACRO_CT2HEXVIEW_SETDATA*)pMacro;
				m_NdsFSI.OnSetntfrwh(m->nSize, m->nFlags, m->nOffset);
			}break;

		case CT2NDSFSI_SETFILESIZE:
			{
				CT_MACRO_NDS_SETFILESIZE *m = (CT_MACRO_NDS_SETFILESIZE*)pMacro;
				m_NdsFSI.OnSetfilesize(m->nNewSize, m->wSub, m->wFileID, m->nNewOffset);
			}break;

		case CT2NDSFSI_IMPORT:
			{
				CT_MACRO_NDS_IMPORTFILE *m = (CT_MACRO_NDS_IMPORTFILE*)pMacro;
				CFile file;
				if(!file.Open(m->szFile, CFile::modeRead|CFile::typeBinary)) {ShowError(); break;}
				m_NdsFSI.ImportFile(&file, m->nOffset, m->nSize, m->wFileID, 0, m->nNewOffset);
			}break;

		case CT2NDSFSI_CHECKCRC16:
			{
				m_NdsFSI.OnFsiCheckcrc16(0);
			}break;

		case CT2NDSFSI_NDSINFO:
			{
				CT_MACRO_NDS_NDSINFO *m = (CT_MACRO_NDS_NDSINFO*)pMacro;
				NDSHEADER *pHeader = (NDSHEADER*)m_pTileView->GetDocument()->m_pRom;
				LPTSTR pStr = (LPTSTR)((BYTE*)pHeader+pHeader->Banner_Offset+0x240);
				m_pTileView->DoUndo((BYTE*)pStr, pHeader->Banner_Offset+0x240, 0x100*7);
				memcpy(pStr, m->Info, 0x100*7);
			}break;

		default:
			pMacro = (CT_MACRO*) ((BYTE*)pMacro + pMacro->wSize);
			continue;
		}
		pMacro = (CT_MACRO*) ((BYTE*)pMacro + pMacro->wSize);
	}
	p->EndProgress();
END:
	UnmapViewOfFile(pMacroFile);
	CloseHandle(hMapFile);
}

void CT2ChildFrm::OnUpdateToolsMacroRun(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nMacroStatus==CT_MACRO_CLOSE && !m_pTileView->GetDocument()->m_nReadOnly);
}
*/
void CT2ChildFrm::OnCreateSFI(CString& name)
{
	CString ext = PathFindExtension(name);

	if(m_pSFI)
	{
		CString ext2 = PathFindExtension(((CSubFileInfo*)m_pSFI)->m_strTitle);
		if(ext2.CompareNoCase(ext)!=0)
		{
			m_pSFI->DestroyWindow();
			delete m_pSFI;
			m_pSFI = NULL;
		}
	}
	UINT nMenu = -1;
	if(!m_pSFI)
	{
		switch(GetFileType(name))
		{
		case SFI_SDAT:
			{
				m_pSFI = (CDialog*) new CSDatInfo;
				m_pSFI->Create(MAKEINTRESOURCE(CSDatInfo::IDD), this);
				nMenu = IDR_CRYSTALPLAYER;
			}break;

		case SFI_PAC:
			{
				m_pSFI = (CDialog*) new CPacFileInfo;
				m_pSFI->Create(MAKEINTRESOURCE(CPacFileInfo::IDD), this);
				nMenu = IDR_SFI;
			}break;

		case SFI_UTILITY_BIN:
			{
				m_pSFI = (CDialog*) new CUtilityFileInfo;
				m_pSFI->Create(MAKEINTRESOURCE(CUtilityFileInfo::IDD), this);
				nMenu = IDR_SFI;
			}break;

		case SFI_NARC:
			{
				m_pSFI = (CDialog*) new CNARCFileInfo;
				m_pSFI->Create(MAKEINTRESOURCE(CNARCFileInfo::IDD), this);
				nMenu = IDR_SFI;
			}break;
		}
		if(m_pSFI && !m_pSFI->GetMenu())
		{
			if(nMenu!=-1)
			{
				CSubFileInfo *sfi = (CSubFileInfo*) m_pSFI;
				sfi->m_Menu.LoadMenu(nMenu);
				sfi->SetMenu(&sfi->m_Menu);
			}else
				::SetMenu(m_pSFI->m_hWnd, NULL);
		}
	}
}

CString CT2ChildFrm::GetSubFileMenuText(CString &name)
{
	CString text, sub;
	text.LoadString(IDS_SFI_MENUTEXT);
	int i = GetFileType(name);
	//AfxExtractSubString(sub, text, m_pSFI!=NULL?1:0);
	AfxExtractSubString(sub, text, i!=SFI_UNKNOWN?1:0);

	text.Format(sub, PathFindExtension(name));

	return text;
}

int CT2ChildFrm::GetFileType(LPCTSTR lpName)
{
	LPTSTR lpExt = PathFindExtension(lpName);

	if(sCompareNoCase(lpExt, _T(".sdat"))==0)
		return SFI_SDAT;
	else
	if(sCompareNoCase(lpExt, _T(".pac"))==0)
		return SFI_PAC;
	else
	if(sCompareNoCase(lpName, _T("utility.bin"))==0)
		return SFI_UTILITY_BIN;
	else
	if(sCompareNoCase(lpExt, _T(".narc"))==0)
		return SFI_NARC;

	return SFI_UNKNOWN;
}
