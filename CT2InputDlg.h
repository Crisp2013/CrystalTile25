#pragma once


// CT2InputDlg �Ի���

class CT2InputDlg : public CDialog
{
	DECLARE_DYNAMIC(CT2InputDlg)

public:
	CT2InputDlg(UINT nIDs, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CT2InputDlg();

// �Ի�������
	enum { IDD = IDD_INPUTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strVal;
	virtual BOOL OnInitDialog();
	CString m_strHint;
};
