// CT2Doc.cpp :  CT2Doc 类的实现
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include ".\CT2Doc.h"
#include "CT2StatusBar.h"
#include "CT2MainFrm.h"
#include "TextFileDialog.h"

#include "CT2HexView.h"
#include "CT2ScriptView.h"
#include "CT2TileView.h"
#include "CT2AsmView.h"

#include "NdsFileSystemInfo.h"
#include "CT2ChildFrm.h"
#include "SubFileInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct Country
{
	char code;
	char* country;
};
Country Countries[] =
{
	{ 'C', "CHN" },	// China???
	{ 'J', "JPN" },
	{ 'E', "USA" },
	{ 'P', "EUR" },
	{ 'D', "NOE" },
	{ 'F', "NOE" },
	{ 'I', "ITA" },
	{ 'S', "SPA" },
	{ 'H', "HOL" },
	{ 'K', "KOR" },
	{ 'X', "EUU" },
	{ 0, NULL },
};
struct GBACompanyName
{
	char* Code;
	char* CompanyName;
};
GBACompanyName GbaInfo[] =
{
	{ "01", "Nintendo" },
	{ "02", "Rocket Games" },
	{ "03", "Imagineer-Zoom" },
	{ "05", "Zamuse" },
	{ "06", "Falcom" },
	{ "08", "Capcom" },
	{ "09", "Hot B Co." },
	{ "0A", "Jaleco" },
	{ "0B", "Coconuts Japan" },
	{ "0C", "Coconuts Japan/G.X.Media" },
	{ "0E", "Technos" },
	{ "0F", "Mebio Software" },
	{ "0G", "Shouei System" },
	{ "0H", "Starfish" },
	{ "0J", "Mitsui Fudosan/Dentsu" },
	{ "0L", "Warashi Inc." },
	{ "0N", "Nowpro" },
	{ "0P", "Game Village" },
	{ "12", "Infocom" },
	{ "13", "Electronic Arts Japan" },
	{ "15", "Cobra Team" },
	{ "16", "Human/Field" },
	{ "17", "KOEI" },
	{ "18", "Hudson Soft Japan" },
	{ "19", "S.C.P." },
	{ "1A", "Yonoman" },
	{ "1C", "Tecmo Products" },
	{ "1D", "Japan Glary Business" },
	{ "1E", "Forum/OpenSystem" },
	{ "1F", "Virgin Games" },
	{ "1G", "SMDE" },
	{ "1J", "Daikokudenki" },
	{ "1P", "Creatures Inc." },
	{ "1Q", "TDK Deep Impresion" },
	{ "20", "Destination Software" },
	{ "22", "VR 1 Japan" },
	{ "23", "Micro World" },
	{ "25", "San-X" },
	{ "26", "Enix" },
	{ "27", "Loriciel/Electro Brain" },
	{ "28", "Kemco Japan" },
	{ "29", "Seta" },
	{ "2A", "Culture Brain" },
	{ "2C", "Palsoft" },
	{ "2D", "Visit Co.,Ltd." },
	{ "2E", "Intec" },
	{ "2F", "System Sacom" },
	{ "2G", "Poppo" },
	{ "2H", "Ubisoft Japan" },
	{ "2J", "Media Works" },
	{ "2K", "NEC InterChannel" },
	{ "2L", "Tam" },
	{ "2M", "Jordan" },
	{ "2N", "Smilesoft" },
	{ "2Q", "Mediakite" },
	{ "30", "Viacom" },
	{ "31", "Carrozzeria" },
	{ "32", "Dynamic" },
	{ "34", "Magifact" },
	{ "35", "Hect" },
	{ "36", "Codemasters" },
	{ "37", "GAGA Communications" },
	{ "38", "Laguna" },
	{ "39", "Telstar Fun and Games" },
	{ "3B", "Arcade Zone Ltd" },
	{ "3D", "Loriciel" },
	{ "3E", "Gremlin Graphics" },
	{ "3F", "K.Amusement Leasing Co." },
	{ "40", "Seika Corp." },
	{ "41", "Ubi Soft Entertainment" },
	{ "42", "Sunsoft" },
	{ "44", "Life Fitness" },
	{ "46", "System 3" },
	{ "47", "Spectrum Holobyte" },
	{ "49", "IREM" },
	{ "4B", "Raya Systems" },
	{ "4C", "Renovation Products" },
	{ "4D", "Malibu Games" },
	{ "4F", "Eidos/U.S. Gold" },
	{ "4J", "Fox Interactive" },
	{ "4K", "Time Warner Interactive" },
	{ "4Q", "Disney" },
	{ "4S", "Black Pearl" },
	{ "4U", "Advanced Productions" },
	{ "4X", "GT Interactive" },
	{ "4Y", "RARE" },
	{ "4Z", "Crave Entertainment" },
	{ "50", "Absolute Entertainment" },
	{ "51", "Acclaim" },
	{ "52", "Activision" },
	{ "53", "American Sammy Corp." },
	{ "54", "Take 2 Interactive" },
	{ "55", "Hi Tech" },
	{ "56", "LJN LTD." },
	{ "58", "Mattel" },
	{ "5A", "Mindscape/Red Orb Ent." },
	{ "5B", "Romstar" },
	{ "5C", "Taxan" },
	{ "5D", "Midway" },
	{ "5F", "American Softworks" },
	{ "5G", "Majesco Sales Inc" },
	{ "5H", "3DO" },
	{ "5K", "Hasbro" },
	{ "5L", "NewKidCo" },
	{ "5M", "Telegames" },
	{ "5N", "Metro3D" },
	{ "5P", "Vatical Entertainment" },
	{ "5Q", "LEGO Media" },
	{ "5S", "Xicat Interactive" },
	{ "5T", "Cryo Interactive" },
	{ "5W", "Red Storm Ent./BKN Ent." },
	{ "5X", "Microids" },
	{ "5Z", "Conspiracy Entertainment Corp." },
	{ "60", "Titus Interactive Studios" },
	{ "61", "Virgin Interactive" },
	{ "62", "Maxis" },
	{ "64", "LucasArts Entertainment" },
	{ "67", "Ocean" },
	{ "69", "Electronic Arts" },
	{ "6B", "Laser Beam" },
	{ "6E", "Elite Systems Ltd." },
	{ "6F", "Electro Brain" },
	{ "6G", "The Learning Company" },
	{ "6H", "BBC" },
	{ "6J", "Software 2000" },
	{ "6L", "BAM! Entertainment" },
	{ "6M", "Studio 3" },
	{ "6Q", "Classified Games" },
	{ "6S", "TDK Mediactive" },
	{ "6U", "DreamCatcher" },
	{ "6V", "JoWood Productions" },
	{ "6W", "SEGA" },
	{ "6X", "Wannado Edition" },
	{ "6Y", "LSP" },
	{ "6Z", "ITE Media" },
	{ "70", "Infogrames" },
	{ "71", "Interplay" },
	{ "72", "JVC Musical Industries Inc" },
	{ "73", "Parker Brothers" },
	{ "75", "SCI" },
	{ "78", "THQ" },
	{ "79", "Accolade" },
	{ "7A", "Triffix Ent. Inc." },
	{ "7C", "Microprose Software" },
	{ "7D", "Universal Interactive Studios" },
	{ "7F", "Kemco" },
	{ "7G", "Rage Software" },
	{ "7H", "Encore" },
	{ "7J", "Zoo" },
	{ "7K", "BVM" },
	{ "7L", "Simon & Schuster Interactive" },
	{ "7M", "Asmik Ace Entertainment Inc./AIA" },
	{ "7N", "Empire Interactive" },
	{ "7Q", "Jester Interactive" },
	{ "7T", "Scholastic" },
	{ "7U", "Ignition Entertainment" },
	{ "7W", "Stadlbauer" },
	{ "80", "Misawa" },
	{ "81", "Teichiku" },
	{ "82", "Namco Ltd." },
	{ "83", "LOZC" },
	{ "84", "KOEI" },
	{ "86", "Tokuma Shoten Intermedia" },
	{ "87", "Tsukuda Original" },
	{ "88", "DATAM-Polystar" },
	{ "8B", "Bulletproof Software" },
	{ "8C", "Vic Tokai Inc." },
	{ "8E", "Character Soft" },
	{ "8F", "I'Max" },
	{ "8G", "Saurus" },
	{ "8J", "General Entertainment" },
	{ "8N", "Success" },
	{ "8P", "SEGA Japan" },
	{ "90", "Takara Amusement" },
	{ "91", "Chun Soft" },
	{ "92", "Video System" },
	{ "93", "BEC" },
	{ "95", "Varie" },
	{ "96", "Yonezawa/S'pal" },
	{ "97", "Kaneko" },
	{ "99", "Victor Interactive Software" },
	{ "9A", "Nichibutsu/Nihon Bussan" },
	{ "9B", "Tecmo" },
	{ "9C", "Imagineer" },
	{ "9F", "Nova" },
	{ "9G", "Den'Z" },
	{ "9H", "Bottom Up" },
	{ "9J", "TGL" },
	{ "9L", "Hasbro Japan" },
	{ "9N", "Marvelous Entertainment" },
	{ "9P", "Keynet Inc." },
	{ "9Q", "Hands-On Entertainment" },
	{ "A0", "Telenet" },
	{ "A1", "Hori" },
	{ "A4", "Konami" },
	{ "A5", "K.Amusement Leasing Co." },
	{ "A6", "Kawada" },
	{ "A7", "Takara" },
	{ "A9", "Technos Japan Corp." },
	{ "AA", "JVC" },
	{ "AC", "Toei Animation" },
	{ "AD", "Toho" },
	{ "AF", "Namco" },
	{ "AG", "Media Rings Corporation" },
	{ "AH", "J-Wing" },
	{ "AJ", "Pioneer LDC" },
	{ "AK", "KID" },
	{ "AL", "MediaFactory" },
	{ "AP", "Infogrames Hudson" },
	{ "AQ", "Kiratto. Ludic Inc" },
	{ "B0", "Acclaim Japan" },
	{ "B1", "ASCII" },
	{ "B2", "Bandai" },
	{ "B4", "Enix" },
	{ "B6", "HAL Laboratory" },
	{ "B7", "SNK" },
	{ "B9", "Pony Canyon Hanbai" },
	{ "BA", "Culture Brain" },
	{ "BB", "Sunsoft" },
	{ "BC", "Toshiba EMI" },
	{ "BD", "Sony Imagesoft" },
	{ "BF", "Sammy" },
	{ "BG", "Magical" },
	{ "BH", "Visco" },
	{ "BJ", "Compile" },
	{ "BL", "MTO Inc." },
	{ "BN", "Sunrise Interactive" },
	{ "BP", "Global A Entertainment" },
	{ "BQ", "Fuuki" },
	{ "C0", "Taito" },
	{ "C2", "Kemco" },
	{ "C3", "Square Soft" },
	{ "C4", "Tokuma Shoten" },
	{ "C5", "Data East" },
	{ "C6", "Tonkin House" },
	{ "C8", "Koei" },
	{ "CA", "Konami/Palcom/Ultra" },
	{ "CB", "Vapinc/NTVIC" },
	{ "CC", "Use Co.,Ltd." },
	{ "CD", "Meldac" },
	{ "CE", "FCI/Pony Canyon" },
	{ "CF", "Angel" },
	{ "CJ", "Boss" },
	{ "CG", "Yumedia/Aroma Co., Ltd" },
	{ "CM", "Konami Computer Entertainment Osaka" },
	{ "CP", "Enterbrain" },
	{ "D0", "Taito/Disco" },
	{ "D1", "Sofel" },
	{ "D2", "Quest" },
	{ "D3", "Sigma Enterprises" },
	{ "D4", "Ask Kodansa" },
	{ "D6", "Naxat" },
	{ "D7", "Copya System" },
	{ "D8", "Capcom Co., Ltd." },
	{ "D9", "Banpresto" },
	{ "DA", "TOMY" },
	{ "DB", "LJN Japan" },
	{ "DD", "NCS" },
	{ "DE", "Human Entertainment" },
	{ "DF", "Altron Corporation" },
	{ "DH", "Gaps Inc." },
	{ "DN", "ELF" },
	{ "E0", "Jaleco" },
	{ "E2", "Yutaka" },
	{ "E3", "Varie" },
	{ "E4", "T&ESoft" },
	{ "E5", "Epoch" },
	{ "E7", "Athena" },
	{ "E8", "Asmik Ace Entertainment Inc." },
	{ "E9", "Natsume" },
	{ "EA", "King Records" },
	{ "EB", "Atlus" },
	{ "EC", "Epic/Sony Records" },
	{ "EE", "IGS" },
	{ "EG", "Chatnoir" },
	{ "EH", "Right Stuff" },
	{ "EL", "Spike" },
	{ "EM", "Konami Computer Entertainment Tokyo" },
	{ "EN", "Alphadream Corporation" },
	{ "F0", "A Wave" },
	{ "F1", "Motown Software" },
	{ "F2", "Left Field Entertainment" },
	{ "F3", "Extreme Ent. Grp." },
	{ "F4", "TecMagik" },
	{ "F9", "Cybersoft" },
	{ "FB", "Psygnosis" },
	{ "FE", "Davidson/Western Tech." },
	{ "G1", "PCCW" },
	{ "G4", "KiKi Co Ltd" },
	{ "G5", "Open Sesame Inc." },
	{ "G6", "Sims" },
	{ "G7", "Broccoli" },
	{ "G8", "Avex" },
	{ "G9", "D3 Publisher" },
	{ "GB", "Konami Computer Entertainment Japan" },
	{ "GD", "Square-Enix" },
	{ "H2", "Aruze Corp." },
	{ "HY", "Sachen" },
	{ "IH", "Yojigen" },
	{ NULL, NULL }
};


struct OPENDATA {
	CT2StatusBar* pPro;
	CArchive *pAr;
	BYTE* pRom;
	UINT nSize;
};

UINT OpenFile(LPVOID lParam)
{
	OPENDATA& od = *(OPENDATA*)lParam;
	UINT nRomSize = od.nSize;
	BYTE* pRom=od.pRom;
	UINT nBufferSize=1024*1024;
	CArchive& loadArchive = *od.pAr;
	CT2StatusBar& pro = *od.pPro;
	g_bBreak = FALSE;
	while(!g_bBreak && nRomSize)
	{
		if(nRomSize<1024*1024) nBufferSize = nRomSize;
		loadArchive.Read(pRom, nBufferSize);
		pRom+=nBufferSize;
		nRomSize-=nBufferSize;
		pro.SetProgressPos( (BYTE)(((LONGLONG)(od.nSize-nRomSize))*100/od.nSize) );
	}

	if(nRomSize)
	{
		Hint(IDS_BREAKOPEN);
	}
	pro.EndModalLoop(IDCANCEL);
	return 0;
}

UINT SaveFile(LPVOID lParam)
{
	OPENDATA& od = *(OPENDATA*)lParam;
	UINT nRomSize = od.nSize;
	BYTE* pRom=od.pRom;
	UINT nBufferSize=1024*1024;
	CArchive& saveArchive = *od.pAr;
	CT2StatusBar& pro = *od.pPro;
	g_bBreak=FALSE;
	while(!g_bBreak && nRomSize)
	{
		if(nRomSize<1024*1024) nBufferSize = nRomSize;
		saveArchive.Write(pRom, nBufferSize);
		pRom+=nBufferSize;
		nRomSize-=nBufferSize;
		pro.SetProgressPos( (BYTE)(((LONGLONG)(od.nSize-nRomSize))*100/od.nSize) );
	}

	if(nRomSize)
		Hint(IDS_BREAKSAVE);

	pro.EndModalLoop(IDCANCEL);
	return 0;
}

// CT2Doc

IMPLEMENT_DYNCREATE(CT2Doc, CDocument)

BEGIN_MESSAGE_MAP(CT2Doc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_COMMAND(ID_TBL_SELTBL, OnTblSeltbl)
	ON_COMMAND_RANGE(ID_CODEPAGE_ACP, ID_CODEPAGE_ACP+100, OnCodepageAcp)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CODEPAGE_ACP, ID_CODEPAGE_ACP+100, OnUpdateCodepageAcp)
	ON_BN_CLICKED(IDC_FAVORITES_ADDBTN, OnBnClickedFavoritesAddbtn)
	ON_COMMAND(ID_EDIT_MEMREFALSH, OnEditMemrefalsh)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MEMREFALSH, OnUpdateEditMemrefalsh)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_TOOLS_CHEATSSERACH, OnToolsCheatsserach)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CHEATSSERACH, OnUpdateToolsCheatsserach)
END_MESSAGE_MAP()


// CT2Doc 构造/析构

CT2Doc::CT2Doc()
: m_pRom(NULL)
, m_nRomSize(0)
, m_nFileSize(0)
, m_hMapFile(NULL)
, m_pPal(NULL)
, m_strRomInfo(_T(""))
, m_nReadOnly(0)
, m_nEndCodeLen(1)
, m_nEndCode(0)
, m_wLastCode(0)
, m_nTblCodePage(GetACP())
, m_nScriptCodePage(GetACP())
, m_nDefaultChar(_T('?'))
, m_nRomType(CT2_RT_NORMAL)
, m_nBlockOffset(0)
, m_strScriptBegin(_T("\r\n------------原文------------\r\n"))
, m_strCHN(_T("\r\n------------译文------------\r\n"))
, m_strScriptEnd(_T("\r\n------------结束------------\r\n"))
, m_nCurCp(0)
{
}

CT2Doc::~CT2Doc()
{
	// DeleteContents();
}
/*
BOOL CT2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}


// CT2Doc 序列化

void CT2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}
*/

// CT2Doc 诊断

#ifdef _DEBUG
void CT2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CT2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CT2Doc 命令

BOOL CT2Doc::OnOpenDocument(LPCTSTR lpszPathName)
{
	m_nOpenMapFile = ::m_nOpenMapFile;
	return OpenRom(lpszPathName);
}

BOOL CT2Doc::OnSaveDocument(LPCTSTR lpszPathName)
{
	int i=0;
	UINT nOff=0;
	while(i<3)
	{
		if(m_strMFileName[i].IsEmpty()) break;

		CFile* pFile = NULL;
		pFile = GetFile(m_strMFileName[i], CFile::modeCreate |
			CFile::modeReadWrite | CFile::shareExclusive, NULL);

		if (pFile == NULL)
		{
			ReportSaveLoadException(m_strMFileName[i], NULL,
				TRUE, AFX_IDP_INVALID_FILENAME);
			return FALSE;
		}

		CArchive saveArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
		saveArchive.m_pDocument = this;
		saveArchive.m_bForceFlat = FALSE;

		DoOpenSaveFile(&saveArchive, m_pRom+nOff, m_nMFileSize[i], IDS_SAVEFILE, FALSE);
		nOff += m_nMFileSize[i];

		if(GetPathName().CompareNoCase(m_strMFileName[i])==0)
			::GetFileTime(pFile->m_hFile, NULL, NULL, &m_ftTime);

		saveArchive.Close();
		ReleaseFile(pFile, FALSE);

		i++;
	}
	SetModifiedFlag(FALSE);     // back to unmodified

	while(!m_UndoList.IsEmpty())
	{
		UNDO& undo=m_UndoList.GetHead();
		if(undo.nSize>4) delete[] undo.pData;
		m_UndoList.RemoveHead();
	}

	return TRUE;        // success
	// return CDocument::OnSaveDocument(lpszPathName);
}

CView* CT2Doc::GetLastView(void)
{
	return (CView*)m_viewList.GetTail();
}

void CT2Doc::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bModified && m_nRomType!=CT2_RT_PCMEM);
}

void CT2Doc::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_hMapFile);
}

void CT2Doc::OnCloseDocument()
{
	if(m_nRomType==CT2_RT_PCMEM)
	{
		SetModifiedFlag(FALSE);
		m_Cheats.DestroyWindow();
	}

	UINT nIndex = theApp.GetMRUIndex(GetPathName());
	if(nIndex!=-1)
	{
		theApp.m_pRecentStatList->SetOK(nIndex);
		STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
		if(pSD)
		{
			lstrcpy(pSD->lpszTblName, m_strTblName);
			pSD->nTblCodePage = (WORD)m_nTblCodePage;
			pSD->nScriptCodePage = m_nScriptCodePage;
			pSD->nBlockOffset = m_nBlockOffset;
		}
	}
	CDocument::OnCloseDocument();
}

BOOL CT2Doc::CanCloseFrame(CFrameWnd* pFrame)
{
	if(!m_UndoList.IsEmpty() && m_hMapFile)
	{
		CString strHint;
		strHint.LoadString(IDS_SAVEAS);
		CString strFileName;
		strFileName.Format(_T("%s\n%s"), GetPathName(), strHint);
		if(Hint(strFileName, MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDNO)
			return FALSE;
	}

	return CDocument::CanCloseFrame(pFrame);
}

void CT2Doc::DoOpenSaveFile(CArchive *pAr, BYTE* pRom, UINT nSize, UINT nRes, BOOL bOpen)
{
	OPENDATA od;
	od.pPro = (CT2StatusBar*) theApp.GetStatusBar();
	od.pPro->BeginProgress(nRes);

	od.pAr = pAr;
	od.pRom = pRom;
	od.nSize = nSize;

	CWinThread *pOpenThread = bOpen?
		AfxBeginThread(OpenFile,
		&od, THREAD_PRIORITY_IDLE, 1024, 0, NULL):
		AfxBeginThread(SaveFile,
		&od, THREAD_PRIORITY_IDLE, 1024, 0, NULL);
	od.pPro->RunModalLoop();

	od.pPro->EndProgress();
}

BOOL CT2Doc::LoadTbl(CString strTblName, BYTE nImTbl)
{
	LPTSTR lpUnicodeBuffer = GetUText(strTblName, m_nTblCodePage);
	if(!lpUnicodeBuffer) return FALSE;

	//AddMacro_LoadTbl(strTblName, nImTbl);

	m_strTblName = strTblName;
	for(int i=0; i<0x10000; i++)
	{
		m_DTBL[i].Empty();
		m_STBL[i].Empty();
	}
	m_VisualScript.RemoveAll();

	LPTSTR lpTbl = lpUnicodeBuffer;
	LPTSTR lpEnd = lpTbl+lstrlen(lpTbl);
	LPTSTR lpTblList = lpTbl;

	if(*(WORD*)lpTbl==0xFEFF) lpTbl = (LPTSTR)((BYTE*)lpTbl+2);

	BYTE nFirst = 1;
	int nTCode;
	TCHAR T[] = _T("\t"); TCHAR N[] = _T("\r\n");
	CString tmp;
	while(*lpTbl)
	{
		if(nFirst)
		{
			if(*lpTbl==_T(';')) // 注释行
				goto skip;

			if(*lpTbl==_T('<'))
			{	// 可视化控制符
				lpTbl++;
				if(!*lpTbl) goto skip;

				if( *(UINT*)lpTbl == 0x003D0062 || // B
					*(UINT*)lpTbl == 0x003D0042 )
				{
					lpTbl+=2;
					if(!*lpTbl) goto skip;
					tmp = GetLine(lpTbl);
					m_strScriptBegin = tmp;//GetLine(lpTbl);
					m_strScriptBegin.Replace(_T("\\n"), N);
					m_strScriptBegin.Replace(_T("\\t"), T);
					continue;
				}else
				if( *(UINT*)lpTbl == 0x003D0063 || // C
					*(UINT*)lpTbl == 0x003D0043 )
				{
					lpTbl+=2;
					if(!*lpTbl) goto skip;
					tmp = GetLine(lpTbl);
					m_strCHN = tmp;
					m_strCHN.Replace(_T("\\n"), N);
					m_strCHN.Replace(_T("\\t"), T);
					continue;
				}else
				if( *(UINT*)lpTbl == 0x003D0065 || // E
					*(UINT*)lpTbl == 0x003D0045 )
				{
					lpTbl+=2;
					if(!*lpTbl) goto skip;
					tmp = GetLine(lpTbl);
					m_strScriptEnd = tmp;
					m_strScriptEnd.Replace(_T("\\n"), N);
					m_strScriptEnd.Replace(_T("\\t"), T);
					continue;
				}else
				{
					//lpTbl++;
					CString l = GetLine(lpTbl);
					int pos = l.Find(_T('='));
					if(pos==-1) continue;
					VISUALSCRIPT vs;
					vs.strCtrl = l.Left(pos);
					vs.strVisual = l.Right(l.GetLength()-pos-1);
					vs.strVisual.Replace(_T("\\n"), N);
					vs.strVisual.Replace(_T("\\t"), T);
					if(vs.strCtrl.IsEmpty() || vs.strVisual.IsEmpty()) continue;
					m_VisualScript.AddTail(vs);
					continue;
				}
			}else
			if(*lpTbl==_T('>'))
			{	// 控制符
				lpTbl++;
				if(lpTbl+4<lpEnd)
				{//>0000
					CString strCode;
					strCode.Append(lpTbl, 4);
					nTCode = StrToIntEX(strCode);
					if(nTCode==-1) goto sc;

					nTCode = (WORD)(nTCode>>8 | nTCode<<8);

					lpTbl+=4;
					UINT nLen = 1;
					if( (lpTbl+1<lpEnd) &&
						(*lpTbl==_T(',')) )
					{//>0000,99
						lpTbl++;
						strCode.Empty();
						strCode.Append(lpTbl, 2);
						nLen = strCode.GetAt(1);
						if( nLen<'0' || nLen>'9' )
							strCode.Delete(1);
						nLen = StrToIntEX(strCode, FALSE);
						if(nLen<0 || nLen>99) nLen = 1;
					}
					if(!nLen)
					{
						m_nEndCode = nTCode;
						m_nEndCodeLen = 2;
					}
					if(nLen==1)
						m_DTBL[nTCode].Format(_T("[%s]"), strCode);
					else
						m_DTBL[nTCode].Format(_T("\n%02d"), nLen);

				}else
				if(lpTbl+2<lpEnd)
				{//>00
					sc:
					CString strCode;
					strCode.Append(lpTbl, 2);
					nTCode = StrToIntEX(strCode);
					if(nTCode!=-1)
					{
						lpTbl+=2;
						UINT nLen = 1;
						if( (lpTbl+1<lpEnd) &&
							(*lpTbl==_T(',')) )
						{//>00,99
							lpTbl++;
							strCode.Empty();
							strCode.Append(lpTbl, 2);
							if( strCode.GetAt(1)<_T('0'))
								strCode.Delete(1);
							nLen = StrToIntEX(strCode, FALSE);
							if(nLen<0 || nLen>99) nLen = 1;
						}
						if(!nLen)
						{
							m_nEndCode = nTCode;
							m_nEndCodeLen = 1;
						}
						m_STBL[nTCode].Format(_T("\n%d"), nLen);
					}
				}
				goto skip;
			}
		}
		if( (*lpTbl==_T('\n')) || (*lpTbl==_T('\r')) )
			goto skip;
		if(*lpTbl!=_T('='))
		{
			lpTbl++;
			nFirst = 0;
			continue;
		}
		lpTbl++;
		if( (*lpTbl) && (*lpTbl!=_T('\r')) && (*lpTbl!=_T('\n')) )
		{
			if((lpTbl-5)>=lpUnicodeBuffer)
			{// DTBL
				CString strCode;
				strCode.Append(lpTbl-5, 4);
				nTCode = StrToIntEX(strCode, TRUE);
				if(nTCode!=-1)
				{
					if(nImTbl)
						m_DTBL[*lpTbl].Format(_T("%02X%02X"), nTCode&0xFF, nTCode>>8);
					else
						m_DTBL[nTCode] = *lpTbl;
					m_wLastCode = nTCode;
				}else
					goto STBL;
			}else
			{
				STBL:
				CString strCode;
				if((lpTbl-3)>=lpUnicodeBuffer)
				{// STBL
					strCode.Append(lpTbl-3, 2);
					nTCode = StrToIntEX(strCode, TRUE);
					if(nTCode!=-1)
					{
						if(nImTbl)
							m_STBL[*lpTbl].Format(_T("%02X"), nTCode&0xFF);
						else
							m_STBL[nTCode] = *lpTbl;
					}
				}
			}
		}
		skip:
		nFirst = 1;
		while(*lpTbl)
		{
			if((*lpTbl==_T('\r'))||
				(*lpTbl==_T('\n')))
			{
				lpTbl++;
				if((*lpTbl==_T('\n'))||
					(*lpTbl==_T('\r')))
					lpTbl++;
				break;
			}
			lpTbl++;
		}
	}
	*lpTblList=0;

	delete[] lpUnicodeBuffer;

	m_wLastCode++;
	//m_nImTbl = nImTbl;
	return TRUE;
}

BOOL CT2Doc::OpenRom(LPCTSTR lpszPathName)
{
	m_nRomType = CT2_RT_PCMEM;
	LPTSTR p = _T("=>");
	if(*(UINT*)lpszPathName==*(UINT*)p)
	{
		if(m_Memory.FindDSEmu(lpszPathName+2))
			goto CT2_RT_PCMEM_OK;
		return FALSE;
	}else
	if(lstrcmp(lpszPathName, _T("->NO$GBA"))==0)
	{
		if(m_Memory.FindDSEmu(_T("No$dlgClass")))
			goto CT2_RT_PCMEM_OK;
		return FALSE;
	}else
	if(lstrcmp(lpszPathName, _T("->DeSmuME"))==0)
	{
		if(m_Memory.FindDSEmu(_T("DeSmuME")))
		{
		CT2_RT_PCMEM_OK:
			m_nReadOnly = 1;
			m_pRom = m_Memory.m_pMemory;
			m_nFileSize = m_nRomSize = m_Memory.m_nSize;

			m_strRomInfo.LoadString(IDS_DSMEM);
			return TRUE;
		}
		return FALSE;
	}

	m_nReadOnly = m_nOpenMapFile&4?TRUE:FALSE;
#ifdef _DEBUG
	if (IsModified())
		TRACE(traceAppMsg, 0, "Warning: OnOpenDocument replaces an unsaved document.\n");
#endif

	CFile* pFile = GetFile(lpszPathName,
		m_nReadOnly?CFile::modeRead:CFile::modeReadWrite|CFile::shareDenyWrite, NULL);
	if (pFile == NULL)
	{
		ReportSaveLoadException(lpszPathName, NULL,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	DeleteContents();
	SetModifiedFlag();  // dirty during de-serialize

	CArchive loadArchive(pFile, CArchive::load | (m_nReadOnly?0:CArchive::store) | CArchive::bNoFlushOnDelete);
	loadArchive.m_pDocument = this;
	loadArchive.m_bForceFlat = FALSE;

	m_nRomType = CT2_RT_NORMAL;
	m_nFileSize = m_nRomSize = (UINT)pFile->GetLength();
	if(m_nRomSize)
	{
		//Serialize(loadArchive);     // load me
		BYTE nStateMapFile = 0;
		UINT nIndex = theApp.GetMRUIndex(lpszPathName);
		if(theApp.m_pRecentStatList->IsOK(nIndex))
		{
			STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
			nStateMapFile = pSD->nStateMapFile;
			m_nBlockOffset = pSD->nBlockOffset;

			m_nScriptCodePage = pSD->nScriptCodePage;
			for(m_nCurCp=0; m_nCurCp<theApp.m_nCPCount; m_nCurCp++)
				if(theApp.m_pCodePage[m_nCurCp]==m_nScriptCodePage)
				{
					m_nCurCp++;
					break;
				}
			CPINFO cpInfo;
			memset(&cpInfo, 0, sizeof(CPINFO));
			GetCPInfo(m_nScriptCodePage, &cpInfo);
			m_nDefaultChar = *(WORD*)cpInfo.DefaultChar;

			if(m_nDefaultChar==0)
				m_nDefaultChar = _T('?');

			m_strTblName = pSD->lpszTblName;
			m_nTblCodePage = pSD->nTblCodePage;
			LoadTbl(m_strTblName);
		}else
		{
			m_nCurCp = theApp.GetProfileInt(CT2Settings, _T("DefCp"), 0);
			if(m_nCurCp!=0)
				m_nTblCodePage = m_nScriptCodePage = theApp.m_pCodePage[m_nCurCp];
		}
		VISUALSCRIPT vs;
		vs.strCtrl = _T("[0A]");
		vs.strVisual = _T("\r\n");
		m_VisualScript.AddTail(vs);

		MEMORYSTATUS ms;
		GlobalMemoryStatus(&ms);

		if( (m_nOpenMapFile&0x1) ||
			(!(m_nOpenMapFile&0x02) && nStateMapFile) ||
			(m_nRomSize>(ms.dwAvailPhys-0x00400000)) )
		{// 映射模式
			m_hMapFile = ::CreateFileMapping(pFile->m_hFile, NULL,
				m_nReadOnly?PAGE_READONLY:PAGE_READWRITE,
				0, m_nRomSize, NULL);
			if(m_hMapFile==NULL)
			{
				ShowError();
				return FALSE;
			}
			UINT off=m_nBlockOffset;
			Seek(off);
			if(m_pRom==NULL)
			{
				ShowError();
				return FALSE;
			}
		}else
		{// 内存模式
			if( g_nRomType == CT2_RT_MFILE ||
				g_nRomType == CT2_RT_NSCR )
			{
				m_nRomType = g_nRomType;
				m_pRom = new BYTE[g_nMFileSize];
				if(m_pRom==NULL) {ShowError();return FALSE;};
				int i=0;
				UINT nOff=0;
				while(i<3)
				{
					AfxExtractSubString(m_strMFileName[i], g_strMFile, i);
					if(m_strMFileName[i].IsEmpty()) break;
					if(m_strMFileName[i].CompareNoCase(lpszPathName)==0)
					{
						m_nMFileSize[i] = m_nRomSize;
						DoOpenSaveFile(&loadArchive,
							m_pRom+nOff, m_nRomSize, IDS_OPENFILE);
						nOff+=m_nRomSize;
					}else
					{
						CFile f;
						if(!f.Open(m_strMFileName[i], CFile::modeRead|CFile::typeBinary))
							{ShowError();delete[] m_pRom;return FALSE;};
						m_nMFileSize[i] = (UINT)f.GetLength();
						CArchive a(&f, CArchive::load | CArchive::bNoFlushOnDelete);
						DoOpenSaveFile(&a,
							m_pRom+nOff, m_nMFileSize[i], IDS_OPENFILE);
						nOff+=m_nMFileSize[i];
					}
					i++;

				}
				m_nFileSize = m_nRomSize = nOff;
			}else
			{
				m_pRom = new BYTE[m_nRomSize];

				m_nMFileSize[0] = m_nRomSize;
				m_strMFileName[0] = lpszPathName;

				if(m_pRom==NULL) {ShowError();return FALSE;};
				DoOpenSaveFile(&loadArchive,
					m_pRom, m_nRomSize, IDS_OPENFILE);
			}
			//m_pEnd = m_pRom+m_nRomSize;
		}
	}else
		return FALSE;
	::m_nOpenMapFile = 0;

	::GetFileTime(pFile->m_hFile, NULL, NULL, &m_ftTime);

	loadArchive.Close();
	ReleaseFile(pFile, FALSE);

	SetModifiedFlag(FALSE);     // start off with unmodified

	// CompanyName
	CString t = PathFindExtension(lpszPathName);
	t.MakeLower();

	if((t==_T(".gba")) && (m_nRomSize>=sizeof(GBAHEADER)))
	{
		GBAHEADER *gbaHeader=(GBAHEADER*)m_pRom;
		LPTSTR lpDst = t.GetBuffer(_MAX_PATH);
		::OemToCharBuff(gbaHeader->GameTitle, lpDst, 12);
		lpDst[12]=0;
		m_strRomInfo.Format(_T("%s"), lpDst);
		::OemToCharBuff(gbaHeader->GameCode, lpDst, 4);
		lpDst[4]=0;
		if(*lpDst)
			m_strRomInfo.AppendFormat(_T(" AGB-%s"), lpDst);
		int i=0;
		while(Countries[i].code)
		{
			if(Countries[i].code==lpDst[3])
			{
			#ifdef UNICODE
				::OemToChar(Countries[i].country, lpDst);
				m_strRomInfo.AppendFormat(_T(" %s"), lpDst);
			#else
				m_strRomInfo.AppendFormat(_T(" %s"), Countries[i].country);
			#endif
				break;
			}i++;
		}
		i=0;
		while(GbaInfo[i].Code)
		{
			if(!memcmp(GbaInfo[i].Code, gbaHeader->MakerCode, 2))
			{
			#ifdef UNICODE
				::OemToChar(GbaInfo[i].CompanyName, lpDst);
				m_strRomInfo.AppendFormat(_T(" %s"), lpDst);
			#else
				m_strRomInfo.AppendFormat(_T(" %s"), GbaInfo[i].CompanyName);
			#endif
				break;
			}i++;
		}
		t.ReleaseBuffer();
		m_nRomType = CT2_RT_GBA;
	}else
	if(((t==_T(".nds"))||(t==_T(".ids"))||(t==_T(".ds"))||(t==_T(".srl"))) && (m_nRomSize>=sizeof(NDSHEADER)))
	{
		NDSHEADER *ndsHeader=(NDSHEADER*)m_pRom;
		// 简单的NDS头部有效性检查
		if( !(GetKeyState(VK_SHIFT)&0x80) && (
			ndsHeader->Application_End_Offset > m_nFileSize ||
			ndsHeader->Arm7_Overlay_Offset+ndsHeader->Arm7_Overlay_Size > m_nFileSize ||
			ndsHeader->Arm7_Rom_Offset+ndsHeader->Arm7_Size > m_nFileSize ||
			ndsHeader->Arm9_Overlay_Offset+ndsHeader->Arm9_Overlay_Size > m_nFileSize ||
			ndsHeader->Arm9_Rom_Offset+ndsHeader->Arm9_Size > m_nFileSize ||
			ndsHeader->Banner_Offset > m_nFileSize ||
			ndsHeader->Fat_Offset+ndsHeader->Fat_Size > m_nFileSize ||
			ndsHeader->Fnt_Offset+ndsHeader->Fnt_Size > m_nFileSize ) )
			goto NormalRom;

		LPTSTR lpDst = t.GetBuffer(_MAX_PATH);
		::OemToCharBuff(ndsHeader->GameTitle, lpDst, 12);
		lpDst[12]=0;
		m_strRomInfo.Format(_T("%s"), lpDst);
		::OemToCharBuff(ndsHeader->GameCode, lpDst, 4);
		lpDst[4]=0;
		if(*lpDst)
			m_strRomInfo.AppendFormat(_T(" NTR-%s"), lpDst);
		int i=0;
		while(Countries[i].code)
		{
			if(Countries[i].code==lpDst[3])
			{
			#ifdef UNICODE
				::OemToChar(Countries[i].country, lpDst);
				m_strRomInfo.AppendFormat(_T(" %s"), lpDst);
			#else
				m_strRomInfo.AppendFormat(_T(" %s"), Countries[i].country);
			#endif
				break;
			}i++;
		}
		i=0;
		while(GbaInfo[i].Code)
		{
			if(!memcmp(GbaInfo[i].Code, ndsHeader->MakerCode, 2))
			{
			#ifdef UNICODE
				::OemToChar(GbaInfo[i].CompanyName, lpDst);
				m_strRomInfo.AppendFormat(_T(" %s"), lpDst);
			#else
				m_strRomInfo.AppendFormat(_T(" %s"), GbaInfo[i].CompanyName);
			#endif
				break;
			}i++;
		}
		t.ReleaseBuffer();

		m_nRomType = CT2_RT_NDS;
		SetTitle(PathFindFileName(lpszPathName));
		CT2ChildFrm *pChild = (CT2ChildFrm*)GetLastView()->GetParent();
		CNdsFileSystemInfo &Fsi = pChild->m_NdsFSI;
		if(Fsi.GetSafeHwnd()==NULL)
			Fsi.Create(MAKEINTRESOURCE(CNdsFileSystemInfo::IDD), pChild);
	}else
	if( (t==_T(".z64")) || (t==_T(".n64")) || (t==_T(".v64")) )
	{
		LPTSTR lpDst = t.GetBuffer(_MAX_PATH);
		::OemToCharBuff((LPCSTR)(m_pRom+32), lpDst, 27);
		lpDst[27]=0;
		m_strRomInfo.Format(_T("%s"), lpDst);

		::OemToCharBuff((LPCSTR)(m_pRom+59), lpDst, 4);
		lpDst[4]=0;
		if(*lpDst)
			m_strRomInfo.AppendFormat(_T(" N64-%s"), lpDst);

		int i=0;
		while(Countries[i].code)
		{
			if(Countries[i].code==lpDst[3])
			{
			#ifdef UNICODE
				::OemToChar(Countries[i].country, lpDst);
				m_strRomInfo.AppendFormat(_T(" %s"), lpDst);
			#else
				m_strRomInfo.AppendFormat(_T(" %s"), Countries[i].country);
			#endif
				break;
			}i++;
		}

		t.ReleaseBuffer();
		m_nRomType = CT2_RT_N64;
	}
NormalRom:
	return m_pRom?TRUE:FALSE;
}

void CT2Doc::OnTblSeltbl()
{
	static CTextFileDialog fd(TRUE, IDS_TBLFILTER);
	if(fd.DoModal()==IDOK)
	{
		m_nTblCodePage = fd.m_nCodePage;
		LoadTbl(fd.GetPathName());
	}
}

void CT2Doc::OnCodepageAcp(UINT nID)
{
	m_nCurCp = nID-ID_CODEPAGE_ACP;
	UINT nCP;
	if(!m_nCurCp)
		nCP = GetACP();
	else
		nCP = theApp.m_pCodePage[m_nCurCp-1];
	if(nCP==m_nScriptCodePage) return;
	m_nScriptCodePage = nCP;

	CPINFO cpInfo;
	if((m_nScriptCodePage==1200)||(m_nScriptCodePage==1201))
		m_nDefaultChar = _T('?');
	else
	{
		GetCPInfo(m_nScriptCodePage, &cpInfo);
		m_nDefaultChar = *(WORD*)cpInfo.DefaultChar;
	}
	theApp.GetCurView()->OnUpdateData();
}

void CT2Doc::OnUpdateCodepageAcp(CCmdUI *pCmdUI)
{
	/*UINT i=0;
	for(; i<theApp.m_nCPCount; i++)
	{
		if(theApp.m_pCodePage[i]==m_nScriptCodePage)
			break;
	}*/
	pCmdUI->SetRadio((pCmdUI->m_nID-ID_CODEPAGE_ACP)==m_nCurCp);
}

void CT2Doc::DeleteContents()
{
	if(m_hMapFile)
	{
		UnmapViewOfFile(m_pRom);
		m_pRom = NULL;
		CloseHandle(m_hMapFile);
		m_hMapFile = NULL;
	}else
	if(m_pRom)
	{
		if(m_nRomType==CT2_RT_PCMEM)
		{
			m_Memory.Close();
		}else
		{
			delete[]  m_pRom;
			m_pRom=NULL;
		}
	}
	while(!m_UndoList.IsEmpty())
	{
		UNDO& undo=m_UndoList.GetHead();
		if(undo.nSize>4) delete[] undo.pData;
		m_UndoList.RemoveHead();
	}
	//m_VisualScript.RemoveAll();

	CDocument::DeleteContents();
}
/*
void CT2Doc::OnCpList()
{
	UINT nType;
	theApp.GetCurView(&nType);
	char nMax = GetLastView()->GetParent()->IsZoomed() && nType==ID_VIEW_HEXVIEW?1:0;
 	HMENU hHex = GetSubMenu(theApp.m_hHexMenu, 3+nMax);
	HMENU hCP = GetSubMenu(hHex, 5);
	CCrystalBar *pBar = theApp.GetExplorerBox();
	CRect rc;
	pBar->GetDlgItem(IDC_CPLIST)->GetClientRect(&rc);
	pBar->GetDlgItem(IDC_CPLIST)->ClientToScreen(&rc);

	OnInitMenuPopup(theApp.m_pMainWnd, hCP, TRUE);
	TrackPopupMenu(hCP, TPM_LEFTBUTTON, rc.left, rc.bottom, 0,
		pBar->m_hWnd, NULL);
}

void CT2Doc::OnUpdateCpList(CCmdUI *pCmdUI)
{
	UINT i;
	CT2View *pView=theApp.GetCurView(&i);
	int nTblMode = 0;
	if(i==ID_VIEW_HEXVIEW)
		nTblMode = !((CT2HexView*)pView)->m_nTblMode;
	else
	if(i==ID_VIEW_SCRIPTVIEW)
		nTblMode = !((CT2ScriptView*)pView)->m_nTblMode;
	pCmdUI->Enable( nTblMode );
}
*/
CString CT2Doc::OffsetToPathName(UINT nOffset, POINT *pPoint)
{
	CString name;
	CT2ChildFrm *pChild = (CT2ChildFrm*)GetLastView()->GetParent();
	if(m_nRomType==CT2_RT_NDS)
	{
		CNdsFileSystemInfo &Fsi = pChild->m_NdsFSI;

		int nCount = Fsi.m_NdsFSI.GetItemCount()-8;
		BYTE *pEnd = m_pRom+m_nRomSize;
		// SPEC FILE
		for(int i=0; i<HEADERCOUNT; i++)
		{
			if( nOffset>=Fsi.m_SpecRec[i].nTop &&
				nOffset<Fsi.m_SpecRec[i].nBottom )
			{
				if(pPoint)
				{
					pPoint->x = Fsi.m_SpecRec[i].nTop;
					pPoint->y = Fsi.m_SpecRec[i].nBottom;
				}
				name = Fsi.m_SpecRec[i].FileName;
				goto SUB;
			}
		}
		// FILE ID
		UINT fat_offset = ((NDSHEADER*)m_pRom)->Fat_Offset;
		for(int i=0; i<nCount; i++)
		{
			NDSFILEREC *rec = (NDSFILEREC*)(m_pRom+(fat_offset+(i<<3)));
			if( (BYTE*)rec<pEnd &&
				nOffset>=rec->top &&
				nOffset<rec->bottom )
			{
				if(pPoint)
				{
					pPoint->x = rec->top;
					pPoint->y = rec->bottom;
				}
				name = Fsi.m_NdsFSI.GetItemText(i+HEADERCOUNT, 0);
				break;
			}
		}
	}
SUB:
	if(pChild->m_pSFI)
	{
		CSubFileInfo *pSFI = (CSubFileInfo*)pChild->m_pSFI;
		CListCtrl &lc = pSFI->m_FileList;
		int nCount = lc.GetItemCount();
		CString t;
		char sub = pSFI->m_nSubOffset+(m_nRomType==CT2_RT_NDS ? 2:1);
		for(int i=0; i<nCount; i++)
		{
			UINT nBegin, nSize;
			t=lc.GetItemText(i, pSFI->m_nSubOffset);
			nBegin=StrToIntEX(t, TRUE);
			t=lc.GetItemText(i, sub);
			nSize=StrToIntEX(t, FALSE);
			if(nOffset>=nBegin && nOffset<(nBegin+nSize))
			{
				if(pPoint)
				{
					pPoint->x = nBegin;
					pPoint->y = nBegin+nSize;
				}
				if(name.IsEmpty())
					name = lc.GetItemText(i, 0);
				else
					name.AppendFormat(_T("->%s"), lc.GetItemText(i, 0));
				break;
			}
		}
	}else
	if( m_nRomType==CT2_RT_MFILE || m_nRomType==CT2_RT_NSCR )
	{
		UINT nOff = 0;
		for(int i=0; i<3; i++)
		{
			if( nOffset>=nOff &&
				nOffset<nOff+m_nMFileSize[i] )
			{
				if(pPoint)
				{
					pPoint->x = nOff;
					pPoint->y = nOff+m_nMFileSize[i];
				}
				name = m_strMFileName[i];
				break;
			}
			nOff += m_nMFileSize[i];
		}
	}

	return name;
}

void CT2Doc::Seek(UINT &nOffset)
{
	if((!m_hMapFile) || (nOffset>=m_nFileSize))
		return;

	UINT nBlockSize = 256*1024*1024;	// 256MBytes
	UINT nBlockOffset = nOffset/nBlockSize*nBlockSize;
	if( m_pRom && nBlockOffset==m_nBlockOffset )
		return;
	if(m_pRom) ::UnmapViewOfFile(m_pRom);

	m_nRomSize = 512*1024*1024;	// 512MBytes
	m_nBlockOffset = nBlockOffset;
	if(m_nBlockOffset+m_nRomSize>m_nFileSize)
	{
		if(m_nBlockOffset>m_nFileSize)
			m_nBlockOffset=0;
		m_nRomSize = m_nFileSize-m_nBlockOffset;
	}

	m_pRom = (BYTE*)::MapViewOfFile(m_hMapFile,
		FILE_MAP_READ|(m_nReadOnly?0:FILE_MAP_WRITE),
		0, m_nBlockOffset, m_nRomSize);
	nOffset-=m_nBlockOffset;
}

void CT2Doc::OnBnClickedFavoritesAddbtn()
{
	CDialog &wndFav = theApp.GetFavorite();

	CString Desc;
	wndFav.GetDlgItem(IDC_FAVORITES_DESC)->GetWindowText(Desc);
	if(Desc.IsEmpty()) return;

	CListCtrl *pList = (CListCtrl*)wndFav.GetDlgItem(IDC_FAVORITES_LIST);

	UINT nIndex;
	CT2View* pView = theApp.GetCurView(&nIndex);

	FAVORITES Fav;
	lstrcpyn(Fav.Doc, GetPathName(), _MAX_PATH);
	lstrcpyn(Fav.Desc, (LPCTSTR)Desc, CT2_FAVLEN);
	Fav.nOffset	= pView->m_nOffset;
	Fav.nCurView = nIndex;

	//if(nIndex==ID_VIEW_TILEVIEW)
	{
		CT2TileView *pTV = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
		memcpy(Fav.pPal, m_pPal, 256*4);
		Fav.nCurView 	= nIndex;
		Fav.nTileFormat = pTV->m_nTileFormat;
		Fav.nWidth 		= pTV->m_nWidth;
		Fav.nHeight 	= pTV->m_nHeight;
		Fav.nScale 		= pTV->m_nScale;
		Fav.nDrawMode 	= pTV->m_nDrawMode;
		Fav.nHCheck 	= pTV->m_nHCheck;
		Fav.nVCheck 	= pTV->m_nVCheck;
		Fav.nL90Check 	= pTV->m_nL90Check;
		Fav.nR90Check 	= pTV->m_nR90Check;
		Fav.n24Check 	= pTV->m_n24Check;
		Fav.nSkipSize	= pTV->m_nSkipSize;
		Fav.nPalIndex 	= pTV->m_nPalIndex;
		Fav.nColCount 	= pTV->m_nColCount;
		Fav.nLineCount 	= pTV->m_nLineCount;
		Fav.nMoOffset 	= pTV->m_nMoOffset;
		Fav.nAutoColFit	= pTV->m_nAutoColFit;

		CT2AsmView *pAV = (CT2AsmView*)theApp.GetView(ID_VIEW_ASMVIEW);
		Fav.nBaseAddress= pAV->m_nBaseAddress;
		Fav.nCpuType	= pAV->m_nCpuType;
		Fav.nAsmViewSize= pAV->m_nSize;

		CT2HexView *pHV = (CT2HexView*)theApp.GetView(ID_VIEW_HEXVIEW);
		Fav.nTblMode	= pHV->m_nTblMode;
		Fav.nSwpCode	= pHV->m_nSwpCode;

		Fav.nScriptCodePage = m_nScriptCodePage;
	}
	//pList->InsertString(0, Fav.Desc);
	pList->InsertItem(0, Fav.Desc);
	g_Favorites.AddHead(Fav);
	wndFav.GetDlgItem(IDC_FAVORITES_DESC)->SetFocus();
}

void CT2Doc::OnEditMemrefalsh()
{
	if(!m_Memory.ReLoadMemory())
	{
		SetModifiedFlag(FALSE);
		m_pRom = NULL;
		m_nFileSize = m_nRomSize = 0;
		OnCloseDocument();
		Hint(IDS_PCMEM_ERROR, MB_ICONERROR|MB_OK);
	}else
	{
        theApp.GetCurView()->Invalidate(FALSE);
	}
}

void CT2Doc::OnUpdateEditMemrefalsh(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_nRomType==CT2_RT_PCMEM &&
		m_Memory.m_pMemory!=NULL );
}

void CT2Doc::OnFileSave()
{
	if(m_nRomType==CT2_RT_PCMEM)
	{
        m_Memory.Update();
		SetModifiedFlag(FALSE);
        theApp.GetCurView()->Invalidate(FALSE);
	}else
	{
		CDocument::OnFileSave();
	}
}

void CT2Doc::OnToolsCheatsserach()
{
	if(!m_Cheats.GetSafeHwnd())
	{
		m_Cheats.Create(CT2CheatsSearch::IDD, GetLastView()->GetParent());
		m_Cheats.m_pDoc = this;
	}
	m_Cheats.ShowWindow(SW_NORMAL);
	m_Cheats.SetFocus();
}

void CT2Doc::OnUpdateToolsCheatsserach(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nRomType==CT2_RT_PCMEM);
}
/*
void CT2Doc::AddMacro_LoadTbl(CString path, BYTE nImTbl)
{
	CT2View *pV = theApp.GetCurView();
	if(!pV) return;

	CT_MACRO_1VAL_1FILE m;
	INITMACRO(m, CT2DOC_LOADTBL);

	m.nVal = nImTbl;
	lstrcpyn(m.szFile, path, _MAX_PATH);

	pV->AddMacro(&m);
}
*/