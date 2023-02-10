#ifndef CT2_OPENFILEDIALOG_H
#define CT2_OPENFILEDIALOG_H
#include "afxwin.h"

#pragma once


// COpenFileDialog 对话框

class COpenFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(COpenFileDialog)

public:
	COpenFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = /*OFN_HIDEREADONLY | */OFN_OVERWRITEPROMPT | OFN_ENABLETEMPLATE,// | OFN_ALLOWMULTISELECT
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0);
	virtual ~COpenFileDialog();

// 对话框数据
	enum { IDD = IDD_OPENFILEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	BOOL m_bMapFile;
	LONGLONG m_nMemSize;
	CListBox m_MFileList;
	BOOL m_bMFile;

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLbnDblclkMfilelist();
	BOOL CheckSize();
	afx_msg void OnBnClickedMfileOpen();
};

#endif // CT2_OPENFILEDIALOG_H