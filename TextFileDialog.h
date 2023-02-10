#pragma once

// CTextFileDialog 对话框

class CTextFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CTextFileDialog)

public:
	CTextFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		UINT nExtList = IDS_ALLFILE,
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLETEMPLATE,// | OFN_ALLOWMULTISELECT
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL,
		DWORD dwSize = 0);
	virtual ~CTextFileDialog();

// 对话框数据
	enum { IDD = IDD_OPENTEXTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CPList;
	CString m_strFilter;
	UINT m_nCodePage;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCodepagelist();
};
