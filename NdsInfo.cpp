// NdsInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include "NdsInfo.h"
#include "NdsFileSystemInfo.h"
#include "CT2View.h"

// CNdsInfo 对话框

IMPLEMENT_DYNAMIC(CNdsInfo, CDialog)
CNdsInfo::CNdsInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CNdsInfo::IDD, pParent)
	, m_nLang(0)
	, m_strInfo(_T(""))
	, m_bChange(FALSE)
{
}

CNdsInfo::~CNdsInfo()
{
}

void CNdsInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_JPNBTN, m_nLang);
	DDX_Text(pDX, IDC_NDSINFOEDIT, m_strInfo);
}


BEGIN_MESSAGE_MAP(CNdsInfo, CDialog)
	ON_COMMAND_RANGE(IDC_JPNBTN, IDC_JPNBTN+5, OnSwapLang)
	ON_EN_CHANGE(IDC_NDSINFOEDIT, OnEnChangeNdsinfoedit)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	//ON_NOTIFY(EN_MSGFILTER, IDC_NDSINFOEDIT, OnEnMsgfilterrichedit)
END_MESSAGE_MAP()


// CNdsInfo 消息处理程序

BOOL CNdsInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	BYTE* pRom = ((CNdsFileSystemInfo*)m_pParentWnd)->m_pRom;
	NDSHEADER *pHeader = (NDSHEADER*)pRom;
	LPTSTR pStr = (LPTSTR)(pRom+pHeader->Banner_Offset+0x240);
	Japanese= pStr; pStr+=0x80; Japanese = Japanese.Left(0x80);
	English	= pStr; pStr+=0x80; English = English.Left(0x80);
	French	= pStr; pStr+=0x80; French = French.Left(0x80);
	Geman	= pStr; pStr+=0x80; Geman = Geman.Left(0x80);
	Italian	= pStr; pStr+=0x80; Italian = Italian.Left(0x80);
	Spanish	= pStr; Spanish = Spanish.Left(0x80);

	m_RC.Create(WS_CHILD|ES_MULTILINE, CRect(0,0,0,0), this, -1);
	if(((CNdsFileSystemInfo*)m_pParentWnd)->m_bReadOnly)
		m_RC.SetReadOnly();
	//m_RC.SetEventMask(ENM_KEYEVENTS);

	m_RC.SetWindowText(Japanese);	m_RC.GetWindowText(Japanese);
	m_RC.SetWindowText(English);	m_RC.GetWindowText(English);
	m_RC.SetWindowText(French);		m_RC.GetWindowText(French);
	m_RC.SetWindowText(Geman);		m_RC.GetWindowText(Geman);
	m_RC.SetWindowText(Italian);	m_RC.GetWindowText(Italian);
	m_RC.SetWindowText(Spanish);	m_RC.GetWindowText(Spanish);

	((CEdit*)GetDlgItem(IDC_NDSINFOEDIT))->SetLimitText(0x80);

	CString format;
	format.LoadString(IDC_ENTRYINFO);
	m_strInfo.Format(format,
		pHeader->Arm9_Rom_Offset,
		pHeader->Arm9_Entry_Address,
		pHeader->Arm9_Ram_Address,
		//pHeader->Arm9_Size,
		((CNdsFileSystemInfo*)m_pParentWnd)->m_SpecRec[1].nBottom-
		((CNdsFileSystemInfo*)m_pParentWnd)->m_SpecRec[1].nTop,
		pHeader->Arm7_Rom_Offset,
		pHeader->Arm7_Entry_Address,
		pHeader->Arm7_Ram_Address,
		pHeader->Arm7_Size);
	SetDlgItemText(IDC_ENTRYINFO, m_strInfo);

	m_strInfo = Japanese;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CNdsInfo::OnSwapLang(UINT nID)
{
	int nLang = m_nLang;
	UpdateData();
	switch(nLang)
	{
	case 0:
		Japanese= m_strInfo;break;
	case 1:
		English	= m_strInfo;break;
	case 2:
		French	= m_strInfo;break;
	case 3:
		Geman	= m_strInfo;break;
	case 4:
		Italian	= m_strInfo;break;
	case 5:
		Spanish	= m_strInfo;break;
	}
	switch(m_nLang)
	{
	case 0:
		m_strInfo	= Japanese; break;
	case 1:
		m_strInfo	= English; break;
	case 2:
		m_strInfo	= French; break;
	case 3:
		m_strInfo	= Geman; break;
	case 4:
		m_strInfo	= Italian; break;
	case 5:
		m_strInfo	= Spanish; break;
	}
	UpdateData(FALSE);
}

void CNdsInfo::OnEnChangeNdsinfoedit(void)
{
	m_bChange = TRUE;
	GetDlgItem(IDOK)->EnableWindow(!((CNdsFileSystemInfo*)m_pParentWnd)->m_bReadOnly);
}

extern WORD GetCrc16(BYTE *pCrc, WORD nSize);
void CNdsInfo::OnBnClickedOk()
{
	OnSwapLang(IDC_JPNBTN+m_nLang);

	CString ln = _T("\r\n");
	CString n = _T("\n");
	Japanese.Replace(ln, n);
	English.Replace(ln, n);
	French.Replace(ln, n);
	Geman.Replace(ln, n);
	Italian.Replace(ln, n);
	Spanish.Replace(ln, n);

	BYTE* pRom = ((CNdsFileSystemInfo*)m_pParentWnd)->m_pRom;
	NDSHEADER *pHeader = (NDSHEADER*)pRom;
	NDSBANNER *pBanner = (NDSBANNER*)(pRom+pHeader->Banner_Offset);

	((CNdsFileSystemInfo*)m_pParentWnd)->m_pTileView->DoUndo((BYTE*)pBanner, pHeader->Banner_Offset, sizeof(NDSBANNER));
	memset((BYTE*)pBanner->JapaneseTitle, 0, 0x100*6);

	lstrcpyn(pBanner->JapaneseTitle, Japanese, 0x80);
	lstrcpyn(pBanner->EnglishTitle, English, 0x80);
	lstrcpyn(pBanner->FrenchTitle, French, 0x80);
	lstrcpyn(pBanner->GermanTitle, Geman, 0x80);
	lstrcpyn(pBanner->ItalianTitle, Italian, 0x80);
	lstrcpyn(pBanner->SpanishTitle, Spanish, 0x80);

	pBanner->wCrc16 = GetCrc16((BYTE*)pBanner->Icon, (WORD)((BYTE*)pBanner->ChineseTitle-pBanner->Icon));

	OnOK();
}
