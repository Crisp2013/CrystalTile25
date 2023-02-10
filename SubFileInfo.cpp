// SubFileInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\SubFileInfo.h"
#include "CT2TileView.h"

// CSubFileInfo 对话框

IMPLEMENT_DYNAMIC(CSubFileInfo, CDialog)
CSubFileInfo::CSubFileInfo(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
	, m_strTitle(_T(""))
	, m_pChild(NULL)
	, m_nOffset(0)
	, m_nSize(0)
	, m_nSubOffset(1)
{
}

CSubFileInfo::~CSubFileInfo()
{
}

void CSubFileInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SFI_FILELIST, m_FileList);
}


BEGIN_MESSAGE_MAP(CSubFileInfo, CDialog)
	ON_WM_SIZE()
	ON_WM_INITMENUPOPUP()
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_SFI_FILELIST, OnLvnItemActivateSdatlist)
	ON_COMMAND(ID_SFI_EXPORT, OnSfiExport)
	ON_UPDATE_COMMAND_UI(ID_SFI_EXPORT, OnUpdateSfiExport)
	ON_COMMAND(ID_SFI_IMPORT, OnSfiImport)
	ON_UPDATE_COMMAND_UI(ID_SFI_IMPORT, OnUpdateSfiImport)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SFI_REFLASH, OnSfiReflash)
	ON_NOTIFY(LVN_ENDSCROLL, IDC_SFI_FILELIST, OnLvnEndScrollFsilist)
	ON_COMMAND(ID_FSI_LOADDATA, OnFsiLoaddata)
	ON_UPDATE_COMMAND_UI(ID_FSI_LOADDATA, OnUpdateFsiLoaddata)
END_MESSAGE_MAP()


// CSubFileInfo 消息处理程序

void CSubFileInfo::OnInit(CString strTitle, CT2ChildFrm* pChild, UINT nOffset, UINT nSize)
{
	m_strTitle = strTitle;
	m_pChild = pChild;
	m_nOffset = nOffset;
	m_nSize = nSize;

	SetHeader();

	Update();
}

BOOL CSubFileInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_FileList.ModifyStyle(LVS_SORTASCENDING|LVS_SORTDESCENDING|
		LVS_ICON|LVS_SMALLICON|LVS_LIST|LVS_EDITLABELS|LVS_NOSORTHEADER,
		LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SHAREIMAGELISTS/*|LVS_OWNERDRAWFIXED*/);
	m_FileList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP);

	GetWindowText(m_strWTitle);

	CreateImageList();

	m_FileList.SendMessage(LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)g_hCTImageList);

	SetIcon(ImageList_GetIcon(g_hCTImageList, FSI_ICON_NDS, 0), FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSubFileInfo::OnSize(UINT nType, int cx, int cy)
{
	if(m_FileList.GetSafeHwnd())
	{
		CRect rc; m_FileList.GetWindowRect(&rc);
		ScreenToClient(rc);
		m_FileList.SetWindowPos(NULL,
			0, 0,
			cx-rc.left,
			cy-rc.top,
			SWP_NOMOVE);
	}

	CDialog::OnSize(nType, cx, cy);
}

void CSubFileInfo::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	::OnInitMenuPopup(this, pPopupMenu->GetSafeHmenu(), TRUE);
}

void CSubFileInfo::GoToFile(int iItem, char cOffset)
{
	if(iItem==-1)
	{
		POSITION pos = m_FileList.GetFirstSelectedItemPosition();
		iItem = m_FileList.GetNextSelectedItem(pos);
		if(iItem==-1) return;
	}

	if(((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIGetActive()!=m_pChild)
		((CMDIFrameWnd*)theApp.m_pMainWnd)->MDIActivate(m_pChild);

	CT2View *pView = m_pChild->GetCurView();
	pView->SetFocus();

	CString Addr = m_FileList.GetItemText(iItem, cOffset);
	UINT nOffset = StrToIntEX(Addr);
	if(nOffset!=-1)
		pView->ScrollTo(nOffset, TRUE);
}

void CSubFileInfo::OnLvnItemActivateSdatlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	GoToFile(pNMIA->iItem);

	*pResult = 0;
}

void CSubFileInfo::SetHeader()
{
	CHeaderCtrl& hc = *m_FileList.GetHeaderCtrl();
	while(hc.GetItemCount()>0)
		m_FileList.DeleteColumn(0);

	CString header, text;
	header.LoadString(IDS_SFI_HEADER);
	BYTE nFormat[]={LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_RIGHT};
	BYTE nWidth[]={112, 64, 64, 64};
	for(int i=0; i<sizeof(nFormat)/sizeof(BYTE); i++)
	{
		if(m_nOffset==0 && i==2) continue;

		AfxExtractSubString(text, header, i);
		m_FileList.InsertColumn(i, text, nFormat[i], nWidth[i]);
	}
}

int CSubFileInfo::GetCurSel()
{
	POSITION pos = m_FileList.GetFirstSelectedItemPosition();
	return m_FileList.GetNextSelectedItem(pos);
}

void CSubFileInfo::OnSfiExport()
{
	OnExport();
}

void CSubFileInfo::OnExport(char cPos)
{
	int sel = GetCurSel();

	UINT nOffset = StrToIntEX(m_FileList.GetItemText(sel, 1+cPos));
	if(nOffset==-1) return;

	m_FileList.GetItemText(sel, 0, g_szBuffer, _MAX_PATH);
	lstrcpy(g_szBuffer, PathFindFileName(g_szBuffer));

	static CFileDialog fd(FALSE);
	fd.m_pOFN->lpstrFile = g_szBuffer;
	fd.m_pOFN->nMaxFile = _MAX_PATH;
	if(fd.DoModal()!=IDOK) return;

	CFile f;
	if(!f.Open(g_szBuffer, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	UINT nSize = StrToIntEX(m_FileList.GetItemText(sel, (m_nOffset==0?2:3)+cPos), FALSE);

	f.Write(m_pChild->m_pTileView->GetDocument()->m_pRom+nOffset, nSize);
}

void CSubFileInfo::OnUpdateSfiExport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetCurSel()!=-1);
}

void CSubFileInfo::OnSfiImport()
{
	OnImport();
}

void CSubFileInfo::OnImport(char cPos)
{
	int sel = GetCurSel();

	UINT nOffset = StrToIntEX(m_FileList.GetItemText(sel, 1+cPos));
	if(nOffset==-1) return;

	m_FileList.GetItemText(sel, 0, g_szBuffer, _MAX_PATH);
	lstrcpy(g_szBuffer, PathFindFileName(g_szBuffer));

	static CFileDialog fd(TRUE);
	fd.m_pOFN->lpstrFile = g_szBuffer;
	fd.m_pOFN->nMaxFile = _MAX_PATH;
	if(fd.DoModal()!=IDOK) return;

	CFile f;
	if(!f.Open(g_szBuffer, CFile::modeRead|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	UINT nSize = StrToIntEX(m_FileList.GetItemText(sel, (m_nOffset==0?2:3)+cPos), FALSE);

	UINT nFileSize = (UINT)f.GetLength();

	if(nFileSize>nSize)
	{
		Hint(IDS_SFI_NORMAL_IMPORTERR);
		return;
	}else
	if(m_nOffset)
	{
		Hint(IDS_SFI_IMPORTERR);
		return;
	}

	CT2View *pView = ((CT2ChildFrm*)m_pChild)->m_pTileView;
	BYTE *pBuf = pView->GetDocument()->m_pRom+nOffset;

	pView->DoUndo(pBuf, nOffset, nSize);

	f.Read(pBuf, nFileSize);
	nSize-=nFileSize;
	if(nSize)
		memset(pBuf+nFileSize, 0, nSize);
}

void CSubFileInfo::OnUpdateSfiImport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetCurSel()!=-1 &&
		!((CT2ChildFrm*)m_pChild)->m_pTileView->GetDocument()->m_nReadOnly);
}

void CSubFileInfo::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu *pMenu = GetMenu();
	if(!pMenu) return;
	pMenu = pMenu->GetSubMenu(0);
	if(!pMenu) return;

	long x = point.x,
		y = point.y;

	if(x<0 || y<0)
	{
		int nSel = GetCurSel();
		if(nSel!=-1)
		{
			CRect rc;
			m_FileList.GetItemRect(nSel, &rc, 0);
			m_FileList.ClientToScreen(&rc);
			x = rc.left;
			y = rc.bottom;
		}
	}
	pMenu->TrackPopupMenu(TPM_LEFTBUTTON, x, y, this);
}

void CSubFileInfo::OnSfiReflash()
{
	int nSel = GetCurSel();
	Update();
	if(nSel!=-1)
	{
		m_FileList.SetItemState(nSel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_FileList.SendMessage(WM_KEYDOWN, VK_LEFT);
	}
}

void CSubFileInfo::OnLvnEndScrollFsilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 此功能要求 Internet Explorer 5.5 或更高版本。
	// 符号 _WIN32_IE 必须是 >= 0x0560。
	LPNMLVSCROLL pStateChanged = reinterpret_cast<LPNMLVSCROLL>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	m_FileList.Invalidate(FALSE);
}

extern void __fastcall OnFsiLoadmap(CT2TileView *pView, UINT nPos);
extern void __fastcall OnFsiLoadtile(CT2TileView *pView, UINT nPos, BYTE nType=0);
extern void __fastcall OnFsiLoadpal(CT2TileView *pView, UINT nPos, WORD nCount=256);

void CSubFileInfo::OnFsiLoaddata()
{
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	if(nSel==-1) return;

	CString pos = m_FileList.GetItemText(nSel, 2);
	UINT nPos = StrToIntEX(pos);
	pos = m_FileList.GetItemText(nSel, 2+m_nSubOffset);	pos.Replace(_T(","), _T(""));
	UINT nCount = StrToIntEX(pos, FALSE)>>1;

	if(CheckSelItemExt(_T(".ncgr")) || CheckSelItemExt(_T(".char")) || CheckSelItemExt(_T(".nbfc")))
		::OnFsiLoadtile((CT2TileView*)((CT2ChildFrm*)m_pChild)->m_pTileView, nPos);
	else
	/*if(CheckSelItemExt(_T(".nbfc")))
		::OnFsiLoadtile((CT2TileView*)((CT2ChildFrm*)m_pChild)->m_pTileView, nPos, 2);
	else*/
	if(CheckSelItemExt(_T(".ntft")))
		::OnFsiLoadtile((CT2TileView*)((CT2ChildFrm*)m_pChild)->m_pTileView, nPos, 1);
	else
	if(CheckSelItemExt(_T(".nclr")))
		::OnFsiLoadpal((CT2TileView*)((CT2ChildFrm*)m_pChild)->m_pTileView, nPos);
	if(CheckSelItemExt(_T(".ntfp")) || CheckSelItemExt(_T(".plt")) || CheckSelItemExt(_T(".nbfp")))
		::OnFsiLoadpal((CT2TileView*)((CT2ChildFrm*)m_pChild)->m_pTileView, nPos, nCount);
	else
	if(CheckSelItemExt(_T(".nscr")) || CheckSelItemExt(_T(".nbfs")))
		::OnFsiLoadmap((CT2TileView*)((CT2ChildFrm*)m_pChild)->m_pTileView, nPos);
}


void CSubFileInfo::OnUpdateFsiLoaddata(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString text=_T("?");
	if(CheckSelItemExt(_T(".ncgr")) || CheckSelItemExt(_T(".char")) || CheckSelItemExt(_T(".ntft")) || CheckSelItemExt(_T(".nbfc")))
		text.LoadString(IDS_LOADTITLE);
	else
	if(CheckSelItemExt(_T(".nclr")) || CheckSelItemExt(_T(".ntfp")) || CheckSelItemExt(_T(".plt")) || CheckSelItemExt(_T(".nbfp")))
		text.LoadString(IDS_LOADPAL);
	else
	if(CheckSelItemExt(_T(".nscr")) || CheckSelItemExt(_T(".nbfs")))
		text.LoadString(IDS_LOADMAP);
	else
		pCmdUI->Enable(FALSE);
	pCmdUI->SetText(text);
}

BOOL CSubFileInfo::CheckSelItemExt(CString ext)
{
	int nSel = GetCurSel();//m_NdsFSI.GetNextSelectedItem(POS);
	if(nSel==-1) return FALSE;

	CString name = m_FileList.GetItemText(nSel, 0);
	CString strext = PathFindExtension(name);
	
	return lstrcmpi(strext, ext)==0;
}
