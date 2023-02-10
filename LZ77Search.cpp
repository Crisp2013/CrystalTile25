// LZ77Search.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\LZ77Search.h"
#include ".\CT2HexView.h"

// CLZ77Search 对话框

IMPLEMENT_DYNAMIC(CLZ77Search, CDialog)
CLZ77Search::CLZ77Search(CWnd* pParent /*=NULL*/)
: CDialog(CLZ77Search::IDD, pParent)
, m_strHead(_T("10"))
, m_strSizelow(_T("0004"))
, m_strSizeHi(_T("FFFF"))
, m_bDown(FALSE)
, m_nStep(4)
, m_bLz77(FALSE)
{
}

CLZ77Search::~CLZ77Search()
{
}

void CLZ77Search::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FALGEDIT, m_strHead);
	DDX_Text(pDX, IDC_SIZELOW, m_strSizelow);
	DDX_Text(pDX, IDC_SIZEHI, m_strSizeHi);
	DDX_Check(pDX, IDC_DOWNCHECK, m_bDown);
	DDX_Control(pDX, IDC_RETLIST, m_LZ77List);
	DDX_Text(pDX, IDC_STEPEDIT, m_nStep);
	DDX_Check(pDX, IDC_LZ77CHECK, m_bLz77);
}


BEGIN_MESSAGE_MAP(CLZ77Search, CDialog)
	ON_COMMAND_RANGE(IDC_FINDBTN, IDC_NEXTBTN, OnFind)
	ON_COMMAND(IDC_LZ77SAVEBTN, OnLz77ListSave)
	ON_UPDATE_COMMAND_UI(IDC_LZ77SAVEBTN, OnUpdateLz77ListSave)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_LZ77_DELETE, OnLz77Delete)
	ON_UPDATE_COMMAND_UI(ID_LZ77_DELETE, OnUpdateLz77Delete)
	ON_NOTIFY(LVN_KEYDOWN, IDC_RETLIST, OnLvnKeydownRetlist)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_RETLIST, OnLvnItemActivateRetlist)
	ON_COMMAND(ID_LZ77_UNCOMPALL, OnLz77Uncompall)
	ON_UPDATE_COMMAND_UI(ID_LZ77_UNCOMPALL, OnUpdateLz77ListSave)
	ON_COMMAND(ID_LZ77_UNCOMPSEL, OnLz77Uncompsel)
	ON_UPDATE_COMMAND_UI(ID_LZ77_UNCOMPSEL, OnUpdateLz77Uncompsel)
	ON_COMMAND(ID_LZ77_BREAK, OnLz77Break)
	ON_UPDATE_COMMAND_UI(ID_LZ77_BREAK, OnUpdateLz77Break)
	//ON_WM_NCACTIVATE()
END_MESSAGE_MAP()


// CLZ77Search 消息处理程序

BOOL CLZ77Search::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pParentWnd = theApp.GetView(ID_VIEW_HEXVIEW);

	m_LZ77List.SetExtendedStyle(m_LZ77List.GetExtendedStyle()|
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	CRect rc; m_LZ77List.GetClientRect(&rc);
	CString HeadText;HeadText.LoadString(IDS_LZ77HEADTEXT);
	m_LZ77List.InsertColumn(0, HeadText, LVCFMT_LEFT, rc.Width()-GetSystemMetrics(SM_CXVSCROLL));
	m_LZ77List.GetHeaderCtrl()->EnableWindow(FALSE);

	GetWindowText(HeadText);
	HeadText.Append(((CView*)m_pParentWnd)->GetDocument()->GetTitle());
	SetWindowText(HeadText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

struct FINDDATA {
	CT2StatusBar *pPro;
	CT2HexView *pView;
	CLZ77Search *pLz77;
	UINT nID;
};

UINT Find(LPVOID lpParam)
{
	FINDDATA &fd = *(FINDDATA*)lpParam;
	CT2StatusBar &sb = *fd.pPro;
	CListCtrl &m_LZ77List = fd.pLz77->m_LZ77List;
	UINT nID = fd.nID;
	CT2HexView *pHex = fd.pView;
	int m_nStep = fd.pLz77->m_nStep;
    BOOL bLz77 = fd.pLz77->m_bLz77;

	UINT nPos = (nID==IDC_NEXTBTN) || (fd.pLz77->m_bDown && (nID==IDC_FINDBTN)) ?
		pHex->m_ptCursorPos.x:0;
	//nPos&=0xFFFFFFFC;//四的倍数
	if(nID==IDC_NEXTBTN) nPos+=m_nStep;
	UINT nEnd = pHex->GetDocument()->m_nRomSize-4;
	//if(nPos>=nEnd) return;

	int nHead = StrToIntEX(fd.pLz77->m_strHead);
	if(nHead==-1) nHead = 0x10;
	UINT nLow = StrToIntEX(fd.pLz77->m_strSizelow),
        nHi = StrToIntEX(fd.pLz77->m_strSizeHi);
	if(nLow<1) nLow = 0x01;
	if(nHi==-1) nHi = 0xFFFF;
	if(nHi<nLow)
	{
		UINT nTmp = nLow;
		nLow = nHi;
		nHi = nTmp;
	}

	if(nID==IDC_FINDBTN) m_LZ77List.DeleteAllItems();
	BYTE *pRom = pHex->GetDocument()->m_pRom;
	CString strConext;
	int nIndex=0;
	BOOL bLoop = (nPos!=0) && (nID==IDC_NEXTBTN);
	UINT nLoopEnd = nPos;
	UINT nBegin = nPos;
	UINT nSearchSize = nEnd-nBegin;
	g_bBreak=FALSE;
Loop:
	while(nPos<nEnd && !g_bBreak)
	{
		UINT nHeader = *(UINT*)(pRom+nPos);
		UINT nSize = nHeader>>8;
		if( ((nHeader&0xFF)==nHead) &&
			!(bLz77 && (*(UINT*)(pRom+nPos-4)!=0x37375A4C)) &&
			(nSize>=nLow) && 
			(nSize<=nHi) &&
			(!(*(pRom+nPos+4)&0xC0)) )
		{
			if(nID==IDC_NEXTBTN)
			{
				fd.nID = nPos;
				bLoop=FALSE;
				break;
			}
			strConext.Format(_T("%08d %08X %08d"),
				nIndex, nPos, (*(UINT*)(pRom+nPos)>>8));
			m_LZ77List.InsertItem(nIndex, strConext);
			nIndex++;
		}
		nPos+=m_nStep;

		sb.SetProgressPos((nPos-nBegin)*100/nSearchSize);
	}
	if(bLoop)
	{
		nEnd = nLoopEnd;
		nPos = 0;
		bLoop = FALSE;
		goto Loop;
	}

	if(nID!=IDC_NEXTBTN)
	{
		strConext.LoadString(IDS_LZ77SEARCHCOUNT);
		CString strHint;
		strHint.Format(strConext, nIndex);
		Hint(strHint, MB_OK|MB_ICONINFORMATION);
	}
	fd.pPro->EndModalLoop(IDCANCEL);
	return 0;
}

void CLZ77Search::OnFind(UINT nID)
{
	UpdateData();

	CT2HexView *pHex = (CT2HexView*)m_pParentWnd;
	if(pHex->GetDocument()->m_nRomSize<=4) return;
	if(m_nStep<1)
	{
		m_nStep = 1;
		UpdateData(FALSE);
	}

	m_LZ77List.SetRedraw(FALSE);
	FINDDATA fd;
	fd.pPro = (CT2StatusBar*) theApp.GetStatusBar();
	fd.pPro->BeginProgress(IDS_LZ77SEARCH_SEARCHING);

	fd.pLz77 = this;
	fd.pView = pHex;
	fd.nID = nID;

	CWinThread *pOpenThread = AfxBeginThread(Find,
		&fd, THREAD_PRIORITY_IDLE, 1024, 0, NULL);
	fd.pPro->RunModalLoop();

	m_LZ77List.SetRedraw();

	fd.pPro->EndProgress();


	if(nID==IDC_NEXTBTN)
	{
		pHex->ScrollTo(fd.nID);

		if(((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIGetActive()!=m_pParentWnd->GetParent())
			((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIActivate(pHex->GetParent());

		if(theApp.GetCurView()!=m_pParentWnd)
			pHex->SetCurView(ID_VIEW_HEXVIEW);
	}
	SetFocus();
}

void CLZ77Search::OnLz77ListSave()
{
	static CFileDialog fd(FALSE);
	int nCount = m_LZ77List.GetItemCount();
	if( (nCount<1) || (fd.DoModal()!=IDOK) ) return;
	CStdioFile file;
	if(!file.Open(fd.GetPathName(),
		CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::typeBinary))
	{
		ShowError();
		return;
	}
	CString strItem;
#ifdef UNICODE
	WORD wUnicode = 0xFEFF;
	file.Write(&wUnicode, 2);
#endif
	strItem.LoadString(IDS_LZ77HEADTEXT);
	file.WriteString(strItem+_T("\r\n"));
	for(int i=0; i<nCount; i++)
	{
		strItem = m_LZ77List.GetItemText(i, 0);
		file.WriteString(strItem+_T("\r\n"));
	}
}

void CLZ77Search::OnUpdateLz77ListSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_LZ77List.GetItemCount()>0);
}

void CLZ77Search::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	::OnInitMenuPopup(this, pPopupMenu->GetSafeHmenu());
}

void CLZ77Search::OnLz77Delete()
{
	m_LZ77List.SetRedraw(FALSE);
	POSITION pos = m_LZ77List.GetFirstSelectedItemPosition();
	if(pos)
	{
		do
		{
			m_LZ77List.DeleteItem(m_LZ77List.GetNextSelectedItem(pos));
		}while(pos=m_LZ77List.GetFirstSelectedItemPosition());
	}else
	{
		m_LZ77List.DeleteAllItems();
	}
	m_LZ77List.SetRedraw();
}

void CLZ77Search::OnUpdateLz77Delete(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_LZ77List.GetItemCount()>0) ||
		(m_LZ77List.GetFirstSelectedItemPosition()!=NULL));
}

void CLZ77Search::OnLvnKeydownRetlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if(pLVKeyDow->wVKey==VK_DELETE)
		OnLz77Delete();

	*pResult = 0;
}

void CLZ77Search::OnLvnItemActivateRetlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int nSel = pNMIA->iItem;
	CT2HexView *pHex = (CT2HexView*)m_pParentWnd;
	if((nSel==-1) || !pHex) return;

	if(((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIGetActive()!=m_pParentWnd->GetParent())
		((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIActivate(pHex->GetParent());

	if(theApp.GetCurView()!=m_pParentWnd)
		pHex->SetCurView(ID_VIEW_HEXVIEW);
	else
		pHex->SetFocus();

	CString Addr = m_LZ77List.GetItemText(nSel, 0);
	pHex->m_nOffset = StrToIntEX(Addr.Mid(9, 8));
	pHex->ScrollTo(pHex->m_nOffset);
}

struct UNCOMP{
	BYTE* pRom;
	CLZ77Search* pLZ77Search;
};
BYTE m_bReady=TRUE;
BYTE m_bBreak=FALSE;

UINT DoUnCompAll(LPVOID lParam)
{
	UNCOMP *Param = (UNCOMP*)lParam;
	CListCtrl& LZ77List = Param->pLZ77Search->m_LZ77List;
	int nCount = LZ77List.GetItemCount();
	//if(nCount<1) {m_bReady = TRUE;return -1;}
	UINT nAddr;
	int nLzSize, nBinSize;
	BYTE *pDst=NULL;
	CFile file;
	CString strItem;
	CString Path = GetPath(Param->pLZ77Search->m_hWnd);
	if(Path.IsEmpty()) {m_bReady = TRUE;return -1;}
	CString filename;
	BYTE *pRom = Param->pRom;
	if(pRom==NULL) {m_bReady = TRUE;return -1;}
	int nI=0;
	for(int nItem=0; (nItem<nCount) && !m_bBreak; nItem++)
	{
		strItem = LZ77List.GetItemText(nItem, 0);
		nAddr = StrToIntEX(strItem.Mid(9,8));
		if(nAddr==-1) continue;
		BOOL bOK = Lz77uncomp(pRom+nAddr, &pDst, nLzSize, nBinSize);
		if(bOK)
		{
			filename.Format(_T("%s\\%04d-%08X-%04X.bin"), Path, nI, nAddr, nLzSize);
			if(file.Open(filename, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
			{
				file.Write(pDst, nBinSize);
				file.Close();
				nI++;
			}
		}
		if(pDst)
		{
			delete[] pDst;
			pDst = NULL;
		}
	}

	m_bReady = TRUE;
	return 0;
}

UINT DoUnCompSel(LPVOID lParam)
{
	UNCOMP *Param = (UNCOMP*)lParam;
	CListCtrl& LZ77List = Param->pLZ77Search->m_LZ77List;
	int nCount = LZ77List.GetItemCount();
	//if(nCount<1) {m_bReady = TRUE;return -1;}
	UINT nAddr;
	int nLzSize, nBinSize;
	BYTE *pDst=NULL;
	CFile file;
	CString strItem;
	CString Path = GetPath(Param->pLZ77Search->m_hWnd);
	if(Path.IsEmpty()) {m_bReady = TRUE;return -1;}
	CString filename;
	BYTE *pRom = Param->pRom;
	if(pRom==NULL) {m_bReady = TRUE;return -1;}
	int nItem,nI=0;
	POSITION pos = LZ77List.GetFirstSelectedItemPosition();
	while( ((nItem = LZ77List.GetNextSelectedItem(pos)) != -1) && !m_bBreak )
	{
		strItem = LZ77List.GetItemText(nItem, 0);
		nAddr = StrToIntEX(strItem.Mid(9,8));
		if(nAddr==-1) continue;
		BOOL bOK = Lz77uncomp(pRom+nAddr, &pDst, nLzSize, nBinSize);
		if(bOK)
		{
			filename.Format(_T("%s\\%08d-%08X-%04X.bin"), Path, nI, nAddr, nLzSize);
			if(file.Open(filename, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
			{
				file.Write(pDst, nBinSize);
				file.Close();
				nI++;
			}
		}
		if(pDst)
		{
			delete[] pDst;
			pDst = NULL;
		}
	}

	m_bReady = TRUE;
	return 0;
}

void CLZ77Search::OnLz77Uncompall()
{
	static UNCOMP Param;
	Param.pLZ77Search = this;
	Param.pRom = ((CT2HexView*)m_pParentWnd)->GetDocument()->m_pRom;
	m_bReady = FALSE;
	m_bBreak = FALSE;
	AfxBeginThread(DoUnCompAll, (LPVOID)&Param, THREAD_PRIORITY_IDLE, 1024, 0, NULL);
}

void CLZ77Search::OnLz77Uncompsel()
{
	static UNCOMP Param;
	Param.pLZ77Search = this;
	Param.pRom = ((CT2HexView*)m_pParentWnd)->GetDocument()->m_pRom;
	m_bReady = FALSE;
	m_bBreak = FALSE;
	AfxBeginThread(DoUnCompSel, (LPVOID)&Param, THREAD_PRIORITY_IDLE, 1024, 0, NULL);
}

void CLZ77Search::OnUpdateLz77Uncompsel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_LZ77List.GetFirstSelectedItemPosition()!=NULL);
}

void CLZ77Search::OnLz77Break()
{
	m_bBreak = TRUE;
}

void CLZ77Search::OnUpdateLz77Break(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bReady);
}

void CLZ77Search::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
	POSITION pos = m_LZ77List.GetFirstSelectedItemPosition();
	if(pos)
	{
		NMITEMACTIVATE ia;
		ia.iItem = m_LZ77List.GetNextSelectedItem(pos);
		LRESULT Result;
		OnLvnItemActivateRetlist((NMHDR*)&ia, &Result);
	}
}
