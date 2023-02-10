#ifndef CT2_LZ77SEARCH_H
#define CT2_LZ77SEARCH_H
#include "afxcmn.h"
#pragma once


// CLZ77Search 对话框

class CLZ77Search : public CDialog
{
	DECLARE_DYNAMIC(CLZ77Search)

public:
	CLZ77Search(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLZ77Search();

// 对话框数据
	enum { IDD = IDD_LZ77SEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strHead;
	CString m_strSizelow;
	CString m_strSizeHi;
	BOOL m_bDown;
	CListCtrl m_LZ77List;
	int m_nStep;
	virtual BOOL OnInitDialog();
	afx_msg void OnFind(UINT nID);
	afx_msg void OnLz77ListSave();
	afx_msg void OnUpdateLz77ListSave(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnLz77Delete();
	afx_msg void OnUpdateLz77Delete(CCmdUI *pCmdUI);
	afx_msg void OnLvnKeydownRetlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivateRetlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLz77Uncompall();
	afx_msg void OnLz77Uncompsel();
	afx_msg void OnUpdateLz77Uncompsel(CCmdUI *pCmdUI);
	afx_msg void OnLz77Break();
	afx_msg void OnUpdateLz77Break(CCmdUI *pCmdUI);
	//afx_msg BOOL OnNcActivate(BOOL bActive);
	BOOL m_bLz77;
protected:
	virtual void OnOK();
};

#endif // CT2_LZ77SEARCH_H