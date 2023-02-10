// T2CheatsSearch.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\CT2CheatsSearch.h"
#include "CT2Doc.h"
#include "CT2InputDlg.h"

// CT2CheatsSearch 对话框

IMPLEMENT_DYNAMIC(CT2CheatsSearch, CDialog)
CT2CheatsSearch::CT2CheatsSearch(CWnd* pParent /*=NULL*/)
	: CDialog(CT2CheatsSearch::IDD, pParent)
	, m_strData(_T(""))
	, m_nDataType(0)
	, m_nDec(0)
	, m_pDoc(NULL)
	, m_pOldMemory(NULL)
{
}

CT2CheatsSearch::~CT2CheatsSearch()
{
	if(m_pOldMemory) delete[] m_pOldMemory;
}

void CT2CheatsSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CHEATS_SEARCHDATA, m_strData);
	DDX_Radio(pDX, IDC_CHEATS_DATATYPE8, m_nDataType);
	DDX_Radio(pDX, IDC_CHEATS_DEC, m_nDec);
	DDX_Control(pDX, IDC_CHEATS_LIST, m_wndCheatsList);
	DDX_Control(pDX, IDC_CHEATS_LOCKLIST, m_wndLockList);
}


BEGIN_MESSAGE_MAP(CT2CheatsSearch, CDialog)
	ON_BN_CLICKED(IDC_CHEATS_SEARCH2, OnBnClickedCheatsSearch2)
	ON_BN_CLICKED(IDC_CHEATS_SEARCH, OnBnClickedCheatsSearch)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_CHEATS_LIST, OnLvnItemActivateCheatsList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CHEATS_LOCKLIST, OnLvnEndlabeleditCheatsLocklist)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_CHEATS_LOCKLIST, OnLvnItemActivateCheatsLocklist)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_CHEATS_LOCKLIST, OnLvnGetdispinfoCheatsLocklist)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_CHEATS_LOCKLIST, OnLvnBeginlabeleditCheatsLocklist)
	ON_NOTIFY(NM_RCLICK, IDC_CHEATS_LOCKLIST, OnNMRclickCheatsLocklist)
	ON_BN_CLICKED(IDC_CHEATS_SAVE, OnBnClickedCheatsSave)
	ON_NOTIFY(LVN_KEYDOWN, IDC_CHEATS_LOCKLIST, OnLvnKeydownCheatsList)
	ON_BN_CLICKED(IDC_CHEATS_DEL, DeleteLockItem)
	ON_BN_CLICKED(IDC_CHEATS_ADD, AddLockItem)
END_MESSAGE_MAP()


// CT2CheatsSearch 消息处理程序

void CT2CheatsSearch::OnBnClickedCheatsSearch2()
{
	m_CheatsList.RemoveAll();
	m_wndCheatsList.DeleteAllItems();
	OnUpdateDataType();
}

static BOOL searcheq(UINT v1, UINT v2)
{
	return v1==v2;
}
static BOOL searchne(UINT v1, UINT v2)
{
	return v1!=v2;
}
static BOOL searchlow(UINT v1, UINT v2)
{
	return v1<v2;
}
static BOOL searchhi(UINT v1, UINT v2)
{
	return v1>v2;
}
static BOOL searchle(UINT v1, UINT v2)
{
	return v1<=v2;
}
static BOOL searchge(UINT v1, UINT v2)
{
	return v1>=v2;
}

void CT2CheatsSearch::OnBnClickedCheatsSearch()
{
	UpdateData();

	UINT nData, nSize;
	switch(m_nDataType)
	{
	default:
		nSize = 1;
		break;
	case 1:
		nSize = 2;
		break;
	case 2:
		nSize = 4;
		break;
	}

	static BOOL (*cheatSearchFunc[])(UINT,UINT) =
	{
		searcheq,
		searchne,
		searchle,
		searchge,
		searchlow,
		searchhi,
	};

	CCrystalMemory *pMem = &((CT2Doc*)m_pDoc)->m_Memory;

	/*if(!m_pOldMemory)
	{
		m_pOldMemory = new BYTE[pMem->m_nSize];
		memcpy(m_pOldMemory, pMem->m_pMemory, pMem->m_nSize);
	}*/

	pMem->ReLoadMemory();

	int fn = -1;
	if(swscanf(m_strData.GetBuffer(), m_nDec==0?_T("%d"):_T("%x"), &nData))
	{
		if(m_CheatsList.IsEmpty())
		{	// First
			//memcpy(m_pOldMemory, pMem->m_pMemory, pMem->m_nSize);
			fn = 0;
			for(UINT i=0; i<pMem->m_nSize; i+=nSize)
			{
				UINT v1;
				switch(m_nDataType)
				{
				default:
					v1 = *(pMem->m_pMemory+i);
					break;
				case 1:
					v1 = *(WORD*)(pMem->m_pMemory+i);
					break;
				case 2:
					v1 = *(UINT*)(pMem->m_pMemory+i);
					break;
				}
				if(cheatSearchFunc[fn](v1, nData))
				{
					Cheats nNew;
					nNew.nOffset = i;
					nNew.nData = nData;
					m_CheatsList.AddTail(nNew);
				}
			}
		}else
		{	// Next
			fn = 0;
			POSITION pos = m_CheatsList.GetHeadPosition();
			while(pos)
			{
				POSITION old = pos;
				Cheats &C= m_CheatsList.GetNext(pos);

				UINT v1;
				switch(m_nDataType)
				{
				default:
					v1 = *(pMem->m_pMemory+C.nOffset);
					break;
				case 1:
					v1 = *(WORD*)(pMem->m_pMemory+C.nOffset);
					break;
				case 2:
					v1 = *(UINT*)(pMem->m_pMemory+C.nOffset);
					break;
				}
				if(!cheatSearchFunc[fn](v1, nData))
				{
					m_CheatsList.RemoveAt(old);
				}else
					C.nData = nData;
			}
		}

	}else
	if(m_strData==_T("="))
	{
		if(m_CheatsList.IsEmpty())
		{
			for(UINT i=0; i<pMem->m_nSize; i+=nSize)
			{
				UINT v1;
				switch(m_nDataType)
				{
				default:
					v1 = *(pMem->m_pMemory+i);
					break;
				case 1:
					v1 = *(WORD*)(pMem->m_pMemory+i);
					break;
				case 2:
					v1 = *(UINT*)(pMem->m_pMemory+i);
					break;
				}
				Cheats nNew;
				nNew.nOffset = i;
				nNew.nData = v1;
				m_CheatsList.AddTail(nNew);
			}
		}else
		{
			POSITION pos = m_CheatsList.GetHeadPosition();
			// fn = 0;
			while(pos)
			{
				POSITION old = pos;
				Cheats &C = m_CheatsList.GetNext(pos);

				UINT v1;
				switch(m_nDataType)
				{
				default:
					v1 = *(pMem->m_pMemory+C.nOffset);
					break;
				case 1:
					v1 = *(WORD*)(pMem->m_pMemory+C.nOffset);
					break;
				case 2:
					v1 = *(UINT*)(pMem->m_pMemory+C.nOffset);
					break;
				}
				if(!cheatSearchFunc[0](v1, C.nData))
				{
					m_CheatsList.RemoveAt(old);
				}else
					C.nData = v1;
			}
		}
	}else
	if(!m_CheatsList.IsEmpty())
	{
		if(m_strData==_T("!"))
		{
			fn = 1;
		}else
		if(m_strData==_T("<="))
		{
			fn = 2;
		}else
		if(m_strData==_T(">="))
		{
			fn = 3;
		}else
		if(m_strData==_T("<"))
		{
			fn = 4;
		}else
		if(m_strData==_T(">"))
		{
			fn = 5;
		}
		if(fn!=-1)
		{
			POSITION pos = m_CheatsList.GetHeadPosition();
			while(pos)
			{
				POSITION old = pos;
				Cheats &C= m_CheatsList.GetNext(pos);

				UINT v1;
				switch(m_nDataType)
				{
				default:
					v1 = *(pMem->m_pMemory+C.nOffset);
					break;
				case 1:
					v1 = *(WORD*)(pMem->m_pMemory+C.nOffset);
					break;
				case 2:
					v1 = *(UINT*)(pMem->m_pMemory+C.nOffset);
					break;
				}
				if(!cheatSearchFunc[fn](v1, C.nData))
				{
					m_CheatsList.RemoveAt(old);
				}else
					C.nData = v1;
			}
		}
	}

	m_wndCheatsList.DeleteAllItems();
	UINT nCount = (UINT)m_CheatsList.GetCount();
	if(nCount>64) nCount = 64;
	CString strItem;
	for(UINT i=0; i<nCount; i++)
	{
		Cheats &Cheats = m_CheatsList.GetAt(m_CheatsList.FindIndex(i));
		strItem.Format(_T("02%06X"), Cheats.nOffset);
		m_wndCheatsList.InsertItem(i, strItem);

		strItem.Format(_T("%d"), Cheats.nData);
		m_wndCheatsList.SetItemText(i, 1, strItem);

		strItem.Format(_T("%x"), Cheats.nData);
		m_wndCheatsList.SetItemText(i, 2, strItem);

		/*UINT nOld;
		switch(m_nDataType)
		{
		default:
			nOld = *(m_pOldMemory+Cheats.nOffset);
			break;
		case 1:
			nOld = *(WORD*)(m_pOldMemory+Cheats.nOffset);
			break;
		case 2:
			nOld = *(UINT*)(m_pOldMemory+Cheats.nOffset);
			break;
		}
		strItem.Format(_T("%x"), nOld);
		m_wndCheatsList.SetItemText(i, 3, strItem);*/
	}
	/*BYTE *pTmp = m_pOldMemory;
	m_pOldMemory = pMem->m_pMemory;
	pMem->m_pMemory = pTmp;*/

	m_strData.ReleaseBuffer();

	OnUpdateDataType();
}

BOOL CT2CheatsSearch::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndCheatsList.ModifyStyle(LVS_SORTASCENDING|LVS_SORTDESCENDING|
		LVS_ICON|LVS_SMALLICON|LVS_LIST|LVS_EDITLABELS|LVS_NOSORTHEADER,
		LVS_REPORT|LVS_SHOWSELALWAYS/*|LVS_OWNERDRAWFIXED*/);
	m_wndCheatsList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP);

	CString header, text;
	header.LoadString(IDS_CHEATSHEADER);
	BYTE nFormat[]={LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT};
	BYTE nWidth[]={64, 64, 64};
	for(int i=0; i<(sizeof(nWidth)/sizeof(BYTE)); i++)
	{
		AfxExtractSubString(text, header, i);
		m_wndCheatsList.InsertColumn(i, text, nFormat[i], nWidth[i]);
	}

	m_wndLockList.ModifyStyle(LVS_SORTASCENDING|LVS_SORTDESCENDING|
		LVS_ICON|LVS_SMALLICON|LVS_LIST|LVS_NOSORTHEADER,
		LVS_EDITLABELS|LVS_REPORT|LVS_SHOWSELALWAYS/*|LVS_OWNERDRAWFIXED*/);
	m_wndLockList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES);
	m_wndLockList.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 64);
	m_wndLockList.InsertColumn(1, _T("Offset:Val"), LVCFMT_LEFT, 64);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CT2CheatsSearch::OnLvnItemActivateCheatsList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	Cheats &C = m_CheatsList.GetAt(m_CheatsList.FindIndex(pNMIA->iItem));
	Locks Lock;
	Lock.nOffset = C.nOffset;
	Lock.nData = C.nData;
	switch(m_nDataType)
	{
	default:
		Lock.nSize = 1;break;
	case 1:
		Lock.nSize = 2;break;
	case 2:
		Lock.nSize = 4;break;
	}
	m_LockList.AddTail(Lock);
	CString strItem;

	int i = m_wndLockList.GetItemCount();
	strItem.Format(_T("Cheats%d"), i);
	m_wndLockList.InsertItem(i, strItem);
	m_wndLockList.SetCheck(i);

	SetTimer(0, 250, NULL);

	*pResult = 0;
}

void CT2CheatsSearch::OnLvnEndlabeleditCheatsLocklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	*pResult = 0;
	if(pDispInfo->item.pszText && *pDispInfo->item.pszText)
	{
		/*
		// 同名检查
		CString t;
		for(int i=0; i<m_wndLockList.GetItemCount(); i++)
		{
			t = m_wndLockList.GetItemText(i,0);
			if(t.CompareNoCase(pDispInfo->item.pszText)==0)
				return;
		}*/
		m_wndLockList.SetItemText(pDispInfo->item.iItem, 0, pDispInfo->item.pszText);
	}
}

void CT2CheatsSearch::OnLvnItemActivateCheatsLocklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	//if(!(GetKeyState(VK_CONTROL)&0x80))
	{
		Locks &l = m_LockList.GetAt(m_LockList.FindIndex(pNMIA->iItem));
		CT2InputDlg id(IDS_CHANGECHEATS,this);
		id.m_strVal.Format(_T("%d"), l.nData);
		if(id.DoModal()==IDOK)
		{
			UINT nVal;
			BOOL bHex = id.m_strVal.Left(2).CompareNoCase(_T("0x"))==0;
			if(bHex)
				id.m_strVal.Delete(0, 2);
			if(swscanf(id.m_strVal, bHex?_T("%x"):_T("%d"), &nVal))
			{
				l.nData = nVal;
				/*id.m_strVal.Format(_T("%X"), nVal);
				l.nSize = id.m_strVal.GetLength();
				l.nSize = l.nSize / 2 + (l.nSize&1);*/
				Invalidate(FALSE);
			}
		}
	}
	*pResult = 0;
}

void CT2CheatsSearch::OnTimer(UINT_PTR nIDEvent)
{
	HANDLE hProcess = ((CT2Doc*)m_pDoc)->m_Memory.m_hProcess;
	UINT nOffset = ((CT2Doc*)m_pDoc)->m_Memory.m_nOffset;
	if(m_wndLockList.GetSafeHwnd())
	for(int i=0; i<m_wndLockList.GetItemCount(); i++)
	{
		if( m_wndLockList.GetCheck(i) )
		{
			Locks &Lock = m_LockList.GetAt(m_LockList.FindIndex(i));

			if(!WriteProcessMemory(hProcess, (LPVOID)(UINT_PTR)(Lock.nOffset+nOffset),
					&Lock.nData, Lock.nSize, NULL))
			{
				// m_LockList.RemoveAll();
				// m_wndLockList.DeleteAllItems();
				for(int i=0; i<m_wndLockList.GetItemCount(); i++)
					m_wndLockList.SetCheck(i, 0);
				KillTimer(0);
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CT2CheatsSearch::OnUpdateDataType()
{
	BOOL bEnable = m_CheatsList.IsEmpty();
	for(UINT id=IDC_CHEATS_DATATYPE8; id<=IDC_CHEATS_DATATYPE32; id++)
		GetDlgItem(id)->EnableWindow(bEnable);
}

void CT2CheatsSearch::OnDestroy()
{
	KillTimer(0);

	CDialog::OnDestroy();
}

void CT2CheatsSearch::OnLvnGetdispinfoCheatsLocklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	Locks &l = m_LockList.GetAt(m_LockList.FindIndex(pDispInfo->item.iItem));
	wsprintf(pDispInfo->item.pszText, _T("%06X:%02X"), l.nOffset, l.nData);

	*pResult = 0;
}

void CT2CheatsSearch::OnLvnBeginlabeleditCheatsLocklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	*pResult = 0;
}

void CT2CheatsSearch::OnNMRclickCheatsLocklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	AddLockItem();

	*pResult = 0;
}

void CT2CheatsSearch::OnBnClickedCheatsSave()
{
	if(m_LockList.IsEmpty()) return;

	CFileDialog fd(FALSE);
	if(fd.DoModal()!=IDOK) return;

	CString str, str2 = _T(".cht"), name=fd.GetPathName(), setion=_T("GameInfo");
	if(name.Right(4).CompareNoCase(str2)!=0)
		name.Append(str2);

	TCHAR pBuffer[102400];
	for(int i=0; i<m_wndLockList.GetItemCount(); i++)
	{
		str = m_wndLockList.GetItemText(i, 0);
		Locks &l = m_LockList.GetAt(m_LockList.FindIndex(i));
		str2.Format(_T("%X"), l.nOffset);
		for(UINT i2=0; i2<l.nSize; i2++)
		{
			str2.AppendFormat(_T(",%02X"), (l.nData>>(i2*8))&0xFF);
		}

		::GetPrivateProfileString(str, _T("ON"), NULL, pBuffer, 102400, name);
		if(*pBuffer)
		{
			str2.Insert(0, _T(";"));
			str2.Insert(0, pBuffer);
		}else
			str2.Append(_T("\r\n"));

		::WritePrivateProfileString(str, _T("ON"), str2, name);
	}

	CCrystalMemory *pMem = &((CT2Doc*)m_pDoc)->m_Memory;
	UINT nOff = pMem->m_nOffset, nSize = pMem->m_nSize;

	if(pMem->FindDsRom())
	{
		NDSHEADER *nds = (NDSHEADER*)pMem->m_pMemory;
		LPTSTR t = str.GetBuffer(12+3+4+1);
		OemToCharBuff(nds->GameTitle, t, 12);
		lstrcpy(t+lstrlen(t), _T(" - "));
		t+=lstrlen(t);
		OemToCharBuff(nds->GameCode, t, 4);
		*(t+4)=0;
		str.ReleaseBuffer();
	}
	pMem->LoadMemory(nOff, nSize);
	if(str.IsEmpty()) 	str = _T("Unknown Nds Rom");

	::WritePrivateProfileString(setion, _T("Name"), str, name);
	::WritePrivateProfileString(setion, _T("System"), _T("NDS"), name);
	str.Append(_T("(Powered by CrystalTile)"));
	::WritePrivateProfileString(setion, _T("Text"), str, name);
}

void CT2CheatsSearch::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// CDialog::OnOK();
}

void CT2CheatsSearch::OnLvnKeydownCheatsList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if(pLVKeyDow->wVKey==VK_DELETE)
	{
		DeleteLockItem();
	}
	*pResult = 0;
}

void CT2CheatsSearch::DeleteLockItem()
{
	POSITION pos = m_wndLockList.GetFirstSelectedItemPosition();
	if(pos)
	{
		int i = m_wndLockList.GetNextSelectedItem(pos);
		m_wndLockList.DeleteItem(i);
		m_LockList.RemoveAt(m_LockList.FindIndex(i));
	}
}

void CT2CheatsSearch::AddLockItem()
{
	CT2InputDlg id(IDS_ADDCHEATS, this);
	if(id.DoModal()==IDOK)
	{
		int i = id.m_strVal.Find(_T(":"));
		if(i!=-1)
		{
			Locks l;
			l.nSize = id.m_strVal.GetLength()-i-1;
			if( swscanf(id.m_strVal.Left(i), _T("%x"), &l.nOffset) &&
				l.nOffset < 0x00400000 &&
				swscanf(id.m_strVal.Right(l.nSize), _T("%x"), &l.nData) )
			{
				l.nSize = l.nSize / 2 + (l.nSize&1);
				m_LockList.AddTail(l);
				i = m_wndLockList.GetItemCount();
				id.m_strVal.Format(_T("Cheats%d"), i);
				m_wndLockList.InsertItem(i, id.m_strVal);

				SetTimer(0, 250, NULL);
			}
		}
	}
}
