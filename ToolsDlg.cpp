// ToolsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\ToolsDlg.h"
#include "CT2MainFrm.h"


// CToolsDlg 对话框

IMPLEMENT_DYNAMIC(CToolsDlg, CDialog)
CToolsDlg::CToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolsDlg::IDD, pParent)
	, m_strTitle(_T(""))
	, m_strCommand(_T(""))
	, m_strR(_T(""))
{
}

CToolsDlg::~CToolsDlg()
{
}

void CToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TOOLS_LIST, m_ToolList);
	DDX_Text(pDX, IDC_TOOLS_TITLEEDIT, m_strTitle);
	DDX_Text(pDX, IDC_TOOLS_COMMANDEDIT, m_strCommand);
	DDX_Text(pDX, IDC_TOOLS_REDIT, m_strR);
}


BEGIN_MESSAGE_MAP(CToolsDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_TOOLS_LIST, OnLbnSelchangeToolsList)
	ON_BN_CLICKED(IDC_TOOLS_ADDBTN, OnBnClickedToolsAddbtn)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_TOOLS_TITLEEDIT, IDC_TOOLS_REDIT, OnEnTextChange)
	ON_BN_CLICKED(IDC_TOOLS_PATHBTN, OnBnClickedToolsPathbtn)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_LBN_SETFOCUS(IDC_TOOLS_LIST, OnLbnSetfocusToolsList)
	ON_BN_CLICKED(IDC_TOOLS_DELBTN, OnBnClickedToolsDelbtn)
	ON_BN_CLICKED(IDC_TOOLS_UPBTN, OnBnClickedToolsUpbtn)
	ON_BN_CLICKED(IDC_TOOLS_DOWNBTN, OnBnClickedToolsDownbtn)
END_MESSAGE_MAP()


// CToolsDlg 消息处理程序

BOOL CToolsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CStringArray &Tl = ((CT2MainFrm*)theApp.GetMainWnd())->m_ToolList;
	CString t,tmp;
	for(int i=0; i<Tl.GetCount(); i++)
	{
		tmp = Tl.GetAt(i);
		m_strToolList.Add(tmp);
		AfxExtractSubString(t, tmp, 0);
		m_ToolList.AddString(t);
	}
	//m_ToolList.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CToolsDlg::OnLbnSelchangeToolsList()
{
	int nSel = m_ToolList.GetCurSel();
	if(nSel==-1) return;
	CString p = m_strToolList.GetAt(nSel);
	AfxExtractSubString(m_strTitle, p, 0);
	AfxExtractSubString(m_strCommand, p, 1);
	AfxExtractSubString(m_strR, p, 2);

	UpdateData(FALSE);
}

void CToolsDlg::OnBnClickedToolsAddbtn()
{
	if(m_ToolList.GetCount()>=32)
	{
		Hint(IDS_MAXTOOL);
		return;
	}
	m_strTitle.LoadString(IDS_NEWTOOL);
	m_strToolList.Add(m_strTitle);
	m_ToolList.AddString(m_strTitle);
	m_ToolList.SetCurSel(m_ToolList.GetCount()-1);
	UpdateData(FALSE);
}

void CToolsDlg::OnEnTextChange(UINT nID)
{
	int nSel = m_ToolList.GetCurSel();
	if(nSel==-1) return;
	UpdateData();
	CString t;
	t.Format(_T("%s\n%s\n%s"), m_strTitle, m_strCommand, m_strR);
	m_strToolList.SetAt(nSel, t);
	m_ToolList.DeleteString(nSel);
	m_ToolList.InsertString(nSel, m_strTitle);
	m_ToolList.SetCurSel(nSel);
}

void CToolsDlg::OnBnClickedToolsPathbtn()
{
	int nSel = m_ToolList.GetCurSel();
	if(nSel==-1)
	{
		OnBnClickedToolsAddbtn();
		nSel = m_ToolList.GetCurSel();
		if(nSel==-1) return;
	}
	CFileDialog fd(TRUE);
	CString f;
	f.LoadString(IDS_EXEC);
	f.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter = f;
	if(fd.DoModal()!=IDOK) return;
	f = fd.GetPathName();
	//if(f.Right(4).CompareNoCase(_T(".exe"))!=0) return;
	UpdateData();
	CString t, t2;
	GetDlgItemText(IDC_TOOLS_TITLEEDIT, t);
	t2.LoadString(IDS_NEWTOOL);
	if(t2.Compare(t)==0 || t.IsEmpty())
	{
		m_strTitle = PathFindFileName(f);
	}
	m_strCommand = f;
	UpdateData(FALSE);
	OnEnTextChange(0);
}

void CToolsDlg::OnBnClickedOk()
{
	OnEnTextChange(0);

	CStringArray &Tl = ((CT2MainFrm*)theApp.GetMainWnd())->m_ToolList;
	Tl.RemoveAll();
	CString tmp, r;
	for(int i=0; i<m_strToolList.GetCount(); i++)
	{
		tmp = m_strToolList.GetAt(i);
		AfxExtractSubString(r, tmp, 1);
		if(r.IsEmpty()/* || (r.Right(4).CompareNoCase(_T(".exe"))!=0)*/) continue;
		Tl.Add(tmp);
	}

	((CT2MainFrm*)theApp.GetMainWnd())->LoadToolBarEx();
	OnOK();
}

void CToolsDlg::OnLbnSetfocusToolsList()
{
	OnEnTextChange(0);
}

void CToolsDlg::OnBnClickedToolsDelbtn()
{
	int nSel = m_ToolList.GetCurSel();
	if(nSel==-1) return;
	m_strToolList.RemoveAt(nSel);
	m_ToolList.DeleteString(nSel);
}

void CToolsDlg::OnBnClickedToolsUpbtn()
{
	int nSel = m_ToolList.GetCurSel();
	if(nSel<1) return;

	CString t1,t2;
	t1 = m_strToolList.GetAt(nSel-1);
	t2 = m_strToolList.GetAt(nSel);
	m_strToolList.SetAt(nSel-1, t2);
	m_strToolList.SetAt(nSel, t1);
	m_ToolList.DeleteString(nSel-1);
	AfxExtractSubString(t2, t1, 0);
	m_ToolList.InsertString(nSel, t2);
}

void CToolsDlg::OnBnClickedToolsDownbtn()
{
	int nSel = m_ToolList.GetCurSel();
	if(nSel==-1 || nSel>=(m_ToolList.GetCount()-1)) return;

	CString t1,t2;
	t1 = m_strToolList.GetAt(nSel);
	t2 = m_strToolList.GetAt(nSel+1);
	m_strToolList.SetAt(nSel, t2);
	m_strToolList.SetAt(nSel+1, t1);
	m_ToolList.DeleteString(nSel);
	AfxExtractSubString(t2, t1, 0);
	m_ToolList.InsertString(nSel+1, t2);
}
