#pragma once


// CT2InputDlg 对话框

class CT2InputDlg : public CDialog
{
	DECLARE_DYNAMIC(CT2InputDlg)

public:
	CT2InputDlg(UINT nIDs, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CT2InputDlg();

// 对话框数据
	enum { IDD = IDD_INPUTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strVal;
	virtual BOOL OnInitDialog();
	CString m_strHint;
};
