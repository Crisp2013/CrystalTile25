#pragma once


// CNdsInfo �Ի���

class CNdsInfo : public CDialog
{
	DECLARE_DYNAMIC(CNdsInfo)

public:
	CNdsInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNdsInfo();

// �Ի�������
	enum { IDD = IDD_NDSINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
