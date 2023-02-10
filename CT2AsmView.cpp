// CT2AsmView.cpp : CT2AsmView 类的实现
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include "CT2Doc.h"
#include ".\CT2AsmView.h"
#include "CT2TileView.h"
#include "Asm.h"
#include ".\AsmDlg.h"
#include "CT2InputDlg.h"
#include "CT2ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CPU_A9	0
#define CPU_A7	1

// CT2AsmView

IMPLEMENT_DYNCREATE(CT2AsmView, CT2View)

BEGIN_MESSAGE_MAP(CT2AsmView, CT2View)
	ON_WM_PAINT()
	ON_COMMAND_RANGE(ID_ARM_ARM9_ARM, ID_ARM_ARM7_THUMB, OnArmList)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ARM_ARM9_ARM, ID_ARM_ARM7_THUMB, OnUpdateArmList)
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_ARM_BASEADDRESS, OnBaseChange)
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_COMMAND(ID_EDIT_EXPORT, OnEditExport)
	ON_COMMAND(ID_ASM_FOLLOW, OnAsmFollow)
	ON_COMMAND(ID_ASM_UNFOLLOW, OnAsmUnfollow)
	ON_UPDATE_COMMAND_UI(ID_ASM_UNFOLLOW, OnUpdateAsmUnfollow)
END_MESSAGE_MAP()

// CT2AsmView 构造/析构

CT2AsmView::CT2AsmView()
: m_nHeight(12)
, m_nLines(1)
, m_nSize(4)
, m_nCpuType(CPU_A9)
, m_nBaseAddress(0)
{
	// TODO: 在此处添加构造代码
}

CT2AsmView::~CT2AsmView()
{
	UINT nIndex = theApp.GetMRUIndex(GetDocument()->GetPathName());
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	if(pSD)
	{
		pSD->nAsmViewOffset = m_nOffset;
		pSD->nAsmBaseAddress = m_nBaseAddress;
		pSD->nAsmSize = m_nSize;
		pSD->nAsmCpuType = m_nCpuType;
	}
}

// CT2AsmView 消息处理程序

void CT2AsmView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc; GetClientRect(&rc);

	CBitmap memBM;
	memBM.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	memDC.SelectObject(&memBM);

	FillRect(memDC.GetSafeHdc(), &rc,
		((CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW))->m_hBkBrush);

	HFONT hAsm = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
	memDC.SetBkMode(TRANSPARENT);

	// 绘制头部
	CRect rcHeader(m_nHeight*4+2, 0, rc.right/* m_nWidth*74*/, m_nHeight);

	memDC.FillRect(&rcHeader, &CBrush(::GetSysColor(COLOR_BTNFACE)));
	memDC.Draw3dRect(&rcHeader, 0xFFFFFF, 0x0);

	CString strText, strHeader;

	memDC.SelectObject(theApp.GetOptionBox().GetFont());
	strText.LoadString(IDS_ASMVIEW_HEADER);
	AfxExtractSubString(strHeader, strText, 0);
	rcHeader.left+=m_nHeight;
	memDC.TextOut(rcHeader.left, 2, strHeader);

	rcHeader.left += m_nHeight*4;

	int nType = m_nCpuType*2+1;
	if(m_nSize==2) nType++;

	AfxExtractSubString(strHeader, strText, nType);
	memDC.TextOut(rcHeader.left, 2, strHeader);

	memDC.SelectObject(hAsm);
	rcHeader = rc;
	rcHeader.top+=m_nHeight;

	UINT nOffset = m_nOffset;
	BYTE nSize;
	for(int i=0; i<m_nLines && nOffset<GetDocument()->m_nRomSize; i++)
	{
		nSize = GetAsm(strHeader, nOffset, 3);

		if(i==m_ptCursorPos.x)
		{
			CRect rcFocus(rcHeader.left, rcHeader.top+i*m_nHeight,
				rcHeader.right, rcHeader.top+(i+1)*m_nHeight);

			memDC.FillRect(&rcFocus, &CBrush(0xFFFF));
		}

		memDC.TextOut(rcHeader.left, rcHeader.top+i*m_nHeight, strHeader);

		nOffset+=nSize;
	}

	dc.BitBlt(0, 0, rc.Width(), rc.Height(),
		&memDC, 0, 0, SRCCOPY);

	memBM.DeleteObject();	memDC.DeleteDC();
}

void CT2AsmView::OnInitialUpdate()
{
	CT2View::OnInitialUpdate();

	CString Name = GetDocument()->GetPathName();
	UINT nIndex = theApp.GetMRUIndex(Name);
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	if(theApp.m_pRecentStatList->IsOK(nIndex))
	{
		m_nOffset = pSD->nAsmViewOffset;
		m_nBaseAddress = pSD->nAsmBaseAddress;
		m_nSize = pSD->nAsmSize;
		m_nCpuType = pSD->nAsmCpuType;
	}else
	{
		if(GetDocument()->m_nRomType==CT2_RT_NDS)
		{
			NDSHEADER *header = ((CT2ChildFrm*)GetParent())->m_NdsFSI.m_pHeader;
			m_nBaseAddress = header->Arm9_Ram_Address-header->Arm9_Rom_Offset;
			//m_nBaseAddress = 0x02000000;
		}else
		if(GetDocument()->m_nRomType==CT2_RT_PCMEM)
			m_nBaseAddress = 0x02000000;
		else
			m_nBaseAddress = 0x08000000;
	}

	CDC* pDC = GetDC();
	pDC->SelectObject(CFont::FromHandle((HFONT)GetStockObject(SYSTEM_FIXED_FONT)));
	CSize szExtent = pDC->GetTextExtent(_T(" "));

	m_nHeight = (BYTE)szExtent.cy;

	ReleaseDC(pDC);

	m_BaseEdit.Create(WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|ES_RIGHT|ES_UPPERCASE|WS_VISIBLE, CRect(0,0,m_nHeight*4,m_nHeight),this,IDC_ARM_BASEADDRESS);
	m_BaseEdit.ShowWindow(SW_NORMAL);

	CString strBase;
	strBase.Format(_T("%08X"), m_nBaseAddress);
	m_BaseEdit.SetWindowText(strBase);

	//OnUpdateData();
	ScrollTo(m_nOffset);
}

void CT2AsmView::OnUpdateData(void)
{
	CRect rc; GetClientRect(&rc);
	if(rc.IsRectEmpty()) return;
	m_nLines = (rc.Height()-m_nHeight)/m_nHeight;

	if(m_nSize==4)
		m_nOffset &= ~3;

	CSize sizeTotal;
	sizeTotal.cx = 0;
	sizeTotal.cy = (GetDocument()->m_nFileSize/m_nSize+2)*m_nHeight;

	SetScrollSizes(MM_TEXT, sizeTotal);
	SetScrollPos(SB_VERT, (m_nOffset+GetDocument()->m_nBlockOffset)/m_nSize*m_nHeight);

	Invalidate(FALSE);

	if(m_BaseEdit.GetSafeHwnd())
	{
		m_BaseEdit.SetWindowPos(NULL,
			0, 0, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	}

	// Nds文件提示
	CT2StatusBar *pSB = theApp.GetStatusBar();
	pSB->SetPaneText(0, GetDocument()->OffsetToPathName(m_nOffset+m_ptCursorPos.x*m_nSize));

	CT2View::OnUpdateData();
}

void CT2AsmView::OnArmList(UINT nID)
{
	switch(nID)
	{
	case ID_ARM_ARM9_ARM:
	case ID_ARM_ARM9_THUMB:
		m_nCpuType = CPU_A9;
		break;
	case ID_ARM_ARM7_ARM:
	case ID_ARM_ARM7_THUMB:
		m_nCpuType = CPU_A7;
		break;
	}
	switch(nID)
	{
	case ID_ARM_ARM9_ARM:
	case ID_ARM_ARM7_ARM:
		m_nSize = 4;
		break;
	case ID_ARM_ARM9_THUMB:
	case ID_ARM_ARM7_THUMB:
		m_nSize = 2;
		break;
	}

	OnUpdateData();
}

void CT2AsmView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	if(point.y<m_nHeight)
	{
 		HMENU hPop = LoadMenu(NULL, MAKEINTATOM(IDR_CT2POPMENU));
		HMENU hAsm = GetSubMenu(hPop, 2);

		CPoint pt(point.x, m_nHeight);
		ClientToScreen(&pt);
		::OnInitMenuPopup(theApp.m_pMainWnd, hAsm, TRUE);
		TrackPopupMenu(hAsm, TPM_LEFTBUTTON, pt.x, pt.y, 0, m_hWnd, NULL);
	}else
	{
		m_ptCursorPos.x = (point.y-m_nHeight)/m_nHeight;
		OnUpdateData();
	}

	CT2View::OnLButtonDown(nFlags, point);
}

void CT2AsmView::OnBaseChange()
{
	CString strBaseAddress;
	GetDlgItemText(IDC_ARM_BASEADDRESS, strBaseAddress);
	UINT nBase = StrToIntEX(strBaseAddress);
	if(nBase!=-1)
	{
		m_nBaseAddress = nBase;
		OnUpdateData();
	}
}

void CT2AsmView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
			m_nOffset = si.nTrackPos/m_nHeight*m_nSize;
		}break;
	case SB_LINEUP:
		if(m_nOffset>=m_nSize)
			m_nOffset-=m_nSize;
		break;
	case SB_LINEDOWN:
		if((m_nOffset+m_nSize*m_nLines)<GetDocument()->m_nFileSize)
			m_nOffset+=m_nSize;
		break;
	case SB_TOP:
		m_nOffset=0;
		break;
	case SB_BOTTOM:
		m_nOffset = GetDocument()->m_nFileSize;
		if(m_nOffset>(UINT)m_nLines*m_nSize)
			m_nOffset -= m_nLines*m_nSize;
		else
			m_nOffset = 0;
		break;
	case SB_PAGEUP:
		if(m_nOffset>=((UINT)(m_nLines-1)*m_nSize))
			m_nOffset-=((m_nLines-1)*m_nSize);
		else
			m_nOffset = 0;
		break;
	case SB_PAGEDOWN:
		if((m_nOffset+((m_nLines-1)*m_nSize))<GetDocument()->m_nFileSize)
			m_nOffset+=((m_nLines-1)*m_nSize);
		if( m_nOffset >= (GetDocument()->m_nFileSize-((m_nLines-1)*m_nSize)) )
			m_nOffset = GetDocument()->m_nFileSize-(m_nLines*m_nSize);
		break;
	}
	GetDocument()->Seek(m_nOffset);
	//m_nOffset-=GetDocument()->m_nBlockOffset;
	OnUpdateData();

	// CT2View::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CT2AsmView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_nOffset+=GetDocument()->m_nBlockOffset;
	switch(nChar)
	{
	case VK_UP:
		if(m_ptCursorPos.x>=1)
			m_ptCursorPos.x-=1;
		else
		if(m_nOffset>=m_nSize)
			m_nOffset-=m_nSize;
		break;
	case VK_DOWN:
		if(m_ptCursorPos.x<(m_nLines-1))
			m_ptCursorPos.x++;
		else
		if(m_nOffset+m_nLines*m_nSize<GetDocument()->m_nFileSize)
			m_nOffset+=m_nSize;
		break;
	case VK_HOME:
		if(GetKeyState(VK_CONTROL)&0x80)
			m_nOffset = 0;
		m_ptCursorPos.x = 0;
		break;
	case VK_END:
		if(GetKeyState(VK_CONTROL)&0x80)
		{
			m_nOffset = GetDocument()->m_nFileSize;
			if(m_nOffset>(UINT)m_nLines*m_nSize)
				m_nOffset -= m_nLines*m_nSize;
			else
				m_nOffset = 0;
		}
		m_ptCursorPos.x = m_nLines-1;
		break;
	case VK_PRIOR:
		if(m_nOffset>=((UINT)(m_nLines-1)*m_nSize))
			m_nOffset-=((m_nLines-1)*m_nSize);
		else
			m_nOffset = 0;
		break;
	case VK_NEXT:
		if((m_nOffset+((m_nLines-1)*m_nSize))<GetDocument()->m_nFileSize)
			m_nOffset+=((m_nLines-1)*m_nSize);
		if( m_nOffset >= (GetDocument()->m_nFileSize-((m_nLines-1)*m_nSize)) )
			m_nOffset = GetDocument()->m_nFileSize-(m_nLines*m_nSize);
		break;
	}
	GetDocument()->Seek(m_nOffset);
	//m_nOffset-=GetDocument()->m_nBlockOffset;
	switch(nChar)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_HOME:
	case VK_END:
	case VK_PRIOR:
	case VK_NEXT:
		OnUpdateData();
		break;
	}

	CT2View::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CT2AsmView::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN && LOWORD(pMsg->wParam)==VK_TAB)
	{
		if(GetFocus()==this)
			m_BaseEdit.SetFocus();
		else
			SetFocus();
	}else
	if(pMsg->message==WM_CONTEXTMENU)
	{
 		HMENU hPop = LoadMenu(NULL, MAKEINTATOM(IDR_CT2POPMENU));
		HMENU hAsm = GetSubMenu(hPop, 2);

		CPoint pt(m_nHeight*9+2, m_nHeight);
		ClientToScreen(&pt);
		::OnInitMenuPopup(theApp.m_pMainWnd, hAsm, TRUE);
		TrackPopupMenu(hAsm, TPM_LEFTBUTTON, pt.x, pt.y, 0, m_hWnd, NULL);
	}

	return CT2View::PreTranslateMessage(pMsg);
}

void CT2AsmView::ScrollTo(UINT nOffset, BOOL bForce)
{
	if(nOffset>GetDocument()->m_nFileSize)
		return;
	if(nOffset>=(UINT)m_ptCursorPos.x*m_nSize)
		m_nOffset = nOffset-m_ptCursorPos.x*m_nSize;
	else
		m_nOffset = nOffset;
	m_nOffset += GetDocument()->m_nBlockOffset;
	GetDocument()->Seek(m_nOffset);
	//m_nOffset -= GetDocument()->m_nBlockOffset;

	OnUpdateData();
}

void CT2AsmView::OnUpdateArmList(CCmdUI* pCmdUI)
{
	int nType = m_nCpuType*2;
	if(m_nSize==2) nType += 1;

	pCmdUI->SetRadio(pCmdUI->m_nID-ID_ARM_ARM9_ARM == nType);
}

void CT2AsmView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CT2View::OnChar(nChar, nRepCnt, nFlags);
	if( (GetDocument()->m_nReadOnly && GetDocument()->m_nRomType!=CT2_RT_PCMEM) ||
		nChar==VK_TAB || (GetKeyState(VK_CONTROL)&0x80)) return;

	static CAsmDlg ad;
	ad.m_nType = m_nCpuType*2;
	if(m_nSize==2) ad.m_nType += 1;
	ad.m_nCurOffset = m_nOffset;
	UINT nSize;//, nCode;
	for(int i=0; i<m_ptCursorPos.x; i++)
	{
		nSize = GetAsm(ad.m_strAsm, ad.m_nCurOffset, 0);
		ad.m_nCurOffset += nSize;
	}
	if(ad.m_nCurOffset>GetDocument()->m_nRomSize)
		return;

	if(!IsCharAlpha(nChar)) return;

	ad.m_strAsm = (TCHAR)nChar;
	ad.m_nCurOffset += m_nBaseAddress;
	if(ad.DoModal()==IDOK && ad.m_nType)
	{
		BYTE *pRom = GetDocument()->m_pRom;
		UINT nOff = ad.m_nCurOffset-m_nBaseAddress;
		DoUndo(pRom+nOff, nOff, ad.m_nSize);
		memcpy(pRom+nOff, &ad.m_nType, ad.m_nSize);

		if(GetDocument()->m_nRomType==CT2_RT_PCMEM)
			GetDocument()->m_Memory.Update(nOff, ad.m_nSize);

		Invalidate(FALSE);
	}
}

void CT2AsmView::OnEditExport()
{
	CFileDialog fd(FALSE);

	CString strAsm;
	strAsm.LoadString(NULL, IDS_SCRIPTFILE, 0);
	strAsm.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter=strAsm;
	if(fd.DoModal()!=IDOK) return;
	CStdioFile file;
	strAsm = fd.GetPathName();
	if(strAsm.Right(4).CompareNoCase(_T(".txt"))!=0)
		strAsm.Append(_T(".txt"));
	if(!file.Open(strAsm, CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::typeBinary))
		return;

	UINT nOffset = 0xFEFF;
	file.Write(&nOffset, 2);
	nOffset = m_nOffset + m_ptCursorPos.x*m_nSize;

	CT2InputDlg IDlg(IDS_ASMEXPORT);
	if(IDlg.DoModal()!=IDOK) return;
	int nLines = StrToIntEX(IDlg.m_strVal, FALSE);

	if(nLines<1) return;

	for(int i=0; i<nLines && nOffset<GetDocument()->m_nRomSize; i++)
	{
		nOffset += GetAsm(strAsm, nOffset, 3);

		file.WriteString(strAsm+_T("\r\n"));
	}
}

#define	INDEX(i) ((((i)>>16)&0xFF0)|(((i)>>4)&0xF))
int CT2AsmView::GetAsm(CString &strAsm, UINT nOffset, BOOL bType)
{
	BYTE *pAsm = GetDocument()->m_pRom+nOffset;
	UINT nCode;
	strAsm.Empty();
	int nSize;

	if(bType&1)
		strAsm.AppendFormat(_T("%08X"), nOffset+m_nBaseAddress);
	if(m_nSize==4)
	{// ARM
		nCode = *(UINT*)pAsm;
		*m_strAsm = m_nCpuType;
		nSize =
		des_arm_instructions_set[INDEX(nCode)](nOffset+m_nBaseAddress, nCode, m_strAsm);

		if(bType&2)
		{
			if(bType&1)
				strAsm.AppendChar(_T(' '));
			strAsm.AppendFormat(_T("%08X"), nCode);
		}
		//nSize = 4;
	}else
	{// THUMB
		nCode = *(WORD*)pAsm;
		*m_strAsm = m_nCpuType;
		nSize =
		des_thumb_instructions_set[nCode>>6](nOffset+m_nBaseAddress, nCode, m_strAsm);

		if((nSize&7)==2)
		{
			//strHeader.Format(_T("%08X   %04X  "), nOffset, nCode);
			if(bType&2)
			{
				if(bType&1)
					strAsm.Append(_T("   "));
				strAsm.AppendFormat(_T("%04X"), nCode);
			}
			// nSize = 2;
		}else
		{
			nCode = *(WORD*)(pAsm+2);
			if(nCode>>11!=0x1F)
			{
				nSize=2;
				//strHeader.Format(_T("%08X   %04X  "), nOffset, *(WORD*)pAsm);
				if(bType&2)
				{
					if(bType&1)
						strAsm.Append(_T("   "));
					strAsm.AppendFormat(_T("%04X"), *(WORD*)pAsm);
				}
				strcpy(m_strAsm, "[???]");
			}else
			{
				*m_strAsm = m_nCpuType;

				des_thumb_instructions_set[nCode>>6](nOffset+m_nBaseAddress, nCode, m_strAsm);

				nCode = *(UINT*)pAsm;
				//strHeader.Format(_T("%08X %08X"), nOffset, nCode);
				if(bType&2)
				{
					if(bType&1)
						strAsm.AppendChar(_T(' '));
					strAsm.AppendFormat(_T("%08X"), nCode);
				}
			}
			// nSize = 4;
		}
	}
	OemToChar(m_strAsm, m_wstrAsm);
	if(bType&3)
		strAsm.AppendChar(_T(' '));
	strAsm.AppendFormat(_T("%s"), m_wstrAsm);
	if(nSize>>3)
	{
		nSize &=0x7FFFFFFF;
		nOffset += (nSize&7)+(nSize>>3);
		int val=*(int*)(GetDocument()->m_pRom + nOffset);
		strAsm.AppendFormat(_T("    ;[%08X]=0x%08X(%i)"), nOffset, val, val);
	}
	return nSize&7;
}

void CT2AsmView::OnAsmFollow()
{
	if(GetFocus()!=GetDlgItem(IDC_ARM_BASEADDRESS))
	{
		UINT nOffset = m_nOffset;
		CString tmp;
		for(int i=0; i<m_ptCursorPos.x; i++)
			nOffset += GetAsm(tmp, nOffset, 0);
		GetAsm(tmp, nOffset, 0);
		tmp = tmp.Right(9);
		tmp.Delete(0);
		if(tmp.Right(1)==_T("h"))
		{
			tmp.Delete(tmp.GetLength()-1);
			UINT nFollow = StrToIntEX(tmp);
			if(nFollow!=-1)
			{
				m_Follow.Add(nOffset);
				if(nFollow>m_nBaseAddress)
					nFollow-=m_nBaseAddress;
				ScrollTo(nFollow);
			}
		}
	}
}

void CT2AsmView::OnAsmUnfollow()
{
	INT_PTR i = m_Follow.GetCount()-1;
	UINT nOff = m_Follow.GetAt(i);
	ScrollTo(nOff);
	m_Follow.RemoveAt(i);
}

void CT2AsmView::OnUpdateAsmUnfollow(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_Follow.IsEmpty() && GetFocus()!=GetDlgItem(IDC_ARM_BASEADDRESS));
}
