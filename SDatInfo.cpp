// NdsFileSystemInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\SDatInfo.h"

#include "CT2InputDlg.h"

// CSDatInfo 对话框

IMPLEMENT_DYNAMIC(CSDatInfo, CSubFileInfo)
CSDatInfo::CSDatInfo(CWnd* pParent /*=NULL*/)
	: CSubFileInfo(pParent)
{
}

CSDatInfo::~CSDatInfo()
{
}

void CSDatInfo::DoDataExchange(CDataExchange* pDX)
{
	CSubFileInfo::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSDatInfo, CSubFileInfo)
	ON_WM_SIZE()
	ON_COMMAND_RANGE(ID_PLAYER_MONO, ID_PLAYER_STEREO, OnPlayerChannels)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PLAYER_MONO, ID_PLAYER_STEREO, OnUpdatePlayerChannels)
	ON_COMMAND_RANGE(ID_PLAYER_4BIT, ID_PLAYER_16BIT, OnPlayer4_16bit)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PLAYER_4BIT, ID_PLAYER_16BIT, OnUpdatePlayer4_16bit)
	ON_COMMAND_RANGE(ID_PLAYER_11KHZ, ID_PLAYER_NKHZ, OnPlayerNkhz)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PLAYER_11KHZ, ID_PLAYER_NKHZ, OnUpdatePlayerNkhz)
	ON_COMMAND(ID_PLAYER_PLAY, OnPlayerPlay)
	ON_COMMAND(ID_PLAYER_STOP, OnPlayerStop)
END_MESSAGE_MAP()


// CSDatInfo 消息处理程序

BOOL CSDatInfo::OnInitDialog()
{
	CSubFileInfo::OnInitDialog();

	// SetHeader();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSDatInfo::Update()
{
	BYTE *pSDat = m_pChild->m_pTileView->GetDocument()->m_pRom + m_nOffset;
	m_FileList.SetRedraw(FALSE);
	m_FileList.DeleteAllItems();

	NDSSDAT *sdat	= (NDSSDAT*)	pSDat;
	SDATSYMB *symb	= (SDATSYMB*)	(pSDat+sdat->nSymbPos);
	SDATFAT *fat	= (SDATFAT*)	(pSDat+sdat->nSDatFatPos);
	SDATINFO *info	= (SDATINFO*)	(pSDat+sdat->nSDatInfoPos);
	if( *(UINT*)sdat->flags!=0x54414453 ||	// "SDAT"
		*(UINT*)symb->falgs!=0x424D5953 ||	// "SYMB"
		*(UINT*)fat->flags!=0x20544146 ||	// "FAT "
		*(UINT*)info->falgs!=0x4F464E49 )	// "INFO"
		return;

	CString subitem;
	UINT nRec=0, nInfoRec=0, nInfoCount=0, nFat=0;
	int nRecCount=0, iItem=0;
	SDATINFORECPOS *recpos;
	SDATSYMBREC *rec;

	UINT *res = &symb->nSequences;
	UINT *ires = &info->nSequences;
	TCHAR *type[]={
		_T("Sequences"),	// 0
		_T("SoundEffects"),	// 1
		_T("Banks"),		// 2
		_T("Waves"),		// 3
		_T("Players"),		// 4
		_T("Groups"),		// 5
		_T("Streams"),		// 6
		_T("UnknownMD")		// 7
	};

	for(int c=0; c<8; c++)
	{
		if(res[c])
		rec = (SDATSYMBREC*)((BYTE*)symb+res[c]);
		recpos = (SDATINFORECPOS*)((BYTE*)info+ires[c]);
		//*g_szBuffer = 0;
		//nFat=0;
		for(UINT i=0; i<rec->nCount; i++)
		{
			if(rec->nSymbPos[i])
			{
				SDATINFOREC *irec = (SDATINFOREC*) ((BYTE*)info+recpos->nInfoPos[i]);

				LPSTR lpStr = (LPSTR)symb+rec->nSymbPos[i];
				if(*lpStr)
					OemToChar((LPCSTR) lpStr, g_szBuffer);
				else
					_stprintf(g_szBuffer, _T("Track%03d"), irec->wFileID);

				if(fat->rec[i].nPos && irec->wFileID<fat->nFatCount && c!=5)
				{
					LPTSTR p=g_szBuffer+lstrlen(g_szBuffer);
					*p++ = _T('.');
					*(p+4) = 0;
					OemToCharBuff((LPCSTR)pSDat+fat->rec[irec->wFileID].nPos, p, 4);
				}

				subitem.Format(_T("%s\\%03d-%s"), type[c], irec->wFileID, g_szBuffer);
				m_FileList.InsertItem(iItem, subitem);

				char sub=1;
				if(irec->wFileID>=fat->nFatCount || c==5)
					sub=3;
				else
				{
					// rom pos
					subitem.Format(_T("%08X"), m_nOffset+fat->rec[irec->wFileID].nPos);
					m_FileList.SetItemText(iItem, sub++, subitem);
					// sdat pos
					if(m_nOffset)
					{
						subitem.Format(_T("%08X"), fat->rec[irec->wFileID].nPos);
						m_FileList.SetItemText(iItem, sub++, subitem);
					}
					// size
					subitem.Format(_T("%d"), fat->rec[irec->wFileID].nSize);
					m_FileList.SetItemText(iItem, sub++, subitem);
				}
				// info
				subitem.Format(_T("%03d-%04X-%03d-%03d-%03d-%03d"),
					iItem, irec->wBnk, irec->nVol, irec->nVar1, irec->nVar2, irec->nVar3);
				m_FileList.SetItemText(iItem++, sub++, subitem);
			}
		}
	}
	/*CUIntArray FileID;
	for(int i=0; nFat<fat->nFatCount; nFat++)
	{
		// info
		while(nInfoCount==0)
		{
			recpos = (SDATINFORECPOS*)((BYTE*)info+info->nInfoRecPos[nInfoRec++]);
			nInfoCount = recpos->nCount;
		}
		SDATINFOREC *irec = (SDATINFOREC*)
			((BYTE*)info+recpos->nInfoPos[recpos->nCount-nInfoCount--]);

		for(int i2=0; i2<FileID.GetCount(); i2++)
		{
			if(FileID.GetAt(i2)==irec->wFileID)
			{
				nRecCount--;
				break;
			}
		}
		FileID.Add(irec->wFileID);

		// name
		int pos;
		do
		{
			while(nRecCount<=0)
			{
				rec = (SDATSYMBREC*)((BYTE*)symb+symb->nSymbRecPos[nRec++]);
				nRecCount = rec->nCount;
			}
			pos=rec->nSymbPos[rec->nCount-nRecCount];
			nRecCount--;
		}while(pos==0);

		OemToChar((LPCSTR)( (BYTE*)symb+pos), g_szBuffer);
		subitem.Format(_T("%d-%s.%c%c%c%c"), i, g_szBuffer,
						*(pSDat+fat->rec[nFat].nPos+0),
						*(pSDat+fat->rec[nFat].nPos+1),
						*(pSDat+fat->rec[nFat].nPos+2),
						*(pSDat+fat->rec[nFat].nPos+3));
		m_FileList.InsertItem(i, subitem);

		char sub=1;
		// rom pos
		subitem.Format(_T("%08X"), m_nOffset+fat->rec[nFat].nPos);
		m_FileList.SetItemText(i, sub++, subitem);
		// sdat pos
		if(m_nOffset)
		{
			subitem.Format(_T("%08X"), fat->rec[nFat].nPos);
			m_FileList.SetItemText(i, sub++, subitem);
		}
		// size
		subitem.Format(_T("%d"), fat->rec[nFat].nSize);
		m_FileList.SetItemText(i, sub++, subitem);

		// info
		subitem.Format(_T("%03d-%04X-%03d-%03d-%03d-%03d"),
			nFat, irec->wBnk, irec->nVol, irec->nVar1, irec->nVar2, irec->nVar3);
		m_FileList.SetItemText(i++, sub++, subitem);
	}*/

	SDATFILEINFO *sfi = (SDATFILEINFO*) (pSDat+sdat->nSDatFileInfoPos);
	subitem.Format(m_strWTitle, m_strTitle, iItem, sfi->nSoundCount);
	SetWindowText(subitem);
	m_FileList.SetRedraw();
}

void CSDatInfo::OnPlayerChannels(UINT nID)
{
	m_Player.m_wfx.nChannels = nID-ID_PLAYER_MONO+1;
	m_Player.m_wfx.nBlockAlign = m_Player.m_wfx.wBitsPerSample/8 * m_Player.m_wfx.nChannels;
	m_Player.m_wfx.nAvgBytesPerSec = m_Player.m_wfx.nSamplesPerSec * m_Player.m_wfx.nBlockAlign;
	m_Player.Close();
}

void CSDatInfo::OnUpdatePlayerChannels(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_Player.m_wfx.nChannels==(pCmdUI->m_nID-ID_PLAYER_MONO+1));
}

void CSDatInfo::OnPlayer4_16bit(UINT nID)
{
	switch(nID)
	{
	case ID_PLAYER_4BIT:
		m_Player.m_wfx.wBitsPerSample = 4;
		break;
	case ID_PLAYER_8BIT:
		m_Player.m_wfx.wBitsPerSample = 8;
		break;
	case ID_PLAYER_16BIT:
		m_Player.m_wfx.wBitsPerSample = 16;
		break;
	}
	m_Player.m_wfx.nBlockAlign = m_Player.m_wfx.wBitsPerSample * m_Player.m_wfx.nChannels / 8;
	m_Player.m_wfx.nAvgBytesPerSec = m_Player.m_wfx.nSamplesPerSec * m_Player.m_wfx.nBlockAlign;
	m_Player.Close();
}

void CSDatInfo::OnUpdatePlayer4_16bit(CCmdUI *pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case ID_PLAYER_4BIT:
		pCmdUI->SetRadio(m_Player.m_wfx.wBitsPerSample == 4);
		break;
	case ID_PLAYER_8BIT:
		pCmdUI->SetRadio(m_Player.m_wfx.wBitsPerSample == 8);
		break;
	case ID_PLAYER_16BIT:
		pCmdUI->SetRadio(m_Player.m_wfx.wBitsPerSample == 16);
		break;
	}
}

void CSDatInfo::OnPlayerNkhz(UINT nID)
{
	if(nID==ID_PLAYER_NKHZ)
	{
		static CT2InputDlg id(IDS_PLAYER_NKHZ);
		if(id.DoModal()!=IDOK) return;
		UINT s = StrToIntEX(id.m_strVal, FALSE);
		if( s==-1 ||
			s<5500 )
			s = 5500;
		else
		if( s>192000 )
			s=192000;
		m_Player.m_wfx.nSamplesPerSec = s;
	}else
	{
		UINT s[] = {11025,22050,44100,48000,96000};
		m_Player.m_wfx.nSamplesPerSec = s[nID-ID_PLAYER_11KHZ];
	}
	m_Player.m_wfx.nAvgBytesPerSec = m_Player.m_wfx.nSamplesPerSec * m_Player.m_wfx.nBlockAlign;
	m_Player.Close();
}

void CSDatInfo::OnUpdatePlayerNkhz(CCmdUI *pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case ID_PLAYER_11KHZ:
		pCmdUI->SetRadio(m_Player.m_wfx.nSamplesPerSec==11025);
		break;
	case ID_PLAYER_22KHZ:
		pCmdUI->SetRadio(m_Player.m_wfx.nSamplesPerSec==22050);
		break;
	case ID_PLAYER_44KHZ:
		pCmdUI->SetRadio(m_Player.m_wfx.nSamplesPerSec==44100);
		break;
	case ID_PLAYER_48KHZ:
		pCmdUI->SetRadio(m_Player.m_wfx.nSamplesPerSec==48000);
		break;
	case ID_PLAYER_96KHZ:
		pCmdUI->SetRadio(m_Player.m_wfx.nSamplesPerSec==96000);
		break;
	default:
		{
			UINT s[] = {11025,22050,44100,48000,96000};
			for(int i=0; i<sizeof(s)/sizeof(UINT); i++)
			{
				if(m_Player.m_wfx.nSamplesPerSec==s[i])
				{
					pCmdUI->SetRadio(FALSE);
					return;
				}
			}
			pCmdUI->SetRadio();
		}break;
	}
}

void CSDatInfo::OnPlayerPlay()
{
	POSITION pos = m_FileList.GetFirstSelectedItemPosition();
	int nSel = m_FileList.GetNextSelectedItem(pos);
	if(nSel==-1) return;

	CString Addr = m_FileList.GetItemText(nSel, 1);
	UINT nOffset = StrToIntEX(Addr);
	//CT2View *pView;
	UINT nSize;
	if(m_nOffset)
		Addr = m_FileList.GetItemText(nSel, 3);
	else
		Addr = m_FileList.GetItemText(nSel, 2);
	nSize = StrToIntEX(Addr, FALSE);
	LPSTR pSound = (LPSTR)(m_pChild->m_pTileView->GetDocument()->m_pRom+nOffset);
	NDSFILEHEADER *header = (NDSFILEHEADER*)(pSound);
	pSound += header->nSize;
	nSize -= header->nSize;
	WORD i=1;
	while(i<(header->nSubHeaderCount&7))
	{
		SUBHEADER *sub = (SUBHEADER*)pSound;
		pSound += sub->nSize;
		nSize -= sub->nSize;
		i++;
	}
	pSound += 0x10;
	nSize -= 0x10;

	m_Player.Play(pSound, nSize);
}

void CSDatInfo::OnPlayerStop()
{
	m_Player.Close();
}

void CSDatInfo::OnOK()
{
	GoToFile();
}

void CSDatInfo::SetHeader()
{
	CHeaderCtrl& hc = *m_FileList.GetHeaderCtrl();
	while(hc.GetItemCount()>0)
		m_FileList.DeleteColumn(0);
	CString header, text;
	header.LoadString(IDS_SDATINFOHEADER);
	BYTE nFormat[]={LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_LEFT};
	BYTE nWidth[]={112, 64, 64, 44, 64};
	for(int i=0; i<sizeof(nFormat)/sizeof(BYTE); i++)
	{
		if(m_nOffset==0 && i==2) continue;

		AfxExtractSubString(text, header, i);
		m_FileList.InsertColumn(i, text, nFormat[i], nWidth[i]);
	}
}
