#pragma once
#include "afxwin.h"

// CAsmHint

class CAsmHint : public CStatic
{
public:
	CString m_strCompSrc;

	CAsmHint();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
};


// CAsmDlg 对话框

class CAsmDlg : public CDialog
{
	DECLARE_DYNAMIC(CAsmDlg)

public:
	CAsmDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAsmDlg();

// 对话框数据
	enum { IDD = IDD_ASMDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_AsmCB;
	CAsmHint m_AsmHint;
	CString m_strAsmHint;
	UINT m_nType;
	BYTE m_nSize;
	CString m_strAsm;
	//char m_cstrAsm[_MAX_PATH];
	UINT m_nCurOffset;
	CString m_strAsmList;

	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnEditchangeAsmcb();
	virtual BOOL OnInitDialog();
	void UpdateAsmDesc();
	CString m_strAsmDesc;
};
