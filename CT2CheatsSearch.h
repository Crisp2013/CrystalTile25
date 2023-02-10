#ifndef _CT2CHEATSSEARCH_H_
#define _CT2CHEATSSEARCH_H_
#include "afxcmn.h"

#pragma once

// CT2CheatsSearch 对话框

class CT2CheatsSearch : public CDialog
{
	DECLARE_DYNAMIC(CT2CheatsSearch)

public:
	CT2CheatsSearch(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CT2CheatsSearch();

// 对话框数据
	enum { IDD = IDD_CHEATS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	struct Cheats
	{
		UINT nOffset;
		UINT nData;
	};
	struct Locks
	{
		UINT nOffset;
		UINT nData;
		UINT nSize;
	};

	CList<Cheats, Cheats&> m_CheatsList;
	CList<Locks, Locks&> m_LockList;
	CString m_strData;
	int m_nDataType;
	int m_nDec;
	CDocument *m_pDoc;
	BYTE *m_pOldMemory;
	CListCtrl m_wndCheatsList;

	afx_msg void OnBnClickedCheatsSearch2();
	afx_msg void OnBnClickedCheatsSearch();
	virtual BOOL OnInitDialog();
	CListCtrl m_wndLockList;
	afx_msg void OnLvnItemActivateCheatsList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditCheatsLocklist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivateCheatsLocklist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	void OnUpdateDataType();
	afx_msg void OnDestroy();
	afx_msg void OnLvnGetdispinfoCheatsLocklist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBeginlabeleditCheatsLocklist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickCheatsLocklist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheatsSave();
	afx_msg void OnLvnKeydownCheatsList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void DeleteLockItem();
	afx_msg void AddLockItem();
};

#endif // _CT2CHEATSSEARCH_H_