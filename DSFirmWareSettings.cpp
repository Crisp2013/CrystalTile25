// DSFirmWareSettings.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\DSFirmWareSettings.h"


// CDSFirmWareSettings 对话框

IMPLEMENT_DYNAMIC(CDSFirmWareSettings, CDialog)
CDSFirmWareSettings::CDSFirmWareSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDSFirmWareSettings::IDD, pParent)
	, m_nUser(0)
{
}

CDSFirmWareSettings::~CDSFirmWareSettings()
{
}

void CDSFirmWareSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_DSFIRMWARE_1, m_nUser);
}


BEGIN_MESSAGE_MAP(CDSFirmWareSettings, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPENFIRMWARE, OnBnClickedButtonOpenfirmware)
	ON_COMMAND_RANGE(IDC_RADIO_DSFIRMWARE_1, IDC_RADIO_DSFIRMWARE_2, OnBnClickedRadioDsfirmware)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDSFirmWareSettings 消息处理程序

void CDSFirmWareSettings::OnBnClickedButtonOpenfirmware()
{
	CFileDialog fd(TRUE);
	if(fd.DoModal()!=IDOK) return;
	m_strFirmWare = fd.GetPathName();
	CFile file;
	if(!file.Open(m_strFirmWare, CFile::modeRead|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	if(file.Seek(0x3FE00, SEEK_SET)!=0x3FE00)
		return;
	if(file.Read(m_UserSettings, sizeof(m_UserSettings))!=sizeof(m_UserSettings))
		return;

	OnBnClickedRadioDsfirmware(0);

	GetDlgItem(IDC_RADIO_DSFIRMWARE_1)->EnableWindow();
	GetDlgItem(IDC_RADIO_DSFIRMWARE_2)->EnableWindow();
	GetDlgItem(IDOK)->EnableWindow();
}

void CDSFirmWareSettings::OnBnClickedRadioDsfirmware(UINT nID)
{
	if(nID)
		Update();

	int nOldUser=m_nUser;
	UpdateData();
	//if(m_nUser==nOldUser) return;

	g_szBuffer[0]=0;
	lstrcpyn(g_szBuffer, (WCHAR*)m_UserSettings[m_nUser].Nickname, *(WORD*)m_UserSettings[m_nUser].Nickname_Length+1);
	SetDlgItemText(IDC_DSFIRMWARE_NICKNAME, g_szBuffer);

	g_szBuffer[0]=0;
	lstrcpyn(g_szBuffer, (WCHAR*)m_UserSettings[m_nUser].Message, *(WORD*)m_UserSettings[m_nUser].Message_Length+1);
	SetDlgItemText(IDC_DSFIRMWARE_MESSAGE, g_szBuffer);

	CString tmp;
	tmp.Format(_T("%02d:%02d"), m_UserSettings[m_nUser].Birthday_Month, m_UserSettings[m_nUser].Birthday_Day);
	SetDlgItemText(IDC_DSFIRMWARE_BIRTHDAY, tmp);

	tmp.Format(_T("%d"), m_UserSettings[m_nUser].Favorite_Color);
	SetDlgItemText(IDC_DSFIRMWARE_COLOR, tmp);

	((CComboBox*)GetDlgItem(IDC_COMBO_DS_FIRMWARE_LANG))->SetCurSel(m_UserSettings[m_nUser].Language[0]&7);
}

extern WORD GetCrc16(BYTE *pCrc, WORD nSize);
void CDSFirmWareSettings::Update()
{
	memset(g_szBuffer, 0, 20);
	GetDlgItemText(IDC_DSFIRMWARE_NICKNAME, g_szBuffer, 10+1);
	memcpy(m_UserSettings[m_nUser].Nickname, g_szBuffer, 20);
	*(WORD*)m_UserSettings[m_nUser].Nickname_Length = lstrlen(g_szBuffer);

	memset(g_szBuffer, 0, 52);
	GetDlgItemText(IDC_DSFIRMWARE_MESSAGE, g_szBuffer, 26+1);
	memcpy(m_UserSettings[m_nUser].Message, g_szBuffer, 52);
	*(WORD*)m_UserSettings[m_nUser].Message_Length= lstrlen(g_szBuffer);

	GetDlgItemText(IDC_DSFIRMWARE_BIRTHDAY, g_szBuffer, _MAX_PATH);
	LPTSTR lpTmp = sFindChar(g_szBuffer, _T(':'));
	int nTmp;
	if(lpTmp)
	{
		if(_stscanf(g_szBuffer, _T("%d"), &nTmp))
			m_UserSettings[m_nUser].Birthday_Month = (BYTE)nTmp;

		if(_stscanf(lpTmp+1, _T("%d"), &nTmp))
			m_UserSettings[m_nUser].Birthday_Day = (BYTE)nTmp;
	}

	GetDlgItemText(IDC_DSFIRMWARE_COLOR, g_szBuffer, _MAX_PATH);
	if(_stscanf(g_szBuffer, _T("%d"), &nTmp))
		m_UserSettings[m_nUser].Favorite_Color = (BYTE)nTmp;


	nTmp = ((CComboBox*)GetDlgItem(IDC_COMBO_DS_FIRMWARE_LANG))->GetCurSel()&7;
	m_UserSettings[m_nUser].Language[0]&=~7;
	m_UserSettings[m_nUser].Language[0]|=nTmp;

	*(WORD*)m_UserSettings[m_nUser].CRC16 = GetCrc16((BYTE*)&m_UserSettings[m_nUser], 0x70);
}

void CDSFirmWareSettings::OnBnClickedOk()
{
	Update();
	CFile file;
	if(file.Open(m_strFirmWare, CFile::modeWrite|CFile::typeBinary))
	{
		file.Seek(0x3FE00, SEEK_SET);
		file.Write(m_UserSettings, sizeof(m_UserSettings));
	}
}
