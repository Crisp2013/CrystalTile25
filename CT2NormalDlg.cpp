// T2NormalDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\CT2NormalDlg.h"


// CT2NormalDlg 对话框

IMPLEMENT_DYNAMIC(CT2NormalDlg, CDialog)
CT2NormalDlg::CT2NormalDlg(CWnd* pParent /*=NULL*/)
	//: CDialog(CT2NormalDlg::IDD, pParent)
{
}

CT2NormalDlg::~CT2NormalDlg()
{
}

void CT2NormalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CT2NormalDlg, CDialog)
END_MESSAGE_MAP()


// CT2NormalDlg 消息处理程序

BOOL CT2NormalDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(GetParent()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	if(AfxGetMainWnd()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CT2NormalDlg::PreTranslateMessage(MSG* pMsg)
{
	if(TranslateAccelerator(GetParentFrame()->GetSafeHwnd(),
		GetParentFrame()->m_hAccelTable,
		pMsg))
		return TRUE;

	if(pMsg->message==WM_KEYDOWN)
	{
		WORD wKey=LOWORD(pMsg->wParam);
		if(wKey==VK_ESCAPE)
		{
			GetParentFrame()->SetFocus();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CT2NormalDlg::OnOK()
{
}
