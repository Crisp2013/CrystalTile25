#pragma once


// CT2NormalDlg �Ի���

class CT2NormalDlg : public CDialog
{
	DECLARE_DYNAMIC(CT2NormalDlg)

public:
	CT2NormalDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CT2NormalDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnOK();
};
