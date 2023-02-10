// CT2Favorite.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include "CT2Favorite.h"

#include "CT2TileView.h"
#include ".\CT2Favorite.h"
#include "CT2ChildFrm.h"
#include "CT2AsmView.h"
#include "CT2HexView.h"

// CT2Favorite 对话框

IMPLEMENT_DYNAMIC(CT2Favorite, CT2NormalDlg)
CT2Favorite::CT2Favorite(CWnd* pParent /*=NULL*/)
	//: CDialog(CT2Favorite::IDD, pParent)
{
}

CT2Favorite::~CT2Favorite()
{
}

void CT2Favorite::DoDataExchange(CDataExchange* pDX)
{
	CT2NormalDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CT2Favorite, CT2NormalDlg)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_FAVORITES_LIST, OnLvnItemActivateFavoritesList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FAVORITES_LIST, OnLvnEndlabeleditFavoritesList)
	ON_BN_CLICKED(IDC_FAVORITES_DELBTN, OnBnClickedFavoritesDelbtn)
END_MESSAGE_MAP()


// CT2Favorite 消息处理程序

BOOL CT2Favorite::OnInitDialog()
{
	CT2NormalDlg::OnInitDialog();

	CListCtrl *pList = (CListCtrl*)GetDlgItem(IDC_FAVORITES_LIST);
	CRect rc; pList->GetClientRect(&rc);
	pList->InsertColumn(0, NULL, LVCFMT_LEFT, rc.Width()-GetSystemMetrics(SM_CXVSCROLL));
	pList->SetExtendedStyle( pList->GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CT2Favorite::OnLvnItemActivateFavoritesList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	Goto(pNMIA->iItem);

	*pResult = 0;
}

void CT2Favorite::OnLvnEndlabeleditFavoritesList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	POSITION pos = g_Favorites.FindIndex(pDispInfo->item.iItem);
	FAVORITES &sq = g_Favorites.GetAt(pos);
	if(pDispInfo->item.pszText)
		lstrcpyn(sq.Desc,pDispInfo->item.pszText, CT2_FAVLEN);

	*pResult = 1;
}

void CT2Favorite::OnBnClickedFavoritesDelbtn()
{
	CListCtrl *pList = (CListCtrl*)GetDlgItem(IDC_FAVORITES_LIST);
	POSITION pos = pList->GetFirstSelectedItemPosition();
	while(pos)
	{
		int nSel = pList->GetNextSelectedItem(pos);
		pList->DeleteItem(nSel);
		
		g_Favorites.RemoveAt(g_Favorites.FindIndex(nSel));
		pos = pList->GetFirstSelectedItemPosition();
	}
}

void CT2Favorite::Goto(int nItem)
{
	POSITION pos = g_Favorites.FindIndex(nItem);
	FAVORITES &Fav = g_Favorites.GetAt(pos);

	pos = theApp.m_pDocManager->GetFirstDocTemplatePosition();
	CDocTemplate *dt = theApp.m_pDocManager->GetNextDocTemplate(pos);
	pos = dt->GetFirstDocPosition();

	CT2View *pView = NULL;
	CString name;
	while(pos)
	{
		CDocument *pDoc = dt->GetNextDoc(pos);
		name = pDoc->GetPathName();
		if(name.CompareNoCase(Fav.Doc)==0)
		{
			pos = pDoc->GetFirstViewPosition();
			pView = (CT2View*)pDoc->GetNextView(pos);

			if(((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIGetActive()!=pView->GetParent())
				((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIActivate(pView->GetParent());
			break;
		}
	}
	if(pView==NULL)
	{
		CDocument *pDoc = theApp.OpenDocumentFile(Fav.Doc);
		if(pDoc==NULL) return;

		pos = pDoc->GetFirstViewPosition();
		pView = (CT2View*)pDoc->GetNextView(pos);
	}

	CT2ChildFrm *child = (CT2ChildFrm*)pView->GetParent();
	switch(Fav.nCurView)
	{
	case ID_VIEW_TILEVIEW:
		{
		CT2TileView *pTV = (CT2TileView*)child->m_pTileView;
		memcpy(pView->GetDocument()->m_pPal, Fav.pPal, 256*4);
		pTV->m_nTileFormat	= Fav.nTileFormat;
		pTV->m_nWidth		= Fav.nWidth;
		pTV->m_nHeight		= Fav.nHeight;
		pTV->m_nScale		= Fav.nScale;
		pTV->m_nDrawMode	= Fav.nDrawMode;
		pTV->m_nHCheck		= Fav.nHCheck;
		pTV->m_nVCheck		= Fav.nVCheck;
		pTV->m_nL90Check	= Fav.nL90Check;
		pTV->m_nR90Check	= Fav.nR90Check;
		pTV->m_n24Check		= Fav.n24Check;
		pTV->m_nSkipSize	= Fav.nSkipSize;
		pTV->m_nPalIndex	= Fav.nPalIndex;
		pTV->m_nColCount	= Fav.nColCount;
		pTV->m_nLineCount	= Fav.nLineCount;
		pTV->m_nMoOffset	= Fav.nMoOffset;
		pTV->m_nAutoColFit	= Fav.nAutoColFit;
		}break;
	case ID_VIEW_ASMVIEW:
		{
		CT2AsmView *pAV = (CT2AsmView*)child->m_pAsmView;
		pAV->m_nBaseAddress	= Fav.nBaseAddress;
		pAV->m_nCpuType		= Fav.nCpuType;
		pAV->m_nSize		= Fav.nAsmViewSize;
		}break;

	case ID_VIEW_HEXVIEW:
		{
			CT2HexView *pHV = (CT2HexView*)child->m_pHexView;
			pHV->m_nTblMode = Fav.nTblMode;
			pHV->m_nSwpCode = Fav.nSwpCode;

			pHV->GetDocument()->m_nScriptCodePage = Fav.nScriptCodePage;
		}break;
	}

	// 最近激活项目自动上移
	if(nItem>0)
	{
		g_Favorites.AddHead(Fav);
		g_Favorites.RemoveAt(g_Favorites.FindIndex(nItem+1));

		CListCtrl *pList = (CListCtrl*)GetDlgItem(IDC_FAVORITES_LIST);
		name = pList->GetItemText(nItem, 0);
		pList->DeleteItem(nItem);
		pList->InsertItem(0, name);
	}

	UINT nType;
	child->GetCurView(&nType);
	if(!(Fav.nCurView==ID_VIEW_HEXVIEW && nType==ID_VIEW_HEXVIEW))
		child->SetCurView(Fav.nCurView);
	pView = theApp.GetCurView();
	pView->ScrollTo(Fav.nOffset, TRUE);
	pView->SetFocus();
}
