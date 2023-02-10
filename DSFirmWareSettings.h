#pragma once


// CDSFirmWareSettings �Ի���

class CDSFirmWareSettings : public CDialog
{
	DECLARE_DYNAMIC(CDSFirmWareSettings)

public:
	CDSFirmWareSettings(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDSFirmWareSettings();

// �Ի�������
	enum { IDD = IDD_DS_FIRMWARE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CString m_strFirmWare;
	NDSFIRMWARE_USERSETTINGS m_UserSettings[2];
	int m_nUser;

	afx_msg void OnBnClickedButtonOpenfirmware();
	void ShowUserSettings(int user);
	afx_msg void OnBnClickedRadioDsfirmware(UINT nID);
	void Update();
	afx_msg void OnBnClickedOk();
};
