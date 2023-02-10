// OpenTextDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\TextFileDialog.h"


// CTextFileDialog 对话框

IMPLEMENT_DYNAMIC(CTextFileDialog, CFileDialog)
CTextFileDialog::CTextFileDialog(BOOL bOpenFileDialog,
		UINT nExtList,
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
, m_nCodePage(GetACP())
{
	m_pOFN->lpTemplateName = MAKEINTRESOURCE(CTextFileDialog::IDD);

	if(m_strFilter.LoadString(nExtList))
	{
		m_strFilter.AppendChar(_T('\0'));
		m_pOFN->lpstrFilter = m_strFilter;
	}
}

CTextFileDialog::~CTextFileDialog()
{
}

void CTextFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CODEPAGELIST, m_CPList);
}


BEGIN_MESSAGE_MAP(CTextFileDialog, CFileDialog)
	ON_CBN_SELCHANGE(IDC_CODEPAGELIST, OnCbnSelchangeCodepagelist)
END_MESSAGE_MAP()


// CTextFileDialog 消息处理程序

BOOL CTextFileDialog::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	CString CP;
	//CDC *pDC = GetDC();
	//pDC->SelectObject(GetParent()->GetFont());
	//int cx=0;
	//CSize sz;
	UINT i=0, nSel=-1;
	for(;i<theApp.m_nCPCount; i++)
	{
		CP = GetCodePageDescription(theApp.m_pCodePage[i]);
		if(theApp.m_pCodePage[i]==m_nCodePage)
			nSel = i;
		m_CPList.InsertString(i, CP);
		//sz = pDC->GetTextExtent(CP);
		//if(sz.cx>cx) cx = sz.cx;
	}
	CP.LoadString(IDS_DEFCODEPAGE);
	m_CPList.InsertString(i, CP);
	m_CPList.SetCurSel(nSel==-1?i:nSel);

	//ReleaseDC(pDC);
	CRect rc, rc2; m_CPList.GetWindowRect(&rc);
	ScreenToClient(&rc);
	GetParent()->GetDlgItem(cmb1)->GetWindowRect(&rc2);
	m_CPList.MoveWindow(rc.left, rc.top,
		rc2.Width(), rc.Height()*(theApp.m_nCPCount));

	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_OPENTEXTEDIT);
	pEdit->GetWindowRect(&rc2);
	ScreenToClient(&rc2);
	GetParent()->GetClientRect(&rc);
	pEdit->MoveWindow(rc2.left, rc2.top,
		rc.Width(), rc2.Height());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CTextFileDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LPOFNOTIFY pofn = (LPOFNOTIFY)lParam;
	switch(pofn->hdr.code)
	{
	case CDN_FILEOK:
		{
		int nCPIndex = m_CPList.GetCurSel();
		if(nCPIndex == m_CPList.GetCount()-1)
			m_nCodePage = GetACP();
		else
			m_nCodePage = theApp.m_pCodePage[m_CPList.GetCurSel()];
		}break;
	case CDN_SELCHANGE:
		OnCbnSelchangeCodepagelist();
		break;
	}

	return CFileDialog::OnNotify(wParam, lParam, pResult);
}

void CTextFileDialog::OnCbnSelchangeCodepagelist()
{
	int nCPIndex = m_CPList.GetCurSel();
	if(nCPIndex == m_CPList.GetCount()-1)
		m_nCodePage = GetACP();
	else
		m_nCodePage = theApp.m_pCodePage[m_CPList.GetCurSel()];

	CString Text;
	CommDlg_OpenSave_GetFilePath(::GetParent(m_hWnd),
		g_szBuffer, _MAX_PATH);
	CStdioFile file;
	if(file.Open(g_szBuffer, CStdioFile::modeRead|CStdioFile::typeBinary))
	{
		UINT nSize;
		file.Read(&nSize, 2);
		if(nSize!=0xFEFF && nSize!=0xFFFE)
			file.SeekToBegin();
		BYTE buffer[2048]={0};
		nSize = file.Read(buffer, 2046);
		{
			if(m_nCodePage==1200)
				Text.Append((TCHAR*)buffer);
			else
			if(m_nCodePage==1201)
			{
				LPTSTR lpT = (LPTSTR)buffer;
				int nCount = nSize/2;
				for(int i=0; i<nCount; i++)
				{
					lpT[i] = (lpT[i]<<8)|(lpT[i]>>8);
				}
				Text.Append(lpT);
			}else
			{
				TCHAR lpu[2048]={0};
				LPTSTR lpText = (LPTSTR)lpu;

				MultiByteToWideChar(m_nCodePage, 0,
					(LPCSTR)buffer, nSize,
					lpText, 2046);
				if(*(WORD*)lpText==0xFEFF) lpText = (LPTSTR)((BYTE*)lpText+2);

				Text.Append(lpText);
			}
		}
	}
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_OPENTEXTEDIT);
	DWORD dwSel = pEdit->GetSel();
	int nLine = pEdit->GetFirstVisibleLine();
	pEdit->SetWindowText(Text);
	pEdit->LineScroll(nLine);
	pEdit->SetSel(dwSel);
}
