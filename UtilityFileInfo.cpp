// UtilityFileInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\UtilityFileInfo.h"


// CUtilityFileInfo 对话框

IMPLEMENT_DYNAMIC(CUtilityFileInfo, CSubFileInfo)
CUtilityFileInfo::CUtilityFileInfo(CWnd* pParent /*=NULL*/)
	: CSubFileInfo(pParent)
{
	m_nSubOffset=2;
}

CUtilityFileInfo::~CUtilityFileInfo()
{
}

void CUtilityFileInfo::DoDataExchange(CDataExchange* pDX)
{
	CSubFileInfo::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUtilityFileInfo, CSubFileInfo)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_SFI_FILELIST, OnLvnGetdispinfoSfiFilelist)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_SFI_FILELIST, OnLvnItemActivateSfiFilelist)
	ON_COMMAND(ID_SFI_EXPORT, OnSfiExport)
	ON_COMMAND(ID_SFI_IMPORT, OnSfiImport)
END_MESSAGE_MAP()


// CUtilityFileInfo 消息处理程序

void CUtilityFileInfo::Update()
{
	m_FileList.SetRedraw(FALSE);
	m_FileList.DeleteAllItems();

	BYTE *pFile = (((CT2ChildFrm*)m_pChild)->m_pTileView->GetDocument()->m_pRom+m_nOffset);
	BYTE *pEnd = pFile + m_nSize;

	UTITLITY_HEADER* h = (UTITLITY_HEADER*)pFile;
	CString path;// = _T("/");
	SFIExtractDirectory(path, 0, pFile, h->nFntOffset, h->nFatOffset, m_FileList);

	int i = m_FileList.GetItemCount();
	path.Format(m_strWTitle, m_strTitle, i, i);
	SetWindowText(path);
	m_FileList.SetRedraw();
}

void CUtilityFileInfo::SetHeader()
{
	CHeaderCtrl& hc = *m_FileList.GetHeaderCtrl();
	while(hc.GetItemCount()>0)
		m_FileList.DeleteColumn(0);

	CString header, text;
	header.LoadString(IDS_SFI_UTILITY_HEADER);
	BYTE nFormat[]={LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_RIGHT};
	BYTE nWidth[]={144, 48, 64, 64, 64};
	for(int i=0; i<(sizeof(nWidth)/sizeof(BYTE)); i++)
	{
		if(m_nOffset==0 && i==3) continue;

		AfxExtractSubString(text, header, i);
		m_FileList.InsertColumn(i, text, nFormat[i], nWidth[i]);
	}
}

void CUtilityFileInfo::OnLvnGetdispinfoSfiFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if(pDispInfo->item.mask & LVIF_TEXT)
	{
		BYTE *pFile = (((CT2ChildFrm*)m_pChild)->m_pTileView->GetDocument()->m_pRom+m_nOffset);
		UTITLITY_HEADER* h = (UTITLITY_HEADER*)pFile;

		WORD FileID = GETFILEID(pDispInfo->item.lParam);

		NDSFILEREC *pFileRec;
		CString text;

		pFileRec = (NDSFILEREC*)(pFile+h->nFatOffset+(FileID<<3));
		switch(pDispInfo->item.iSubItem)
		{
		case 1://ID
			text.Format(_T("%04d"), FileID);
			break;
		case 2://Offset
			text.Format(_T("%08X"), pFileRec->top+m_nOffset);
			break;
		case 3://Offset/Size
			if(m_nOffset)
			{
				text.Format(_T("%08X"), pFileRec->top);
				break;
			}
		case 4://size
			text.Format(_T("%d"), pFileRec->bottom-pFileRec->top);
			break;
		}
		if(text.GetLength()>pDispInfo->item.cchTextMax)
		{
			text = text.Left(pDispInfo->item.cchTextMax-4);
			text.Append(_T("..."));
		}
		::lstrcpy(pDispInfo->item.pszText, text);
	}

	*pResult = 0;
}

void CUtilityFileInfo::OnLvnItemActivateSfiFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	GoToFile(pNMIA->iItem, 2);

	*pResult = 0;
}

void CUtilityFileInfo::OnOK()
{
	GoToFile(-1, 2);
}

void CUtilityFileInfo::OnSfiExport()
{
	OnExport(1);
}

void CUtilityFileInfo::OnSfiImport()
{
	//OnImport(1);

	int nSel = GetCurSel();
	m_FileList.GetItemText(nSel, 0, g_szBuffer, _MAX_PATH);
	lstrcpy(g_szBuffer, PathFindFileName(g_szBuffer));

	static CFileDialog fd(TRUE);
	fd.m_pOFN->lpstrFile = g_szBuffer;
	fd.m_pOFN->nMaxFile = _MAX_PATH;
	if(fd.DoModal()!=IDOK) return;

	CT2View *pView = ((CT2ChildFrm*)m_pChild)->m_pTileView;
	CT2Doc *pDoc = pView->GetDocument();
	BYTE *pFile = pDoc->m_pRom+m_nOffset;

	CFile imfile;
	if(!imfile.Open(g_szBuffer, CFile::modeRead|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	UTITLITY_HEADER &header = *(UTITLITY_HEADER*)pFile;

	UINT nSize = (UINT)imfile.GetLength();

	CString id=m_FileList.GetItemText(nSel, 1);
	UINT nID = StrToIntEX(id, FALSE);
	NDSFILEREC *rec = (NDSFILEREC*)(pFile+header.nFatOffset+(nID<<3));

	UINT nOffset = rec->top;
	UINT recSize = rec->bottom-rec->top;
	if(m_nOffset || nSize==recSize)
	{
		if(nSize>recSize)
		{
			Hint(IDS_SFI_IMPORTERR);
			return;
		}else
		{
			pView->DoUndo(pFile+nOffset, nOffset+m_nOffset, recSize);
			pView->DoUndo((BYTE*)&rec->bottom, (UINT)((BYTE*)&rec->bottom-pFile)+m_nOffset, 4);
			imfile.Read(pFile+nOffset, nSize);
			UINT zlen = recSize-nSize;
			if(zlen)
				memset(pFile+nOffset+nSize, 0, zlen);
			rec->bottom = rec->top + nSize;
			return;
		}
	}

	CString name=pDoc->GetPathName();
	CFile file;
	if(!file.Open(name, CFile::modeReadWrite|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	int iPos = nSize-recSize;
	char zlen=4-(nSize&3);
	if(zlen!=4)
		iPos+=zlen;
	for(int i=nSel+1; i<m_FileList.GetItemCount(); i++)
	{
		id = m_FileList.GetItemText(i, 1);
		nID = StrToIntEX(id, FALSE);
		NDSFILEREC *rec2 = (NDSFILEREC*)(pFile+header.nFatOffset+(nID<<3));
		rec2->top += iPos;
		rec2->bottom += iPos;
	}

	UINT nOff = nOffset+recSize;

	rec->bottom = rec->top + nSize;
	file.Write(pFile, nOffset);
	BYTE *pBuffer = new BYTE[nSize];
	imfile.Read(pBuffer, nSize);
	file.Write(pBuffer, nSize);
	delete[] pBuffer;

	TCHAR zero[3]={0};
	if(zlen!=4)
		file.Write(zero, zlen);
	file.Write(pFile+nOff, m_nSize-nOff);

	pDoc->DeleteContents();
	file.SetLength(file.GetPosition());
	file.Close();

	pDoc->OpenRom(name);

	Update();
	m_FileList.EnsureVisible(nSel, TRUE);
}
