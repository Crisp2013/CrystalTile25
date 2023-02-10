// RelativeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include "RelativeDlg.h"
#include "CT2HexView.h"


// CRelativeDlg 对话框

IMPLEMENT_DYNAMIC(CRelativeDlg, CDialog)
CRelativeDlg::CRelativeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRelativeDlg::IDD, pParent)
	, m_nValType(1)
	, m_nBytes(2)
	, m_strAdd(_T(""))
	, m_nFindCount(0)
	, m_bSwpCheck(FALSE)
	, m_bOffsetCheck(FALSE)
	, m_nStart(-1)
	, m_nEnd(-1)
	, m_strStart(_T(""))
	, m_strEnd(_T(""))
{
}

CRelativeDlg::~CRelativeDlg()
{
}

void CRelativeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_DECRADIO, m_nValType);
	DDX_Radio(pDX, IDC_4BITRADIO, m_nBytes);
	DDX_Text(pDX, IDC_VALEDIT, m_strAdd);
	DDX_Control(pDX, IDC_FINDLIST, m_FindList);
	DDX_Control(pDX, IDC_RESULTLIST, m_ResultList);
	DDX_Check(pDX, IDC_SWPCHECK, m_bSwpCheck);
	DDX_Check(pDX, IDC_OFFCHECK, m_bOffsetCheck);
	DDX_Text(pDX, IDC_STARTEDIT, m_strStart);
	DDX_Text(pDX, IDC_ENDEDIT, m_strEnd);
}


BEGIN_MESSAGE_MAP(CRelativeDlg, CDialog)
	ON_BN_CLICKED(IDC_ADDBTN, OnBnClickedAddbtn)
	ON_BN_CLICKED(IDC_DELBTN, OnBnClickedDelbtn)
	ON_BN_CLICKED(IDC_CLRBTN, OnBnClickedClrbtn)
	ON_BN_CLICKED(IDC_FINDBTN, OnBnClickedFindbtn)
	ON_BN_CLICKED(IDC_SAVEBTN, OnBnClickedSavebtn)
	ON_LBN_DBLCLK(IDC_RESULTLIST, OnLbnDblclkResultlist)
	ON_COMMAND_RANGE(IDC_4BITRADIO, IDC_2BITRADIO, OnBnClicked1bitradio)
	ON_NOTIFY_EX(TTN_GETDISPINFO, 0, OnToolTipText)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_OFFCHECK, OnBnClickedOffcheck)
END_MESSAGE_MAP()


// CRelativeDlg 消息处理程序

void CRelativeDlg::OnBnClickedAddbtn()
{
	UpdateData();
	if( (m_nFindCount>100) || m_strAdd.IsEmpty() ) return;
	CString strAdd; int nPos1=0,nPos2=0;
	while(1)
	{
		nPos2 = m_strAdd.Find(_T(','), nPos1);
		if( (nPos2==-1) || (m_nValType==2) )
			strAdd = m_strAdd.Right(m_strAdd.GetLength()-nPos1);
		else
			strAdd = m_strAdd.Mid(nPos1, nPos2-nPos1);
		nPos1=nPos2+1;
		if(strAdd.IsEmpty()) break;
		switch(m_nValType)
		{
		default:
		case 0://DEC
			m_Find[m_nFindCount].nVal1 = StrToIntEX(strAdd, FALSE);
			break;
		case 1://HEX
			m_Find[m_nFindCount].nVal1 = StrToIntEX(strAdd);
			break;
		case 2://STR
			{
				int nCount = strAdd.GetLength();
				for(int i=0; i<nCount; i++)
				{
					m_Find[m_nFindCount].nVal1 = strAdd.GetAt(i);
					switch(m_nBytes)//1Byte
					{
					case 1://byte
						m_Find[m_nFindCount].nVal1&=0xFF; break;
					case 2://word
						if(m_bSwpCheck)// && (m_nValType==1))
							m_Find[m_nFindCount].nVal1 = (m_Find[m_nFindCount].nVal1>>8)|(m_Find[m_nFindCount].nVal1<<8);
						m_Find[m_nFindCount].nVal1&=0xFFFF;
						break;
					case 0://int
						break;
					}
					if(m_nFindCount)
					{
						m_Find[m_nFindCount].nVal2 =
							m_Find[m_nFindCount].nVal1 - m_Find[m_nFindCount-1].nVal1;
					}else
						m_Find[m_nFindCount].nVal2 = 0;
					m_nFindCount++;
				}
				OnRefresh();
				CEdit *pEdit = (CEdit*)GetDlgItem(IDC_VALEDIT);
				pEdit->SetFocus();
				pEdit->SetSel(0, -1);
				return;
			}
			break;
		}
		switch(m_nBytes)
		{
		case 1://byte
			m_Find[m_nFindCount].nVal1&=0xFF; break;
		case 2://word
			if(m_bSwpCheck)// && (m_nValType==1))
				m_Find[m_nFindCount].nVal1 = (m_Find[m_nFindCount].nVal1>>8)|(m_Find[m_nFindCount].nVal1<<8);
			m_Find[m_nFindCount].nVal1&=0xFFFF;
			break;
		case 0://int
			break;
		}
		if(m_nFindCount)
		{
			m_Find[m_nFindCount].nVal2 =
				m_Find[m_nFindCount].nVal1 - m_Find[m_nFindCount-1].nVal1;
		}else
			m_Find[m_nFindCount].nVal2 = 0;
		m_nFindCount++;
		if(nPos2==-1) break;
	}
	OnRefresh();
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_VALEDIT);
	pEdit->SetFocus();
	pEdit->SetSel(0, -1);
}

void CRelativeDlg::OnBnClickedDelbtn()
{
	int nSel = m_FindList.GetCurSel();
	if(nSel == -1) return;
	for(int i=nSel; i<m_nFindCount; i++)
	{
		m_Find[i].nVal1 = m_Find[i+1].nVal1;
		m_Find[i].nVal2 = m_Find[i+1].nVal2;
	}
	m_nFindCount--;
	OnRefresh();
	m_FindList.SetCurSel(nSel);
}

void CRelativeDlg::OnRefresh(void)
{
	m_FindList.ResetContent();
	UpdateData();
	CString strItem;
	if(m_nBytes!=1)
	{
		CString strFormat=m_nBytes==2?_T("%04Xh"):_T("%08Xh");
		for(int i=0; i<m_nFindCount; i++)
		{
			if(m_nBytes==2)
				{m_Find[i].nVal1&=0xFFFF; m_Find[i].nVal2=m_Find[i].nVal1-(m_Find[i-1].nVal1&0xFFFF);}
			else
				{m_Find[i].nVal2=m_Find[i].nVal1-m_Find[i-1].nVal1;}
			if(i)
				strItem.Format(strFormat+_T("\t[%08d]"), m_Find[i].nVal1, m_Find[i].nVal2);
			else
				strItem.Format(strFormat, m_Find[i].nVal1);
			m_FindList.InsertString(-1, strItem);
		}
	}else
	{
		for(int i=0; i<m_nFindCount; i++)
		{
			m_Find[i].nVal1&=0xFF;
			m_Find[i].nVal2=m_Find[i].nVal1-m_Find[i-1].nVal1;
			if(i)
				strItem.Format(_T("%02Xh\t[%08d]"), m_Find[i].nVal1, m_Find[i].nVal2);
			else
				strItem.Format(_T("%02Xh"), m_Find[i].nVal1);
			m_FindList.InsertString(-1, strItem);
		}
	}
	m_FindList.SetCurSel(m_FindList.GetCount()-1);
}

void CRelativeDlg::OnBnClickedClrbtn()
{
	m_nFindCount = 0;
	m_FindList.ResetContent();
	m_ResultList.ResetContent();
}

void CRelativeDlg::OnBnClickedFindbtn()
{
	CT2HexView *pHex = (CT2HexView*)m_pParentWnd;
	BYTE *pRom = pHex->GetDocument()->m_pRom;
	UINT nSize = pHex->GetDocument()->m_nRomSize;
	if(!pHex || !pRom || (nSize<(UINT)(m_nFindCount*m_nBytes))) return;

	UpdateData();
	OnInitOffset();

	if(m_nStart==-1) m_nStart = 0;
	if(m_nEnd==-1 || (m_nEnd+m_nFindCount*m_nBytes)>nSize)
		m_nEnd = nSize-m_nFindCount*m_nBytes;

	m_ResultList.SetRedraw(FALSE);
	m_ResultList.ResetContent();

	int nCur = m_bOffsetCheck?pHex->m_ptCursorPos.x:m_nStart;

	int nEnd = m_nEnd;//-m_nFindCount;
	BOOL bOK = FALSE;
	CString strResult;
	CString strHint;
	int nCount = 0;
	switch(m_nBytes)
	{// 双字节
	case 2:
		while(nCur<nEnd)
		{
			for(int i=1; i<m_nFindCount; i++)
			{
				bOK = (*(WORD*)(pRom+nCur+i*2)) - (*(WORD*)(pRom+nCur+i*2-2)) == m_Find[i].nVal2;
				if(!bOK) break;
			}
			if(bOK)
			{
				if(m_ResultList.GetCount()>=9999)
				{
					Hint(IDS_RELATIVE_COUNTOUT, MB_OK|MB_ICONSTOP);
					goto BreakEnd;
				}
				nCount++;
				strResult.Format(_T("%04d %08X "), nCount, nCur);
				for(int i=0; i<m_nFindCount; i++)
					strResult.AppendFormat(_T("%02X%02X|"), *(pRom+nCur+i*2), *(pRom+nCur+i*2+1));
				m_ResultList.InsertString(-1, strResult);
				//nCur+=m_nFindCount*2-1;
			}
			nCur++;
		}break;
	case 1:// 单字节
		while(nCur<nEnd)
		{
			for(int i=1; i<m_nFindCount; i++)
			{
				bOK = (pRom[nCur+i]-pRom[nCur+i-1]) == m_Find[i].nVal2;
				if(!bOK) break;
			}
			if(bOK)
			{
				if(m_ResultList.GetCount()>=9999)
				{
					Hint(IDS_RELATIVE_COUNTOUT, MB_OK|MB_ICONSTOP);
					goto BreakEnd;
				}
				nCount++;
				strResult.Format(_T("%04d %08X "), nCount, nCur);
				for(int i=0; i<m_nFindCount; i++)
					strResult.AppendFormat(_T("%02X|"), pRom[nCur+i]);
				m_ResultList.InsertString(-1, strResult);
				//nCur+=m_nFindCount-1;
			}
			nCur++;
		}
		break;
	case 0://int
		{
			int *pintRom = (int*)pRom;
			nEnd/=4;nCur/=4;
			while(nCur<nEnd)
			{
				for(int i=1; i<m_nFindCount; i++)
				{
					bOK = (pintRom[nCur+i]-pintRom[nCur+i-1]) == m_Find[i].nVal2;
					if(!bOK) break;
				}
				if(bOK)
				{
					if(m_ResultList.GetCount()>=9999)
					{
						Hint(IDS_RELATIVE_COUNTOUT, MB_OK|MB_ICONSTOP);
						goto BreakEnd;
					}
					nCount++;
					strResult.Format(_T("%04d %08X "), nCount, nCur*4);
					for(int i=0; i<m_nFindCount; i++)
						strResult.AppendFormat(_T("%08X|"), pintRom[nCur+i]);
					m_ResultList.InsertString(-1, strResult);
					//nCur+=m_nFindCount-1;
				}
				nCur++;
			}
		}
		break;
	}
	strResult.LoadString(IDS_RELATIVE_FINDCOUNT);
	strHint.Format(strResult, nCount);
	Hint(strHint, MB_OK|MB_ICONINFORMATION);

BreakEnd:
	m_ResultList.SetRedraw();
}

BOOL CRelativeDlg::PreTranslateMessage(MSG* pMsg)
{
	GetDlgItem(IDC_FINDBTN)->EnableWindow(m_nFindCount>1);

	GetDlgItem(IDC_SWPCHECK)->EnableWindow(m_nBytes==2);

	/*if(m_pParentWnd &&
		(pMsg->message==WM_KEYDOWN) && (pMsg->wParam==VK_ESCAPE))
	{
		m_pParentWnd->SetFocus();
		return TRUE;
	}*/

	return CDialog::PreTranslateMessage(pMsg);
}

void CRelativeDlg::OnBnClickedSavebtn()
{
	static CFileDialog fd(FALSE);
	int nCount = m_ResultList.GetCount();
	CString strItem;
	strItem.LoadString(IDS_SCRIPTFILE);
	strItem.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter = strItem;
	if( (nCount<1) || (fd.DoModal()!=IDOK) ) return;
	CStdioFile file;
	strItem = PathFindExtension(fd.GetPathName());
	strItem.MakeLower();
	if(strItem!=_T(".txt"))
		strItem = fd.GetPathName()+_T(".txt");
	else
		strItem = fd.GetPathName();
	if(!file.Open(strItem,
		CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::typeBinary) ||
		(nCount<1) )
		return;
#ifdef UNICODE
	WORD wUnicode = 0xFEFF;
	file.Write(&wUnicode, 2);
#endif
	for(int i=0; i<nCount; i++)
	{
		m_ResultList.GetText(i, strItem);
		file.WriteString(strItem+_T("\r\n"));
	}
}

void CRelativeDlg::OnLbnDblclkResultlist()
{
	int nSel = m_ResultList.GetCurSel();
	CT2HexView *pHex = (CT2HexView*)m_pParentWnd;
	if( (nSel==-1) || !pHex ) return;

	if(((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIGetActive()!=m_pParentWnd->GetParent())
		((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIActivate(pHex->GetParent());

	if(theApp.GetCurView()!=m_pParentWnd)
		pHex->SetCurView(ID_VIEW_HEXVIEW);
	else
		pHex->SetFocus();

	CString Addr;
	m_ResultList.GetText(nSel, Addr);
	pHex->ScrollTo(StrToIntEX(Addr.Mid(5, 8)));

	//pHex->SetFocus();
}

BOOL CRelativeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pParentWnd = theApp.GetView(ID_VIEW_HEXVIEW);

	m_ResultList.SendMessage(WM_SETFONT, (WPARAM)(HFONT)::GetStockObject(SYSTEM_FIXED_FONT), 1);
	m_FindList.SendMessage(WM_SETFONT, (WPARAM)(HFONT)::GetStockObject(SYSTEM_FIXED_FONT), 1);
	m_FindList.SetTabStops(16);

	EnableToolTips();

	CString HeadText;
	GetWindowText(HeadText);
	HeadText.Append(((CView*)m_pParentWnd)->GetDocument()->GetTitle());
	SetWindowText(HeadText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CRelativeDlg::OnBnClicked1bitradio(UINT nID)
{
	//if(nID>IDC_2BITRADIO)
	//	UpdateData();
	//else
		OnRefresh();
}

BOOL CRelativeDlg::OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	TOOLTIPTEXT *pTTT=(TOOLTIPTEXT*)pNMHDR;
	UINT nID=(UINT)pNMHDR->idFrom;
	if(pTTT->uFlags&TTF_IDISHWND)
		nID=::GetDlgCtrlID((HWND)pNMHDR->idFrom);

	if(nID)
	{
		pTTT->lpszText=MAKEINTRESOURCE(nID);
		pTTT->hinst=AfxGetResourceHandle();
		CString hint;
		hint.LoadString(nID);
		theApp.GetStatusBar()->SetPaneText(0, hint);
		return TRUE;
	}
	return FALSE;
}

void CRelativeDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if( (nState==WA_ACTIVE) || (nState==WA_CLICKACTIVE) )
	{
		OnInitOffset();
	}
}

void CRelativeDlg::OnInitOffset(void)
{
	CT2HexView *pHex=(CT2HexView*)m_pParentWnd;
	if(pHex)
	{
		m_nStart = StrToIntEX(m_strStart);
		m_nEnd = StrToIntEX(m_strEnd);
		UINT nRomSize = pHex->GetDocument()->m_nRomSize;
		if((m_nStart>=nRomSize)||(m_nStart<0)) m_nStart = 0;
		if((m_nEnd>=nRomSize)||(m_nEnd<0)) m_nEnd = nRomSize-1;
		if(m_bOffsetCheck)
		{
			UINT nCur = pHex->m_ptCursorPos.x;
			if(!m_nBytes) nCur=nCur/4*4;
			m_nStart = nCur;
			if(nCur>m_nEnd) {m_nStart = m_nEnd; m_nEnd = nRomSize-1;}
		}
		if(m_nStart==m_nEnd) {m_nStart = 0; m_nEnd = nRomSize-1;}
		m_strStart.Format(_T("%08X"), m_nStart);
		m_strEnd.Format(_T("%08X"), m_nEnd);
		UpdateData(FALSE);
	}
}

void CRelativeDlg::OnBnClickedOffcheck()
{
	OnInitOffset();
}
