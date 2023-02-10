#pragma once


// CDSFirmWareSettings 对话框

class CDSFirmWareSettings : public CDialog
{
	DECLARE_DYNAMIC(CDSFirmWareSettings)

public:
	CDSFirmWareSettings(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDSFirmWareSettings();

// 对话框数据
	enum { IDD = IDD_DS_FIRMWARE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
