// CT2HexView.cpp : CT2HexView 类的实现
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include "CT2Doc.h"
#include ".\CT2HexView.h"
#include "CT2TileView.h"
#include "RelativeDlg.h"
#include "CT2ChildFrm.h"
#include "SDatInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);
// CT2HexView

IMPLEMENT_DYNCREATE(CT2HexView, CT2View)

BEGIN_MESSAGE_MAP(CT2HexView, CT2View)
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(IDC_HEXVIEW_TBL, OnEnableTbl)
	ON_UPDATE_COMMAND_UI(IDC_HEXVIEW_TBL, OnUpdateEnableTbl)
	ON_COMMAND_RANGE(ID_VIEW_2BYTEALIGN, ID_VIEW_4BYTEALIGN, OnViewalign)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_2BYTEALIGN, ID_VIEW_4BYTEALIGN, OnUpdateViewalign)
	ON_COMMAND(ID_VIEW_COLORTEXT, OnViewColortext)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLORTEXT, OnUpdateViewColortext)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_HEXVIEW_SWPCODE, OnSwpCode)
	ON_UPDATE_COMMAND_UI(ID_HEXVIEW_SWPCODE, OnUpdateViewSwpCode)
	ON_COMMAND(ID_SEARCH_FIND, OnSearchFind)
	ON_COMMAND(ID_SEARCH_REPLACE, OnSearchReplace)
	ON_COMMAND(ID_SEARCH_FINDNEXT, OnSearchFindnext)
	ON_COMMAND(ID_EDIT_DATA2PAL, OnEditData2pal)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DATA2PAL, OnUpdateEditData2pal)
	ON_COMMAND(ID_EDIT_PAL2DATA, OnEditPal2data)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PAL2DATA, OnUpdateEditPal2data)
	ON_COMMAND(ID_SEARCH_LZ77, OnSearchLz77)
	ON_COMMAND(ID_TOOLS_LZ77UNCOMP, OnToolsLz77uncomp)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_LZ77UNCOMP, OnUpdateToolsLz77uncomp)
	ON_COMMAND(ID_SEARCH_RELATIVE, OnSearchRelative)
	ON_COMMAND(ID_EDIT_EXPORT, OnExportData)
	ON_UPDATE_COMMAND_UI(ID_EDIT_EXPORT, OnUpdateExportData)
	ON_COMMAND(ID_EDIT_IMPORT, OnImportData)
	ON_COMMAND(ID_HEXVIEW_IMPORT, OnImportText)
	ON_UPDATE_COMMAND_UI(ID_HEXVIEW_IMPORT, OnUpdateImportText)
	ON_COMMAND(IDC_HEXVIEW_TBLCTRL, OnHexviewTblctrl)
	ON_UPDATE_COMMAND_UI(IDC_HEXVIEW_TBLCTRL, OnUpdateHexviewTblctrl)
	ON_UPDATE_COMMAND_UI(ID_EDIT_IMPORT, OnUpdateImport)
	ON_WM_RBUTTONDOWN()
	//ON_COMMAND_RANGE(ID_FONT_LIST, ID_FONT_LIST+99, OnFontList)
	ON_COMMAND(ID_FONT_LIST, OnFontList)
	ON_COMMAND(ID_TOOLS_GETCRC16, OnToolsGetcrc16)
END_MESSAGE_MAP()

// CT2HexView 构造/析构

CT2HexView::CT2HexView()
: m_nHeight(16)
, m_nWidth(8)
, m_nAsciiMode(0)
, m_nByteLR(0)
, m_nTblMode(0)
, m_nBeginSel(0)
, m_nLines(1)
, m_nAlign(0)
, m_nColorText(0)
, m_nSwpCode(0)
, m_pFrd(NULL)
, m_pFindData(NULL)
, m_pFindText(NULL)
, m_pReplaceData(NULL)
{
	// TODO: 在此处添加构造代码
	HFONT hHex = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
	::GetObject(hHex, sizeof(LOGFONT), &m_lf);
	m_lf.lfFaceName[0]=0;
}

CT2HexView::~CT2HexView()
{
	UINT nIndex = theApp.GetMRUIndex(GetDocument()->GetPathName());
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	if(pSD)
	{
		pSD->nHexViewOffset = m_nOffset;
	}
	if(m_pFrd)
		m_pFrd->DestroyWindow();
	if(m_pFindData) delete[] m_pFindData;
	if(m_pFindText) delete[] m_pFindText;
	if(m_pReplaceData) delete[] m_pReplaceData;

	if(m_Lz77Search.GetSafeHwnd()!=NULL)
		m_Lz77Search.DestroyWindow();

	if(m_RelativeDlg.GetSafeHwnd()!=NULL)
		m_RelativeDlg.DestroyWindow();
}

// CT2HexView 消息处理程序

void CT2HexView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc; GetClientRect(&rc);

	CBitmap memBM;
	memBM.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	memDC.SelectObject(&memBM);
	HFONT hHex = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
	
	//if(GetDocument()->m_nReadOnly)
		memDC.SetBkMode(TRANSPARENT);
	//memDC.FillRect(&rc, &CBrush(GetSysColor(GetDocument()->m_nReadOnly?COLOR_BTNFACE:COLOR_WINDOW)));
	FillRect(memDC.GetSafeHdc(), &rc,
		((CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW))->m_hBkBrush);

	UINT nOffset = m_nOffset&(~0x0F);
	char curline = (char)((m_ptCursorPos.x-m_nOffset)/16);
	BYTE *pHex = GetDocument()->m_pRom+nOffset;
	BYTE *pEnd = GetDocument()->m_pRom+GetDocument()->m_nRomSize;
	CString strText, strCodePageDes;

	UINT nCodePage = GetDocument()->m_nScriptCodePage;
	WORD nDefaultChar = _T('.');	// GetDocument()->m_nDefaultChar;
	strCodePageDes = GetCodePageDescription(nCodePage, &strText);
	if(strText.IsEmpty())
		GetCodePageDescription(GetACP(), &strText);
	if(*m_lf.lfFaceName==0)
	{
		//::GetObject(hHex, sizeof(LOGFONT), &m_lf);
		lstrcpy(m_lf.lfFaceName, strText);
		switch(nCodePage)
		{
		case 932:
			m_lf.lfCharSet = SHIFTJIS_CHARSET;
			break;
		case 950:
			m_lf.lfCharSet = CHINESEBIG5_CHARSET;
			break;
		case 949:
			m_lf.lfCharSet = HANGEUL_CHARSET;
			break;
		}
	}
	HFONT hChar = CreateFontIndirect(&m_lf);

	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);

	UINT nBegin =(UINT) min(m_ptCursorPos.x, m_ptCursorPos.y);
	UINT nEnd = (UINT) max(m_ptCursorPos.x, m_ptCursorPos.y);

	// 绘制头部
	CRect rcHeader(0, 0, rc.right/* m_nWidth*74*/, m_nHeight);
	memDC.FillRect(&rcHeader, &CBrush(::GetSysColor(COLOR_BTNFACE)));
	memDC.Draw3dRect(&rcHeader, 0xFFFFFF, 0x0);

	memDC.SelectObject(hHex);
	for(int i=0; i<16; i++)
	{
		strText.Format(_T("%02X"), i);
		memDC.TextOut(2-si.nPos+m_nWidth*9+i*m_nWidth*3, 0, strText);
	}

	memDC.SelectObject(theApp.GetOptionBox().GetFont());
	strText.LoadString(IDS_HEXVIEW_HEADER);
	if(m_nTblMode==CT2_TBL_USERTBL)
		AfxExtractSubString(strCodePageDes, strText, 1);
	else
	{
		strCodePageDes = strCodePageDes.Left(strCodePageDes.Find(_T('\t')));
		strCodePageDes.AppendFormat(_T(" [%s]"), m_lf.lfFaceName);
	}
	memDC.TextOut(2-si.nPos+m_nWidth*57, 2, strCodePageDes);
	AfxExtractSubString(strCodePageDes, strText, 0);
	memDC.TextOut(2-si.nPos, 2, strCodePageDes);

	memDC.SetBkColor(0xFF8080);
	BYTE nAlign = m_nAlign==0?2:4;
	int l=0;char nSkip=0;
	for(; (l<=m_nLines) && (pHex<pEnd); l++)
	{
		memDC.SetTextColor(GetDocument()->m_hMapFile?0xFF00FF:(::GetSysColor(COLOR_WINDOWTEXT)));
		//memDC.SetBkColor(::GetSysColor(COLOR_WINDOW));
		memDC.SelectObject(hHex);
		int x=2-si.nPos;
		int y=l*m_nHeight+m_nHeight;

		if(l==curline)
		{
			CRect rcHighlight(0, y, m_nWidth*74 + (m_nTblMode==CT2_TBL_USERTBL?16*m_nWidth:0), y+m_nHeight);
			memDC.FillRect(&rcHighlight, &CBrush(0xFFFF));
		}

		strText.Format(_T("%08X"), nOffset+GetDocument()->m_nBlockOffset);
		memDC.TextOut(x, y, strText);
		x+=m_nWidth*9;

		int x2=0;
		for(; x2<16; x2++)
		{
			BYTE *pData = pHex+x2;
			if(pData>=pEnd) break;
			if( (nBegin!=nEnd || GetFocus()!=this) &&
				((nOffset+x2)>=nBegin) &&
				(nOffset+x2)<=nEnd )
			{
				memDC.SetTextColor(0xFF0000);
				//memDC.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
				if( (nOffset+x2==nEnd) ||
					(((nOffset+x2)&0x0F)==0x0F) )
					strText.Format(_T("%02X"), *pData);
				else
					strText.Format(_T("%02X "), *pData);
			}else
			{
				if(m_nColorText)
				{
					COLORREF clrColor=::GetSysColor(COLOR_WINDOWTEXT);
					BYTE *pColor = pHex+(x2&(~(nAlign-1)));
					if((pColor+nAlign)<=pEnd)
					switch(m_nAlign)
					{
					case 0:
						clrColor = *(WORD*)pColor;
						clrColor = WordToColor(clrColor);
						break;
					case 1:
						clrColor = *(UINT*)pColor;
						clrColor&=0xFFFFFF;
						break;
					}
					memDC.SetTextColor(clrColor&0xFFFFFF);
				}else
					memDC.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
				//memDC.SetBkColor(::GetSysColor(COLOR_WINDOW));
				strText.Format(_T("%02X"), *pData);
			}
			memDC.TextOut(x, y, strText);

			if( m_nAsciiMode && (m_nOffset+l*16+x2==m_ptCursorPos.x) )
				memDC.Draw3dRect(x, y, m_nWidth*2, m_nHeight, 0, 0);
			x+=3*m_nWidth;
		}
		if(x2<16)
			x+=(16-x2)*3*m_nWidth;
		for(int x3=0; x3<x2; /*x3++*/)
		{
			BYTE *pData = pHex+x3;
			BYTE *pWord = pHex+x3+1;
			BYTE nLow = pWord>=pEnd?0:*pWord;
			if( (nBegin!=nEnd) &&
				((nOffset+x3)>=nBegin) &&
				(nOffset+x3)<=nEnd )
			{
				memDC.SetTextColor(0xFF);
				//memDC.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
			}else
			{
				memDC.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
				//memDC.SetBkColor(::GetSysColor(COLOR_WINDOW));
			}
			BYTE nSize=1;
			if(m_nTblMode==CT2_TBL_USERTBL)
			{
				UINT nCode = m_nSwpCode?MAKEWORD((*pData), nLow):
							MAKEWORD(nLow, (*pData));
				if(!GetDocument()->m_DTBL[nCode].IsEmpty() && !nSkip)
				{
					if(GetDocument()->m_DTBL[nCode].GetAt(0)==_T('\n'))
					{
						strText.Empty();
						//strText = _T("\081 ");
						CString strLen = GetDocument()->m_DTBL[nCode];
						strLen.Delete(0);
						nSize = StrToIntEX(strLen, FALSE);
						if(nSize<1) nSize=2;
						int i=nSize;
						while(i--)
							strText.AppendChar(_T('.'));
						memDC.SetTextColor(0xFFFF);
						memDC.SetBkMode(OPAQUE);
					}else
					{
						strText = GetDocument()->m_DTBL[nCode];
						nSize = 2;
						if(x3==15)
							nSkip=1;
					}
				}else
				if(!GetDocument()->m_STBL[*pData].IsEmpty() && !nSkip)
				{
					if(GetDocument()->m_STBL[*pData].GetAt(0)==_T('\n'))
					{
						strText.Empty();
						//strText = _T("\081 ");
						CString strLen = GetDocument()->m_STBL[*pData];
						strLen.Delete(0);
						nSize = StrToIntEX(strLen, FALSE);
						if(nSize<1) nSize=1;
						int i=nSize;
						while(i--)
							strText.AppendChar(_T('.'));
						memDC.SetTextColor(0xFFFF);
						memDC.SetBkMode(OPAQUE);
					}else
					{
						strText = GetDocument()->m_STBL[*pData];
					}
				}else
				{
					strText = nSkip?_T(""):_T(".");
					nSkip = 0;
				}
			}else
			{
				UINT nCode = m_nSwpCode?MAKEWORD(nLow, (*pData)):
										MAKEWORD((*pData), nLow);
				memDC.SelectObject(hChar);
				//TCHAR wCode = MAKEWORD(nLow, (*pData));
				//GetShiftJIS(wCode);
				UINT nChar=0;

				if(m_nTblMode==CT2_TBL_USERCTL && GetDocument()->m_DTBL[nCode].GetAt(0)==_T('\n'))
				{
					strText.Empty();
					//strText = _T("\081 ");
					CString strLen = GetDocument()->m_DTBL[nCode];
					strLen.Delete(0);
					nSize = StrToIntEX(strLen, FALSE);
					if(nSize<1) nSize=2;
					int i=nSize;
					while(i--)
						strText.AppendChar(_T('.'));
					memDC.SetTextColor(0xFFFF);
					memDC.SetBkMode(OPAQUE);
				}else
				{
					//memDC.SetTextColor(0);
					//memDC.SetBkMode(TRANSPARENT);

					if(m_nTblMode==CT2_TBL_USERCTL)
					{
						WORD nTCode = (WORD)(nCode>>8 | nCode<<8);
						if(!GetDocument()->m_DTBL[nTCode].IsEmpty())
						{
							strText = GetDocument()->m_DTBL[nTCode];
                            nSize = 2;
						}else
						if(!GetDocument()->m_STBL[nTCode].IsEmpty())
						{
							strText = GetDocument()->m_STBL[nTCode];
						}else
							goto SYSCODE;

					}else
					{
					SYSCODE:
						if(nCodePage==65001)
							mcharTowchar(&nChar, pData, nCodePage);
						else
							mcharTowchar(&nChar, &nCode, nCodePage);

						if( (pData<pEnd) &&
							//mcharTowchar(&nChar, nCode, nCodePage) &&
							(nChar!=nDefaultChar) &&
							!nSkip /*&&
							!(nChar>>16)*/ ||
							(nCodePage==1200 || nCodePage==1201) )
						{
							if(nCodePage==1200)
								strText = (WCHAR)nCode;
							else
							if(nCodePage==1201)
								//strText = (WCHAR)((*pData<<8)|*(pData+1));
								strText = (WCHAR)(((nCode<<8)&0xFFFF)|((nCode>>8)&0xFF));
							else
							if(nChar<=_T('~'))
								goto Ansi;
							else
							if(nCodePage==65001)
							{
								nSize++;
								strText = (TCHAR)nChar;
							}else
								strText = (TCHAR)nChar;
							if(nChar<0xFF61)	// 半角检查
								nSize++;
							if(x3==15)
								nSkip=1;
						}else
							Ansi:
						{
							if(m_nTblMode==CT2_TBL_USERCTL && GetDocument()->m_STBL[*pData].GetAt(0)==_T('\n'))
							{
								strText.Empty();
								//strText = _T("\081 ");
								CString strLen = GetDocument()->m_STBL[*pData];
								strLen.Delete(0);
								nSize = StrToIntEX(strLen, FALSE);
								if(nSize<1) nSize=1;
								int i=nSize;
								while(i--)
									strText.AppendChar(_T('.'));
								memDC.SetTextColor(0xFFFF);
								memDC.SetBkMode(OPAQUE);
							}else
							{
								if(*pData>=_T(' ') && *pData<=_T('~') && !nSkip)
									strText.Format(_T("%c"), *pData);
								else
								{
									if(nSkip)
										strText = _T("");
									else
										strText = (TCHAR)nDefaultChar;
									nSkip=0;
								}
							}
						}
					}
				}
			}
			memDC.TextOut(x, y, strText);

			memDC.SetTextColor(0);
			memDC.SetBkMode(TRANSPARENT);
			memDC.SetBkColor(0xFF8080);

			CSize szWidth = memDC.GetTextExtent(strText);
			if(szWidth.cx<m_nWidth) szWidth.cx=m_nWidth*2;

			if( (!m_nAsciiMode) && (m_nOffset+l*16+x3==m_ptCursorPos.x) )
				memDC.Draw3dRect(x, y, szWidth.cx, m_nHeight, 0, 0);
			x+=szWidth.cx;
			x3+=nSize;
		}

		pHex+=16;nOffset+=16;
	}
	DeleteObject(hChar);
	//分割线
	CPen pen(PS_DASHDOTDOT, 1, 0x7F7F7F);
	memDC.SelectObject(&pen);
	int x=2-si.nPos-m_nWidth/2+m_nWidth*9;
	int c=16/nAlign;
	for(int i=0; i<=c; i++)
	{
		memDC.MoveTo(x, 0);
		memDC.LineTo(x, m_nLines*m_nHeight+m_nHeight);
		x+=m_nWidth*(nAlign*3);
	}
	//memDC.ExcludeClipRect(-si.nPos, 0, m_nWidth*74-si.nPos+2, l*m_nHeight+m_nHeight);

	//FillRect(memDC.GetSafeHdc(), &rc,
	//	((CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW))->m_hBkBrush);

	dc.BitBlt(0, 0, rc.Width(), rc.Height(),
		&memDC, 0, 0, SRCCOPY);

	memBM.DeleteObject();	memDC.DeleteDC();
}

void CT2HexView::OnInitialUpdate()
{
	CT2View::OnInitialUpdate();

	CString Name = GetDocument()->GetPathName();
	UINT nIndex = theApp.GetMRUIndex(Name);
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	if(theApp.m_pRecentStatList->IsOK(nIndex))
	{
		m_nOffset = pSD->nHexViewOffset;
	}

	CDC* pDC = GetDC();
	pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(SYSTEM_FIXED_FONT)));
	CSize szExtent = pDC->GetTextExtent(_T(" "));

	m_nWidth = (BYTE)szExtent.cx;
	m_nHeight = (BYTE)szExtent.cy;

	ReleaseDC(pDC);

	OnUpdateData();
	ScrollTo(m_nOffset);
}

void CT2HexView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CT2View::OnActivateView(bActivate, pActivateView, pDeactiveView);

}

void CT2HexView::OnSetFocus(CWnd* pOldWnd)
{
	CT2View::OnSetFocus(pOldWnd);

	::CreateCaret(m_hWnd, 0, m_nWidth, m_nHeight);
	ShowCaret();
	SetCaretPos();
}

void CT2HexView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);
	point.x += si.nPos;

	if(point.y<m_nHeight && point.x>m_nWidth*57)
	{
		m_nTblMode = CT2_TBL_SYS;
		char nMax = GetParent()->IsZoomed()?1:0;
 		HMENU hHex = GetSubMenu(theApp.m_hHexMenu, 3+nMax);
		HMENU hCP = GetSubMenu(hHex, 5);

		CPoint pt(2-si.nPos+point.x, m_nHeight);
		ClientToScreen(&pt);
		::OnInitMenuPopup(theApp.m_pMainWnd, hCP, TRUE);
		TrackPopupMenu(hCP, TPM_LEFTBUTTON, pt.x, pt.y, 0, m_hWnd, NULL);
	}else
	if( (point.y<m_nHeight) ||
		((point.y-m_nHeight)>(m_nLines*m_nHeight)) ||
		((UINT)((point.y-m_nHeight)/m_nHeight*16+m_nOffset)>=GetDocument()->m_nRomSize) )
		return;
	m_nBeginSel = TRUE;

	long x=point.x-m_nWidth*9-2;
	if(x>=0)
	{
		if(x<m_nWidth*47)
		{
			m_nAsciiMode=FALSE;
			m_ptCursorPos.y = x/m_nWidth;
			m_ptCursorPos.y-=(m_ptCursorPos.y+1)/3;
			m_nByteLR = (BYTE)m_ptCursorPos.y&1;
			m_ptCursorPos.y>>=1;
			m_ptCursorPos.y+=(point.y-m_nHeight)/m_nHeight*16;
			m_ptCursorPos.y+=m_nOffset;
		}else
		if(x>m_nWidth*48)
		{
			x-=m_nWidth*48;
			m_nAsciiMode=TRUE;
			//if(m_nTblMode==CT2_TBL_USERTBL)
			{
				UINT nOff;
				CPoint pt(x, point.y);
				HitTest(nOff, pt);
				m_ptCursorPos.y = nOff;
				//m_ptCursorPos.y+=m_nOffset;
			}/*else
			if(x<m_nWidth*64)
			{
				m_ptCursorPos.y = x/m_nWidth;
				m_ptCursorPos.y+=(point.y-m_nHeight)/m_nHeight*16;
				m_ptCursorPos.y+=m_nOffset;
			}*/
		}
	}
	if(!(GetKeyState(VK_SHIFT)&0x80))
		m_ptCursorPos.x = m_ptCursorPos.y;
	SetCaretPos();

	CT2View::OnLButtonDown(nFlags, point);
}

void CT2HexView::SetCaretPos()
{
	CWnd *p = GetFocus();
	if(p && p->GetParent()==m_pFrd) return;
	{
		SCROLLINFO si;
		GetScrollInfo(SB_HORZ, &si);
		int x;
		if(m_nTblMode==CT2_TBL_USERTBL && m_nAsciiMode)
		{
			UINT nOff = m_ptCursorPos.y;
			CPoint pt;
			HitTest(nOff, pt, FALSE);
			x = pt.x + m_nWidth*57+2;
		}else
		{
			x = m_ptCursorPos.y&0x0F;
			UINT nOff = m_ptCursorPos.y;
			CPoint pt;
			HitTest(nOff, pt, FALSE);
			x = (m_nAsciiMode?57:9) * m_nWidth+
				(m_nAsciiMode?pt.x:
				2+x*(m_nAsciiMode?1:3)*m_nWidth);
			if(m_nByteLR && !m_nAsciiMode) x+=m_nWidth;
		}
		x -= si.nPos;

		int y = (m_ptCursorPos.y-m_nOffset)/16*m_nHeight+m_nHeight;

		::SetCaretPos(x, y<m_nHeight?m_nHeight:y);
	}
	BYTE nAlign=m_nAlign==0?2:4;
	UINT nOffset=m_ptCursorPos.x&(~(nAlign-1));
	CString strBits;
	if((nOffset+nAlign)<GetDocument()->m_nRomSize)
	{
		int bits = nAlign==2?15:31;
		UINT nBits = nAlign==2?*(WORD*)(GetDocument()->m_pRom+nOffset):
								*(UINT*)(GetDocument()->m_pRom+nOffset);
		for(int i=bits; i>=0; i--)
			strBits.AppendFormat(_T("%d"), (nBits>>i)&1);
	}
	CT2StatusBar *pSB = theApp.GetStatusBar();
	pSB->SetPaneText(pSB->CommandToIndex(IDC_HEXVIEW_BITS), strBits);

	strBits.Format(_T("%08X"), m_ptCursorPos.x);
	pSB->SetPaneText(pSB->CommandToIndex(ID_TILEEDITOR_OFFSET), strBits);

	// Nds文件提示
	pSB->SetPaneText(0, GetDocument()->OffsetToPathName(m_ptCursorPos.x));

	Invalidate(FALSE);
}

void CT2HexView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch(nSBCode)
	{
	case SB_ENDSCROLL:
		return;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			SCROLLINFO si;
			GetScrollInfo(SB_HORZ, &si);
			SetScrollPos(SB_HORZ, si.nTrackPos);
		}break;
	}

	SetCaretPos();

	CT2View::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CT2HexView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(nSBCode==SB_ENDSCROLL)
		return;
	m_nOffset+=GetDocument()->m_nBlockOffset;
	switch(nSBCode)
	{
	case SB_ENDSCROLL:
		return;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			SCROLLINFO si;
			GetScrollInfo(SB_VERT, &si);
			SetScrollPos(SB_VERT, si.nTrackPos);
			m_nOffset = si.nTrackPos/m_nHeight*16;
		}break;
	case SB_LINEUP:
		if(m_nOffset>=16)
			m_nOffset-=16;
		break;
	case SB_LINEDOWN:
		if((m_nOffset+16*m_nLines)<GetDocument()->m_nFileSize)
			m_nOffset+=16;
		break;
	case SB_TOP:
		m_nOffset=0;
		break;
	case SB_BOTTOM:
		m_nOffset = GetDocument()->m_nFileSize;
		if(m_nOffset>(UINT)m_nLines*16)
			m_nOffset -= m_nLines*16;
		else
			m_nOffset = 0;
		break;
	case SB_PAGEUP:
		if(m_nOffset>=((UINT)(m_nLines-1)*16))
			m_nOffset-=((m_nLines-1)*16);
		else
			m_nOffset = 0;
		break;
	case SB_PAGEDOWN:
		if((m_nOffset+((m_nLines-1)*16))<GetDocument()->m_nFileSize)
			m_nOffset+=((m_nLines-1)*16);
		if( m_nOffset >= (GetDocument()->m_nFileSize-((m_nLines-1)*16)) )
			m_nOffset = GetDocument()->m_nFileSize-(m_nLines*16);
		break;
	}
	GetDocument()->Seek(m_nOffset);
	//m_nOffset-=GetDocument()->m_nBlockOffset;
	OnUpdateData();

	// CT2View::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CT2HexView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	SetRedraw(FALSE);
	BOOL bRet = CT2View::OnMouseWheel(nFlags, zDelta, pt);
	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si);
	SetScrollPos(SB_VERT, si.nTrackPos);
	m_nOffset = si.nTrackPos/m_nHeight*16;
	SetRedraw();
	OnUpdateData();
	return bRet;
}

void CT2HexView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_nBeginSel && (nFlags&MK_LBUTTON))
	{
		if( (point.y<m_nHeight) ||
			((point.y-m_nHeight)>(m_nLines*m_nHeight)) ||
			((UINT)((point.y-m_nHeight)/m_nHeight*16+m_nOffset)>=GetDocument()->m_nRomSize) )
			return;
		SCROLLINFO si;
		GetScrollInfo(SB_HORZ, &si);
		point.x += si.nPos;
		long x=point.x-m_nWidth*9-2;
		long y=m_ptCursorPos.y;
		BYTE nByteLR=m_nByteLR;
		if(x>=0)
		{
			if(x<m_nWidth*47)
			{
				m_nAsciiMode=FALSE;
				y = x/m_nWidth;
				y-=(y+1)/3;
				nByteLR = (BYTE)y&1;
				y>>=1;
				y+=(point.y-m_nHeight)/m_nHeight*16;
				y+=m_nOffset;
			}else
			if(x>m_nWidth*48)
			{
				x-=m_nWidth*48;
				m_nAsciiMode=TRUE;
				//if(m_nTblMode==CT2_TBL_USERTBL)
				{
					UINT nOff;
					CPoint pt(x, point.y);
					HitTest(nOff, pt);
					y = nOff;
					//y += m_nOffset;
				}/*else
				{
					y = x/m_nWidth;
					y +=(point.y-m_nHeight)/m_nHeight*16;
					y +=m_nOffset;
				}*/
			}
			if( (y!=m_ptCursorPos.y) || (nByteLR!=m_nByteLR) )
			{
				m_ptCursorPos.y=y;
				m_nByteLR = nByteLR;
				SetCaretPos();
			}
		}
	}

	CT2View::OnMouseMove(nFlags, point);
}

void CT2HexView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_nBeginSel = FALSE;

	CT2View::OnLButtonUp(nFlags, point);
}

void CT2HexView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CT2View::OnChar(nChar, nRepCnt, nFlags);
	if( (GetDocument()->m_nReadOnly && GetDocument()->m_nRomType!=CT2_RT_PCMEM) ||
		nChar==VK_ESCAPE || nChar==VK_RETURN )
		return;

	UINT nData=0;
	UINT nKey=towlower(nChar);
	if( m_nAsciiMode )
	{
		if( nChar==VK_TAB )
			return;
		if(m_nTblMode==CT2_TBL_USERTBL)
		{
			CString Tbl;
			char bok=0;
			for(int i=0; i<0x10000; i++)
			{
				Tbl = GetDocument()->m_DTBL[i];
				nData = Tbl.GetAt(0);
				if(nData==nChar)
				{
					nData=i>>8|i<<8;
					bok=1;
					break;
				}
			}
			for(int i=0; !bok && i<0x10000; i++)
			{
				Tbl = GetDocument()->m_STBL[i];
				nData = Tbl.GetAt(0);
				if(nData==nChar)
				{
					nData=i;
					bok=1;
					break;
				}
			}
			if(!bok) return;
		}else
		{
			UINT nCodePage = GetDocument()->m_nScriptCodePage;
			if(nCodePage==1200)
				nData = nChar;
			else
			if(nCodePage==1201)
				nData = nChar>>8|nChar<<8;
			else
			{
				BOOL bUsed=FALSE;
				WideCharToMultiByte(nCodePage,
					0, (LPCTSTR)&nChar, 4, (LPSTR)&nData, 4,
					nCodePage==65001?
					NULL:(LPCSTR)&GetDocument()->m_nDefaultChar,
					nCodePage==65001?NULL:&bUsed);
				if(bUsed)
					return;
			}
		}
	}else
	if( (nKey>=_T('0')) && (nKey<=_T('9')) )
		nData = nChar-_T('0');
	else
	if( (nKey>=_T('a')) && (nKey<=_T('f')) )
		nData = 10+nKey-_T('a');
	else
		return;

	BYTE *pData = GetDocument()->m_pRom+m_ptCursorPos.y;
	BYTE nSize = 1;
	if(GetDocument()->m_nScriptCodePage==65001)
		nSize = 3;
	else
	{
		nData&=0xFFFF;
		if(nData>>8)
			nSize = 2;
	}

	if(pData+nSize>(GetDocument()->m_pRom+GetDocument()->m_nRomSize))
		return;
	UINT nOff = (UINT)m_ptCursorPos.y;

	SetData(nOff, nData, nSize);

	if(GetDocument()->m_nRomType==CT2_RT_PCMEM)
	{
		GetDocument()->m_Memory.Update(nOff, nSize);
	}

	if( (UINT)m_ptCursorPos.y >= GetDocument()->m_nRomSize )
	{
		m_ptCursorPos.y = GetDocument()->m_nRomSize-1;
		m_nByteLR = 1;
	}
	m_ptCursorPos.x=m_ptCursorPos.y;
	SetCaretPos();
}

void CT2HexView::SetData(UINT nOff, UINT nData, BYTE nSize)
{
	BYTE *pData = GetDocument()->m_pRom+nOff;
	DoUndo(pData, nOff, nSize);
	//AddMacro_SetData(nOff, nData, nSize);
	if(m_nAsciiMode)
	{
		if(m_nSwpCode && nSize==2)
			nData = (nData<<8) | (nData>>8);
		memcpy(pData, &nData, nSize);
		m_ptCursorPos.y+=nSize;
	}else
	{
		*pData &= ~(0xF0>>(m_nByteLR*4));
		*pData |= (nData<<(4-m_nByteLR*4));
		if(m_nByteLR)
		{
			m_ptCursorPos.y++;
			m_nByteLR=0;
		}else
			m_nByteLR=1;
	}
}

void CT2HexView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	char up=0;
upup:
	m_nOffset+=GetDocument()->m_nBlockOffset;
	m_ptCursorPos.x+=GetDocument()->m_nBlockOffset;
	switch(nChar)
	{
	case VK_UP:
		if(GetKeyState(VK_CONTROL)&0x80)
		{
			if(m_nOffset>=16)
				m_nOffset-=16;
		}else
		if(m_ptCursorPos.x>=16)
			m_ptCursorPos.x-=16;
		break;
	case VK_DOWN:
		if(GetKeyState(VK_CONTROL)&0x80)
		{
			if((m_nOffset+16*m_nLines)<GetDocument()->m_nFileSize)
				m_nOffset+=16;
		}else
		if(((UINT)m_ptCursorPos.x+16)<GetDocument()->m_nFileSize)
		{
			m_ptCursorPos.x+=16;
			if((UINT)m_ptCursorPos.x>=(m_nOffset+m_nLines*16))
				m_nOffset+=16;
		}
		break;
	case VK_LEFT:
		if(m_nByteLR && !m_nAsciiMode)
			m_nByteLR = 0;
		else
		if(m_ptCursorPos.x)
		{
			char w=1;
			if(m_nAsciiMode)
			{
				if(GetDocument()->m_nScriptCodePage==1200||GetDocument()->m_nScriptCodePage==1201)
					w=2;
				else
				if(GetDocument()->m_nScriptCodePage==65001)
					w=3;
				/*else
				{
					CPINFO cpi;
					GetCPInfo(GetDocument()->m_nScriptCodePage, &cpi);
					w = cpi.MaxCharSize;
				}*/
			}
			m_ptCursorPos.x-=w;
			m_nByteLR = 1;
		}
		break;
	case VK_RIGHT:
		if(!m_nByteLR && !m_nAsciiMode)
			m_nByteLR = 1;
		else
		if((UINT)m_ptCursorPos.x<(GetDocument()->m_nFileSize-1))
		{
			char w=1;
			if(m_nAsciiMode)
			{
				if(GetDocument()->m_nScriptCodePage==1200||GetDocument()->m_nScriptCodePage==1201)
					w=2;
				else
				if(GetDocument()->m_nScriptCodePage==65001)
					w=3;
				/*else
				{
					CPINFO cpi;
					GetCPInfo(GetDocument()->m_nScriptCodePage, &cpi);
					w = cpi.MaxCharSize;
				}*/
			}
			m_ptCursorPos.x+=w;
			m_nByteLR = 0;

			/*if( (((UINT)m_ptCursorPos.x+16)<GetDocument()->m_nFileSize) &&
				(UINT)m_ptCursorPos.x>=(m_nOffset+m_nLines*16) )
				m_nOffset+=16;*/
		}
		break;
	case VK_HOME:
		if(GetKeyState(VK_CONTROL)&0x80)
			m_nOffset = m_ptCursorPos.x = 0;
		else
			m_ptCursorPos.x &= ~0x0F;
		m_nByteLR = 0;
		break;
	case VK_END:
		if(GetKeyState(VK_CONTROL)&0x80)
		{
			m_nOffset = GetDocument()->m_nFileSize;
			if(m_nOffset>(UINT)m_nLines*16)
				m_nOffset -= m_nLines*16;
			else
				m_nOffset = 0;
			m_ptCursorPos.x = GetDocument()->m_nFileSize-1;
		}else
			m_ptCursorPos.x|=0x0F;
		m_nByteLR = 1;
		break;
	case VK_TAB:
		m_nAsciiMode = !m_nAsciiMode;
		break;
	case VK_PRIOR:
		if(m_nOffset>=((UINT)(m_nLines-1)*16))
			m_nOffset-=((m_nLines-1)*16);
		else
			m_nOffset = 0;
		m_ptCursorPos.x=m_nOffset;
		break;
	case VK_NEXT:
		if((m_nOffset+((m_nLines-1)*16))<GetDocument()->m_nFileSize)
			m_nOffset+=((m_nLines-1)*16);
		if( m_nOffset >= (GetDocument()->m_nFileSize-((m_nLines-1)*16)) )
			m_nOffset = GetDocument()->m_nFileSize-(m_nLines*16);
		m_ptCursorPos.x=m_nOffset;
		break;
	}
	m_nOffset-=GetDocument()->m_nBlockOffset;
	m_ptCursorPos.x-=GetDocument()->m_nBlockOffset;
	switch(nChar)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_HOME:
	case VK_END:
	case VK_TAB:
	case VK_PRIOR:
	case VK_NEXT:
		m_ptCursorPos.y = m_ptCursorPos.x;
		OnUpdateData();
		if( !up && (nChar==VK_HOME || nChar==VK_END) )
		{
			up=1;
			goto upup;
		}
		//SetCaretPos();
		ScrollTo(m_ptCursorPos.y);
		break;
	}

	CT2View::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CT2HexView::OnUpdateData(void)
{
	CRect rc; GetClientRect(&rc);
	if(rc.Height()>m_nHeight)
		m_nLines = (rc.Height()-m_nHeight)/m_nHeight;
	else
		m_nLines = 1;

	//if(m_nOffset&0x0F) m_ptCursorPos.x = m_ptCursorPos.y = m_nOffset;
	SetCaretPos();
	m_nOffset &= ~0x0F;

	/*if(GetDocument()->m_nImTbl)
		m_nTblMode=CT2_TBL_SYS;*/

	CSize sizeTotal;
	sizeTotal.cx = m_nWidth*75 + (m_nTblMode==CT2_TBL_USERTBL?16*m_nWidth:0);
	sizeTotal.cy = (GetDocument()->m_nFileSize/16+3)*m_nHeight;
	//if(GetDocument()->m_nFileSize%16) sizeTotal.cy += m_nHeight;
	SetScrollSizes(MM_TEXT, sizeTotal);
	SetScrollPos(SB_VERT, (m_nOffset+GetDocument()->m_nBlockOffset)/16*m_nHeight);

	CT2View::OnUpdateData();
}

void CT2HexView::ScrollTo(UINT nOffset, BOOL bForce)
{
	BYTE curline = (BYTE)(((UINT)m_ptCursorPos.y>m_nOffset?m_ptCursorPos.x-m_nOffset:m_nOffset-m_ptCursorPos.x)/16);
	m_ptCursorPos.y = m_ptCursorPos.x = nOffset;
	if(nOffset<m_nOffset)
	{
		m_nOffset = nOffset&(~0x0F);
		if(m_nOffset>(UINT)(curline*16))
			m_nOffset-=curline*16;
	}else
	{
		UINT nSize = m_nLines*16+m_nOffset&(~0x0F);
		if((nOffset>=nSize)/* && nOffset*/)
		{
			/*if((m_nOffset+16)<=(UINT)m_ptCursorPos.x)
			{
				//m_ptCursorPos.y=m_ptCursorPos.x = nOffset;
				nOffset+= (m_nLines - curline) * 16;
			}*/
			m_nOffset = nOffset-curline*16;
		}
	}
	m_nOffset += GetDocument()->m_nBlockOffset;
	GetDocument()->Seek(m_nOffset);
	//m_nOffset-=GetDocument()->m_nBlockOffset;

	SetCaretPos();
	OnUpdateData();
}

void CT2HexView::OnEditCopy()
{
	CWnd *pFocus = GetFocus();
	if( pFocus!=this )
	{
		pFocus->SendMessage(WM_COPY);
	}else
	{
		if(theApp.m_pCopyData)
			delete[] theApp.m_pCopyData;
		UINT nOffset = m_ptCursorPos.x>m_ptCursorPos.y?
			m_ptCursorPos.y:m_ptCursorPos.x;
		theApp.m_nCopyDataSize = abs(m_ptCursorPos.y-m_ptCursorPos.x)+1;
		theApp.m_pCopyData = new BYTE[theApp.m_nCopyDataSize];
		memcpy(theApp.m_pCopyData, GetDocument()->m_pRom+nOffset,
			theApp.m_nCopyDataSize);
	}
}

void CT2HexView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_ptCursorPos.x!=m_ptCursorPos.y) ||
		(GetFocus()!=this));
}

void CT2HexView::OnEditPaste()
{
	CWnd *pFocus = GetFocus();
	if( pFocus!=this )
	{
		pFocus->SendMessage(WM_PASTE);
	}else
	{
		UINT nOffset = m_ptCursorPos.x>m_ptCursorPos.y?
			m_ptCursorPos.y:m_ptCursorPos.x;
		UINT nSize =
			(nOffset+theApp.m_nCopyDataSize)>GetDocument()->m_nRomSize?
			GetDocument()->m_nRomSize-nOffset:
			theApp.m_nCopyDataSize;
		DoUndo(GetDocument()->m_pRom+nOffset,
			nOffset,
			nSize);
		memcpy(GetDocument()->m_pRom+nOffset,
			theApp.m_pCopyData, nSize);
		Invalidate(FALSE);
	}
}

void CT2HexView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((theApp.m_pCopyData!=NULL && !GetDocument()->m_nReadOnly) ||
		GetFocus()!=this);
}

void CT2HexView::OnEnableTbl()
{
	m_nTblMode=m_nTblMode==CT2_TBL_USERTBL?CT2_TBL_SYS:CT2_TBL_USERTBL;
	if(m_nTblMode==CT2_TBL_USERTBL)
	{
		CT2Doc *pDoc = GetDocument();
		if(pDoc->m_strTblName.IsEmpty())
		{
			pDoc->OnTblSeltbl();
			m_nTblMode = pDoc->m_strTblName.IsEmpty()?CT2_TBL_SYS:CT2_TBL_USERTBL;
		}
	}
	OnUpdateData();
	//Invalidate(FALSE);
}

void CT2HexView::OnUpdateEnableTbl(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nTblMode==CT2_TBL_USERTBL);
	//pCmdUI->Enable(!GetDocument()->m_nImTbl);
}

void CT2HexView::OnViewalign(UINT nID)
{
	switch(nID)
	{
	case ID_VIEW_2BYTEALIGN:
		m_nAlign=0;
		break;
	case ID_VIEW_4BYTEALIGN:
		m_nAlign=1;
		break;
	}
	SetCaretPos();
}

void CT2HexView::OnUpdateViewalign(CCmdUI *pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case ID_VIEW_2BYTEALIGN:
		pCmdUI->SetRadio(m_nAlign==0);
		break;
	case ID_VIEW_4BYTEALIGN:
		pCmdUI->SetRadio(m_nAlign==1);
		break;
	}
}

void CT2HexView::OnViewColortext()
{
	m_nColorText = !m_nColorText;
	Invalidate(FALSE);
}

void CT2HexView::OnUpdateViewColortext(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nColorText);
}

void CT2HexView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CT2View::OnLButtonDblClk(nFlags, point);
	BYTE nAlign=m_nAlign==0?2:4;
	UINT nOffset=m_ptCursorPos.x&(~(nAlign-1));
	if( !m_nColorText ||
		(point.y<m_nHeight) ||
		(point.y>(m_nLines*m_nHeight)) ||
		((nOffset+nAlign)>GetDocument()->m_nRomSize) ||
		GetDocument()->m_nReadOnly )
		return;

	static CColorDialog cd(0, CC_RGBINIT);
	BYTE *pColor = GetDocument()->m_pRom+nOffset;
	cd.m_cc.rgbResult = nAlign==2?
		WordToColor(*(WORD*)pColor):
		(*(COLORREF*)pColor)&0xFFFFFF;
	if(cd.DoModal()!=IDOK) return;
	DoUndo(pColor, nOffset, nAlign);
	COLORREF clrColor = cd.GetColor();
	nAlign==2?
		*(WORD*)pColor = ColorToWord(clrColor):
		*(COLORREF*)pColor = clrColor;
	Invalidate(FALSE);
}

void CT2HexView::SetBits(int nBits)
{
	BYTE nAlign=m_nAlign==0?2:4;
	nBits = (nAlign==2?15:31)-nBits;
	UINT nOffset=m_ptCursorPos.x&(~(nAlign-1));
	if( ((nOffset+nAlign)>=GetDocument()->m_nRomSize) ||
		GetDocument()->m_nReadOnly )
		return;

	BYTE *pData = GetDocument()->m_pRom+nOffset;
	DoUndo(pData, nOffset, nAlign);

	BYTE nAsciiMode=m_nAsciiMode;
	BYTE nSwpCode=m_nSwpCode;
	m_nAsciiMode = 1;
	m_nSwpCode = 0;
	if(nAlign==2)
	{
		WORD *pWord = (WORD*)pData;
		nBits = 1<<nBits;
		*pWord ^= nBits;
		//AddMacro_SetData(nOffset, *pWord, 2);
	}else
	{
		UINT *pUINT = (UINT*)pData;
		nBits = 1<<nBits;
		*pUINT ^= nBits;
		//AddMacro_SetData(nOffset, *pUINT, 4);
	}
	m_nAsciiMode = nAsciiMode;
	m_nSwpCode = nSwpCode;
	SetCaretPos();
}

void CT2HexView::OnSwpCode()
{
	m_nSwpCode = !m_nSwpCode;
	Invalidate(FALSE);
}

void CT2HexView::OnUpdateViewSwpCode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nSwpCode);
}

void CT2HexView::DoFindReplace(BOOL bReplace)
{
	static BOOL Replace;
	if(m_pFrd)
	{
		if(Replace!=bReplace)
		{
			m_pFrd->SetFocus();
			return;
		}
		m_pFrd->DestroyWindow();
	}
	Replace = !bReplace;
	m_pFrd = new CFindReplaceDialog;
	m_pFrd->m_fr.Flags=FR_HIDEMATCHCASE|FR_HIDEWHOLEWORD|FR_HIDEUPDOWN|FR_ENABLEHOOK;
	if(!m_pFrd->Create(Replace, m_strFind, m_strReplace, FR_DOWN, this))
		delete m_pFrd;
}

LONG CT2HexView::OnFindReplace(UINT wParam, LONG lParam)
{
	FINDREPLACE &fr = *((FINDREPLACE*)(UINT_PTR)lParam);
	if(m_pFrd->IsTerminating())
	{
		//m_pFrd->DestroyWindow();
		m_pFrd = NULL;
		return 0;
	}

	m_strFind		=m_pFrd->GetFindString();
	m_strReplace	=m_pFrd->GetReplaceString();

	// FindText
	if(m_pFindText) delete[] m_pFindText;
	if(m_nTblMode==CT2_TBL_USERTBL)
	{
		//m_pFindText=NULL;
		int len = m_strFind.GetLength();
		CString *pDTBL = GetDocument()->m_DTBL;
		CString *pSTBL = GetDocument()->m_STBL;
		CString t, strText;

		for(int i=0; i<len; i++)
		{
			WORD c = m_strFind.GetAt(i);
			char ok=0;
			for(int i=0; i<0x10000; i++)
			{
				if(pDTBL[i]==c)
				{
					strText.AppendFormat(_T("%04X"), m_nSwpCode?((i<<8)|(i>>8))&0xFFFF:i);
					ok = 1;
					break;
				}
			}
			if(ok) continue;
			for(int i=0; i<0x100; i++)
			{
				if(pSTBL[i]==c)
				{
					strText.AppendFormat(_T("%02X"), i);
					break;
				}
			}
		}
		// Find Tbl
		m_nFindTextSize=strText.GetLength();
		m_nFindTextSize/=2;

		if(m_nFindTextSize)
		{
			m_pFindText = new char[m_nFindTextSize];
			for(UINT i=0; i<m_nFindTextSize; i++)
			{
				int nVal = StrToIntEX(strText.Mid(i<<1, 2));
				if(nVal==-1)
				{
					delete[] m_pFindText;
					m_pFindText= NULL;
					break;
				}
				m_pFindText[i]=(BYTE)nVal;
			}
		}else
			m_pFindText = NULL;
	}else
	{
		m_nFindTextSize = m_strFind.GetLength()*2+2;
		m_pFindText = new char[m_nFindTextSize];
	#ifdef UNICODE
		//CharToOem(m_strFind, m_pFindText);
		CPINFO cpInfo; BOOL bUsed;
		UINT nCodePage=GetDocument()->m_nScriptCodePage;
		GetCPInfo(nCodePage, &cpInfo);
		if(nCodePage==1200)
		{
			lstrcpy((LPTSTR)m_pFindText, m_strFind);
			m_nFindTextSize = m_strFind.GetLength()*2;
		}else
		if(nCodePage==1201)
		{
			lstrcpy((LPTSTR)m_pFindText, m_strFind);
			m_nFindTextSize = m_strFind.GetLength();
			TCHAR *pFT=(TCHAR*)m_pFindText;
			for(UINT i=0; i<m_nFindTextSize; i++)
				pFT[i]=pFT[i]>>8|pFT[i]<<8;
			m_nFindTextSize=m_nFindTextSize*2;
		}else
		{
			memset(m_pFindText, 0, m_nFindTextSize);
			WideCharToMultiByte(nCodePage, 0,
				m_strFind, -1, m_pFindText, m_nFindTextSize,
				(LPCSTR)cpInfo.DefaultChar, &bUsed);
			m_nFindTextSize=(int)strlen(m_pFindText);
		}
		char *p = new char[m_nFindTextSize];
		//strcpy(p, m_pFindText);
		memcpy(p, m_pFindText, m_nFindTextSize);
		delete[] m_pFindText;
		m_pFindText=p;
	#elif
		strcpy(m_pFindText, m_strFind);
	#endif
	}
	// Find
	CString strFind = m_strFind;
	strFind.Replace(_T(" "), _T(""));
	m_nFindSize=strFind.GetLength();
	m_nFindSize+=m_nFindSize&1;
	m_nFindSize/=2;

	if(m_pFindData) delete[] m_pFindData;
	m_pFindData = new BYTE[m_nFindSize];
	for(UINT i=0; i<m_nFindSize; i++)
	{
		int nVal = StrToIntEX(strFind.Mid(i<<1, 2));
		if(nVal==-1)
		{
			delete[] m_pFindData;
			m_pFindData = NULL;
			break;
		}
		m_pFindData[i]=(BYTE)nVal;
	}

	// Replace
	strFind = m_strReplace;
	UINT nReplaceSize;
	strFind.Replace(_T(" "), _T(""));
	nReplaceSize=strFind.GetLength();
	nReplaceSize+=nReplaceSize&1;
	nReplaceSize/=2;
	if(m_pReplaceData)
		delete[] m_pReplaceData;
	m_pReplaceData = NULL;
	if(m_nFindSize==nReplaceSize)
	{
		m_pReplaceData = new BYTE[m_nFindSize];
		for(UINT i=0; i<nReplaceSize; i++)
		{
			int nVal = StrToIntEX(strFind.Mid(i<<1, 2));
			if(nVal==-1)
			{
				delete[] m_pReplaceData;
				m_pReplaceData = NULL;
				break;
			}
			m_pReplaceData[i]=(BYTE)nVal;
		}
	}

	if(fr.Flags&FR_FINDNEXT)
		OnFind();
	else
		if(fr.Flags&FR_REPLACE)
			OnReplace();
		else
			if(fr.Flags&FR_REPLACEALL)
				OnReplace(TRUE);
	return 0;
}

void CT2HexView::OnSearchFind()
{
	DoFindReplace();
}

void CT2HexView::OnSearchReplace()
{
	DoFindReplace(TRUE);
}

void CT2HexView::OnFind()
{
	if(m_pFindData==NULL && m_pFindText==NULL) return;
	UINT nOffset = m_ptCursorPos.x+1;
	UINT nRomSize = GetDocument()->m_nRomSize;
	BYTE *pEnd = GetDocument()->m_pRom+nRomSize;
	if( (nOffset+m_nFindSize>nRomSize) &&
		(nOffset+m_nFindTextSize>nRomSize) )
		nOffset = 0;
	BYTE *pRom = GetDocument()->m_pRom+nOffset;

	BOOL bReFind = nOffset;
	g_bBreak=FALSE;
REFIND:
	BOOL bFind = FALSE, bFindText = FALSE;
	while( ((pRom+m_nFindSize <= pEnd) ||
			(pRom+m_nFindTextSize<= pEnd)) && !g_bBreak )
	{
		if(m_pFindData)
		{
			for(UINT i=0; i<m_nFindSize; i++)
			{
				bFind = (*(pRom+i) == *(m_pFindData+i));
				if(!bFind) break;
			}
		}
		//if(m_pFindData) bFind = memcmp(pRom, m_pFindData, m_nFindSize)==0;

		if(!bFind && m_pFindText)
		{
			for(UINT i=0; i<m_nFindTextSize; i++)
			{
				bFindText = (*(pRom+i) == *((BYTE*)m_pFindText+i));
				if(!bFindText) break;
			}
		}
		//if(!bFind && m_pFindText) bFindText = strncmp((char*)pRom, m_pFindText, nLen)==0;

		if(bFind)
		{
			ScrollTo((LONG)(pRom-GetDocument()->m_pRom));
			m_ptCursorPos.y = m_ptCursorPos.x+m_nFindSize-1;
			break;
		}else
		if(bFindText)
		{
			ScrollTo((LONG)(pRom-GetDocument()->m_pRom));
			m_ptCursorPos.y = m_ptCursorPos.x+m_nFindTextSize-1;
			break;
		}
		pRom++;
	}

	if(!bFind && !bFindText && bReFind)
	{
		pRom = GetDocument()->m_pRom;
		pEnd = GetDocument()->m_pRom+nOffset;
		bReFind = FALSE;
		goto REFIND;
	}

	if(!bFind && !bFindText)
	{
		Hint(IDS_NOTFIND, MB_OK|MB_ICONINFORMATION);
		if(m_pFrd)
		{
			m_pFrd->DestroyWindow();
			m_pFrd = NULL;
		}
	}
}

void CT2HexView::OnReplace(BOOL bAll)
{
	if(m_pFindData==NULL || m_pReplaceData==NULL || GetDocument()->m_nReadOnly) return;

	UINT nOffset = m_ptCursorPos.x;
	UINT nRomSize = GetDocument()->m_nRomSize;
	BYTE *pEnd = GetDocument()->m_pRom+nRomSize;
	if( (nOffset+m_nFindSize>nRomSize) || bAll)
		nOffset = 0;
	BYTE *pRom = GetDocument()->m_pRom+nOffset;

	BOOL bReFind = nOffset;
	g_bBreak = FALSE;
REFIND:
	BOOL bFind = FALSE; UINT nReplace = 0;
	while( !g_bBreak && (pRom+m_nFindSize <= pEnd) )
	{
		if(m_pFindData)
		{
			for(UINT i=0; i<m_nFindSize; i++)
			{
				bFind = (*(pRom+i) == *(m_pFindData+i));
				if(!bFind) break;
			}
		}

		if(bFind)
		{
			UINT nAddr = (UINT)(pRom-GetDocument()->m_pRom);
			DoUndo(pRom, nAddr, m_nFindSize);
			memcpy(pRom, m_pReplaceData, m_nFindSize);
			if(bAll)
			{
				nReplace++;
				pRom+=m_nFindSize;
				continue;
			}
			ScrollTo(nAddr);
			m_ptCursorPos.y = m_ptCursorPos.x+m_nFindSize-1;
			break;
		}
		pRom++;
	}

	if(!bFind && bReFind)
	{
		pRom = GetDocument()->m_pRom;
		pEnd = GetDocument()->m_pRom+nOffset;
		bReFind = FALSE;
		goto REFIND;
	}

	if(nReplace)
	{
		CString count, format;
		format.LoadString(IDS_REPLACECOUNT);
		count.Format(format, nReplace);
		Hint(count, MB_OK|MB_ICONINFORMATION);
	}else
	if(!bFind)
		Hint(IDS_NOTFIND, MB_OK|MB_ICONINFORMATION);
}

void CT2HexView::OnSearchFindnext()
{
	if(m_pFindData==NULL && m_pFindText==NULL)
		DoFindReplace();
	else
		OnFind();
}

#include "CT2MainFrm.h"
void CT2HexView::OnEditData2pal()
{
	CT2TileView *pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);

	OnEditData2pal(m_ptCursorPos.x, pView->m_nPalIndex, pView->m_nBitCount);
}

void CT2HexView::OnEditData2pal(UINT nOffset, BYTE nIndex, BYTE nBitCount)
{
	UINT nCount = 1<<nBitCount;

	RGBQUAD *pPal = GetDocument()->m_pPal+nIndex;
	BYTE *pRom = GetDocument()->m_pRom+nOffset;
	for(UINT i=0; i<nCount; i++)
	{
		if(m_nAlign==0)
			*(UINT*)pPal = WordToRgbQuad( *((WORD*)pRom+i) );
		else
		{
			BYTE *pData = pRom+i*4;
			pPal->rgbRed = *pData;
			pPal->rgbGreen = *(pData+1);
			pPal->rgbBlue = *(pData+2);
		}
		pPal++;
	}
	//AddMacro_Pal2data(nOffset, nIndex, nBitCount, CT2HEXVIEW_DATA2PAL);
	theApp.GetPalDlg().Invalidate(FALSE);
}

void CT2HexView::OnUpdateEditData2pal(CCmdUI *pCmdUI)
{
	CT2TileView *pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	UINT nSize = (1<<pView->m_nBitCount)*(m_nAlign==0?2:4);
	pCmdUI->Enable(m_ptCursorPos.x+nSize <= GetDocument()->m_nRomSize);
}

void CT2HexView::OnUpdateEditPal2data(CCmdUI *pCmdUI)
{
	CT2TileView *pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	UINT nSize = (1<<pView->m_nBitCount)*(m_nAlign==0?2:4);
	pCmdUI->Enable(m_ptCursorPos.x+nSize <= GetDocument()->m_nRomSize &&
		!GetDocument()->m_nReadOnly);
}

void CT2HexView::OnEditPal2data()
{
	if(GetDocument()->m_nReadOnly) return;
	CT2TileView *pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);

	OnEditPal2data(m_ptCursorPos.x, pView->m_nPalIndex, pView->m_nBitCount);
}

void CT2HexView::OnEditPal2data(UINT nOffset, BYTE nIndex, BYTE nBitCount)
{
	UINT nCount = 1<<nBitCount;

	RGBQUAD *pPal = GetDocument()->m_pPal+nIndex;
	BYTE *pRom = GetDocument()->m_pRom+nOffset;
	DoUndo(pRom, nOffset, nCount*(m_nAlign==0?2:4));
	for(UINT i=0; i<nCount; i++)
	{
		if(m_nAlign==0)
			*((WORD*)pRom+i) = RgbQuadToWord(*(UINT*)pPal);
		else
		{
			COLORREF clrColor = RGB(pPal->rgbRed, pPal->rgbGreen, pPal->rgbBlue);
			*((UINT*)pRom+i) = clrColor;
		}
		pPal++;
	}
	Invalidate(FALSE);

	//AddMacro_Pal2data(nOffset, nIndex, nBitCount, CT2HEXVIEW_PAL2DATA);
}

void CT2HexView::OnSearchLz77()
{
	if(m_Lz77Search.GetSafeHwnd()==NULL)
		m_Lz77Search.Create(CLZ77Search::IDD, this);
	m_Lz77Search.ShowWindow(SW_SHOWNORMAL);
	m_Lz77Search.SetFocus();
}

void CT2HexView::OnToolsLz77uncomp()
{
	static CFileDialog fd(FALSE);
	if(fd.DoModal()!=IDOK) return;

	UINT nStart=m_ptCursorPos.x;

	BYTE *pData=NULL;	int nLzSize, nSize;
	if(!Lz77uncomp(GetDocument()->m_pRom+nStart, &pData, nLzSize, nSize) || !nSize)
	{
		if(pData) delete[] pData;
		return;
	}
	CString LZS; LZS.Format(_T("_%08X"), nLzSize);
	CString hint = fd.GetPathName();
	CString ext = PathFindExtension(hint);
	if(!ext.IsEmpty())
	{
		int nImm = hint.ReverseFind(_T('.'));
		hint.Insert(nImm, LZS);
	}else
		hint.Append(LZS);
	CFile file;
	if(file.Open(hint,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
	{
		file.Write(pData, nSize);
		file.Close();
	}
	delete[] pData;

	if(Hint(IDS_LZ77UNCOMP, MB_YESNO|MB_ICONQUESTION)==IDYES)
		theApp.OpenDocumentFile(hint);
}

void CT2HexView::OnUpdateToolsLz77uncomp(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((UINT)m_ptCursorPos.x<GetDocument()->m_nRomSize-4);
}

void CT2HexView::OnSearchRelative()
{
	if(m_RelativeDlg.GetSafeHwnd()==NULL)
		m_RelativeDlg.Create(CRelativeDlg::IDD, this);
	m_RelativeDlg.ShowWindow(SW_SHOWNORMAL);
	m_RelativeDlg.SetFocus();
}

void CT2HexView::OnExportData()
{
	static CFileDialog fd(FALSE);
	CString filter;
	filter.LoadString(IDS_ALLFILE);
	filter.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter = filter;
	
	if(fd.DoModal()!=IDOK) return;

	CFile file;
	if(!file.Open(fd.GetPathName(),
		CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	UINT nSize = abs(m_ptCursorPos.x-m_ptCursorPos.y)+1;
	file.Write(GetDocument()->m_pRom+
		(m_ptCursorPos.x>m_ptCursorPos.y?
		m_ptCursorPos.y:m_ptCursorPos.x), nSize);
}

void CT2HexView::OnUpdateExportData(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ptCursorPos.x!=m_ptCursorPos.y);
}

void CT2HexView::OnImportData()
{
	//if(GetDocument()->m_nReadOnly) return;
	static CFileDialog fd(TRUE);
	CString filter;
	filter.LoadString(IDS_ALLFILE);
	filter.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter = filter;
	
	if(fd.DoModal()!=IDOK) return;

	OnImportData(fd.GetPathName(), m_ptCursorPos.x);
}

void CT2HexView::OnImportData(CString path, UINT nOffset)
{
	CFile file;
	if(!file.Open(path, CFile::modeRead|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	UINT nSize = (UINT)file.GetLength();
	UINT nRomSize = GetDocument()->m_nRomSize;
	if(m_ptCursorPos.x+nSize>nRomSize)
	{
		nSize = nRomSize-nOffset;
		Hint(IDS_SIZETOOLONG, MB_OK|MB_ICONINFORMATION);
	}
	DoUndo(GetDocument()->m_pRom+nOffset, nOffset, nSize);
	file.Read(GetDocument()->m_pRom+nOffset, nSize);
	if(GetDocument()->m_nRomType==CT2_RT_PCMEM)
	{
		GetDocument()->m_Memory.Update(nOffset, nSize);
	}
	Invalidate(FALSE);

	//AddMacro_ImportData(path, nOffset);
}

void CT2HexView::HitTest(UINT &nOffset, CPoint &pt, BOOL bOff)
{
	UINT nRomSize = GetDocument()->m_nRomSize;
	CString strText;
	int x=0;
	CDC *pDC=GetDC();

	UINT nCodePage = GetDocument()->m_nScriptCodePage;
	WORD nDefaultChar = GetDocument()->m_nDefaultChar;

	HFONT hChar = CreateFontIndirect(&m_lf);
	pDC->SelectObject(hChar);

	UINT nOff = m_nOffset&(~0x0F);
	BYTE *pHex = GetDocument()->m_pRom+nOff;
	BYTE *pEnd = GetDocument()->m_pRom+GetDocument()->m_nRomSize;

	//SCROLLINFO si;	GetScrollInfo(SB_HORZ, &si);

	int l=0;char nSkip=0;
	for(; (l<=m_nLines) && (pHex<pEnd); l++)
	{
		CRect rc;
		rc.left = 2;//-si.nPos;
		rc.top = (nOff-m_nOffset)/16*m_nHeight+m_nHeight;
		rc.bottom = rc.top+m_nHeight;

		for(int x3=0; x3<16; /*x3++*/)
		{
			BYTE *pData = pHex+x3;
			BYTE *pWord = pHex+x3+1;

			BYTE nLow = pWord>=pEnd?0:*pWord;
			BYTE nSize=1;

			if(m_nTblMode==CT2_TBL_USERTBL)
			{
				UINT nCode = m_nSwpCode?MAKEWORD((*pData), nLow):
							MAKEWORD(nLow, (*pData));
				if(!GetDocument()->m_DTBL[nCode].IsEmpty() && !nSkip)
				{
					if(GetDocument()->m_DTBL[nCode].GetAt(0)==_T('\n'))
					{
						strText.Empty();
						//strText = _T("\081 ");
						CString strLen = GetDocument()->m_DTBL[nCode];
						strLen.Delete(0);
						nSize = StrToIntEX(strLen, FALSE);
						if(nSize<1) nSize=1;
						int i=nSize;
						while(i--)
							strText.AppendChar(_T('.'));
					}else
					{
						strText = GetDocument()->m_DTBL[nCode];
						nSize = 2;
						if(x3==15)
							nSkip=1;
					}
				}else
				if(!GetDocument()->m_STBL[*pData].IsEmpty() && !nSkip)
				{
					if(GetDocument()->m_STBL[*pData].GetAt(0)==_T('\n'))
					{
						strText.Empty();
						//strText = _T("\081 ");
						CString strLen = GetDocument()->m_STBL[*pData];
						strLen.Delete(0);
						nSize = StrToIntEX(strLen, FALSE);
						if(nSize<1) nSize=1;
						int i=nSize;
						while(i--)
							strText.AppendChar(_T('.'));
					}else
						strText = GetDocument()->m_STBL[*pData];
				}else
				{
					strText = nSkip?_T(""):_T(".");
					nSkip = 0;
				}
			}else
			{
				UINT nCode = m_nSwpCode?MAKEWORD(nLow, (*pData)):
										MAKEWORD((*pData), nLow);
				if(m_nTblMode==CT2_TBL_USERCTL && GetDocument()->m_DTBL[nCode].GetAt(0)==_T('\n'))
				{
					strText.Empty();
					//strText = _T("\081 ");
					CString strLen = GetDocument()->m_DTBL[nCode];
					strLen.Delete(0);
					nSize = StrToIntEX(strLen, FALSE);
					if(nSize<1) nSize=1;
					int i=nSize;
					while(i--)
						strText.AppendChar(_T('.'));
				}else
				{
					UINT nChar=0;
					if(nCodePage==65001)
						mcharTowchar(&nChar, pData, nCodePage);
					else
						mcharTowchar(&nChar, &nCode, nCodePage);
					if( (pWord<pEnd) &&
						//mcharTowchar(&nChar, nCode, nCodePage) &&
						(nChar!=nDefaultChar) &&
						!nSkip /*&&
						!(nChar>>16)*/ ||
						(nCodePage==1200 || nCodePage==1201) )
					{
						if(nCodePage==1200)
							strText = (WCHAR)nCode;
						else
						if(nCodePage==1201)
							//strText = (WCHAR)((*pData<<8)|*(pData+1));
							strText = (WCHAR)(((nCode<<8)&0xFFFF)|((nCode>>8)&0xFF));
						else
						if(nChar<=_T('~'))
							goto Ansi;
						else
						if(nCodePage==65001)
						{
							nSize++;
							strText = (TCHAR)nChar;
						}else
							strText = (TCHAR)nChar;
						nSize++;
						if(x3==15)
							nSkip=1;
					}else
						Ansi:
					{
						if(m_nTblMode==CT2_TBL_USERCTL && GetDocument()->m_STBL[*pData].GetAt(0)==_T('\n'))
						{
							strText.Empty();
							//strText = _T("\081 ");
							CString strLen = GetDocument()->m_STBL[*pData];
							strLen.Delete(0);
							nSize = StrToIntEX(strLen, FALSE);
							if(nSize<1) nSize=1;
							int i=nSize;
							while(i--)
								strText.AppendChar(_T('.'));
						}else
						{
							if(*pData>=_T(' ') && *pData<=_T('~') && !nSkip)
								strText.Format(_T("%c"), *pData);
							else
							{
								if(nSkip)
									strText = _T("");
								else
									strText = (TCHAR)nDefaultChar;
								nSkip=0;
							}
						}
					}
				}
			}
			CSize szWidth = pDC->GetTextExtent(strText);
			if(szWidth.cx<m_nWidth) szWidth.cx=m_nWidth*2;

			//rc.left = x;
			rc.right = rc.left+szWidth.cx;

			if(bOff)
			{
				if(rc.PtInRect(pt))
				{
					nOffset = nOff+x3;
					goto End;
				}
			}else
			{
				if(nOff+x3==nOffset)
				{
					pt = rc.TopLeft();
					goto End;
				}
			}

			rc.left += szWidth.cx;
			x3+=nSize;
		}

		pHex+=16;nOff+=16;
	}
	bOff?nOffset=0:pt.SetPoint(0, 0);
End:
	DeleteObject(hChar);
	ReleaseDC(pDC);
}

void CT2HexView::OnImportText()
{
	/*if(!GetDocument()->m_nImTbl)
	{
		Hint(IDS_SWPIMTBL, MB_OK|MB_ICONINFORMATION);
		return;
	}*/
	GetDocument()->LoadTbl(GetDocument()->m_strTblName, TRUE);

	OnImportText(g_szBuffer, m_ptCursorPos.x);
}

void CT2HexView::OnImportText(CString Text, UINT nOffset)
{
	// 导入文本
	CString *IDTBL = GetDocument()->m_DTBL;
	CString *ISTBL = GetDocument()->m_STBL;
	BYTE *pRom = GetDocument()->m_pRom;
	UINT nTextAddPos=0;
	UINT nSize = GetDocument()->m_nRomSize-nOffset;
	int nTextPos=0, nTextLen=Text.GetLength();
	UINT nCodePage = GetDocument()->m_nScriptCodePage;
	char nSysCodeLen = nCodePage==CP_UNICODE_UTF8?3:2;
	CString CtrlList;
	while(nTextPos<nTextLen)
	{
		TCHAR Char = Text.GetAt(nTextPos);
		if(Char==_T('['))
		{	// 控制符
			int nCtrlPos = Text.Find(_T(']'), nTextPos);
			if(nCtrlPos!=-1)
			{
				nTextPos++;
				CtrlList = Text.Mid(nTextPos, nCtrlPos-nTextPos);
				nCtrlPos = CtrlList.GetLength();
				if(nCtrlPos&1)
				{
					nCtrlPos++;
					CtrlList.Insert(0, _T('0'));
				}
				nTextPos+=nCtrlPos+1;
				DoUndo(pRom+nOffset+nTextAddPos, nOffset+nTextAddPos, nCtrlPos/2);
				for(int i=0; i<nCtrlPos; i+=2)
				{
					int dwCode=StrToIntEX(CtrlList.Mid(i, 2));
					if( (dwCode!=-1) &&
						(nTextAddPos<nSize) )
					{
						memcpy(pRom+nOffset+nTextAddPos, &dwCode, 1);
						nTextAddPos++;
					}
					
				}
				continue;
			}
		}

		if(!m_nTblMode)
		{
			// 系统码表
			if(nTextAddPos<(int)(nSize-nSysCodeLen))
			{
				LPSTR lpOut = (LPSTR)(pRom+nOffset+nTextAddPos);
				BYTE l = Char<0x80?1:nSysCodeLen;

				DoUndo((BYTE*)lpOut, nOffset+nTextAddPos, l);

				WideCharToMultiByte(nCodePage, 0, &Char, 2, lpOut, l, NULL, NULL);
				if(m_nSwpCode && l==2)
					*(WORD*)lpOut = (WORD)(*((BYTE*)lpOut+1) | (*(BYTE*)lpOut<<8));

				nTextAddPos += l;
				nTextPos++;
				continue;
			}
		}else
		{
			if(!IDTBL[Char].IsEmpty() && (nTextAddPos<(nSize-1)))
			{
				int dwCode = StrToIntEX(IDTBL[Char]);
				if(dwCode==-1) continue;
				DoUndo(pRom+nOffset+nTextAddPos, nOffset+nTextAddPos, 2);
				memcpy(pRom+nOffset+nTextAddPos, &dwCode, 2);
				nTextAddPos+=2;
				nTextPos++;
				//nWordCount++;
				continue;
			}
			if(!ISTBL[Char].IsEmpty() && (nTextAddPos<nSize))
			{
				int Code = StrToIntEX(ISTBL[Char]);
				if(Code==-1) continue;
				DoUndo(pRom+nOffset+nTextAddPos, nOffset+nTextAddPos, 1);
				*(pRom+nOffset+nTextAddPos)=(BYTE)Code;
				nTextAddPos++;
				nTextPos++;
				//nByteCount++;
				continue;
			}
		}
		nTextPos++;
	}

	GetDocument()->LoadTbl(GetDocument()->m_strTblName);
	/*CEdit *pEdit =
		(CEdit*)theApp.GetExplorerBox()->GetDlgItem(IDC_HEXVIEW_IMPORTEDIT);
	pEdit->SetSel(0, -1);
	pEdit->SetFocus();*/

	Invalidate(FALSE);
}

void CT2HexView::OnUpdateImportText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((!GetDocument()->m_strTblName.IsEmpty()||m_nTblMode!=CT2_TBL_USERTBL) &&
		!GetDocument()->m_nReadOnly );
}

void CT2HexView::OnHexviewTblctrl()
{
	m_nTblMode=m_nTblMode==CT2_TBL_USERCTL?CT2_TBL_SYS:CT2_TBL_USERCTL;
	if(m_nTblMode==CT2_TBL_USERCTL)
	{
		CT2Doc *pDoc = GetDocument();
		if(pDoc->m_strTblName.IsEmpty())
		{
			pDoc->OnTblSeltbl();
			m_nTblMode = pDoc->m_strTblName.IsEmpty()?CT2_TBL_SYS:CT2_TBL_USERCTL;
		}
	}
	OnUpdateData();
	//Invalidate(FALSE);
}

void CT2HexView::OnUpdateHexviewTblctrl(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nTblMode==CT2_TBL_USERCTL);
}

void CT2HexView::OnUpdateEditImport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_nReadOnly);
}

void CT2HexView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CT2HexView::OnLButtonDown(nFlags, point);

	/*CSDatInfo *si = NULL;
	CT2ChildFrm *c = (CT2ChildFrm*)GetParent();
	if(c->m_pSFI)
		si = (CSDatInfo*)c->m_pSFI;
	else
		return;

	CListCtrl &lc = si->m_FileList;
	for(int i=0; i<lc.GetItemCount(); i++)
	{
		CString Addr = lc.GetItemText(i, 1);
		UINT nOffset = StrToIntEX(Addr);
		if(si->m_nOffset)
			Addr = lc.GetItemText(i, 3);
		else
			Addr = lc.GetItemText(i, 2);
		UINT nSize = StrToIntEX(Addr, FALSE);
		if((UINT)m_ptCursorPos.x>=nOffset && (UINT)m_ptCursorPos.x<(nOffset+nSize))
		{
			nSize-=m_ptCursorPos.x-nOffset;
			si->m_Player.Play((LPSTR)(GetDocument()->m_pRom+nOffset), nSize);
			break;
		}
	}*/
}
/*
void CT2HexView::OnFontList(UINT nID)
{
	char nMax = GetParent()->IsZoomed()?1:0;
	HMENU hHexView = GetSubMenu(theApp.m_hHexMenu, 3+nMax);
	int nItem=GetMenuItemIndex(hHexView, ID_HEXVIEW_SWPCODE)+3;
	hHexView = GetSubMenu(hHexView, nItem);
	GetMenuString(hHexView, nID, m_strFaceName.GetBuffer(32), 32, MF_STRING|MF_BYCOMMAND);
	m_strFaceName.ReleaseBuffer();
	Invalidate(FALSE);
}
*/

void CT2HexView::OnFontList()
{
	LOGFONT lf;
	memcpy(&lf, &m_lf, sizeof(lf));
	CFontDialog fd(&lf, CF_SCREENFONTS|CF_INITTOLOGFONTSTRUCT|CF_TTONLY|CF_NOOEMFONTS);

	if(fd.DoModal()!=IDOK) return;
	lstrcpyn(m_lf.lfFaceName, lf.lfFaceName, sizeof(lf.lfFaceName)/sizeof(TCHAR));
	m_lf.lfCharSet = lf.lfCharSet;
	Invalidate(FALSE);
}

void CT2HexView::OnUpdateImport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_nReadOnly && GetDocument()->m_nRomType!=CT2_RT_PCMEM);
}

void CT2HexView::AppendVar()
{
	CCrystalTreeCtrl &tc = ((CT2MainFrm*)theApp.GetMainWnd())->m_TreeBar.m_Tree;
	int i=CTAV_CT2VIEW;
	tc.AppendVal(i++, &m_nTblMode, CTCS_BYTE);
	tc.AppendVal(i++, &m_nSwpCode, CTCS_BYTE);

	tc.AppendVal(i, &GetDocument()->m_nCurCp, CTCS_BYTE);
	CString strItem;
	strItem.LoadString(IDS_DEFCODEPAGE);
	for(UINT i2=0; i2<theApp.m_nCPCount; i2++)
		strItem.AppendFormat(_T(";%s"), GetCodePageDescription(theApp.m_pCodePage[i2]));
	int v=0;
	HTREEITEM hItem = tc.IndexToItem(i++, tc.GetRootItem(), v);
	tc.SetVarData(hItem, strItem);

	tc.AppendVal(i++, &m_nAlign, CTCS_BYTE);
	tc.AppendVal(i++, &m_nColorText, CTCS_BYTE);
	tc.AppendVal(i, &g_szBuffer, CTCS_LPTSTR, 0, _MAX_PATH);
}
/*
void CT2HexView::AddMacro_SetData(UINT nOffset, UINT nData, BYTE nSize)
{
	CT_MACRO_CT2HEXVIEW_SETDATA m;
	INITMACRO(m, CT2HEXVIEW_SETDATA);

	m.nData = nData;
	m.nOffset = nOffset;
	m.nSize = nSize;
	m.nFlags = (m_nByteLR<<2) | (m_nAsciiMode<<1) | (m_nSwpCode);

	AddMacro(&m);
}

void CT2HexView::AddMacro_ImportData(CString path, UINT nOffset)
{
	CT_MACRO_1VAL_1FILE m;
	INITMACRO(m, CT2HEXVIEW_IMPORTDATA);

	m.nVal = nOffset;
	lstrcpyn(m.szFile, path, _MAX_PATH);

	AddMacro(&m);
}

void CT2HexView::AddMacro_ImportText(CString Text, UINT nOffset)
{
	CT_MACRO_1VAL_1FILE m;
	INITMACRO(m, CT2HEXVIEW_IMPORTTEXT);

	m.nVal = nOffset;
	lstrcpyn(m.szFile, Text, _MAX_PATH);

	AddMacro(&m);
}

void CT2HexView::AddMacro_Pal2data(UINT nOffset, BYTE nIndex, BYTE nBitCount, BYTE nT)
{
	CT_MACRO_CT2HEXVIEW_SETDATA m;
	INITMACRO(m, nT);

	m.nOffset = nOffset;
	m.nData = nIndex;
	m.nFlags = nBitCount;
	m.nSize = m_nAlign;

	AddMacro(&m);
}
*/

extern WORD GetCrc16(BYTE *pCrc, WORD nSize);
void CT2HexView::OnToolsGetcrc16()
{
	UINT nBegin = min(m_ptCursorPos.x, m_ptCursorPos.y);
	UINT nEnd = max(m_ptCursorPos.x, m_ptCursorPos.y);
	WORD crc16 = GetCrc16(GetDocument()->m_pRom+nBegin, nEnd-nBegin+1);
	CString str;
	str.Format(_T("%04X"), crc16);
	Hint(str);
}
