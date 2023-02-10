#pragma once
#include "afxwin.h"


// CToolsDlg 对话框

class CToolsDlg : public CDialog
{
	DECLARE_DYNAMIC(CToolsDlg)

public:
	CToolsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolsDlg();

// 对话框数据
	enum { IDD = IDD_TOOLSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListBox m_ToolList;
	afx_msg void OnLbnSelchangeToolsList();
	CString m_strTitle;
	CString m_strCommand;
	CString m_strR;
	CStringArray m_strToolList;
	afx_msg void OnBnClickedToolsAddbtn();
	afx_msg void OnEnTextChange(UINT nID);
	afx_msg void OnBnClickedToolsPathbtn();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnSetfocusToolsList();
	afx_msg void OnBnClickedToolsDelbtn();
	afx_msg void OnBnClickedToolsUpbtn();
	afx_msg void OnBnClickedToolsDownbtn();
};
