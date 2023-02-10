// FileCompDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include "FileCompDlg.h"
#include ".\filecompdlg.h"


// CFileCompDlg 对话框

IMPLEMENT_DYNAMIC(CFileCompDlg, CDialog)
CFileCompDlg::CFileCompDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileCompDlg::IDD, pParent)
	, m_strFileOne(_T(""))
	, m_strFileTwo(_T(""))
{
}

CFileCompDlg::~CFileCompDlg()
{
}

void CFileCompDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_COMP_FILEONE, m_strFileOne);
	DDX_Text(pDX, IDC_COMP_FILETWO, m_strFileTwo);
	DDX_Control(pDX, IDC_COMPLIST, m_CompList);
}


BEGIN_MESSAGE_MAP(CFileCompDlg, CDialog)
	ON_COMMAND_RANGE(IDC_COMP_FILEONEBTN, IDC_COMP_FILETWOBTN, OnBnClickedSelFile)
	ON_BN_CLICKED(IDC_COMP_COMFILE, OnBnClickedCompComfile)
END_MESSAGE_MAP()


// CFileCompDlg 消息处理程序

void CFileCompDlg::OnBnClickedSelFile(UINT nID)
{
	static CFileDialog fd(TRUE);
	CString filter;
	filter.LoadString(IDS_ALLFILE);
	filter.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter = filter;
	if(fd.DoModal()!=IDOK) return;

	nID==IDC_COMP_FILEONEBTN?
		m_strFileOne = fd.GetPathName():
		m_strFileTwo = fd.GetPathName();
	UpdateData(FALSE);
}

struct COMPDATA {
	CT2StatusBar *pPro;
	CListCtrl *pCompList;
	BYTE *pFile1;
	BYTE *pFile2;
	UINT nSize;
};

UINT CompData(LPVOID lpParam)
{
	COMPDATA &cd = *(COMPDATA*)lpParam;
	CT2StatusBar &sb = *cd.pPro;
	BYTE *pFile1 = cd.pFile1;
	BYTE *pFile2 = cd.pFile2;
	CListCtrl &m_CompList = *cd.pCompList;
	UINT nFileSize = cd.nSize;

	//StartComp
	UINT nOffset, nSize, nItem=0;
	TCHAR Val1[22];
	TCHAR Val2[22];
	TCHAR Offset[9];
	TCHAR Size[10];
	UINT nPos=0;
	for(; nPos<nFileSize; nPos++, pFile1++, pFile2++)
	{
Comp:
		if(!(*pFile1^*pFile2))
		{
			sb.SetProgressPos(nPos*100/nFileSize);
			continue;
		}
		nSize = 0;
		LPTSTR pVal1=Val1;
		LPTSTR pVal2=Val2;
		for(UINT nCompPos=nPos; nCompPos<nFileSize; nCompPos++, pFile1++, pFile2++)
		{
			if(*pFile1^*pFile2)
			{
				if(!nSize)
				{
					nOffset = nCompPos;	// 起始地址
				}
				if(nSize<10)	// 列出头10个不同的字节
				{
					wsprintf(pVal1, _T("%02X"), *pFile1);
					wsprintf(pVal2, _T("%02X"), *pFile2);
					pVal1+=2;
					pVal2+=2;
				}
				nSize++;
				sb.SetProgressPos(nCompPos*100/nFileSize);
				continue;
			}
			wsprintf(Offset, _T("%08X"), nOffset);
			wsprintf(Size, _T("%d"), nSize);
			m_CompList.InsertItem(nItem, Offset);
			m_CompList.SetItemText(nItem, 1, Size);
			m_CompList.SetItemText(nItem, 2, Val1);
			m_CompList.SetItemText(nItem++, 3, Val2);
			if(nItem>99999) goto End;
			nPos+=nSize;
			goto Comp;
		}
	}
	//
End:
	sb.EndModalLoop(IDCANCEL);
	return 0;
}

void CFileCompDlg::OnBnClickedCompComfile()
{
	if(m_strFileOne.Compare(m_strFileTwo)==0)
	{
		Hint(IDS_FILEXIANGTONG, MB_OK|MB_ICONERROR);
		return;
	}
	CFile file1, file2;
	if(!file1.Open(m_strFileOne, CFile::modeRead|CFile::typeBinary))
	{
		ShowError();
		return;
	}
	if(!file2.Open(m_strFileTwo, CFile::modeRead|CFile::typeBinary))
	{
		ShowError();
		return;
	}
	UINT nFileSize1=(UINT)file1.GetLength(),
		nFileSize2=(UINT)file2.GetLength();

	if(nFileSize1!=nFileSize2)
	{
		if(Hint(IDS_SIZERROR, MB_YESNO|MB_ICONQUESTION)!=IDYES)
			return;
		nFileSize1 = nFileSize1>nFileSize2?nFileSize2:nFileSize1;
	}

	HANDLE hFile1 = ::CreateFileMapping(file1.m_hFile,
		NULL, PAGE_READONLY, 0, nFileSize1, 0);
	if(hFile1==NULL)
	{
		ShowError();
		goto End;
	}
	HANDLE hFile2 = ::CreateFileMapping(file2.m_hFile,
		NULL, PAGE_READONLY, 0, nFileSize1, 0);
	if(hFile2==NULL)
	{
		ShowError();
		goto End;
	}

	BYTE *pFile1 =
		(BYTE*)::MapViewOfFile(hFile1, FILE_MAP_READ, 0, 0, 0);

	if(pFile1==NULL)
	{
		ShowError();
		goto End;
	}

	BYTE *pFile2 =
		(BYTE*)::MapViewOfFile(hFile2, FILE_MAP_READ, 0, 0, 0);

	if(pFile2==NULL)
	{
		ShowError();
		goto End;
	}

	m_CompList.SetRedraw(FALSE);
	m_CompList.DeleteAllItems();

	COMPDATA cd;
	cd.pPro = (CT2StatusBar*) theApp.GetStatusBar();
	cd.pPro->BeginProgress(IDS_COMPING);

	cd.pCompList = &m_CompList;
	cd.pFile1 = pFile1;
	cd.pFile2 = pFile2;
	cd.nSize = nFileSize1;

	CWinThread *pOpenThread = AfxBeginThread(CompData,
		&cd, THREAD_PRIORITY_IDLE, 1024, 0, NULL);
	cd.pPro->RunModalLoop();

	cd.pPro->EndProgress();

End:
	m_CompList.SetRedraw();
	if(pFile1) UnmapViewOfFile(hFile1);
	if(pFile2) UnmapViewOfFile(hFile2);
	if(hFile1) CloseHandle(hFile1);
	if(hFile2) CloseHandle(hFile2);
}

BOOL CFileCompDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_CompList.ModifyStyle(LVS_SORTASCENDING|LVS_SORTDESCENDING|LVS_ICON|LVS_SMALLICON|LVS_LIST|LVS_EDITLABELS,
		LVS_REPORT|LVS_SHOWSELALWAYS);
	m_CompList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	CString header, text;
	header.LoadString(IDS_COMPHEADER);
	BYTE nWidth[]={48, 40, 130, 130};
	for(int i=0; i<4; i++)
	{
		AfxExtractSubString(text, header, i);
		m_CompList.InsertColumn(i, text, LVCFMT_LEFT, nWidth[i]);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
