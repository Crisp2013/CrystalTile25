#pragma once
#include "afxwin.h"


// CToolsDlg �Ի���

class CToolsDlg : public CDialog
{
	DECLARE_DYNAMIC(CToolsDlg)

public:
	CToolsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CToolsDlg();

// �Ի�������
	enum { IDD = IDD_TOOLSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
