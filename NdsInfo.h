#pragma once


// CNdsInfo 对话框

class CNdsInfo : public CDialog
{
	DECLARE_DYNAMIC(CNdsInfo)

public:
	CNdsInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNdsInfo();

// 对话框数据
	enum { IDD = IDD_NDSINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_nLang;
	CString Japanese;
	CString English;
	CString French;
	CString Geman;
	CString Italian;
	CString Spanish;
	CString m_strInfo;
	CRichEditCtrl m_RC;
	BOOL m_bChange;

	virtual BOOL OnInitDialog();
	afx_msg void OnSwapLang(UINT nID);
	afx_msg void OnEnChangeNdsinfoedit(void);
	afx_msg void OnBnClickedOk();
};
