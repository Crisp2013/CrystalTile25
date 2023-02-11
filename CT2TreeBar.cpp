// CT2TreeBar.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\CT2TreeBar.h"
#include <AtlBase.h>

// CT2TreeBar
#define TABBOX 16
#define TB_SPLITTE_L	1
#define TB_SPLITTE_R	2
#define TB_SPLITTE_T	3
#define TB_SPLITTE_B	4
#define TB_SPLITTE_S	4

IMPLEMENT_DYNAMIC(CT2TreeBar, CControlBar)
CT2TreeBar::CT2TreeBar()
: m_nSizing(0)
, m_bUpdate(FALSE)
{
}

CT2TreeBar::~CT2TreeBar()
{
	DeleteObject(m_hBrush);

	AfxGetApp()->WriteProfileInt(CT2Settings, _T("TreeSpliter"), m_nSpliter);
}


BEGIN_MESSAGE_MAP(CT2TreeBar, CControlBar)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(TCN_SELCHANGE, AFX_IDC_TAB_CONTROL, OnTabSel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_RESPATH, OnBnClickedButtonRespath)
END_MESSAGE_MAP()



// CT2TreeBar 消息处理程序
void UpdateCmdUI()
{
}

void CT2TreeBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CTreeCtrl *pTC;
	switch(m_Tab.GetCurSel())
	{
	case 0:
		pTC = &m_Tree;
		break;
	case 4:
		pTC = &m_MyRes;
		break;
	default:
		return;
	}
	HTREEITEM hItem=pTC->GetFirstVisibleItem();
	RECT rc={0};
	CCmdUI ui;
	ui.m_pOther=&m_wndPal;
	UINT nID;
	while(hItem)
	{
		nID=m_Tree.IsCmdItem(hItem);
		if(nID!=0 && nID!=IDC_STATIC)
		{
			ui.m_nID=nID;
			if(!OnCmdMsg(ui.m_nID, CN_UPDATE_COMMAND_UI, &ui, NULL))
				ui.DoUpdate(pTarget, bDisableIfNoHndler);
			CTC_PARAM *p = (CTC_PARAM*)m_Tree.GetItemData(hItem);
			p->nStyle &= ~CTCS_EX_DISIBLE;
			if(m_wndPal.GetStyle()&WS_DISABLED)
				p->nStyle |= CTCS_EX_DISIBLE;
		}
		hItem = pTC->GetNextVisibleItem(hItem);
	}
	m_wndPal.EnableWindow();
}

BOOL CT2TreeBar::Create(CWnd* pParentWnd, UINT nStyle, UINT nID)
{
	ASSERT(pParentWnd != NULL);

	// allow chance to modify styles
	m_dwStyle = (nStyle & CBRS_ALL);
	CREATESTRUCT cs;
	memset(&cs, 0, sizeof(cs));
	cs.lpszClass = AFX_WND;
	cs.style = (DWORD)nStyle | WS_CHILD;
	cs.hMenu = (HMENU)(UINT_PTR)nID;
	cs.hInstance = AfxGetInstanceHandle();
	cs.hwndParent = pParentWnd->GetSafeHwnd();

	if (!PreCreateWindow(cs))
		return FALSE;

	// initialize common controls
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTLS_REG));
	AfxDeferRegisterClass(AFX_WND_REG);

	if(!CreateEx(cs.dwExStyle, cs.lpszClass, cs.lpszName,
		cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.lpCreateParams))
		return FALSE;

	m_hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	SetClassLongPtr(m_hWnd, GCL_HBRBACKGROUND, (LONG)(UINT_PTR)m_hBrush);

	SetDlgCtrlID(nID);

	ModifyStyle(0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN);

	m_nSpliter = AfxGetApp()->GetProfileInt(CT2Settings, _T("TreeSpliter"), 256);

	m_wndFav.Create(CT2Favorite::IDD, this);
	CWnd *p=m_wndFav.GetDlgItem(IDC_FAVORITES_DESC);
	m_wndPal.Create(CT2Palette::IDD, this);
	m_wndDefOption.Create(IDD_OPTIONBAR, this);
	m_wndFav.ModifyStyle(0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	//m_wndPal.ModifyStyle(0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
	m_wndDefOption.ModifyStyle(0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	RECT rc={0,TABBOX,m_nSpliter,m_nSpliter};
	m_MyRes.Create(WS_BORDER|WS_TABSTOP|TVS_SHOWSELALWAYS,
		rc, this, IDC_DEF_RESPATH);
	m_Tree.Create(WS_VISIBLE|WS_BORDER|WS_TABSTOP|TVS_SHOWSELALWAYS,
		rc, this, GetDlgCtrlID());

	HIMAGELIST il;
	LoadSystemImageList(il);
	::SendMessage(m_MyRes.m_hWnd, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)il);

	CRegKey rk;
	rk.Open(HKEY_CURRENT_USER, _T("SoftWare\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"));
	ULONG l=_MAX_PATH;
	TCHAR per[_MAX_PATH];
	rk.QueryStringValue(_T("Personal"), per, &l);
	rk.Close();
	m_wndDefOption.SetDlgItemText(IDC_DEF_RESPATH, theApp.GetProfileString(CT2Settings, _T("ResPath"), per));

	rc.top=0;
	rc.bottom=rc.top+TABBOX;
	m_Tab.Create(WS_CHILD|WS_TABSTOP|WS_VISIBLE|TBS_FIXEDLENGTH|TBS_BOTH, rc, this, AFX_IDC_TAB_CONTROL);
	CString tabt=LoadStr(IDS_TREETAB);
	CString tab;int i=0;
	while(TRUE)
	{
		AfxExtractSubString(tab, tabt, i);
		if(tab.IsEmpty()) break;
		m_Tab.InsertItem(i++, tab);
	}
	m_Tab.SetFont(m_Tree.GetFont());

	// init
	CComboBox *pCB = (CComboBox*)m_wndDefOption.GetDlgItem(IDC_OPTION_PALLIST);
	CMenu menu;
	menu.LoadMenu(IDR_CT2POPMENU);
	CMenu *pSub = menu.GetSubMenu(1);
	CString cp;
	for(UINT i=0; i<(UINT)(pSub->GetMenuItemCount()); i++)
	{
		pSub->GetMenuString(i, cp, MF_BYPOSITION);
		pCB->InsertString(i, cp);
	}
	pCB->SetCurSel(theApp.GetProfileInt(CT2Settings, CT2DefPal, 0));

	menu.Detach();
	menu.LoadMenu(IDR_CrystalTile2TYPE);
	int nIndex = GetMenuItemIndex(menu.GetSubMenu(2)->GetSafeHmenu(), ID_VIEW_AUTOFITCOL)+2;
	pSub = menu.GetSubMenu(2)->GetSubMenu(nIndex);
	pCB = (CComboBox*)m_wndDefOption.GetDlgItem(IDC_OPTION_BKLIST);
	for(UINT i=0; i<(UINT)(pSub->GetMenuItemCount()); i++)
	{
		pSub->GetMenuString(i, cp, MF_BYPOSITION);
		pCB->InsertString(i, cp);
	}
	pCB->SetCurSel(theApp.GetProfileInt(CT2Settings, CT2DefBk, 0));

	pCB = (CComboBox*)m_wndDefOption.GetDlgItem(IDC_OPTION_OFFSETLIST);
	pCB->SetCurSel(theApp.GetProfileInt(CT2Settings, CT2DefOffset, 0));

	return TRUE;
}

CSize CT2TreeBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if(!m_nSizing)
	{
		RECT rc;
		GetParentFrame()->GetWindowRect(&rc);
		int tmp = m_nSpliter/2;
		BOOL h = m_dwStyle&(CBRS_ALIGN_LEFT|CBRS_ALIGN_RIGHT);
		if( (UINT)m_pDockContext->m_rectMRUDockPos.right>(UINT)rc.right
			|| (UINT)m_pDockContext->m_rectMRUDockPos.right<TABBOX )
			m_pDockContext->m_rectMRUDockPos.right = h?tmp:m_nSpliter;
		if( (UINT)m_pDockContext->m_rectMRUDockPos.bottom>(UINT)rc.bottom
			|| (UINT)m_pDockContext->m_rectMRUDockPos.bottom<TABBOX )
			m_pDockContext->m_rectMRUDockPos.bottom = (h?m_nSpliter:tmp)+64;
	}
	::SetWindowPos(m_Tree.m_hWnd, HWND_TOP,
		0, 0,
		m_pDockContext->m_rectMRUDockPos.right-m_pDockContext->m_rectMRUDockPos.left-2,
		m_nSpliter,
		SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOREDRAW|SWP_DRAWFRAME|SWP_ASYNCWINDOWPOS);

	if (bStretch) // if not docked stretch to fit
		return CSize(bHorz ? 32767 : m_pDockContext->m_rectMRUDockPos.right-m_pDockContext->m_rectMRUDockPos.left,
			bHorz ? m_pDockContext->m_rectMRUDockPos.bottom : 32767);
	else
		return CSize(m_pDockContext->m_rectMRUDockPos.right-m_pDockContext->m_rectMRUDockPos.left, m_pDockContext->m_rectMRUDockPos.bottom-m_pDockContext->m_rectMRUDockPos.top);
}

BOOL CT2TreeBar::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_NCLBUTTONDOWN:
	case WM_LBUTTONDOWN:
		{
			if(m_nSizing=CanSplitte(pMsg->pt))
			{
				m_ptSizing = pMsg->pt;
				SetCapture();
				return TRUE;
			}
			RECT rc;
			if(pMsg->hwnd==m_Tree.m_hWnd)
			{
				m_Tree.GetWindowRect(&rc);
				rc.top = rc.bottom-4;
				if(PtInRect(&rc, pMsg->pt))
				{
					SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
					m_Tree.SendMessage(WM_NCLBUTTONDOWN, HTBOTTOM, MAKELPARAM(pMsg->pt.x,pMsg->pt.y));
					Invalidate(FALSE);

					::GetWindowRect(m_Tree.m_hWnd, &rc);
					m_nSpliter = rc.bottom-rc.top;

					return TRUE;
				}
			}else
			if(pMsg->hwnd==m_MyRes.m_hWnd)
			{
				m_MyRes.GetWindowRect(&rc);
				rc.top = rc.bottom-4;
				if(PtInRect(&rc, pMsg->pt))
				{
					SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
					m_MyRes.SendMessage(WM_NCLBUTTONDOWN, HTBOTTOM, MAKELPARAM(pMsg->pt.x,pMsg->pt.y));
					Invalidate(FALSE);

					::GetWindowRect(m_MyRes.m_hWnd, &rc);
					m_nSpliter = rc.bottom-rc.top;

					return TRUE;
				}
			}
		}break;

	case WM_NCMOUSEMOVE:
	case WM_MOUSEMOVE:
		{
			if(m_nSizing)
			{
				int x=pMsg->pt.x-m_ptSizing.x;
				int y=pMsg->pt.y-m_ptSizing.y;
				switch(m_nSizing)
				{
				case TB_SPLITTE_R:
					m_pDockContext->m_rectMRUDockPos.right += x;
					break;

				case TB_SPLITTE_L:
					m_pDockContext->m_rectMRUDockPos.right -= x;
					if(IsFloating())
					{
						CWnd *p = GetDockingFrame();
						RECT rc;
						p->GetWindowRect(&rc);
						rc.left += x;
						p->SetWindowPos(NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE|SWP_NOACTIVATE);
					}
					break;

				case TB_SPLITTE_B:
					m_pDockContext->m_rectMRUDockPos.bottom += y;
					m_nSpliter += y;
					break;

				case TB_SPLITTE_T:
					m_pDockContext->m_rectMRUDockPos.bottom -= y;
					m_nSpliter -= y;
					if(IsFloating())
					{
						CWnd *p = GetDockingFrame();
						RECT rc;
						p->GetWindowRect(&rc);
						rc.top += y;
						p->SetWindowPos(NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE|SWP_NOACTIVATE);
					}
					break;
				}
				m_ptSizing = pMsg->pt;
				GetDockingFrame()->RecalcLayout();
			}else
				CanSplitte(pMsg->pt);

			RECT rc;
			if(pMsg->hwnd==m_Tree.m_hWnd)
			{
				m_Tree.GetWindowRect(&rc);
				rc.top = rc.bottom-4;
				if(PtInRect(&rc, pMsg->pt))
					SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
			}else
			if(pMsg->hwnd==m_MyRes.m_hWnd)
			{
				m_MyRes.GetWindowRect(&rc);
				rc.top = rc.bottom-4;
				if(PtInRect(&rc, pMsg->pt))
					SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
			}
		}break;

	case WM_LBUTTONUP:
		{
			if(m_nSizing)
			{
				m_nSizing = 0;
				ReleaseCapture();
				return TRUE;
			}
		}break;

	case WM_PAINT:
		if(pMsg->hwnd==m_hWnd)
		{
			CPaintDC dc(this);
			DrawHint(dc);
		}break;

	case WM_KEYDOWN:
		{
			switch(LOWORD(pMsg->wParam))
			{
			case VK_ESCAPE:
				{
					CT2View *pView = theApp.GetCurView();
					if(pView) pView->SetFocus();
				}break;

			/*case VK_TAB:
				if(GetKeyState(VK_CONTROL)&0x80)
				{
					int f = m_Tab.GetCurFocus();
					int c = m_Tab.GetItemCount();
					if(GetKeyState(VK_SHIFT)&0x80)
					{
						f--;
						if(f<0)
							f = c-1;
					}else
					{
						f++;
						if(f>=c)
							f = 0;
					}
					m_Tab.SetCurFocus(f);
					m_Tab.SetFocus();
				}*/
			}
		}break;
	}

	return CControlBar::PreTranslateMessage(pMsg);
}

BOOL CT2TreeBar::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(nCode==(WORD)TVN_SELCHANGED)
	{
		HTREEITEM hItem=m_Tree.GetSelectedItem();
		if(nID==m_Tree.GetDlgCtrlID() && hItem)
		{
			CTC_PARAM *p = (CTC_PARAM*)m_Tree.GetItemData(hItem);
			m_strHint = p!=NULL ? p->lpHint : NULL;
			Invalidate(FALSE);
		}else
		{
			hItem=m_MyRes.GetSelectedItem();
			if(nID==m_MyRes.GetDlgCtrlID() && hItem)
			{
				m_MyRes.GetWindowText(m_strHint);
				m_strHint += m_MyRes.GetPathName(hItem);
				Invalidate(FALSE);
			}
		}
	}
	CT2View* pView = theApp.GetCurView();
	if(pView && nID==m_Tree.GetDlgCtrlID())
	{
		switch(nCode)
		{
		case CN_COMMAND:
			{
				CT2Doc *pDoc = pView->GetDocument();
				pView->m_nOffset+=pDoc->m_nBlockOffset;
				pDoc->Seek(pView->m_nOffset);
				//pView->m_nOffset-=pDoc->m_nBlockOffset;

				pView->OnUpdateData();
				if(pView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
					return TRUE;
			}break;
		case (WORD)CTCN_UPDATE_F:
			if(m_bUpdate==FALSE)
			{
				pView->m_nOffset+=pView->GetDocument()->m_nBlockOffset;
				m_bUpdate=TRUE;
			}return TRUE;
		case (WORD)CTCN_UPDATE_T:
			if(m_bUpdate==TRUE)
			{
				pView->m_nOffset-=pView->GetDocument()->m_nBlockOffset;
				m_bUpdate=FALSE;
			}return TRUE;
		}
	}else
	if(AfxGetMainWnd()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	return CControlBar::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CT2TreeBar::DrawHint(CDC &dc)
{
	dc.SelectObject(m_Tree.GetFont());
	dc.SetBkMode(TRANSPARENT);

	RECT rc;
	GetClientRect(&rc);
	FillRect(dc.m_hDC, &rc, m_hBrush);

	rc.top += m_nSpliter+TABBOX;
	InflateRect(&rc, -2, -2);
	dc.Draw3dRect(&rc, 0, 0);
	InflateRect(&rc, -2, -2);

//prevent 64bit version not work
#ifndef _WIN64
	DWORD dwSize;
	HANDLE h = LoadRes(IDR_ANGELLOGO, dwSize);
	LPPICTURE lpPix = LoadPic(h, dwSize, TRUE);
	HBITMAP hBM;
	lpPix->get_Handle((OLE_HANDLE*)&hBM);
	BITMAP bm;
	GetObject(hBM, sizeof(BITMAP), &bm);

	CDC mdc;
	mdc.CreateCompatibleDC(&dc);
	mdc.SelectObject(hBM);
	dc.BitBlt(rc.right-bm.bmWidth, rc.bottom-bm.bmHeight,
		bm.bmWidth, bm.bmHeight, &mdc, 0, 0, SRCCOPY);
	mdc.DeleteDC();
	lpPix->Release();
#endif

	dc.DrawText(m_strHint, &rc, DT_LEFT|DT_EDITCONTROL|DT_WORDBREAK|DT_CALCRECT);
	dc.DrawText(m_strHint, &rc, DT_LEFT|DT_EDITCONTROL|DT_WORDBREAK);
}

BOOL CT2TreeBar::OnEraseBkgnd(CDC* pDC)
{
	DrawHint(*pDC);

	return TRUE;//CControlBar::OnEraseBkgnd(pDC);
}

void CT2TreeBar::OnTabSel(NMHDR *pNMHDR, LRESULT *pResult)
{
	int i=m_Tab.GetCurSel();
#define swps SWP_NOSIZE|SWP_SHOWWINDOW
#define swph SWP_NOSIZE|SWP_HIDEWINDOW
	::SetWindowPos(m_Tree.m_hWnd, HWND_TOP, 0, TABBOX, 0, 0,
		i==0?swps:swph);
	::SetWindowPos(m_wndPal.m_hWnd, HWND_TOP, 0, TABBOX, 0, 0,
		i==1?swps:swph);
	::SetWindowPos(m_wndFav.m_hWnd, HWND_TOP, 0, TABBOX, 0, 0,
		i==2?swps:swph);
	::SetWindowPos(m_wndDefOption.m_hWnd, HWND_TOP, 0, TABBOX, 0, 0,
		i==3?swps:swph);
	::SetWindowPos(m_MyRes.m_hWnd, HWND_TOP, 0, TABBOX, 0, 0,
		i==4?swps:swph);
	switch(i)
	{
	case 0:
		m_Tree.SetFocus();
		break;
	case 1:
		::SendMessage(m_wndPal.m_hWnd, WM_SETFOCUS, 0, 0);
		break;
	case 2:
		::SendMessage(m_wndFav.m_hWnd, WM_SETFOCUS, 0, 0);
		break;
	case 3:
		::SendMessage(m_wndDefOption.m_hWnd, WM_SETFOCUS, 0, 0);
		break;
	case 4:
		{
			CString r, res;
			m_MyRes.GetWindowText(res);
			m_wndDefOption.GetDlgItemText(IDC_DEF_RESPATH, r);
			if(res.CompareNoCase(r)!=0)
			{
				m_MyRes.SetRedraw(FALSE);
				m_MyRes.DeleteAllItems();
				m_MyRes.SetWindowText(r);
				m_MyRes.LoadTree(r);
				m_MyRes.SetRedraw();
			}
			m_MyRes.SetFocus();
		}break;
	}
	m_strHint.Empty();
	Invalidate(FALSE);
	*pResult = 0;
}

void CT2TreeBar::OnSize(UINT nType, int cx, int cy)
{
	CControlBar::OnSize(nType, cx, cy);

	::SetWindowPos(m_Tab.m_hWnd, HWND_TOP, 0, 0, cx, TABBOX, SWP_NOACTIVATE|SWP_NOMOVE);
	if(m_MyRes.m_hWnd)
		::SetWindowPos(m_MyRes.m_hWnd, HWND_TOP,
			0, 0,
			m_pDockContext->m_rectMRUDockPos.right-m_pDockContext->m_rectMRUDockPos.left-2,
			m_nSpliter,
			SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOREDRAW|SWP_DRAWFRAME);
}

void CT2TreeBar::OnBnClickedButtonRespath()
{
	CString p=GetPath(m_hWnd);
	if(!p.IsEmpty())
	{
		p.AppendChar(_T('\\'));
		theApp.WriteProfileString(CT2Settings, _T("ResPath"), p);
		m_wndDefOption.SetDlgItemText(IDC_DEF_RESPATH, p);
	}
}

int CT2TreeBar::CanSplitte(POINT pt)
{
	DWORD dw=GetDockingFrame()->GetStyle();
	GetDockingFrame()->ModifyStyle(0, dw|WS_SIZEBOX);
	RECT rc;
	GetWindowRect(&rc);
	rc.left = rc.right-TB_SPLITTE_S;
	if(PtInRect(&rc, pt))
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
		return TB_SPLITTE_R;
	}

	GetWindowRect(&rc);
	rc.right = rc.left+TB_SPLITTE_S;
	if(PtInRect(&rc, pt))
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
		return TB_SPLITTE_L;
	}

	GetWindowRect(&rc);
	rc.top = rc.bottom-TB_SPLITTE_S;
	if(PtInRect(&rc, pt))
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		return TB_SPLITTE_B;
	}

	GetWindowRect(&rc);
	rc.bottom = rc.top+TB_SPLITTE_S;
	if(PtInRect(&rc, pt))
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		return TB_SPLITTE_T;
	}

	return 0;
}
