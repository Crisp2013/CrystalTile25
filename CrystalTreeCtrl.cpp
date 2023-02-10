// CrystalTreeCtrl.cpp : 实现文件
//

#include "stdafx.h"

#include ".\CrystalTreeCtrl.h"
#include "..\CrystalTile2\Common.h"

#include <windowsx.h>
#include <commctrl.h>

// CCrystalTreeCtrl

IMPLEMENT_DYNAMIC(CCrystalTreeCtrl, CTreeCtrl)
CCrystalTreeCtrl::CCrystalTreeCtrl()
: m_nSpliter(0)
, m_bSpliter(0)
, m_nBtn(12)
{
}

CCrystalTreeCtrl::~CCrystalTreeCtrl()
{
	DeleteObject(m_hSymbolFont);
}


BEGIN_MESSAGE_MAP(CCrystalTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, OnTvnSelchanging)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTvnSelchanged)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnTvnDeleteitem)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCrystalTreeCtrl 消息处理程序

CTC* CCrystalTreeCtrl::InsertTree(CTC *pCtc, HTREEITEM hParent)
{
	while(pCtc->nLeftNameLen)
	{
		char tree = pCtc->nLeftNameLen&0x80;
		char len = pCtc->nLeftNameLen&0x7F;
		BYTE *pData=(BYTE*)(pCtc+1);
		TCHAR ln[0x81]={0};
		lstrcpyn(ln, (LPTSTR)pData, len+1);
		len*=sizeof(TCHAR);
		//MultiByteToWideChar(CP_GBK, 0, (char*)pData, len, ln, 81);

		CTC_PARAM *dt=NULL;
		if(!tree)
		{
			dt=new CTC_PARAM;
			memset(dt, 0, sizeof(CTC_PARAM));

			if(pCtc->nHintLen)
			{
				int l = pCtc->nHintLen+1;
				dt->lpHint = new TCHAR[l];
				lstrcpyn(dt->lpHint, (LPTSTR)(pData+len), l);
				//memset(dt->lpHint, 0, l*sizeof(TCHAR));
				//MultiByteToWideChar(CP_GBK, 0, (char*)pData+len, l-1, dt->lpHint, 81);
			}
			if(pCtc->nDataLen)
			{
				int l = pCtc->nDataLen+1;
				dt->pData = new TCHAR[l];
				lstrcpyn(dt->pData, (LPTSTR)(pData+len+pCtc->nHintLen*sizeof(TCHAR)), l);
				//memset(dt->pData, 0, l*sizeof(TCHAR));
				//MultiByteToWideChar(CP_GBK, 0, (char*)pData+len+pCtc->nHintLen, l-1, dt->pData, 81);
			}
			dt->nStyle = pCtc->nStyle & (~DATATYPE_MASK);
			dt->nStyle |= pCtc->nExStyle<<8;
		}
		HTREEITEM hSub = InsertItem(TVIF_TEXT|TVIF_PARAM|TVIF_STATE,
			ln, 0, 0, TVIS_BOLD, TVIS_BOLD, (LPARAM)dt, hParent, TVI_LAST);

		pCtc=(CTC*)(pData+len+pCtc->nHintLen*sizeof(TCHAR)+pCtc->nDataLen*sizeof(TCHAR));
		if(tree)
			pCtc = InsertTree(pCtc, hSub);
	}
	pCtc=(CTC*)((BYTE*)pCtc+1);
	return pCtc;
}

BOOL CCrystalTreeCtrl::LoadTree(CTC *pCtc, UINT nAct)
{
	if(pCtc==NULL) return FALSE;//pCtc=g_Ctc;

	SetRedraw(FALSE);
	if(nAct&CTC_ACT_REMOVEALL)
		DeleteAllItems();

	InsertTree(pCtc);

	if(nAct&CTC_ACT_EXPANDALL)
	{
		HTREEITEM hItem = GetRootItem();
		while(hItem)
		{
			Expand(hItem, TVE_EXPAND);
			hItem=GetNextItem(hItem, 1);
		}
	}
	SelectItem(NULL);
	SetRedraw();

	return TRUE;
}

BOOL CCrystalTreeCtrl::LoadTree(CString strPath, HTREEITEM hParent)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(strPath+_T("*.*"), &fd);
	if(hFind==INVALID_HANDLE_VALUE) return FALSE;
	do
	{
		if( fd.cFileName[0]==_T('.') ||
			fd.dwFileAttributes&(FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM) )
			continue;
		UINT s = fd.dwFileAttributes&0x10?TVIS_BOLD:TVIS_CUT;
		HTREEITEM hSub=InsertItem(TVIF_TEXT|TVIF_STATE, fd.cFileName,
			0, 0, s, s, 0, hParent, TVI_LAST);
		if(s==TVIS_BOLD)
		{
			CString sub;
			sub.Format(_T("%s\\%s\\"), strPath, fd.cFileName);
			LoadTree(sub, hSub);
		}
	}while(FindNextFile(hFind, &fd));
#ifndef DEBUG
	CloseHandle(hFind);
#endif
	return TRUE;
}

BOOL CCrystalTreeCtrl::AppendVal(int i, void *pVal, int nType, UINT nMin, UINT nMax)
{
	HTREEITEM hItem = GetRootItem();
	int ii=0;
	hItem = IndexToItem(i, hItem, ii);
#ifdef DEBUG
	CString text=GetItemText(hItem);
#endif
	if(hItem)
	{
		CTC_PARAM *pData = (CTC_PARAM*)GetItemData(hItem);
		if(pData)
		{
			if( pData->pValue &&
				(pData->nStyle&DATATYPE_MASK)==CTCS_THIS )
			{
				delete[] pData->pValue;
				pData->pValue=NULL;
			}
			if(nType!=CTCS_THIS)
			{
				if(pVal==NULL) return FALSE;
				pData->pValue = pVal;
			}
			pData->nMin=nMin;
			pData->nMax=nMax;
			pData->nStyle &= ~DATATYPE_MASK;
			pData->nStyle |= nType&DATATYPE_MASK;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCrystalTreeCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TVS_HASBUTTONS | TVS_FULLROWSELECT | TVS_NOTOOLTIPS;

	return CTreeCtrl::PreCreateWindow(cs);
}

void CCrystalTreeCtrl::DrawItem(HDC hdc)
{
	HBRUSH hB = CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	RECT rc; CString text;
	GetClientRect(&rc);
	int r=rc.right, b=rc.bottom;
	COLORREF clrText=::GetTextColor(hdc);
	HTREEITEM hItem=GetFirstVisibleItem();

	HIMAGELIST hl = (HIMAGELIST)::SendMessage(m_hWnd, TVM_GETIMAGELIST, (UINT)TVSIL_NORMAL, 0);
	CString path;
	GetWindowText(path);
	while(hItem)
	{
		GetItemRect(hItem, &rc, FALSE);
		if(rc.bottom>b)
			break;

		text=GetItemText(hItem);
		UINT uState=GetItemState(hItem, TVIF_STATE);
		CTC_PARAM *pData = (CTC_PARAM*)GetItemData(hItem);
		if(!pData && (uState&TVIS_BOLD))
		{
			SelectObject(hdc, m_hSymbolFont);

			FillRect(hdc, &rc, hB);
			GetItemRect(hItem, &rc, TRUE);
			rc.top++;
			InflateRect(&rc, -2, -1);

			if(hl)
				rc.left-=m_nBtn;

			if(uState&TVIS_EXPANDED)
				DrawText(hdc, _T("6"), 1, &rc, DT_LEFT);
			else
			if(GetChildItem(hItem) || uState&TVIS_BOLD)
				DrawText(hdc, _T("8"), 1, &rc, DT_LEFT);

			SelectObject(hdc, (HFONT)SendMessage(WM_GETFONT));

			rc.left+=m_nBtn;
			rc.right+=m_nBtn;//=r;

			if(uState&TVIS_SELECTED && ::GetFocus()==m_hWnd)
				DrawFocusRect(hdc, &rc);
		}else
		{
			SelectObject(hdc, (HFONT)SendMessage(WM_GETFONT));
			FillRect(hdc, &rc, hB);

			GetItemRect(hItem, &rc, TRUE);
			int tmp;
			if(tmp=IsCmdItem(hItem))
			{
				if(tmp==IDC_STATIC && hl)
				{
					int i=GetIconIndex( pData ? pData->pData : path+GetPathName(hItem));
					ImageList_DrawEx(hl, i, hdc,
						rc.left-m_nBtn-2, rc.top, 16, 16, ::GetSysColor(COLOR_BTNFACE), 0, 0);
				}

				if(pData && (!pData->pData || pData->nStyle&CTCS_READONLY || pData->nStyle&CTCS_EX_DISIBLE))
					::SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
				else
				{
					DWORD dw=GetMessagePos();
					POINT pt={LOWORD(dw),HIWORD(dw)};
					ScreenToClient(&pt);
					if(PtInRect(&rc, pt))
					{
						COLORREF clr=::SetBkColor(hdc, 0xFF9C9C);
						ExtTextOut(hdc, 0, 0, OPAQUE, &rc, NULL, 0, NULL);
						::SetBkColor(hdc, clr);

					}
					::SetTextColor(hdc, 0xFF0000);
				}

				rc.bottom-=2;
				rc.right=r;
			}else
				rc.right=m_nSpliter;

			InflateRect(&rc, -1, -1);
			ExtTextOut(hdc, 0, 0, OPAQUE, &rc, NULL, 0, NULL);

			if(uState&TVIS_SELECTED)
			{
				HBRUSH hH = CreateSolidBrush(::GetFocus()==m_hWnd?0xFFCCCC:(::GetSysColor(COLOR_BTNFACE)));
				FillRect(hdc, &rc, hH);
				DeleteObject(hH);
			}
		}
		rc.top++;
		rc.left++;
		DrawText(hdc, text, text.GetLength(), &rc, DT_LEFT);
		::SetTextColor(hdc, clrText);

		if(pData && (pData->nStyle&CTRLTYPE_MASK)!=CTCS_LINK)
		{
			text = ParamToString(hItem);
			rc.left=m_nSpliter+2;
			rc.right=r;
			rc.top--;
			ExtTextOut(hdc, 0, 0, OPAQUE, &rc, NULL, 0, NULL);

			if(!text.IsEmpty())
			{
				rc.left++;
				rc.top++;
				if(pData->nStyle&CTCS_READONLY||pData->nStyle&CTCS_EX_DISIBLE)
					::SetTextColor(hdc, ::GetSysColor(COLOR_GRAYTEXT));
				DrawText(hdc, text, text.GetLength(), &rc, DT_LEFT|DT_EXPANDTABS|DT_NOPREFIX);
				::SetTextColor(hdc, ::GetSysColor(COLOR_WINDOWTEXT));
			}
		}
		hItem = GetNextVisibleItem(hItem);
	}
	DeleteObject(hB);
}

void CCrystalTreeCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	if(pNMCD->dwDrawStage&CDDS_PREPAINT)
	{
		HDC hWndDC = ::GetDC(m_hWnd);
		HDC hdc = CreateCompatibleDC(hWndDC);
		RECT rc;GetClientRect(&rc);
		int w=rc.right-rc.left, h=rc.bottom-rc.top;
		HBITMAP hbm = CreateCompatibleBitmap(hWndDC, w, h);
		SelectObject(hdc, hbm);

		HBRUSH hbr=CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
		FillRect(hdc, &rc, hbr);
		DeleteObject(hbr);

		SelectObject(hdc, (HFONT)SendMessage(WM_GETFONT));
		SetBkMode(hdc, TRANSPARENT);

		DrawItem(hdc);

		::BitBlt(hWndDC, rc.left, rc.top, w, h, hdc, rc.left, rc.top, SRCCOPY);

		DeleteObject(hbm);
		DeleteDC(hdc);

		::ReleaseDC(m_hWnd, hWndDC);
	}
	*pResult = CDRF_SKIPDEFAULT;
}

void CCrystalTreeCtrl::OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if(pNMTreeView->itemOld.hItem)
	{
		CTC_PARAM *pData = (CTC_PARAM*)GetItemData(pNMTreeView->itemOld.hItem);
		if(pData)
			UpdateData(pNMTreeView->itemOld.hItem);
	}
	*pResult = 0;
}

void CCrystalTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	InitEditCtrl();
	CTC_PARAM *p = (CTC_PARAM*)pNMTreeView->itemNew.lParam;
	if(p)
	{
		MoveEditCtrl();
		//CString text=ParamToString(pNMTreeView->itemNew.hItem);
		//::SetWindowText(m_Edit.m_hWnd, text);
	}

	::SendMessage(::GetParent(m_hWnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(), TVN_SELCHANGED),
		(LPARAM)m_hWnd);

	*pResult = TVNRET_DEFAULT;
}

int CCrystalTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	RECT rc={0};
	m_Edit.Create(WS_CHILD|CBS_DROPDOWN|CBS_AUTOHSCROLL|WS_TABSTOP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, rc, this, IDC_CTC_EDIT);
	m_hEditBtn = CreateWindow(_T("BUTTON"), NULL, WS_CHILD|WS_TABSTOP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0, 0, 0, 0, m_hWnd, (HMENU)(UINT_PTR)IDC_CTC_EDITBTN, AfxGetInstanceHandle(), NULL);
	::SetClassLongPtr(m_hEditBtn, GCLP_HBRBACKGROUND, (LONG)(UINT_PTR)GetStockObject(NULL_BRUSH));

	m_Edit.SetRedraw(FALSE);
	COMBOBOXINFO cbi;
	cbi.cbSize=sizeof(cbi);
	m_Edit.GetComboBoxInfo(&cbi);
	m_hwndItem=cbi.hwndItem;
	m_hwndList=cbi.hwndList;
	::SendMessage(m_hwndItem, WM_SETREDRAW, TRUE, 0);
	::SendMessage(m_hwndList, WM_SETREDRAW, TRUE, 0);
	::SetClassLongPtr(m_hwndItem, GCLP_HBRBACKGROUND, (LONG)(UINT_PTR)GetStockObject(NULL_BRUSH));
	::SetClassLongPtr(m_hwndList, GCLP_HBRBACKGROUND, (LONG)(UINT_PTR)GetStockObject(NULL_BRUSH));

	HRGN r=CreateRectRgn(0,0,0xFFFF,GetItemHeight()-2);
	m_Edit.SetWindowRgn(r, FALSE);
	DeleteObject(r);

	m_hSymbolFont = (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0);
	::SendMessage(m_Edit, WM_SETFONT, (WPARAM)m_hSymbolFont, 0);

	LOGFONT lf;
	GetObject(m_hSymbolFont, sizeof(lf), &lf);
	lf.lfCharSet = SYMBOL_CHARSET;
	lstrcpy(lf.lfFaceName, _T("Marlett"));
	m_hSymbolFont = CreateFontIndirect(&lf);

	::SendMessage(m_hEditBtn, WM_SETFONT, (WPARAM)m_hSymbolFont, 0);

	m_nBtn = abs((int)lf.lfHeight)+2;
	SetIndent(0);

	SetItemHeight(GetItemHeight()+3);

	SetClassLongPtr(m_hWnd, GCL_HBRBACKGROUND, (LONG)(UINT_PTR)GetStockObject(NULL_BRUSH));

	return 0;
}

void CCrystalTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	RECT rc;
	GetClientRect(&rc);

	if(m_bSpliter)
	{
		if( point.x>(m_nBtn<<1) && point.x<(rc.right-m_nBtn))
		{
			m_nSpliter = point.x;
			MoveEditCtrl();
			Invalidate(FALSE);
			return;
		}
	}else
	{
		HTREEITEM hitem=HitTest(point);
		//static HTREEITEM old;
		if(IsCmdItem(hitem))
		{
			RECT rc;
			GetItemRect(hitem, &rc, TRUE);
			if(PtInRect(&rc, point))
			{
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			}
		}
		//if(hitem!=old)
		{
			Invalidate(FALSE);
		//	old=hitem;
		}
	}

	rc.left = m_nSpliter-4;
	rc.right = m_nSpliter+4;
	if(PtInRect(&rc, point))
	{
		HINSTANCE hInst = AfxFindResourceHandle(
			MAKEINTRESOURCE(AFX_IDC_HSPLITBAR), RT_GROUP_CURSOR);
		HCURSOR h=LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_HSPLITBAR));
		SetClassLongPtr(m_hWnd, GCL_HCURSOR, (LONG)(UINT_PTR)h);
	}else
		SetClassLongPtr(m_hWnd, GCL_HCURSOR, (LONG)(UINT_PTR)LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CCrystalTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bSpliter)
	{
		m_bSpliter = 0;
		ReleaseCapture();
		MoveEditCtrl();
	}
	CTreeCtrl::OnLButtonUp(nFlags, point);
}

HTREEITEM CCrystalTreeCtrl::IndexToItem(int i, HTREEITEM hItem, int &iCur)
{
	CString text;
	while(hItem&& i!=iCur)
	{
		HTREEITEM s;
		if(s=GetChildItem(hItem))
		{
			text=GetItemText(s);
			s = IndexToItem(i, s, iCur);
			if(i==iCur) return s;
		}
		iCur++;
		if(i==iCur) return hItem;

		hItem = GetNextItem(hItem, 1);
		text=GetItemText(hItem);
	}
	return hItem;
}

void GetLBText(int iItem, LPTSTR lpItem, CString &Ret)
{
	int i=0;
	while(lpItem && *lpItem)
	{
		LPTSTR p = _tcschr(lpItem, _T(';'));
		if(p==NULL)
		{
			if(i==iItem)
				Ret=lpItem;
			break;
		}else
		{
			*p=0;
			if(i==iItem)
			{
				Ret=lpItem;
				*p=_T(';');
				break;
			}
			*p=_T(';');
			lpItem = p+1;
		}
		i++;
	}
}

CString CCrystalTreeCtrl::ParamToString(HTREEITEM hItem)
{
	CString text;
	if(hItem)
	{
		UINT uState=GetItemState(hItem, TVIF_STATE);
		CTC_PARAM *pData = (CTC_PARAM*)GetItemData(hItem);
		if(pData && pData->pValue)
		{
			int v;
			::SendMessage(::GetParent(m_hWnd), WM_COMMAND,
							MAKELONG(GetDlgCtrlID(), CTCN_UPDATE_F), (LPARAM)m_hWnd);
			switch(pData->nStyle&DATATYPE_MASK)
			{
			case CTCS_THIS:
				text = (LPTSTR)pData->pValue;
				break;

			case CTCS_LPTSTR:
				text = (LPTSTR)pData->pValue;
				break;

			case CTCS_CSTRING:
				text = *(CString*)pData->pValue;
				break;

			case CTCS_WORD:
				v = *(short*)pData->pValue;
				goto update;

			case CTCS_UINT:
				v = *(int*)pData->pValue;
				goto update;

			case CTCS_BYTE:
				v = *(char*)pData->pValue;
			update:
				char bHex=pData->nStyle&CTCS_HEXMODE;
				if(v<0)
				{
					v = -v;
					bHex=0;
					text.AppendChar(_T('+'));
				}
				switch(pData->nStyle&CTRLTYPE_MASK)
				{
				case CTCS_EDIT:
					text.AppendFormat(bHex?_T("%X"):_T("%d"), v);
					return text;

				case CTCS_COMBOBOX:
					if(!(pData->nStyle&CTCS_EX_GETTXT))
					{
						LPTSTR t = pData->pData;
						if(IsCmdItem(hItem))
						{
							LPTSTR p = _tcschr(t, _T(';'));
							if(p)
								t = p+1;
							else
								t = NULL;
						}
						if(v==0)
							text = t;
						else
							GetLBText(v-1, t, text);
					}break;

				case CTCS_COMBOLBOX:
					if(!(pData->nStyle&CTCS_EX_GETTXT))
					{
						LPTSTR t = pData->pData;
						if(IsCmdItem(hItem))
						{
							LPTSTR p = _tcschr(t, _T(';'));
							if(p)
								t = p+1;
							else
								t = NULL;
						}
						GetLBText(v, t, text);
					}break;
				}
				if(pData->nStyle&CTCS_EX_GETTXT)
					text.AppendFormat(bHex?_T("%X"):_T("%d"), v);
				break;
			}
			::SendMessage(::GetParent(m_hWnd), WM_COMMAND,
						MAKELONG(GetDlgCtrlID(), CTCN_UPDATE_T), (LPARAM)m_hWnd);
		}
	}
	return text;
}

HBRUSH CCrystalTreeCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr;

	HTREEITEM hItem = GetSelectedItem();
	if(hItem && (pWnd->m_hWnd==m_Edit.m_hWnd || m_hwndItem==pWnd->m_hWnd))
	{
		CTC_PARAM *pData = (CTC_PARAM*)GetItemData(hItem);
		if(pData && pData->nStyle&CTCS_READONLY)
			pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));

		hbr = CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
	}else
		hbr = CTreeCtrl::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

void CCrystalTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	RECT rc;
	GetClientRect(&rc);
	rc.left = m_nSpliter-4;
	rc.right = m_nSpliter+4;
	if(PtInRect(&rc, point))
	{
		SetCapture();
		m_bSpliter = 1;
		m_Edit.ShowWindow(SW_HIDE);
		return;
	}
	CTreeCtrl::OnLButtonDown(nFlags, point);
	HTREEITEM hItem=HitTest(point);
	if(hItem==NULL)
	{
		SelectItem(NULL);
	}else
	{
		GetItemRect(hItem, &rc, TRUE);
		if(PtInRect(&rc, point))
			ActiveLink(hItem);

		if(point.x<m_nBtn)
		{
			if(hItem)
			{
				Expand(hItem, TVE_TOGGLE);
				MoveEditCtrl();
			}
		}
	}
}

void CCrystalTreeCtrl::MoveEditCtrl()
{
	HTREEITEM hItem = GetSelectedItem();
	CTC_PARAM *pData;
	if(!m_bSpliter && hItem && (pData = (CTC_PARAM*)GetItemData(hItem)) )
	{
		RECT rc;
		GetItemRect(hItem, &rc, FALSE);
		int left = rc.left+m_nSpliter+3;

#define swp (SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOREPOSITION|SWP_NOSENDCHANGING|SWP_NOZORDER)
		switch(pData->nStyle&CTRLTYPE_MASK)
		{
		case CTCS_COMBOBOX:
		case CTCS_COMBOLBOX:
			{
				rc.right-=16;
				::SetWindowPos(m_hEditBtn, HWND_TOP, rc.right, rc.top, 16, rc.bottom-rc.top, swp);
				rc.bottom=rc.top+256;
			}break;
		case CTCS_PATHBOX:
			rc.right-=16;
			::SetWindowPos(m_hEditBtn, HWND_TOP, rc.right, rc.top, 16, rc.bottom-rc.top, swp);
			break;

		case CTCS_LINK:
			return;
		}
		InflateRect(&rc, 0, -2);

		::SetWindowPos(m_Edit.m_hWnd, HWND_TOP, left, rc.top, rc.right-left-1,rc.bottom-rc.top, swp|SWP_NOREDRAW);

		::GetWindowRect(m_Edit.m_hWnd, &rc);
		::SetWindowPos(m_hwndItem, HWND_TOP, 0, 0, rc.right-rc.left, rc.bottom-rc.top, swp|SWP_NOREDRAW);
	}
}

void CCrystalTreeCtrl::InitEditCtrl()
{
	HTREEITEM hItem = GetSelectedItem();
	CTC_PARAM *pData;
	int ct;
	if( hItem && (pData = (CTC_PARAM*)GetItemData(hItem)) &&
		(ct=pData->nStyle&CTRLTYPE_MASK)!=CTCS_LINK )
	{
		m_Edit.ResetContent();
		switch(ct)
		{
		case CTCS_COMBOBOX:
		case CTCS_COMBOLBOX:
			{
				::SetWindowText(m_hEditBtn, _T("6"));

				LPTSTR lpItem = pData->pData;
				if(IsCmdItem(hItem))
				{
					LPTSTR p = _tcschr(lpItem, _T(';'));
					if(p)
						lpItem = p+1;
					else
						lpItem = NULL;
				}
				while(lpItem && *lpItem)
				{
					LPTSTR p = _tcschr(lpItem, _T(';'));
					if(p==NULL)
					{
						m_Edit.AddString(lpItem);
						break;
					}else
					{
						*p=0;
						m_Edit.AddString(lpItem);
						*p=_T(';');
						lpItem = p+1;
					}
				}
			}break;
		case CTCS_PATHBOX:
			::SetWindowText(m_hEditBtn, _T("i"));
			break;

		case CTCS_LINK:
		default:
			::SetWindowPos(m_hEditBtn, HWND_TOP, 0, 0, 0, 0,
				SWP_HIDEWINDOW|SWP_NOACTIVATE|SWP_NOREDRAW|SWP_NOMOVE|SWP_NOSIZE);
			break;
		}

		if(ct==CTCS_COMBOLBOX || pData->nStyle&CTCS_READONLY)
			::SendMessage(m_hwndItem, EM_SETREADONLY, TRUE, 0L);
		else
			::SendMessage(m_hwndItem, EM_SETREADONLY, FALSE, 0L);


		Reflash(FALSE);
		//CString text=ParamToString(hItem);
		//::SetWindowText(m_Edit.m_hWnd, text);
		//Invalidate(FALSE);
	}else
	{
		::ShowWindow(m_Edit.m_hWnd, SW_HIDE);
		::ShowWindow(m_hEditBtn, SW_HIDE);
	}
}

BOOL CCrystalTreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_CHAR:
		{
			HTREEITEM hItem=GetSelectedItem();
			if(hItem==NULL) break;
			if(pMsg->hwnd==m_Edit.m_hWnd || ::GetParent(pMsg->hwnd)==m_Edit.m_hWnd)
			{
				CTC_PARAM *pData = (CTC_PARAM*)GetItemData(hItem);
				TCHAR ch=(TCHAR)LOWORD(pMsg->wParam);
				if(pData->nStyle&CTCS_HEXMODE)
				{
					if(!( (ch>='0' && ch<='9') || ch==_T('+') || ch==VK_BACK ||
						((ch>='a' && ch<='f') || (ch>='A' && ch<='F')) ))
						return TRUE;
				}else
				if( (pData->nStyle&DATATYPE_MASK)>=CTCS_BYTE && (pData->nStyle&CTRLTYPE_MASK)!=CTCS_COMBOBOX)
				{
					if(!((ch>='0' && ch<='9') || ch==_T('+') || ch==VK_BACK))
						return TRUE;
				}
			}else
			{
				CTC_PARAM *p=(CTC_PARAM*)GetItemData(hItem);
				if(pMsg->hwnd==m_hWnd && hItem && p)
				{
					if((p->nStyle&CTRLTYPE_MASK)!=CTCS_LINK)
					{
						::SetFocus(m_Edit.m_hWnd);
						::SendMessage(m_Edit.m_hWnd, EM_SETSEL, 0, -1);
						m_Edit.SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
						return TRUE;
					}
				}
			}
		}break;

	case WM_KEYDOWN:
		{
			WORD wKey=LOWORD(pMsg->wParam);
			switch(wKey)
			{
			case VK_F4:
				if(pMsg->hwnd==m_hwndItem)
				{
					ShowDropDown();
					return TRUE;
				}break;

			case VK_UP:
			case VK_DOWN:
				{
					HTREEITEM hItem=GetSelectedItem();
					if(hItem && (pMsg->hwnd==m_Edit.m_hWnd||pMsg->hwnd==m_hwndItem))
					{
						CTC_PARAM *pData = (CTC_PARAM*)GetItemData(hItem);
						int dt=pData->nStyle&DATATYPE_MASK;
						int step = wKey==VK_UP?-1:1;
						if(pData && (pData->nStyle&CTRLTYPE_MASK)==CTCS_EDIT)
						{
							switch(dt)
							{
							case CTCS_BYTE:
								*(BYTE*)pData->pValue += step;
								break;
							case CTCS_WORD:
								*(WORD*)pData->pValue += step;
								break;
							case CTCS_UINT:
								*(UINT*)pData->pValue += step;
								break;
							}
							Reflash();
							::SendMessage(::GetParent(m_hWnd), WM_COMMAND,
								MAKELONG(GetDlgCtrlID(), CN_COMMAND), (LPARAM)m_hWnd);
						}
					}
				}break;

			case VK_TAB:
				{
					HTREEITEM hItem=GetSelectedItem();
					if(hItem)
					{
						CTC_PARAM *pData = (CTC_PARAM*)GetItemData(hItem);
						if(pData && (pData->nStyle&CTRLTYPE_MASK)!=CTCS_LINK)
						{
							if(GetKeyState(VK_SHIFT)&0x80)
							{
								if(pMsg->hwnd==m_hEditBtn)
								{
									::SetFocus(m_Edit.m_hWnd);
									::SendMessage(m_Edit.m_hWnd, EM_SETSEL, 0, -1);
								}else
								if(pMsg->hwnd==m_Edit.m_hWnd || ::GetParent(pMsg->hwnd)==m_Edit.m_hWnd)
								{
									UpdateData(GetSelectedItem());
									::SetFocus(m_hWnd);
								}else
								if(pMsg->hwnd==m_hWnd)
									break;
							}else
							{
								if(pMsg->hwnd==m_hWnd)
								{
									::SetFocus(m_Edit.m_hWnd);
									::SendMessage(m_Edit.m_hWnd, EM_SETSEL, 0, -1);
								}else
								if((pMsg->hwnd==m_Edit.m_hWnd || ::GetParent(pMsg->hwnd)==m_Edit.m_hWnd)
									&& (pData->nStyle&(CTCS_COMBOBOX|CTCS_COMBOLBOX|CTCS_PATHBOX)))
									::SetFocus(m_hEditBtn);
								else
								if(pMsg->hwnd==m_hEditBtn)
								{
									UpdateData(GetSelectedItem());
									break;
								}
							}
							return TRUE;
						}
					}
				}break;

			case VK_ESCAPE:
				{
					if(pMsg->hwnd!=m_hWnd)
					{
						Reflash();
						::SetFocus(m_hWnd);
						return TRUE;
					}
				}break;

			case VK_RETURN:
				{
					if(pMsg->hwnd==m_Edit.m_hWnd || ::GetParent(pMsg->hwnd)==m_Edit.m_hWnd)
					{
						UpdateData(GetSelectedItem());
						SetFocus();
						return TRUE;
					}
				}
			case VK_SPACE:
				{
					if(ActiveLink(GetSelectedItem()) || wKey==VK_RETURN)
						return TRUE;
				}break;
			}
		}break;

	case WM_LBUTTONDOWN:
		{
			HTREEITEM hItem = GetSelectedItem();
			RECT rc;
			::GetWindowRect(m_Edit.m_hWnd, &rc);
			if(pMsg->hwnd!=m_Edit.m_hWnd && hItem && PtInRect(&rc, pMsg->pt))
			{
				CTC_PARAM *pData = (CTC_PARAM*)GetItemData(hItem);
				if(pData && (pData->nStyle&CTRLTYPE_MASK)==CTCS_COMBOLBOX)
				{
					ShowDropDown();
					return TRUE;
				}
			}
		}break;
	}

	return CTreeCtrl::PreTranslateMessage(pMsg);
}

void CCrystalTreeCtrl::UpdateData(HTREEITEM hItem)
{
	if(hItem==NULL) return;
	CTC_PARAM *pData = (CTC_PARAM*)GetItemData(hItem);
	if(pData==NULL || pData->nStyle&CTCS_READONLY || pData->nStyle&CTCS_EX_DISIBLE) return;

	CString t = ParamToString(hItem);
	CString t2;
	m_Edit.GetWindowText(t2);
	if(t2.Compare(t)==0) return;

	if(pData)
	{
		int ct=pData->nStyle&CTRLTYPE_MASK;
		if(ct!=CTCS_LINK)
		{
			switch(pData->nStyle&DATATYPE_MASK)
			{
			case CTCS_THIS:
				if(pData->pValue) delete[] pData->pValue;
				pData->pValue = new TCHAR[t2.GetLength()+1];
				lstrcpy((LPTSTR)pData->pValue, t2);
				break;

			case CTCS_LPTSTR:
				lstrcpyn((LPTSTR)pData->pValue, t2, pData->nMax);
				break;

			case CTCS_CSTRING:
				*(CString*)pData->pValue = t2;
				break;

			case CTCS_BYTE:
			case CTCS_WORD:
			case CTCS_UINT:
				{
					UINT v=-1;
					BOOL bOk=TRUE;
					switch(ct)
					{
					/*case CTCS_EDIT:
						if(!_stscanf(t2, pData->nStyle&CTCS_HEXMODE?_T("%X"):_T("%d"), &v))
							bOk=FALSE;
						break;*/

					case CTCS_COMBOBOX:
						v = m_Edit.GetCurSel();
						if(v==-1)
						{
							if(pData->lpText) delete[] pData->lpText;
							v=m_Edit.GetWindowTextLength()+1;
							pData->lpText = new TCHAR[v];
							::GetWindowText(m_Edit.m_hWnd, pData->lpText, v);
							v = 0;
						}else
							v++;
						if(pData->nStyle&CTCS_EX_GETTXT && m_Edit.FindString(0, t2)==-1)
						{
							CTC_PARAM *p = (CTC_PARAM*)GetItemData(hItem);
							if(p)
							{
								int len = t2.GetLength()+2;
								if(p->pData)
									len += lstrlen(p->pData);
								LPTSTR s = new TCHAR[len];
								if(p->pData)
								{
									_stprintf(s, _T("%s;%s"), t2, p->pData);
									delete[] p->pData;
								}else
									_stprintf(s, _T("%s"), t2);
								p->pData=s;
							}
							m_Edit.InsertString(0, t2);
						}
						break;

					case CTCS_COMBOLBOX:
						v = m_Edit.GetCurSel();
						break;
					}
					char bAdd, bHex=pData->nStyle&CTCS_HEXMODE;
					if(bAdd=t2.GetAt(0)==_T('+'))
					{
						t2.Delete(0);
						bHex=FALSE;
					}
					if(pData->nStyle&CTCS_EX_GETTXT || ct==CTCS_EDIT)
						if(!_stscanf(t2, bHex?_T("%X"):_T("%d"), &v))
							bOk=FALSE;
					if(bOk && (pData->nMax==0 ||(v>=pData->nMin && v<=pData->nMax)))
					{
						v = bAdd ? -(int)v : v;
						switch(pData->nStyle&DATATYPE_MASK)
						{
						case CTCS_BYTE:
							*(BYTE*)pData->pValue = (BYTE)v;
							break;

						case CTCS_WORD:
							*(WORD*)pData->pValue = (WORD)v;
							break;

						case CTCS_UINT:
							*(UINT*)pData->pValue = (UINT)v;
							break;
						}
					}
				}break;

			}

			// 通知父窗口数据已经更新
			int id=IsCmdItem(hItem);
			if(id && id!=IDC_STATIC)
			{
				//if(pData->nStyle&CTCS_EX_CMDRANGE && (ct==CTCS_COMBOBOX||ct==CTCS_COMBOLBOX))
				//	id+=m_Edit.GetCurSel();
				::SendMessage(::GetParent(m_hWnd), WM_COMMAND,
					MAKELONG(id, CN_COMMAND), (LPARAM)m_hWnd);
			}else
				::SendMessage(::GetParent(m_hWnd), WM_COMMAND,
					MAKELONG(GetDlgCtrlID(), CN_COMMAND), (LPARAM)m_hWnd);

			Reflash();
		}
	}
}

LRESULT CCrystalTreeCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_KILLFOCUS:
		if( (HWND)wParam!=m_hEditBtn && (HWND)wParam!=m_hwndItem && (HWND)wParam!=m_Edit.m_hWnd)
		{
			//::ShowWindow(m_Edit.m_hWnd, SW_HIDE);
			//::ShowWindow(m_hEditBtn, SW_HIDE);
			::SetWindowPos(m_Edit.m_hWnd, HWND_TOP, 0,0,0,0, SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREDRAW);
			::SetWindowPos(m_hEditBtn, HWND_TOP, 0,0,0,0, SWP_HIDEWINDOW|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOREDRAW);
		}break;

	case WM_SETFOCUS:
		MoveEditCtrl();
		break;

	case WM_VSCROLL:
	case WM_HSCROLL:
	case WM_MOUSEWHEEL:
		SelectItem(NULL);
		//Invalidate(FALSE);
		break;

	case WM_COMMAND:
		{
			WORD id = LOWORD(wParam);
			WORD code = HIWORD(wParam);
			if(code==CBN_CLOSEUP)
			{
				int i = m_Edit.GetCurSel();
				if(i!=-1)
				{
					CString t;
					m_Edit.GetLBText(i, t);
					::SetWindowText(m_Edit.m_hWnd, t);
					UpdateData(GetSelectedItem());
					SetFocus();
				}
			}else
			if(id==IDC_CTC_EDITBTN)
			{
				HTREEITEM hItem=GetSelectedItem();
				if(hItem)
				{
					CTC_PARAM *pData = (CTC_PARAM*)GetItemData(hItem);

					LPTSTR pStr = NULL;//(LPTSTR)::SendMessage(::GetParent(m_hWnd), WM_COMMAND, IDC_CTC_EDITBTN, 0);
					TCHAR str[_MAX_PATH];
					if(pStr==NULL && pData)
						switch(pData->nStyle&CTRLTYPE_MASK)
						{
						case CTCS_COMBOBOX:
						case CTCS_COMBOLBOX:
							{
								m_Edit.GetWindowText(str, _MAX_PATH);
								pStr=str;
								ShowDropDown();
							}break;

						case CTCS_PATHBOX:
							{
								static CFileDialog fd(TRUE);
								if(fd.DoModal()!=IDOK) break;
								pStr = fd.m_pOFN->lpstrFile;
							}break;
						}
					::SetWindowText(m_Edit.m_hWnd, pStr);
				}
			}
		}break;
	}
	return CTreeCtrl::DefWindowProc(message, wParam, lParam);
}

void CCrystalTreeCtrl::OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CTC_PARAM *pData = (CTC_PARAM*) pNMTreeView->itemOld.lParam;
	if(pData)
	{
		CString t= GetItemText(pNMTreeView->itemOld.hItem);
		if(pData->lpHint) delete[] pData->lpHint;
		if(pData->lpText) delete[] pData->lpText;
		if(pData->pData) delete[] pData->pData;
		if((pData->nStyle&DATATYPE_MASK) == CTCS_THIS)
			if(pData->pValue) delete[] pData->pValue;

		delete pData;
	}

	*pResult = 0;
}

void CCrystalTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	HTREEITEM hItem=HitTest(point);
	if(hItem)
	{
		CTC_PARAM *pData=(CTC_PARAM*)GetItemData(hItem);
		if(pData)
		{
			switch(pData->nStyle&CTRLTYPE_MASK)
			{
			case CTCS_COMBOBOX:
			case CTCS_COMBOLBOX:
				{
					int sel=m_Edit.GetCurSel();
					if(sel==-1)
					{
						CString t;
						m_Edit.GetWindowText(t);
						sel=m_Edit.FindString(sel, t);
					}
					if((sel+1)==m_Edit.GetCount())
						m_Edit.SetCurSel(0);
					else
						m_Edit.SetCurSel(sel+1);
					UpdateData(hItem);
					Reflash();
				}break;
			}
			::SetFocus(m_Edit.m_hWnd);
			::SendMessage(m_Edit.m_hWnd, EM_SETSEL, 0, -1);
		}else
		{
			Expand(hItem, TVE_TOGGLE);
			MoveEditCtrl();
			return;
		}
	}
	CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void CCrystalTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CTreeCtrl::OnSize(nType, cx, cy);

	if(m_nSpliter<m_nBtn) m_nSpliter = cx/4;

	MoveEditCtrl();
}

void CCrystalTreeCtrl::ShowDropDown()
{
	::SetFocus(m_Edit.m_hWnd);
	m_Edit.ShowDropDown(TRUE);

	RECT rc;
	GetItemRect(GetSelectedItem(), &rc, FALSE);
	rc.left=m_nSpliter+2;
	ClientToScreen(&rc);
	RECT rc2;
	::GetWindowRect(m_hwndList, &rc2);
	::SetWindowPos(m_hwndList, HWND_TOP, rc.left, rc.bottom,
		rc.right-rc.left, rc2.bottom-rc2.top, SWP_NOACTIVATE);
}

BOOL CCrystalTreeCtrl::ActiveLink(HTREEITEM hItem)
{
	UINT id;
	if(id=IsCmdItem(hItem))
	{
		CTC_PARAM *p=(CTC_PARAM*)GetItemData(hItem);
		if(id!=IDC_STATIC)
		{
			if(p->nStyle&CTCS_EX_DISIBLE)
				return FALSE;
			::SendMessage(::GetParent(m_hWnd), WM_COMMAND, MAKELONG(id, CN_COMMAND), (LPARAM)m_hWnd);
		}else
		{
			CString res;
			GetWindowText(res);
			::ShellExecute(NULL, _T("OPEN"), p?p->pData:res+GetPathName(hItem), NULL, NULL, SW_SHOWNORMAL);
		}
		SetFocus();
		return TRUE;
	}
	return FALSE;
}

UINT CCrystalTreeCtrl::IsCmdItem(HTREEITEM hItem)
{
	if(hItem!=NULL)
	{
		CTC_PARAM *p = (CTC_PARAM*)GetItemData(hItem);
		if(p && p->pData)
		{
			if(*p->pData==_T('#'))
			{
				LPTSTR pID = sFindChar(p->pData, _T(';'));
				if(pID) *pID=0;
				UINT nID=0;
				_stscanf(p->pData+1, _T("%d"), &nID);
				if(pID) *pID=_T(';');

				int ct=p->nStyle&CTRLTYPE_MASK;
				if(p->nStyle&CTCS_EX_CMDRANGE && (ct==CTCS_COMBOBOX||ct==CTCS_COMBOLBOX))
				{
					int i=m_Edit.GetCurSel();
					if(i!=-1)
						nID+=i;
				}
				return nID;
			}else
			if((p->nStyle&CTRLTYPE_MASK)==CTCS_LINK)
				return IDC_STATIC;
		}else
		if(GetItemState(hItem, TVIF_STATE)&TVIS_CUT)
			return IDC_STATIC;
	}
	return 0;
}

void CCrystalTreeCtrl::OnClear(HTREEITEM hItem)
{
	if(hItem==NULL) return;
	while(hItem)
	{
		HTREEITEM hChild = GetChildItem(hItem);
		if(hChild)
			OnClear(hChild);
		else
		{
			CTC_PARAM *p = (CTC_PARAM*)GetItemData(hItem);
			if( p && (p->nStyle&DATATYPE_MASK)!=CTCS_THIS)
			{
				p->pValue = NULL;
				p->nStyle &= ~DATATYPE_MASK;
			}
		}
		hItem = GetNextItem(hItem, 1);
	}
}

void CCrystalTreeCtrl::ClearVar()
{
	HTREEITEM hItem = GetRootItem();
	OnClear(hItem);
	Invalidate(FALSE);
}

void CCrystalTreeCtrl::Reflash(BOOL bErase)
{
	HTREEITEM hItem=GetSelectedItem();
	if(hItem)
	{
		CString text=ParamToString(hItem);
		DWORD sel=m_Edit.GetEditSel();
		::SetWindowText(m_Edit.m_hWnd, text);
		m_Edit.SetEditSel(LOWORD(sel), HIWORD(sel));
	}
	if(bErase) Invalidate(FALSE);
}

void CCrystalTreeCtrl::SetVarData(HTREEITEM hItem, CString &strData)
{
	if(hItem)
	{
		CTC_PARAM *p = (CTC_PARAM*)GetItemData(hItem);
		if(p)
		{
			UINT id = IsCmdItem(hItem);
			if(p->pData) delete[] p->pData;

			CString si;
			if(id!=0 && id!=IDC_STATIC)
				si.Format(_T("#%d;"), id);
			si.Append(strData);
			int l=si.GetLength()+1;
			p->pData = new TCHAR[l];
			si.Append(strData);
			lstrcpyn(p->pData, si, l);
		}
	}
}

CString CCrystalTreeCtrl::GetPathName(HTREEITEM hItem)
{
	HTREEITEM hParent=GetParentItem(hItem);
	CString p;
	if(hParent)
		p = GetPathName(hParent)+_T("\\");
	return p+GetItemText(hItem);
}
