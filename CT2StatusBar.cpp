// CT2StatusBar.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\CT2StatusBar.h"
#include "CT2HexView.h"

// CT2StatusBar

IMPLEMENT_DYNAMIC(CT2StatusBar, CStatusBar)
CT2StatusBar::CT2StatusBar()
: m_nPos(0)
{
}

CT2StatusBar::~CT2StatusBar()
{
}


BEGIN_MESSAGE_MAP(CT2StatusBar, CStatusBar)
	ON_WM_PAINT()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CT2StatusBar 消息处理程序

void CT2StatusBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	UpdateAllPanes(FALSE, TRUE);

	CRect rc;	GetClientRect(&rc);

	CDC memDC;	memDC.CreateCompatibleDC(&dc);
	CBitmap bm;	bm.CreateCompatibleBitmap(&dc,rc.Width(),rc.Height());
	memDC.SelectObject(&bm);

	memDC.SelectObject(GetFont());	memDC.SetBkMode(TRANSPARENT);

	//memDC.FillRect(&rc, &CBrush(GetSysColor(COLOR_BTNFACE)));
	memDC.DrawFrameControl(&rc, DFC_SCROLL,DFCS_SCROLLSIZEGRIP);
	rc.InflateRect(0,0,-GetSystemMetrics(SM_CXVSCROLL),0);

	UINT nLeft = rc.left, nID, nStyle; int cxWidth;
	CString strText;CSize szWidth;
	for(int i=m_nCount-1; i>=0; i--)
	{
		GetPaneInfo(i, nID, nStyle, cxWidth);
		strText = GetPaneText(i);
		if(nID==ID_PROGRESSBAR)
		{
			szWidth.cx=104; //100%+4;
			COLORREF clrColor = StrToIntEX(strText);
			CRect rcPos(rc);
			rcPos.left = rcPos.right-szWidth.cx;
			rcPos.InflateRect(-2,-2,-2,-2);
			if(m_nPos)
			{	// 百分比模式 绘制进度栏
				rcPos.right = rcPos.left+m_nPos;
				memDC.FillRect(rcPos, &CBrush(0xFF4040));
			}else
			if(clrColor!=-1)
				memDC.FillRect(rcPos, &CBrush(clrColor));
		}else
		{
			szWidth = memDC.GetTextExtent(strText);
			szWidth.cx+=4;
		}
		if((i==0) && (nID==ID_SEPARATOR))
		{
			rc.left=nLeft;
			CString idle;idle.LoadString(AFX_IDS_IDLEMESSAGE);
			//if(strText.IsEmpty()) strText = idle;
			if(!strText.IsEmpty() && strText!=idle)
			//if(strText!=idle)
			{
				memDC.FillRect(&rc, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
				memDC.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			}else
			{
				MEMORYSTATUS ms;
				GlobalMemoryStatus(&ms);
				strText.Format(idle, ms.dwAvailPhys/(1024*1024));
			}
		}else
		{
			if(strText.IsEmpty()) continue; // 消隐空面板
			rc.left = rc.right-szWidth.cx;
			if(i==1)
				memDC.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		}
		CRect rcDraw(rc);
		memDC.Draw3dRect(&rc, GetSysColor(COLOR_GRAYTEXT), GetSysColor(COLOR_GRAYTEXT));
		rcDraw.InflateRect(-2,-3,-2,-2);
		memDC.DrawText(strText, &rcDraw, nID==ID_PROGRESSBAR?DT_CENTER:DT_LEFT);
		rc.right-=szWidth.cx+2;
	}
	GetClientRect(&rc);
	dc.BitBlt(rc.left,rc.top,rc.Width(),rc.Height(), &memDC,0,0,SRCCOPY);
	bm.DeleteObject();	memDC.DeleteDC();
}

void CT2StatusBar::SetProgressPos(BYTE nPos)
{
	(nPos>=100)?m_nPos=100:m_nPos=nPos;
}

BOOL CT2StatusBar::SetIndicators(const UINT* lpIDArray, int nIDCount)
{
	ASSERT_VALID(this);
	ASSERT(nIDCount >= 1);  // must be at least one of them
	ASSERT(lpIDArray == NULL ||
		AfxIsValidAddress(lpIDArray, sizeof(UINT) * nIDCount, FALSE));
	ASSERT(::IsWindow(m_hWnd));

	// first allocate array for panes and copy initial data
	#define SBPF_UPDATE 0x0001  // pending update of text
	struct AFX_STATUSPANE
	{
		UINT    nID;        // IDC of indicator: 0 => normal text area
		int     cxText;     // width of string area in pixels
							//   on both sides there is a 3 pixel gap and
							//   a one pixel border, making a pane 6 pixels wider
		UINT    nStyle;     // style flags (SBPS_*)
		UINT    nFlags;     // state flags (SBPF_*)
		CString strText;    // text in the pane
	};
	if (!AllocElements(nIDCount, sizeof(AFX_STATUSPANE)))
		return FALSE;
	ASSERT(nIDCount == m_nCount);

	// copy initial data from indicator array
	BOOL bResult = TRUE;
	if (lpIDArray != NULL)
	{
		HFONT hFont = (HFONT)SendMessage(WM_GETFONT);
		CClientDC dcScreen(NULL);
		HGDIOBJ hOldFont = NULL;
		if (hFont != NULL)
			hOldFont = dcScreen.SelectObject(hFont);

		AFX_STATUSPANE* pSBP = (AFX_STATUSPANE*)m_pData;//_GetPanePtr(0);
		for (int i = 0; i < nIDCount; i++)
		{
			pSBP->nID = *lpIDArray++;
			pSBP->nFlags |= SBPF_UPDATE;
			if (pSBP->nID != 0)
			{
				pSBP->cxText = 0;//dcScreen.GetTextExtent(pSBP->strText).cx;
				//ASSERT(pSBP->cxText >= 0);
				if (!SetPaneText(i, pSBP->strText, FALSE))
				{
					bResult = FALSE;
					break;
				}
			}
			else
			{
				// no indicator (must access via index)
				// default to 1/4 the screen width (first pane is stretchy)
				pSBP->cxText = ::GetSystemMetrics(SM_CXSCREEN);
				if (i == 0)
					pSBP->nStyle |= (SBPS_STRETCH | SBPS_NOBORDERS);
			}
			++pSBP;
		}
		if (hOldFont != NULL)
			dcScreen.SelectObject(hOldFont);
	}
	UpdateAllPanes(TRUE, TRUE);

	return bResult;
}

BOOL CT2StatusBar::SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate)
{
	if(lstrcmp(lpszNewText, L"就绪")==0)
		int a=1;
	BOOL bRet = CStatusBar::SetPaneText(nIndex, lpszNewText, bUpdate);
	Invalidate(FALSE);
	return bRet;
}

void CT2StatusBar::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt=*(POINT*)((BYTE*)pNMHDR+20);
	int nItem = HitTest(pt);
	if(nItem!=-1)
	{
		UINT nID = GetItemID(nItem);
		if(nID==IDC_HEXVIEW_BITS)
		{
			CRect rc;
			GetItemRect(nItem, rc);
			CDC *pDC=GetDC();
			pDC->SelectObject(GetFont());
			CSize szW = pDC->GetTextExtent(_T("0"));
			int nBit=(pt.x-rc.left)/szW.cx;
			CT2HexView *pHex = (CT2HexView*)theApp.GetView(ID_VIEW_HEXVIEW);
			if(pHex) pHex->SetBits(nBit);
			ReleaseDC(pDC);
		}
	}

	*pResult = 0;
}

int CT2StatusBar::HitTest(CPoint pt)
{
	int i, nItem=-1; CRect rc;
	for(i=m_nCount-1; i>=0; i--)
	{
		GetItemRect(i, rc);
		if(rc.PtInRect(pt))
		{
			nItem = i;
			break;
		}
	}
	return nItem;
}

void CT2StatusBar::GetItemRect(int nItem, CRect& rcItem)
{
	CDC *pDC=GetDC();
	pDC->SelectObject(GetFont());
	CRect rc; GetClientRect(&rc); rc.InflateRect(0,0,-16,0);
	UINT nLeft = rc.left, nID, nStyle; int cxWidth;
	CString strText;CSize szWidth;
	int i;
	rcItem.SetRectEmpty();
	for(i=m_nCount-1; i>=0; i--)
	{
		GetPaneInfo(i, nID, nStyle, cxWidth);
		strText = GetPaneText(i);
		if(nID==ID_PROGRESSBAR)
		{
			szWidth.cx=104; //100%+4;
		}else
		{
			szWidth = pDC->GetTextExtent(strText);
			szWidth.cx+=4;
		}
		if((i==0) && (nID==ID_SEPARATOR))
			rc.left=nLeft;
		else
		{
			if(strText.IsEmpty()) continue; // 消隐空面板
			rc.left = rc.right-szWidth.cx;
		}
		if(i==nItem)
		{
			rcItem = rc;
			break;
		}
		rc.right-=szWidth.cx+2;
	}
	ReleaseDC(pDC);
}

void CT2StatusBar::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	Invalidate(FALSE);

	CStatusBar::OnTimer(nIDEvent);
}

void CT2StatusBar::BeginProgress(UINT nIDs)
{
	AfxGetApp()->BeginWaitCursor();
	GetParentFrame()->EnableWindow(FALSE);
	SetProgressPos(0);
	if(nIDs)
		SetPaneText(CommandToIndex(ID_PROGRESSBAR), LoadStr(nIDs));

	SetTimer(0, 500, NULL);
}

void CT2StatusBar::EndProgress()
{
	GetParentFrame()->EnableWindow(TRUE);
	SetProgressPos(0);
	SetPaneText(CommandToIndex(ID_PROGRESSBAR), _T(""));
	KillTimer(0);
	AfxGetApp()->EndWaitCursor();
}
