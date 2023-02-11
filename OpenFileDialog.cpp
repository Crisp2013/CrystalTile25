// OpenFileDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include "OpenFileDialog.h"
#include ".\openfiledialog.h"


// COpenFileDialog 对话框

IMPLEMENT_DYNAMIC(COpenFileDialog, CFileDialog)
COpenFileDialog::COpenFileDialog(BOOL bOpenFileDialog,
		LPCTSTR lpszDefExt,
		LPCTSTR lpszFileName,
		DWORD dwFlags,
		LPCTSTR lpszFilter,
		CWnd* pParentWnd,
		DWORD dwSize)
:		CFileDialog(bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		lpszDefExt,
		lpszFileName,
		dwFlags,
		lpszFilter,
		pParentWnd,
		dwSize)
, m_bMapFile(FALSE)
, m_nMemSize(0)
{
	m_pOFN->lpTemplateName = MAKEINTRESOURCE(COpenFileDialog::IDD);
	MEMORYSTATUSEX ms;
	GlobalMemoryStatusEx(&ms);
	m_nMemSize = ms.ullAvailPhys;
	m_nMemSize = (m_nMemSize*90)/100;
}

COpenFileDialog::~COpenFileDialog()
{
}

void COpenFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_MAPFILE, m_bMapFile);
	DDX_Control(pDX, IDC_MFILELIST, m_MFileList);
}


BEGIN_MESSAGE_MAP(COpenFileDialog, CFileDialog)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_LBN_DBLCLK(IDC_MFILELIST, OnLbnDblclkMfilelist)
	ON_BN_CLICKED(IDC_MFILE_OPEN, OnBnClickedMfileOpen)
END_MESSAGE_MAP()


// COpenFileDialog 消息处理程序

BOOL COpenFileDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LPOFNOTIFY pofn = (LPOFNOTIFY)lParam;
	switch(pofn->hdr.code)
	{
	case CDN_SELCHANGE:
		{
			CommDlg_OpenSave_GetFilePath(::GetParent(m_hWnd),
				g_szBuffer, _MAX_PATH);
			CFile file;
			if(!file.Open(g_szBuffer, CFile::modeRead|CFile::typeBinary))
				break;
			UINT nSize = (UINT)file.GetLength();
			file.Close();
			CButton* pBtn = ((CButton*)GetDlgItem(IDC_MAPFILE));
			pBtn->SetCheck((nSize>(0x32*1024*1024)) || (nSize>m_nMemSize)?1:0);

			pBtn->EnableWindow(nSize<=m_nMemSize);

			int i = lstrlen(g_szBuffer)-5;
			TCHAR szNSCR[] = _T(".nscr");
			TCHAR szNCGR[] = _T(".ncgr");
			TCHAR szNCLR[] = _T(".nclr");
			LPTSTR lpExt = g_szBuffer+i;

			pBtn = ((CButton*)GetDlgItem(IDC_MFILE_OPEN));
			g_nMFileSize = 0;
			m_bMFile = FALSE;
			CString F;
			if( lstrcmpi(lpExt, _T(".nscr"))==0 )
			{	// MAP FILE
				m_MFileList.ResetContent();
				g_szBuffer[i] = 0;
				F = g_szBuffer;
				CString strFile = F+szNSCR;
				if(!file.Open(strFile, CFile::modeRead|CFile::typeBinary))
					break;
				m_MFileList.AddString(strFile);
				g_nMFileSize += (UINT)file.GetLength();
				file.Close();

				strFile = F+szNCGR;
				if(!file.Open(strFile, CFile::modeRead|CFile::typeBinary))
					break;
				m_MFileList.AddString(strFile);
				g_nMFileSize += (UINT)file.GetLength();
				file.Close();

				strFile = F+szNCLR;
				if(!file.Open(strFile, CFile::modeRead|CFile::typeBinary))
					break;
				m_MFileList.AddString(strFile);
				g_nMFileSize += (UINT)file.GetLength();
				file.Close();

				pBtn->EnableWindow(g_nMFileSize<=m_nMemSize && m_MFileList.GetCount()>1);
				g_nRomType = CT2_RT_NSCR;
			}else
			{
				nSize = 0;
				for(int i=0; i<m_MFileList.GetCount(); i++)
				{
					m_MFileList.GetText(i, F);
					if(!file.Open(F, CFile::modeRead|CFile::typeBinary))
						break;
					nSize += (UINT)file.GetLength();
					file.Close();
				}
				pBtn->EnableWindow(nSize<=m_nMemSize && m_MFileList.GetCount()>1);
				//g_nRomType = CT2_RT_NORMAL;
			}
		}break;
	case CDN_FILEOK:
		{
			UpdateData();
			if(!m_bMFile)
			{
				g_strMFile = GetPathName();
				g_nRomType = CT2_RT_NORMAL;
			}
		}break;
	}

	return CFileDialog::OnNotify(wParam, lParam, pResult);
}

BOOL COpenFileDialog::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	g_nRomType = CT2_RT_NORMAL;


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void COpenFileDialog::OnSize(UINT nType, int cx, int cy)
{
	CFileDialog::OnSize(nType, cx, cy);

	CRect rc;
	m_MFileList.GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);
	m_MFileList.MoveWindow(rc.left, rc.top, cx, rc.Height());
}

void COpenFileDialog::OnDropFiles(HDROP hDropInfo)
{
	UINT nFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	for(UINT i=0; i<nFileCount && i<3; i++)
	{
		::DragQueryFile(hDropInfo, i, g_szBuffer, _MAX_PATH);
		int c = m_MFileList.FindString(0, g_szBuffer);
		if(c!=-1) m_MFileList.DeleteString(c);
		c = m_MFileList.GetCount();
		if(c>=3)
		{
			m_MFileList.DeleteString(0);
			c--;
		}
		m_MFileList.InsertString(c, g_szBuffer);
	}
	GetParent()->GetDlgItem(IDOK)->EnableWindow(CheckSize());

	CFileDialog::OnDropFiles(hDropInfo);
}

void COpenFileDialog::OnLbnDblclkMfilelist()
{
	int nSel = m_MFileList.GetCurSel();
	if(nSel!=-1)
	{
		m_MFileList.DeleteString(nSel);
		GetParent()->GetDlgItem(IDOK)->EnableWindow(CheckSize());
	}
}

BOOL COpenFileDialog::CheckSize()
{
	CString File;
	int c = m_MFileList.GetCount();

	ULONGLONG nSize = 0;
	for(int i=0; i<c && c>1; i++)
	{
		m_MFileList.GetText(i, File);
		CFile file;
		if(!file.Open(File, CFile::modeRead|CFile::typeBinary))
			break;
		nSize += file.GetLength();
	}

	return nSize<(ULONGLONG)m_nMemSize;
}

void COpenFileDialog::OnBnClickedMfileOpen()
{
	UpdateData();
	CString n;
	int c = m_MFileList.GetCount();
	g_strMFile.Empty();
	if(c>1)
	{
		for(int i=0; i<c; i++)
		{
			m_MFileList.GetText(i, n);
			g_strMFile.AppendFormat(_T("%s\n"), n);
		}
		m_bMFile = TRUE;
	}
	GetParent()->SendMessage(WM_COMMAND, IDOK);
}
