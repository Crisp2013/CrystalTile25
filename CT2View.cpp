// CT2View.cpp : CT2View 类的实现
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include "CT2Doc.h"
#include ".\CT2View.h"
#include "CT2TileView.h"
#include "CT2HexView.h"
#include "CT2ChildFrm.h"

#include "CT2MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CT2View

IMPLEMENT_DYNCREATE(CT2View, CScrollView)

BEGIN_MESSAGE_MAP(CT2View, CScrollView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	ON_UPDATE_COMMAND_UI(IDC_CT2VIEW_OFFSET, OnUpdateOffset)
	ON_COMMAND(IDC_CT2VIEW_OFFSET, OnEditGo)
	ON_COMMAND(IDC_SCRIPTVIEW_BEGIN, OnEditGo)
	ON_UPDATE_COMMAND_UI(IDC_SCRIPTVIEW_BEGIN, OnUpdateScriptViewBegin)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_RESET, OnEditReset)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RESET, OnUpdateEditReset)
END_MESSAGE_MAP()

// CT2View 构造/析构

CT2View::CT2View()
: m_nOffset(0)
, m_pPaintPanel(NULL)
, m_bUndo(0)
{
	// TODO: 在此处添加构造代码
	m_ptCursorPos.x = m_ptCursorPos.y = 0;
}

CT2View::~CT2View()
{
	if(m_pPaintPanel) delete[] m_pPaintPanel;
	CCrystalTreeCtrl *t = & ((CT2MainFrm*)theApp.GetMainWnd())->m_TreeBar.m_Tree;
	t->ClearVar();
}

BOOL CT2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return CScrollView::PreCreateWindow(cs);
}

// CT2View 绘制

void CT2View::OnDraw(CDC* pDC)
{
	CT2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

void CT2View::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// 去掉背景刷
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND, (LONG)(INT_PTR)::GetStockObject(NULL_BRUSH));

	CRect rc;	GetClientRect(&rc);
	memset(&m_biPaintPanel, 0, sizeof(m_biPaintPanel));
	m_biPaintPanel.biBitCount = 32;
	m_biPaintPanel.biPlanes = 1;
	m_biPaintPanel.biSize = sizeof(m_biPaintPanel);

	EnableScrollBarCtrl(SB_BOTH);
}


// CT2View 诊断

#ifdef _DEBUG
void CT2View::AssertValid() const
{
	CScrollView::AssertValid();
}

void CT2View::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CT2Doc* CT2View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CT2Doc)));
	return (CT2Doc*)m_pDocument;
}
#endif //_DEBUG


// CT2View 消息处理程序

void CT2View::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	OnUpdateData();
}

void CT2View::OnUpdateData(void)
{
	CCrystalTreeCtrl *t = & ((CT2MainFrm*)theApp.GetMainWnd())->m_TreeBar.m_Tree;
	t->Reflash(TRUE);
	Invalidate(FALSE);
}

void CT2View::OnDestroy()
{

	CScrollView::OnDestroy();
}

void CT2View::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CT2StatusBar *pSb=theApp.GetStatusBar();
	if( bActivate ||
		!pDeactiveView ||
		!pActivateView ||
		(pActivateView!=pDeactiveView))
	for(int i=pSb->GetCount()-1; i>0; i--)
		pSb->SetPaneText(i, _T(""));

	CCrystalTreeCtrl *t = & ((CT2MainFrm*)theApp.GetMainWnd())->m_TreeBar.m_Tree;
	if(bActivate)
	{
		theApp.SetCT2Tree(GetViewType());
		t->AppendVal(1, &m_nOffset, CTCS_UINT, 0, GetDocument()->m_nFileSize-1);
		AppendVar();

		pSb->SetPaneText(pSb->CommandToIndex(ID_ROMINFO), GetDocument()->m_strRomInfo);
		CString strOpenMode, strMode;
		int nMode=0;
		if(GetDocument()->m_nReadOnly)
			nMode|=1;
		if(GetDocument()->m_hMapFile)
			nMode|=2;
		strMode.LoadString(IDS_OPENMODE);
		AfxExtractSubString(strOpenMode, strMode, nMode);
		pSb->SetPaneText(pSb->CommandToIndex(IDS_OPENMODE), strOpenMode);

		if(GetDocument()->m_nRomType==CT2_RT_PCMEM)
		{
			GetDocument()->m_Memory.ReLoadMemory();
			GetDocument()->m_Memory.SuspendThread();
			Invalidate(FALSE);
		}

		OnUpdateData();
	}else
	{
		// t->ClearVar();
		if(GetDocument()->m_nRomType==CT2_RT_PCMEM)
		{
			GetDocument()->m_Memory.ResumeThread();
		}
	}

	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CT2View::OnEditGo()
{
	UINT nType;
	CT2View *pView = (CT2View*)theApp.GetCurView(&nType);
	CCrystalTreeCtrl &tc = ((CT2MainFrm*)AfxGetMainWnd())->m_TreeBar.m_Tree;
	int i=0;
	tc.SetFocus();
	tc.SelectItem(tc.IndexToItem(1, tc.GetRootItem(), i));

	switch(nType)
	{
	case ID_VIEW_TILEVIEW:
		nType = pView->m_nOffset+pView->m_ptCursorPos.x*((CT2TileView*)pView)->m_nTileSize;
		break;
	case ID_VIEW_HEXVIEW:
		nType = pView->m_ptCursorPos.x;
		break;
	default:
		nType = -1;
		break;
	}
	if(nType!=-1)
	{
		CString off;
		off.Format(_T("%X"), nType+GetDocument()->m_nBlockOffset);
		::SetWindowText(tc.m_hwndItem, off);
	}

	tc.m_Edit.SetEditSel(0, -1);
	tc.m_Edit.SetFocus();
}

void CT2View::OnUpdateOffset(CCmdUI *pCmdUI)
{
	UINT nType;
	CView *pView = theApp.GetCurView(&nType);
	pCmdUI->Enable((pView==this) &&
		(nType!=ID_VIEW_SCRIPTVIEW) &&
		!(GetDocument()->m_nRomType==CT2_RT_NSCR && (nType==ID_VIEW_TILEVIEW||nType==ID_VIEW_TILEEDITORVIEW)));
}

void CT2View::DoUndo(BYTE *pData, UINT nOffset, UINT nSize)
{
	nOffset+=GetDocument()->m_nBlockOffset;
	CT2Doc* pDoc = GetDocument();
	UNDO undo;
	undo.nOffset=nOffset;
	undo.nSize=nSize;
	if(nSize>4)
	{
		undo.pData=new BYTE[nSize];
		memcpy(undo.pData, pData, nSize);
	}else
	{
		memcpy(&undo.pData, pData, nSize);
	}
	pDoc->m_UndoList.AddTail(undo);
	if(pDoc->m_hMapFile==NULL)
		pDoc->SetModifiedFlag();
}

void CT2View::OnEditUndo()
{
	CT2Doc* pDoc = GetDocument();
	if(pDoc->m_UndoList.IsEmpty())
		GetFocus()->SendMessage(WM_UNDO);
	else
	{
		UNDO &undo=pDoc->m_UndoList.GetTail();

		pDoc->Seek(undo.nOffset);
		undo.nOffset-=pDoc->m_nBlockOffset;

		if(undo.nSize>4)
		{
			memcpy(pDoc->m_pRom+undo.nOffset, undo.pData, undo.nSize);
			delete[] undo.pData;
		}else
			memcpy(pDoc->m_pRom+undo.nOffset, &undo.pData, undo.nSize);
		ScrollTo(undo.nOffset);

		if(pDoc->m_nRomType==CT2_RT_PCMEM)
		{
			pDoc->m_Memory.Update(undo.nOffset, undo.nSize);
			pDoc->m_Memory.ReLoadMemory();
		}

		pDoc->m_UndoList.RemoveTail();
		if(pDoc->m_UndoList.IsEmpty())
			pDoc->SetModifiedFlag(FALSE);

		OnUpdateData();
	}
}

void CT2View::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	CWnd *pFocus = GetFocus();
	pCmdUI->Enable( !GetDocument()->m_UndoList.IsEmpty() ||
        ( pFocus && pFocus->SendMessage(EM_CANUNDO)) );
}

void CT2View::OnEditCut()
{
	GetFocus()->SendMessage(WM_CUT);
}

void CT2View::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	UINT nSel = 0;
	CWnd *pFocus = GetFocus();
	if(pFocus)
	nSel = (UINT)pFocus->SendMessage(EM_GETSEL);

	pCmdUI->Enable( LOWORD(nSel)!=HIWORD(nSel) );
}

void CT2View::OnEditCopy()
{
	GetFocus()->SendMessage(WM_COPY);
}

void CT2View::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	UINT nSel = 0;
	CWnd *pFocus = GetFocus();
	if(pFocus)
	nSel = (UINT)pFocus->SendMessage(EM_GETSEL);

	pCmdUI->Enable( LOWORD(nSel)!=HIWORD(nSel) );
}

void CT2View::OnEditPaste()
{
	GetFocus()->SendMessage(WM_PASTE);
}

void CT2View::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( GetFocus()!=this );
}

BOOL CT2View::PreTranslateMessage(MSG* pMsg)
{
	CView *pView = ((CT2ChildFrm*)GetParent())->GetCurView();
	if(pView && pView!=this &&
		((pMsg->message>=WM_KEYFIRST&&pMsg->message<=WM_KEYLAST)||
		(pMsg->message>=WM_MOUSEFIRST&&pMsg->message<=WM_MOUSELAST)) )
	{
		pView->SetFocus();
		pView->SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		return TRUE;
	}
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if( (GetKeyState(VK_CONTROL)&0x80) &&
			(towlower((UINT)pMsg->wParam)=='w') )
		{
			CT2ChildFrm *pChild = (CT2ChildFrm*)GetParent();
			UINT nCurView=pChild->m_nCurView;
			if(GetKeyState(VK_SHIFT)&0x80)
			{
				nCurView--;
				if(nCurView<CT2_ID_VIEW_FIRST)
					nCurView=CT2_ID_VIEW_LAST;
			}else
			{
				nCurView++;
				if(nCurView>CT2_ID_VIEW_LAST)
					nCurView=CT2_ID_VIEW_FIRST;
			}
			pChild->SetCurView(nCurView);
		}
	}

	return CScrollView::PreTranslateMessage(pMsg);
}

void CT2View::ScrollTo(UINT nOffset, BOOL bForce)
{
	OnUpdateData();
}

void CT2View::SetCurView(UINT nView)
{
	CT2ChildFrm *pChild = (CT2ChildFrm*)GetParent();
	pChild->SetCurView(nView);
}

void CT2View::Invalidate(BOOL bErase)
{
	CScrollView::Invalidate(bErase);
	GetDocument()->UpdateAllViews(this);
}

void CT2View::OnUpdateScriptViewBegin(CCmdUI *pCmdUI)
{
	UINT nType;
	theApp.GetCurView(&nType);
	pCmdUI->Enable(nType==ID_VIEW_SCRIPTVIEW);
}

void CT2View::OnEditReset()
{
	CT2Doc *pDoc = GetDocument();
	while(!pDoc->m_UndoList.IsEmpty())
	{
		UNDO &undo=pDoc->m_UndoList.GetTail();

		pDoc->Seek(undo.nOffset);
		//undo.nOffset-=pDoc->m_nBlockOffset;

		if(undo.nSize>4)
		{
			memcpy(pDoc->m_pRom+undo.nOffset, undo.pData, undo.nSize);
			delete[] undo.pData;
		}else
			memcpy(pDoc->m_pRom+undo.nOffset, &undo.pData, undo.nSize);

		pDoc->m_UndoList.RemoveTail();
	}

	pDoc->SetModifiedFlag(FALSE);

	OnUpdateData();
}

void CT2View::OnUpdateEditReset(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( !GetDocument()->m_UndoList.IsEmpty() );
}

UINT CT2View::GetViewType()
{
	return ((CT2ChildFrm*)GetParent())->m_nCurView;
}

void CT2View::AppendVar()
{
}
/*
int CT2View::AddMacro(void *pMacro)
{
	CT2ChildFrm *pChild = (CT2ChildFrm*)GetParent();
	switch(pChild->m_nMacroStatus)
	{
	case CT_MACRO_OPEN:
		MessageBeep(0x3F);
		return pChild->m_Macro.AddMacro((CT_MACRO*)pMacro);

	case CT_MACRO_PAUSE:
		return pChild->m_Macro.GetCount()-1;

	case CT_MACRO_CLOSE:
		return -1;
	}
	return -1;
}
*/