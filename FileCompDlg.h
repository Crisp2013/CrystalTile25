#pragma once
#include "afxcmn.h"


// CFileCompDlg �Ի���

class CFileCompDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileCompDlg)

public:
	CFileCompDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFileCompDlg();

// �Ի�������
	enum { IDD = IDD_FILECOMP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelFile(UINT nID);
	CString m_strFileOne;
	CString m_strFileTwo;
	afx_msg void OnBnClickedCompComfile();
	CListCtrl m_CompList;
	virtual BOOL OnInitDialog();
};
