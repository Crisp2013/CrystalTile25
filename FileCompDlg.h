#pragma once
#include "afxcmn.h"


// CFileCompDlg 对话框

class CFileCompDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileCompDlg)

public:
	CFileCompDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFileCompDlg();

// 对话框数据
	enum { IDD = IDD_FILECOMP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelFile(UINT nID);
	CString m_strFileOne;
	CString m_strFileTwo;
	afx_msg void OnBnClickedCompComfile();
	CListCtrl m_CompList;
	virtual BOOL OnInitDialog();
};
