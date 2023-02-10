// CT2InputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include "CT2InputDlg.h"
#include ".\ct2inputdlg.h"


// CT2InputDlg 对话框

IMPLEMENT_DYNAMIC(CT2InputDlg, CDialog)
CT2InputDlg::CT2InputDlg(UINT nIDs, CWnd* pParent /*=NULL*/)
: CDialog(CT2InputDlg::IDD, pParent)
, m_strVal(_T(""))
, m_strHint(_T(""))
{
	m_strHint.LoadString(nIDs);
}

CT2InputDlg::~CT2InputDlg()
{
}

void CT2InputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUTDLG_EDIT, m_strVal);
	DDX_Text(pDX, IDC_INPUTDLG_HINT, m_strHint);
}


BEGIN_MESSAGE_MAP(CT2InputDlg, CDialog)
END_MESSAGE_MAP()


// CT2InputDlg 消息处理程序

BOOL CT2InputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
