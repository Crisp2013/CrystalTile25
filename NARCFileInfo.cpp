// NARCFileInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\NARCFileInfo.h"

// CNARCFileInfo 对话框

IMPLEMENT_DYNAMIC(CNARCFileInfo, CSubFileInfo)
CNARCFileInfo::CNARCFileInfo(CWnd* pParent /*=NULL*/)
	: CSubFileInfo(pParent)
{
	m_nSubOffset=2;
}

CNARCFileInfo::~CNARCFileInfo()
{
}

void CNARCFileInfo::DoDataExchange(CDataExchange* pDX)
{
	CSubFileInfo::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNARCFileInfo, CSubFileInfo)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_SFI_FILELIST, OnLvnGetdispinfoSfiFilelist)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_SFI_FILELIST, OnLvnItemActivateSfiFilelist)
END_MESSAGE_MAP()


// CNARCFileInfo 消息处理程序

void CNARCFileInfo::Update()
{
	m_FileList.DeleteAllItems();

	BYTE *pFile = (((CT2ChildFrm*)m_pChild)->m_pTileView->GetDocument()->m_pRom+m_nOffset);
	NINTENDOFILE_HEADER *header = (NINTENDOFILE_HEADER*)pFile;
	NARCFILE_FAT *fat = (NARCFILE_FAT*)(pFile+header->nSize);
	NARCFILE_FNT *fnt = (NARCFILE_FNT*)(pFile+header->nSize+fat->nSize);

	UINT nFat_Offset = (UINT)((BYTE*)&fat->nFat_Offset-pFile);
	UINT nFnt_Offset = (UINT)((BYTE*)&fnt->nFnt_Offset-pFile);

	CString dir;
	SFIExtractDirectory(dir, 0, pFile, nFnt_Offset, nFat_Offset, m_FileList);

	int i = m_FileList.GetItemCount();
	CString title;
	title.Format(m_strWTitle, m_strTitle, i, fat->nFileCount);
	SetWindowText(title);
}

void CNARCFileInfo::OnOK()
{
	GoToFile(-1, 2);
}

BOOL CNARCFileInfo::OnInitDialog()
{
	CSubFileInfo::OnInitDialog();

	// SetHeader();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CNARCFileInfo::SetHeader()
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

void CNARCFileInfo::OnLvnGetdispinfoSfiFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if(pDispInfo->item.mask & LVIF_TEXT)
	{
		BYTE *pFile = (((CT2ChildFrm*)m_pChild)->m_pTileView->GetDocument()->m_pRom+m_nOffset);

		NINTENDOFILE_HEADER *header = (NINTENDOFILE_HEADER*)pFile;
		NARCFILE_FAT *fat = (NARCFILE_FAT*)(pFile+header->nSize);
		NARCFILE_FNT *fnt = (NARCFILE_FNT*)((BYTE*)fat+fat->nSize);

		UINT nBase = header->nSize+fat->nSize+fnt->nSize+sizeof(NARCFILE_FIMG);
		UINT nFatOffset = (UINT)((BYTE*)&fat->nFat_Offset-pFile);
		UINT nFntOffset = (UINT)((BYTE*)&fnt->nFnt_Offset-pFile);

		WORD FileID = GETFILEID(pDispInfo->item.lParam);

		NDSFILEREC *pFileRec;
		CString text;

		pFileRec = (NDSFILEREC*)(pFile+nFatOffset+(FileID<<3));
		switch(pDispInfo->item.iSubItem)
		{
		case 1://ID
			text.Format(_T("%04d"), FileID);
			break;
		case 2://Offset
			text.Format(_T("%08X"), pFileRec->top+nBase+m_nOffset);
			break;
		case 3://Offset/Size
			if(m_nOffset)
			{
				text.Format(_T("%08X"), pFileRec->top+nBase);
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

void CNARCFileInfo::OnLvnItemActivateSfiFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	GoToFile(pNMIA->iItem, 2);

	*pResult = 0;
}
