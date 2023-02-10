// ToolBarEx.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include "ToolBarEx.h"
#include "CT2MainFrm.h"

// CToolBarEx

IMPLEMENT_DYNAMIC(CToolBarEx, CToolBar)
CToolBarEx::CToolBarEx()
{
}

CToolBarEx::~CToolBarEx()
{
}


BEGIN_MESSAGE_MAP(CToolBarEx, CToolBar)
	ON_NOTIFY_REFLECT(TBN_GETINFOTIP, OnTbnGetInfoTip)
END_MESSAGE_MAP()



// CToolBarEx 消息处理程序


void CToolBarEx::OnTbnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTBGETINFOTIP pNMTBGIT = reinterpret_cast<LPNMTBGETINFOTIP>(pNMHDR);

	CStringArray &l = ((CT2MainFrm*)theApp.GetMainWnd())->m_ToolList;
	CString tmp, t;
	tmp = l.GetAt(pNMTBGIT->iItem-ID_TOOLS_TOOLS-1);
	AfxExtractSubString(t, tmp, 0);
	lstrcpyn(pNMTBGIT->pszText, t, pNMTBGIT->cchTextMax);

	*pResult = 0;
}
