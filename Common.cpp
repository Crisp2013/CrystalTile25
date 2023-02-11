#include "stdafx.h"
#include "CrystalTile2.h"

#include "Common.h"

// 变量
CString g_strMFile;
UINT g_nMFileSize;
UINT g_nRomType;
HIMAGELIST g_hCTImageList=NULL;
TCHAR g_szBuffer[_MAX_PATH];
//TCHAR g_szBuffer2[_MAX_PATH];
CHAR g_szBufferA[_MAX_PATH];
//CHAR g_szBufferA2[_MAX_PATH];
BYTE g_bBreak=FALSE;

// 类
CWaveFile::CWaveFile()
{
	memset(&m_wf, 0, sizeof(m_wf));

	*(UINT*)m_wf.RIFF	= 0x46464952;
	*(UINT*)m_wf.WAVE	= 0x45564157;
	*(UINT*)m_wf.fmt	= 0x20746D66;
	*(UINT*)m_wd.data	= 0x61746164;

	m_wf.nFmtSize		= sizeof(m_wfx);
	memset(&m_wfx, 0, m_wf.nFmtSize);

	m_wfx.cbSize = 0;//sizeof(WAVEFORMATEX);
	m_wfx.nChannels = 2;
	m_wfx.wFormatTag = WAVE_FORMAT_PCM;
	m_wfx.nSamplesPerSec = 44100;
	m_wfx.wBitsPerSample = 16;
	m_wfx.nBlockAlign = (m_wfx.wBitsPerSample / 8) * m_wfx.nChannels;
	m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;
}

CWaveFile::~CWaveFile()
{
	Close();
}

BOOL CWaveFile::Open(CString wavename)
{
	Close();
	BOOL bRet = CFile::Open(wavename, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	if(bRet)
	{
		Write(&m_wf, sizeof(m_wf));
		Write(&m_wfx, sizeof(m_wfx));
		Write(&m_wd, sizeof(m_wd));
	}
	return bRet;
}

BOOL CWaveFile::AddSound(BYTE *pWav, UINT nSize)
{
	if(m_hFile==INVALID_HANDLE_VALUE)
		return FALSE;
	Write(pWav, nSize);
	return TRUE;
}

void CWaveFile::Close()
{
	if(m_hFile!=INVALID_HANDLE_VALUE)
	{
		m_wf.nFileSize = (UINT)GetLength()-8;
		m_wd.nWaveSize = (UINT)GetLength()-(sizeof(m_wf)+sizeof(m_wd));
		SeekToBegin();
		Write(&m_wf, sizeof(m_wf));
		Write(&m_wfx, sizeof(m_wfx));
		Write(&m_wd, sizeof(m_wd));
		CFile::Close();
	}
}

CHyperlink::CHyperlink(void)
{
	m_hCursor = LoadCursor(NULL, MAKEINTRESOURCE(32649));
}

CHyperlink::~CHyperlink(void)
{
	m_Font.DeleteObject();
	DeleteObject(m_hCursor);
}
BEGIN_MESSAGE_MAP(CHyperlink, CStatic)
	ON_CONTROL_REFLECT(STN_CLICKED, OnStnClicked)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

void CHyperlink::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();

	DWORD dwStyle = GetStyle();
	::SetWindowLongPtr(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	SetClassLongPtr(GetSafeHwnd(), GCL_HCURSOR, (LONG)(INT_PTR)m_hCursor);

	CFont *pFont = GetFont();
	LOGFONT lg;
	pFont->GetLogFont(&lg);
	lg.lfUnderline = TRUE;
	m_Font.CreateFontIndirect(&lg);
	SetFont(&m_Font);

	CString text, caption;
	GetWindowText(text);
	AfxExtractSubString(caption, text, 0);
	AfxExtractSubString(m_strLink, text, 1);
	if(m_strLink.IsEmpty())
		m_strLink = caption;
	else
	{
		SetWindowText(caption);
		CDC *pDC = GetDC();
		pDC->SelectObject(&m_Font);
		CSize szWidth = pDC->GetTextExtent(caption);
		CRect rc; GetWindowRect(&rc);
		SetWindowPos(NULL, 0, 0, szWidth.cx, rc.Height(), SWP_NOMOVE);
	}
}

void CHyperlink::OnStnClicked()
{
	::ShellExecute(0, _T("open"), m_strLink, 0, 0, SW_SHOWNORMAL);
}

HBRUSH CHyperlink::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0,0,240));
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

#pragma comment(lib, "version.lib")
CVerInfo::CVerInfo()
//: VerData(NULL)
{
	DWORD dwSize, dwVerHandle;
	CString SubBlockName;
	LPTSTR lpSubBlockName = SubBlockName.GetBuffer(_MAX_PATH);
	GetModuleFileName(NULL, lpSubBlockName, _MAX_PATH);
	dwSize = GetFileVersionInfoSize(lpSubBlockName, &dwVerHandle);
	if(dwSize)
	{
		TCHAR *lpText;
		BYTE *VerData = new BYTE[dwSize];

		GetFileVersionInfo(lpSubBlockName, dwVerHandle, dwSize, VerData);

		lstrcpy(lpSubBlockName, _T("\\VarFileInfo\\Translation"));
		VerQueryValue((LPVOID*)VerData, lpSubBlockName,
			(LPVOID*)&lpText, (UINT*)&dwSize);

		DWORD dwLang = MAKELONG(HIWORD(*(UINT*)lpText), LOWORD(*(UINT*)lpText));

		CString strFormat = _T("\\StringFileInfo\\%08lx\\%s");

		SubBlockName.Format(strFormat, dwLang, _T("ProductName"));
		VerQueryValue(VerData, lpSubBlockName,
			(LPVOID*)&lpText, (UINT*)&dwSize);
		ProductName = lpText;

		SubBlockName.Format(strFormat, dwLang, _T("FileVersion"));
		VerQueryValue(VerData, lpSubBlockName,
			(LPVOID*)&lpText, (UINT*)&dwSize);
		FileVersion = lpText;

		SubBlockName.Format(strFormat, dwLang, _T("LegalCopyright"));
		VerQueryValue(VerData, lpSubBlockName,
			(LPVOID*)&lpText, (UINT*)&dwSize);
		LegalCopyright = lpText;

		SubBlockName.Format(strFormat, dwLang, _T("PrivateBuild"));
		VerQueryValue(VerData, lpSubBlockName,
			(LPVOID*)&lpText, (UINT*)&dwSize);
		PrivateBuild = lpText;

		delete[] VerData;
	}
	SubBlockName.ReleaseBuffer();
}

CVerInfo::~CVerInfo()
{
	//if(VerData) delete[] VerData;
}

// 函数

CString LoadStr(UINT nID)
{
	CString str;
	str.LoadString(AfxGetApp()->m_hInstance, nID);
	return str;
}

void ShowError()
{
	DWORD dwLastError = GetLastError();
	if(dwLastError)
	{
		LPCTSTR lpMsgBuf;
		::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
						FORMAT_MESSAGE_FROM_SYSTEM |
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, dwLastError,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						(LPTSTR)&lpMsgBuf, 0, NULL);
		AfxGetApp()->m_pMainWnd->MessageBox(lpMsgBuf, 0, MB_OK|MB_ICONERROR);
	}
}

BOOL exeProgram(CString lpProgram, CString lpParameters)
{
	CWaitCursor wc;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	CString strComm;
	strComm.Format(_T("%s %s"), lpProgram, lpParameters);

	// Start the child process. 
	if( !CreateProcess( NULL,	// No module name (use command line). 
		(LPTSTR)(LPCTSTR)strComm,// Command line. 
		NULL,					// Process handle not inheritable. 
		NULL,					// Thread handle not inheritable. 
		FALSE,					// Set handle inheritance to FALSE. 
		CREATE_NO_WINDOW,		// No creation flags. 
		NULL,					// Use parent's environment block. 
		NULL,					// Use parent's starting directory. 
		&si,					// Pointer to STARTUPINFO structure.
		&pi )					// Pointer to PROCESS_INFORMATION structure.
	) 
	{
		return FALSE;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
	return TRUE;
}

#define Load32 *(UINT*)pStart;pStart+=4;
#define Load8 *pStart++;
#define Save8(data,pos) *(pData+pos++)=data;
BOOL Lz77uncomp(BYTE* pRes, BYTE** pDst, int& nLzSize, int& nBinSize)
{
	BYTE* pStart = pRes;
	int nSize = Load32;
	nSize>>=8;	nSize&=0x1FFFFFF;
	nBinSize = nSize;
	BYTE *pData = NULL;
	pData = new BYTE[nSize];
	if(pData==NULL)
	{
		//SetStatusBar("申请空间失败！");
		return FALSE;
	}
	*pDst = pData;
	int nUnPos = 0, nLZ,nImm,n8;
	g_bBreak=FALSE;
	while(nSize>0 && !g_bBreak)
	{
		nLZ = Load8;
		n8=8;
Loop1:
		n8--;
		if(n8<0) continue;
		if(nLZ&0x80)
		{
			nImm = Load8;
			int nSize2 = (nImm>>4)+3;
			int nWin = (nImm&0x0F)<<8;
			nImm = Load8;
			nWin = (nImm|nWin)+1;
			nSize-=nSize2;
			if(nWin>nUnPos)
			{
				//SetStatusBar("解压出错，强行中断！");
				return FALSE;
			}
			do
			{
				nImm = *(pData+nUnPos-nWin);
				Save8(nImm,nUnPos);
				nSize2--;
			}while(nSize2>0);
		}else
		{
			nImm = Load8;
			Save8(nImm, nUnPos);
			nSize--;
		}
		if(nSize>0)
		{
			nLZ<<=1;
			goto Loop1;
		}
	}
	nLzSize = (int)(pStart-pRes);
	return TRUE;
}

BOOL Lz77uncomp(BYTE* pRes, BYTE* pDst, int& nLzSize, int& nBinSize)
{
	BYTE* pStart = pRes;
	int nSize = Load32;
	nSize>>=8;	nSize&=0x1FFFFFF;
	nBinSize = nSize;
	BYTE *pData = pDst;
	if(pData==NULL)
	{
		//SetStatusBar("申请空间失败！");
		return FALSE;
	}
	int nUnPos = 0, nLZ,nImm,n8;
	g_bBreak=FALSE;
	while(nSize>0 && !g_bBreak)
	{
		nLZ = Load8;
		n8=8;
Loop1:
		n8--;
		if(n8<0) continue;
		if(nLZ&0x80)
		{
			nImm = Load8;
			int nSize2 = (nImm>>4)+3;
			int nWin = (nImm&0x0F)<<8;
			nImm = Load8;
			nWin = (nImm|nWin)+1;
			nSize-=nSize2;
			if(nWin>nUnPos)
			{
				//SetStatusBar("解压出错，强行中断！");
				return FALSE;
			}
			do
			{
				nImm = *(pData+nUnPos-nWin);
				Save8(nImm,nUnPos);
				nSize2--;
			}while(nSize2>0);
		}else
		{
			nImm = Load8;
			Save8(nImm, nUnPos);
			nSize--;
		}
		if(nSize>0)
		{
			nLZ<<=1;
			goto Loop1;
		}
	}
	nLzSize = (int)(pStart-pRes);
	return TRUE;
}

int StrToIntEX(CString& Str, BOOL bHex, BOOL bFix)
{
	int nLen=Str.GetLength();
	int Int=0, tmp=0, H=1;
	if(nLen<1) return -1;
	for(int i=nLen-1; i>=0; i--)
	{
		tmp=Str.GetAt(i);
		if( bFix && (tmp==_T(',') || tmp==_T(' ')) )
			continue;
		if((tmp>=_T('0')) && (tmp<=_T('9')))
			tmp-=_T('0');
		else
		{
			if(!bHex) return -1;
			if((tmp>=_T('A')) && (tmp<=_T('F')))
				tmp-=_T('A')-10;
			else
			{
				if((tmp>=_T('a')) && (tmp<=_T('f')))
					tmp-=_T('a')-10;
				else
				if( bFix && tmp==_T('h') )
					continue;
				else
					return -1;
			}
		}
		Int += (tmp*H);
		if(bHex)
			H*=16;
		else
			H*=10;
	}
	return Int;
}
/*
// 动态数据交换

CT2DataExchange::CT2DataExchange(CWnd* pDlgWnd, BOOL bSaveAndValidate)
{
	m_bSaveAndValidate = bSaveAndValidate;
	m_pDlgWnd = pDlgWnd;
	m_bUpdate = FALSE;
}

CT2DataExchange::CT2DataExchange(CT2DataExchange& dx)
{
	m_bSaveAndValidate = dx.m_bSaveAndValidate;
	m_pDlgWnd = dx.m_pDlgWnd;
	m_bUpdate = dx.m_bUpdate;
}

void CT2DataExchange::CT2_DDX_Text(CT2DataExchange& dx, UINT nIDC, BYTE& Byte, BOOL bHex, BYTE nMin, BYTE nMax)
{
	CWnd *pWnd = dx.m_pDlgWnd->GetDlgItem(nIDC);
	if(!pWnd) return;
	CString text;
	if(dx.m_bSaveAndValidate)
	{
		pWnd->GetWindowText(text);
		int nVal = StrToIntEX(text, bHex);
		if( (nMin == nMax) ||
            ((nVal>=nMin) && (nVal<=nMax)) )
		{
			dx.m_bUpdate|=nVal!=Byte;
			Byte = nVal;
		}else
		{
			CT2DataExchange dx2(dx);
			dx2.m_bSaveAndValidate = FALSE;
			CT2_DDX_Text(dx2, nIDC, Byte, bHex, nMin, nMax);
		}
	}else
	{
		bHex?text.Format(_T("%X"), Byte):text.Format(_T("%d"), Byte);
		pWnd->SetWindowText(text);
		pWnd->SendMessage(EM_SETSEL, 0, -1);
	}
}

void CT2DataExchange::CT2_DDX_Text(CT2DataExchange& dx, UINT nIDC, WORD& wWord, BOOL bHex, WORD nMin, WORD nMax)
{
	CWnd *pWnd = dx.m_pDlgWnd->GetDlgItem(nIDC);
	if(!pWnd) return;
	CString text;
	if(dx.m_bSaveAndValidate)
	{
		pWnd->GetWindowText(text);
		int nVal = StrToIntEX(text, bHex);
		if( (nMin == nMax) ||
            ((nVal>=nMin) && (nVal<=nMax)) )
		{
			dx.m_bUpdate|=nVal!=wWord;
			wWord = nVal;
		}else
		{
			CT2DataExchange dx2(dx);
			dx2.m_bSaveAndValidate = FALSE;
			CT2_DDX_Text(dx2, nIDC, wWord, bHex, nMin, nMax);
		}
	}else
	{
		bHex?text.Format(_T("%X"), wWord):text.Format(_T("%d"), wWord);
		pWnd->SetWindowText(text);
		pWnd->SendMessage(EM_SETSEL, 0, -1);
	}
}

void CT2DataExchange::CT2_DDX_Text(CT2DataExchange& dx, UINT nIDC, UINT& nUint, BOOL bHex, UINT nMin, UINT nMax)
{
	CWnd *pWnd = dx.m_pDlgWnd->GetDlgItem(nIDC);
	if(!pWnd) return;
	CString text;
	if(dx.m_bSaveAndValidate)
	{
		pWnd->GetWindowText(text);
		char bAdd;;
		if(bAdd=text.GetAt(0)==_T('+'))
		{
			text.Delete(0);
			bHex = FALSE;
		}
		UINT nVal = (UINT)StrToIntEX(text, bHex);
		if( (nMin == nMax) ||
            ((nVal>=nMin) && (nVal<=nMax)) )
		{
			if(nVal!=-1)
			{
				dx.m_bUpdate|=nVal!=nUint;
				nUint = bAdd?(-(int)nVal):nVal;
			}
		}else
		{
			CT2DataExchange dx2(dx);
			dx2.m_bSaveAndValidate = FALSE;
			CT2_DDX_Text(dx2, nIDC, nUint, bHex, nMin, nMax);
		}
	}else
	{
		UINT v = nUint;
		if((int)v<0)
		{
			v = -(int)v;
			text.AppendChar(_T('+'));
			bHex = FALSE;
		}
		bHex?text.Format(_T("%X"), v):text.AppendFormat(_T("%d"), v);
		pWnd->SetWindowText(text);
		pWnd->SendMessage(EM_SETSEL, 0, -1);
	}
}

void CT2DataExchange::CT2_DDX_Text(CT2DataExchange& dx, UINT nIDC, CString& strText)
{
	CWnd *pWnd = dx.m_pDlgWnd->GetDlgItem(nIDC);
	if(!pWnd) return;
   if (dx.m_bSaveAndValidate)
   {
		CString Text;
		pWnd->GetWindowText(Text);
		dx.m_bUpdate|=Text.Compare(strText)==0?FALSE:TRUE;
		pWnd->GetWindowText(strText);
   }else
   {
		CString Text;
		pWnd->GetWindowText(Text);
		if(Text.Compare(strText)!=0)
			pWnd->SetWindowText(strText);
		pWnd->SendMessage(EM_SETSEL, 0, -1);
   }
}

void CT2DataExchange::CT2_DDX_CBIndex(CT2DataExchange& dx, UINT nIDC, BYTE& nIndex)
{
   HWND hWndCtrl;
   dx.m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
   if(!hWndCtrl)
	   return;
	if (dx.m_bSaveAndValidate)
	{
		BYTE nVal = (BYTE)::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0L);
		dx.m_bUpdate|=nVal!=nIndex;
		nIndex = nVal;
	}else
		::SendMessage(hWndCtrl, CB_SETCURSEL, (WPARAM)nIndex, 0L);
}
*/
int Hint(UINT nIDS, UINT nType)
{
	CString hint;
	hint.LoadString(nIDS);
	return AfxGetApp()->m_pMainWnd->MessageBox(hint, 0, nType);
}

int Hint(CString strHint, UINT nType)
{
	return AfxGetApp()->m_pMainWnd->MessageBox(strHint, 0, nType);
}

void OnInitMenuPopup(CWnd* pTarget, HMENU hMenu, BOOL bDisableIfNoHndler)
{
	CCmdUI state;
	state.m_pMenu = CMenu::FromHandle(hMenu);
	if(pTarget->GetSystemMenu(FALSE)==state.m_pMenu) return;
	state.m_nIndexMax = state.m_pMenu->GetMenuItemCount();
	for(state.m_nIndex = 0;
		state.m_nIndex < state.m_nIndexMax;
        state.m_nIndex++)
	{
		state.m_nID = state.m_pMenu->GetMenuItemID(state.m_nIndex);
		if( (state.m_nID == 0) ||
			(state.m_nID >= 0xF000) ) continue; //跳过分割栏或系统菜单
		state.DoUpdate(pTarget, bDisableIfNoHndler);
	}
}

CString GetPath(HWND hWnd)
{
	LPITEMIDLIST idl=NULL;
	BROWSEINFO bi; memset(&bi,0,sizeof(bi));

	bi.hwndOwner = hWnd;
	bi.pidlRoot = idl;
	bi.ulFlags = BIF_EDITBOX|BIF_NEWDIALOGSTYLE;

	LPMALLOC pMalloc = NULL;
	SHGetMalloc(&pMalloc);
	SHGetFolderLocation(hWnd, 1, 0, 0, &idl);

	bi.lpszTitle = _T("请选择一个文件夹");
	LPITEMIDLIST ret = SHBrowseForFolder(&bi);
	ret->mkid;
	if(idl)
		pMalloc->Free(idl);

	SHGetPathFromIDList(ret, g_szBuffer);
	if(ret)
		pMalloc->Free(ret);
	pMalloc->Release();
	return CString(g_szBuffer);
}

void EnumSystemCodePages(UINT **pCodePage, UINT &nCount)
{
	// EnumSystemCodePages();
	HKEY hKey = NULL;
	UINT nCPList[16]={1200,1201}; nCount=2;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Classes\\MIME\\Database\\Codepage\\"),
		0, KEY_READ, &hKey)==ERROR_SUCCESS)
	{
		TCHAR lpVal[10];
		DWORD dwIndex=0; int nVal; CPINFO cpInfo;
		while( (RegEnumKey(hKey, dwIndex,
				lpVal, _MAX_PATH) == ERROR_SUCCESS) )
		{
			swscanf(lpVal, _T("%d"), &nVal);
			*(WORD*)cpInfo.DefaultChar = 0;
			GetCPInfo(nVal, &cpInfo);
			if( IsValidCodePage(nVal) &&
				(*(WORD*)cpInfo.DefaultChar!=0) &&
				(cpInfo.MaxCharSize>=2) &&
				(cpInfo.MaxCharSize<=4) )
				nCPList[nCount++] = nVal;
			if(nCount>=16) break;
			dwIndex++;
		}
		RegCloseKey(hKey);
	}
	//if(nCount)
	{
		*pCodePage = new UINT[nCount];
		memcpy(*pCodePage, nCPList, nCount*4);
	}
}

CString GetCodePageDescription(UINT nCodePage, CString *lpstrFontName)
{
	CString strValue, strSubKey;
	//if(!IsValidCodePage(nCodePage) &&
	//	(nCodePage!=1200) && (nCodePage!=1201) ) return strValue;

	HKEY hKey = NULL;
	strSubKey.Format(_T("%s\\%d"),
		_T("SOFTWARE\\Classes\\MIME\\Database\\Codepage"),
		nCodePage);
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		strSubKey,
		0,
		KEY_READ,
		&hKey)==ERROR_SUCCESS)
	{
		DWORD dwSize, dwType;
		//strSubKey = _T("Description");
		strSubKey = _T("BodyCharset");
		if(RegQueryValueEx(hKey, strSubKey,
			NULL, &dwType, NULL, &dwSize)==ERROR_SUCCESS)
		{
			RegQueryValueEx(hKey, strSubKey,
				NULL, &dwType, (LPBYTE)strValue.GetBuffer(dwSize/sizeof(TCHAR)), &dwSize);
			strValue.ReleaseBuffer();
		}

		if(lpstrFontName)
		{
			strSubKey = _T("FixedWidthFont");
			if(RegQueryValueEx(hKey, strSubKey,
				NULL, &dwType, NULL, &dwSize)==ERROR_SUCCESS)
			{
				RegQueryValueEx(hKey, strSubKey,
					NULL, &dwType, (LPBYTE)lpstrFontName->GetBuffer(dwSize/sizeof(TCHAR)), &dwSize);
				lpstrFontName->ReleaseBuffer();
			}
		}
		RegCloseKey(hKey);
	}

	//if(!strValue.IsEmpty())
	{
		CString Format;
		Format.LoadString(IDS_CODEPAGEHINT);
		strValue.AppendFormat(Format, nCodePage);;
	}
	return strValue;
}

LPPICTURE LoadPic(HANDLE hHandle, UINT nSize, BOOL bLoadAndFree)
{
	IStream* pStream;
	HRESULT hr = CreateStreamOnHGlobal(hHandle, bLoadAndFree, &pStream);
	if(hr!=S_OK) return NULL;

	LPPICTURE pPix;
	hr = OleLoadPicture(pStream, nSize, TRUE, IID_IPicture, (LPVOID*)&pPix);
	pStream->Release();

	return pPix;
}

HANDLE LoadRes(UINT nIDR, DWORD &dwSize, LPCTSTR lpType)
{
	TCHAR Crystal[]=_T("CRYSTAL");
	if(lpType==NULL) lpType = Crystal;

	HRSRC hRsrc=FindResource(NULL,
		MAKEINTRESOURCE(nIDR), lpType);
	HANDLE hHandle=NULL;
	if(hRsrc)
	{
		HANDLE hRes=LoadResource(NULL, hRsrc);

		dwSize = SizeofResource(NULL, (HRSRC)hRsrc);
		hHandle = GlobalAlloc(GMEM_MOVEABLE, dwSize);

		void *pDst = GlobalLock(hHandle);
		void *pRes = GlobalLock(hRes);
		memcpy(pDst, pRes, dwSize);
		GlobalUnlock(hHandle);
		GlobalUnlock(hRes);

		FreeResource(hRes);
	}
	return hHandle;
}

//如果uFlages=SHGFI_SMALLICON|SHGFI_LARGEICON，图标将是小/大图标。
//返回的是系统图标，请不要进行释放（删除）
BOOL LoadSystemImageList(HIMAGELIST& hImages, UINT uFlags)
{
	SHFILEINFO sfiInfo;
	memset(&sfiInfo, 0, sizeof(SHFILEINFO));
	GetSystemDirectory(sfiInfo.szDisplayName, sizeof(sfiInfo.szDisplayName));
	hImages = (HIMAGELIST)	(SHGetFileInfo (sfiInfo.szDisplayName, 0, &sfiInfo,
			sizeof(sfiInfo), SHGFI_SYSICONINDEX | uFlags));

	return hImages!=NULL;
}

//如果uFlages=SHGFI_OPENICON|SHGFI_SMALLICON|SHGFI_LARGEICON，目录图标将是打开的。小/大图标。
int GetIconIndex(LPCTSTR pszPath, UINT uFlags)
{
	SHFILEINFO sfi;
	memset(&sfi, 0, sizeof(sfi));

	SHGetFileInfo (pszPath, 0, &sfi, sizeof(sfi), 
		SHGFI_SYSICONINDEX | uFlags);

	return sfi.iIcon;
}

int GetMenuItemIndex(HMENU hMenu, UINT nID)
{
	int i=0;
	while(::GetMenuItemID(hMenu, i)!=nID)
	{
		if(i>::GetMenuItemCount(hMenu)) break;
		i++;
	}
	return i;
}

LPWSTR GetUText(CString strPathName, UINT nCodePage)
{
	CStdioFile file;
	if(!file.Open(strPathName,
		CStdioFile::modeRead|CStdioFile::typeBinary))
		return NULL;

	WORD nTmp;
	file.Read(&nTmp, 2);
	UINT nSize = (UINT)file.GetLength();

	LPWSTR lpUnicodeBuffer;

	if( nTmp!=0xFEFF && nTmp!=0xFFFE )
	{
		file.SeekToBegin();
		char *lpAnsiBuffer = new char[nSize];
		memset(lpAnsiBuffer, 0, nSize);
		nSize = file.Read(lpAnsiBuffer, nSize)+1;
		lpUnicodeBuffer = (LPWSTR)new BYTE[nSize*2];
		memset(lpUnicodeBuffer, 0, nSize*2);
		MultiByteToWideChar(nCodePage, 0,
			lpAnsiBuffer, nSize,
			lpUnicodeBuffer, nSize*2);

		delete[] lpAnsiBuffer;
	}else
	{
		lpUnicodeBuffer = (LPWSTR)new BYTE[nSize];
		memset(lpUnicodeBuffer, 0, nSize);
		nSize = file.Read(lpUnicodeBuffer, nSize);
		if(nTmp==0xFFFE)
		{	// big-endian
			for(UINT i=0; i<nSize/2; i++)
				lpUnicodeBuffer[i] =
				(lpUnicodeBuffer[i]>>8)|((lpUnicodeBuffer[i]&0xFF)<<8);
		}
	}

	return lpUnicodeBuffer;
}

int nAVal1,nAVal2,nAVal3,nAVal4;
BOOL CheckAsm(char *pFormat, char *pAsm)
{
	int nLen = (int)strlen(pFormat);
	int *pAVal[4] = {&nAVal1,&nAVal2,&nAVal3,&nAVal4};
	int nAValNum=0;
	BOOL bS = FALSE;
	while(*pFormat)
	{
		if( *pFormat == '?' )
		{
			pFormat++;
			bS = TRUE;
			continue;
		}else
		if( *pFormat == '[' && !bS)
		{
			pFormat++;
			BOOL bHex = *pFormat=='$';
			if(!bHex)
			{
				if(*pFormat=='?')
				{
					if(!*pAsm) return TRUE;
					pFormat++;
					if(*pFormat == *pAsm && *pAsm)
						pAsm++;
					pFormat+=2;
					continue;
				}

				if(*pFormat<'0' || *pFormat>'9')
					return FALSE;
			}else
				pFormat++;

			int nVLen = 0;
			int nVal;
			while(*pFormat!='-')
			{
				if(bHex)
				{
					if( (*pFormat<'0' || *pFormat>'9') &&
						(*pFormat<'a' || *pFormat>'f') )
						break;
				}else
				{
					if(*pFormat<'0' || *pFormat>'9')
						break;
				}
				nVLen++;
				pFormat++;
			}
			if(nVLen)
			{
				CString strVal;
				while(nVLen)
				{
					strVal.AppendChar(*(pFormat-nVLen));
					nVLen--;
				}
				nVal = StrToIntEX(strVal, bHex);
				if(nVal==-1) return FALSE;
				//pFormat--;

			}else
				return FALSE;

			if(*pFormat == '-')
			{
				char *pTmp = pAsm;
				nVLen=0;
				while(*pTmp!=',' && *pTmp!='\0' && *pTmp!='h' && *pTmp!=']')
				{
					nVLen++;
					pTmp++;
				}
				pAsm += nVLen-1;
				CString strVal;
				while(nVLen)
				{
					strVal.AppendChar(*(pTmp-nVLen));
					nVLen--;
				}
				*pAVal[nAValNum] = StrToIntEX(strVal, bHex);
				if(*pAVal[nAValNum] ==-1) return FALSE;
				if(nVal>*pAVal[nAValNum]) return FALSE;

				pFormat++;

				bHex = *pFormat=='$';
				if(!bHex)
				{
					if(*pFormat<'0' || *pFormat>'9')
						return FALSE;
				}else
					pFormat++;

				nVLen=0;
				while(*pFormat!=']')
				{
					if(bHex)
					{
						if( (*pFormat<'0' || *pFormat>'9') &&
							(*pFormat<'a' || *pFormat>'f') )
							break;
					}else
					{
						if(*pFormat<'0' || *pFormat>'9')
							break;
					}
					nVLen++;
					pFormat++;
				}
				if(nVLen)
				{
					CString strVal;
					while(nVLen)
					{
						strVal.AppendChar(*(pFormat-nVLen));
						nVLen--;
					}
					nVal = StrToIntEX(strVal, bHex);
					if(nVal==-1) return FALSE;
					//pFormat--;

					if(nVal<*pAVal[nAValNum]) return FALSE;
				}else
					return FALSE;
			}else
			{
				char *pTmp = pAsm;
				nVLen=0;
				while(*pTmp!=']')
					nVLen++;
				pAsm += nVLen;
				CString strVal;
				while(nVLen)
				{
					strVal.AppendChar(*(pAsm-nVLen));
					nVLen--;
				}
				*pAVal[nAValNum] = StrToIntEX(strVal);
				if(nVal!=*pAVal[nAValNum]) return FALSE;
			}

			if(*pFormat != ']')
				return FALSE;
			nAValNum++;
		}else
		if( *pFormat != *pAsm )
			return FALSE;

		bS = FALSE;
		pFormat++;
		pAsm++;
	}
	return *pAsm || *pFormat?FALSE:TRUE;
}

UINT GetAsmCode(CString strAsm, UINT nCurOffset, BYTE &nSize, BYTE nThumb)
{
	//CString strAsm = lpAsm;
	strAsm.MakeLower();
	strAsm.TrimLeft();
	strAsm.TrimRight();
	while(strAsm.Find(_T("  "))!=-1)
		strAsm.Replace(_T("  "), _T(" "));
	while(strAsm.Find(_T(", "))!=-1)
		strAsm.Replace(_T(", "), _T(","));
	strAsm.Replace(_T("pc"), _T("r15"));
	strAsm.Replace(_T("sp"), _T("r13"));
	strAsm.Replace(_T("lr"), _T("r14"));

	CharToOem(strAsm, g_szBufferA);
	nSize = 0;

	char * pAsm = g_szBufferA;

	UINT nCode = 0;

	if(nThumb)
	{
		nSize = 2;
		switch( *(UINT*)pAsm )
		{// THUMB
		case 0x206C736C:	// lsl
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7],[$0-$1f][?h]", pAsm))
			{// lsl r0,r0,0h
				nCode = nAVal1 | (nAVal2<<3) | (nAVal3<<6);
			}else
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// lsl r0,r0
				nCode = nAVal1 | (nAVal2<<3) | 0x4080;
			}else
				nSize = 0;
			break;

		case 0x2072736C:	// lsr
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7],[$0-$1f][?h]", pAsm))
			{// lsr r0,r0,0h
				nCode = nAVal1 | (nAVal2<<3) | (nAVal3<<6) | 0x0800;
			}else
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// lsr r0,r0
				nCode = nAVal1 | (nAVal2<<3) | 0x40C0;
			}else
				nSize = 0;
			break;

		case 0x20727361:	// asr
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7],[$0-$1f][?h]", pAsm))
			{// asr r0,r0,0h
				nCode = nAVal1 | (nAVal2<<3) | (nAVal3<<6) | 0x0800;
			}else
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// asr r0,r0
				nCode = nAVal1 | (nAVal2<<3) | 0x1000;
			}else
				nSize = 0;
			break;

		case 0x20646461:	// add
			pAsm+=4;
			if(CheckAsm("r[0-7],[$0-$ff][?h]", pAsm))
			{// add r0,0h
				nCode = (nAVal1<<8) | nAVal2 | 0x3000;
			}else
			if(CheckAsm("r[0-7],r[0-7],r[0-7]", pAsm))
			{// add r0,r0,r0
				nCode = nAVal1 | (nAVal2<<3) | (nAVal3<<6) | 0x1800;
			}else
			if( CheckAsm("r[0-15],r[0-15]",pAsm) &&
				(nAVal1>7 || nAVal2>7) )
			{// add r0,r0
				nCode = nAVal1&7 | (nAVal2&7)<<3;
				if(nAVal1>7)
					nCode |= 0x4480;
				if(nAVal2>7)
					nCode |= 0x4440;
			}else
			if( CheckAsm("r[0-7],r13,[$0-$03fc][?h]",pAsm) )
			{
				nCode = nAVal1<<8 | nAVal2>>2 | 0xA800;
			}else
			if( CheckAsm("r[0-7],r15,[$0-$03fc][?h]",pAsm) )
			{
				nCode = nAVal1<<8 | nAVal2>>2 | 0xA000;
			}else
				nSize = 0;
			break;

		case 0x20627573:	// sub
			pAsm+=4;
			if(CheckAsm("r[0-7],[$0-$7f][?h]", pAsm))
			{// sub r0,0h
				nCode = (nAVal1<<8) | nAVal2 | 0x3800;
			}else
			if(CheckAsm("r[0-7],r[0-7],r[0-7]", pAsm))
			{// sub r0,r0,r0
				nCode = nAVal1 | (nAVal2<<3) | (nAVal3<<6) | 0x1A00;
			}else
				nSize = 0;
			break;

		case 0x20766F6D:	// mov
			pAsm+=4;
			if(CheckAsm("r[0-7],[$0-$ff][?h]", pAsm))
			{// mov r0,0h
				nCode = (nAVal1<<8) | nAVal2 | 0x2000;
			}else
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// mov r0,r0
				nCode = 0x4600 | (nAVal1&7) | ((nAVal2&7)<<3);
			}else
			if(CheckAsm("r[0-14],r[8-15]", pAsm))
			{// mov r0,r8
				nCode = 0x4600 | (nAVal1&7) | ((nAVal2&7)<<3);
				if(nAVal1>7) nCode |= 0x80;
				if(nAVal2>7) nCode |= 0x40;
			}else
			if(CheckAsm("r[8-14],r[0-15]", pAsm))
			{// mov r8,r0
				nCode = 0x4600 | nAVal1&7 | ((nAVal2&7)<<3);
				if(nAVal1>7) nCode |= 0x80;
				if(nAVal2>7) nCode |= 0x40;
			}else
			if(CheckAsm("r15,r[0-15]", pAsm))
			{// mov r15,r0
				nCode = 0x4687 | (nAVal1&7)<<3;
				if(nAVal1>7) nCode |= 0x40;
			}else
				nSize = 0;
			break;

		case 0x20706D63:	// cmp
			pAsm+=4;
			if(CheckAsm("r[0-7],[$0-$ff][?h]", pAsm))
			{// cmp r0,0h
				nCode = nAVal1<<8 | nAVal2 | 0x2800;
			}else
			if(CheckAsm("r[0-15],r[0-15]", pAsm))
			{// cmp r0,r0
				nCode = (nAVal1&7) | (nAVal2&7)<<3;
				if(nAVal1>7)
					nCode |= 0x80;
				if(nAVal2>7)
					nCode |= 0x40;
				if(nAVal1>7 && nAVal2>7)
					nCode |= 0x4500;
				else
					nCode |= 0x4280;
			}else
				nSize = 0;
			break;

		case 0x20646E61:	// and
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// and r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x4000;
			}else
				nSize = 0;
			break;

		case 0x20726F65:	// eor
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// eor r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x4040;
			}else
				nSize = 0;
			break;

		case 0x20636461:	// adc
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// adc r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x4140;
			}else
				nSize = 0;
			break;

		case 0x20636273:	// sbc
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// sbc r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x4180;
			}else
				nSize = 0;
			break;

		case 0x20726F72:	// ror
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// ror sbc r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x41C0;
			}else
				nSize = 0;
			break;

		case 0x20747374:	// tst
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// tst r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x4200;
			}else
				nSize = 0;
			break;

		case 0x2067656E:	// neg
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// neg r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x4240;
			}else
				nSize = 0;
			break;

		case 0x00706F6E:	// nop
			//pAsm+=4;
			if(strcmp(pAsm, "nop")==0)
			{
				nCode = 0x46C0;
			}else
				nSize = 0;
			break;

		case 0x206E6D63:	// cmn
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// cmn r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x42C0;
			}else
				nSize = 0;
			break;

		case 0x2072726F:	// orr
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// orr r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x4300;
			}else
				nSize = 0;
			break;

		case 0x206C756D:	// mul
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// mul r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x4340;
			}else
				nSize = 0;
			break;

		case 0x20636962:	// bic
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// bic r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x4380;
			}else
				nSize = 0;
			break;

		case 0x206E767D:	// mvn
			pAsm+=4;
			if(CheckAsm("r[0-7],r[0-7]", pAsm))
			{// mvn r0,r0
				nCode = nAVal1 | nAVal2<<3 | 0x43C0;
			}else
				nSize = 0;
			break;

		//case 0x20786C62:	// blx

		case 0x2072646C:	// ldr
			pAsm+=4;
			if( CheckAsm("r[0-7],?[r15,[$0-$03fc][?h]?]", pAsm) )
			{// ldr r0,[pc,0h]
				nCode = nAVal1<<8 | (nAVal2>>2) | 0x4800;
			}else
			if( CheckAsm("r[0-7],?[r13,[$0-$03fc][?h]?]", pAsm) )
			{
				nCode = nAVal1<<8 | nAVal3>>2 | 0x9800;
			}else
			{
				nAVal3 = 0;
				if( CheckAsm("r[0-7],?[r[0-7],[$0-$7c][?h]?]", pAsm) ||
					CheckAsm("r[0-7],?[r[0-7]?]", pAsm) )
				{// ldr r0,[r0,0h]
					nCode = nAVal1 | nAVal2<<3 | (nAVal3>>2<<6) | 0x6800;
				}else
					nSize = 0;
			}
			break;

		case 0x6872646C:	// ldrh
			pAsm+=4;
			if(*pAsm!=' ') {nSize=0; break;}
			pAsm++;
			nAVal3 = 0;
			if( CheckAsm("r[0-7],?[r[0-7],[$0-$3c][?h]?]", pAsm) ||
				CheckAsm("r[0-7],?[r[0-7]?]", pAsm) )
			{// ldrb r0,[r0,0h]
				nCode = nAVal1 | nAVal2<<3 | (nAVal3>>2<<7) | 0x8800;
			}else
				nSize = 0;
			break;

		case 0x6272646C:	// ldrb
			pAsm+=4;
			if(*pAsm!=' ') {nSize=0; break;}
			pAsm++;
			nAVal3 = 0;
			if( CheckAsm("r[0-7],?[r[0-7],[$0-$1f][?h]?]", pAsm) ||
				CheckAsm("r[0-7],?[r[0-7]?]", pAsm) )
			{// ldrb r0,[r0,0h]
				nCode = nAVal1 | nAVal2<<3 | (nAVal3<<6) | 0x7800;
			}else
				nSize = 0;
			break;

		case 0x20727473:	// str
			pAsm+=4;
			nAVal3 = 0;
			if( CheckAsm("r[0-7],?[r[0-7],[$0-$7c][?h]?]", pAsm) ||
				CheckAsm("r[0-7],?[r[0-7]?]", pAsm) )
			{// str r0,[r0,0h]
				nCode = nAVal1 | nAVal2<<3 | (nAVal3>>2<<6) | 0x6000;
			}else
			if( CheckAsm("r[0-7],?[r13,[$0-$7c][?h]?]", pAsm) )
			{// str r0,[r0,0h]
				nCode = nAVal1<<8 | nAVal3>>2 | 0x9000;
			}else
				nSize = 0;
			break;

		case 0x68727473:	// strh
			pAsm+=4;
			if(*pAsm!=' ') {nSize=0; break;}
			pAsm++;
			nAVal3 =0;
			if( CheckAsm("r[0-7],?[r[0-7],[$0-$3c][?h]?]", pAsm) ||
				CheckAsm("r[0-7],?[r[0-7]?]", pAsm) )
			{// strb r0,[r0,0h]
				nCode = nAVal1 | nAVal2<<3 | (nAVal3>>2<<7) | 0x8000;
			}else
				nSize = 0;
			break;

		case 0x62727473:	// strb
			pAsm+=4;
			if(*pAsm!=' ') {nSize=0; break;}
			pAsm++;
			nAVal3 =0;
			if( CheckAsm("r[0-7],?[r[0-7],[$0-$1f][?h]?]", pAsm) ||
				CheckAsm("r[0-7],?[r[0-7]?]", pAsm) )
			{// strb r0,[r0,0h]
				nCode = nAVal1 | nAVal2<<3 | (nAVal3<<6) | 0x7000;
			}else
				nSize = 0;
			break;

		case 0x68737570:	// push
			{
			pAsm+=4;
			if(*pAsm!=' ') {nSize=0; break;}
			pAsm++;
			nCode = 0xB400;
			UINT nLBegin = -1;
			while(*pAsm=='r' || *pAsm=='-')
			{
				if(*pAsm=='-' && nLBegin!=-1)
				{// List
					pAsm++;
					if(*pAsm!='r')
					{
						nCode = 0;
						nSize=0;
						break;
					}
					pAsm++;

					UINT nT = *pAsm;
					if(nT && nT>='0' && nT<='7')
					for(UINT nF = nLBegin; nF<=nT-'0'; nF++)
						nCode |= 1<<nF;

					nLBegin = -1;
				}

				pAsm++;
				UINT nT = *pAsm;
				if(*(WORD*)pAsm==0x3431)
				{// push lr
					nCode |= 0xB500;
					pAsm+=2;
				}else
				if(nT && nT>='0' && nT<='7')
				{
					nT-='0';
					nCode |= 1<<nT;
					nLBegin = nT;
					pAsm++;
				}
				if(!*pAsm) break;
				 if((nLBegin!=-1 && *pAsm=='-'))
					 continue;
				if(*pAsm!=',')
				{
					nCode=0;
					nSize=0;
					break;
				}
                pAsm++;
			}
			if(*(WORD*)pAsm==0x726C)
				nCode |= 0xB500;
			}
			break;

		case 0x20706F70:	// pop
			{
			pAsm+=4;
			nCode = 0xBC00;
			UINT nLBegin = -1;
			while(*pAsm=='r' || *pAsm=='-')
			{
				if(*pAsm=='-' && nLBegin!=-1)
				{// List
					pAsm++;
					if(*pAsm!='r')
					{
						nCode = 0;
						nSize = 0;
						break;
					}
					pAsm++;

					UINT nT = *pAsm;
					if(nT && nT>='0' && nT<='7')
					for(UINT nF = nLBegin; nF<=nT-'0'; nF++)
						nCode |= 1<<nF;

					nLBegin = -1;
				}

				pAsm++;
				UINT nT = *pAsm;
				if(*(WORD*)pAsm==0x3531)
				{// pop pc
					nCode |= 0xBD00;
					pAsm+=2;
				}else
				if(nT && nT>='0' && nT<='7')
				{
					nT-='0';
					nCode |= 1<<nT;
					nLBegin = nT;
					pAsm++;
				}
				if(!*pAsm) break;
				 if((nLBegin!=-1 && *pAsm=='-'))
					 continue;
				if(*pAsm!=',')
				{
					nCode=0;
					nSize=0;
					break;
				}
                pAsm++;
			}
			if(*(WORD*)pAsm==0x6370)
				nCode |= 0xBD00;
			}
			break;

		//case bkpt:
		case 0x696D7473:	// stmi[a]
			pAsm+=4;
			if(*(WORD*)pAsm!=0x2061) {nSize=0;break;}	// a 
			pAsm+=2;
			{// stmia [r0]!,r0-r1
				if(*(WORD*)pAsm!=0x725B) {nSize=0; break;}	// [r
				pAsm+=2;
				BYTE nRd = *pAsm;
				if(nRd<'0' || nRd>'7') {nSize=0;break;}
				nRd-='0';
				pAsm++;
				if(*(UINT*)pAsm!=0x722C215D) {nSize=0;break;}	// ]!,r
				// 处理完 stmia [r0]!,r
				pAsm+=3;

				// list
				nCode = 0xC000 | nRd<<8;
				UINT nLBegin = -1;
				while(*pAsm=='r' || *pAsm=='-')
				{
					if(*pAsm=='-' && nLBegin!=-1)
					{// List
						pAsm++;
						if(*pAsm!='r')
						{
							nCode = 0;
							nSize=0;
							break;
						}
						pAsm++;

						UINT nT = *pAsm;
						if(nT && nT>='0' && nT<='7')
						for(UINT nF = nLBegin; nF<=nT-'0'; nF++)
							nCode |= 1<<nF;

						nLBegin = -1;
					}
					pAsm++;
					UINT nT = *pAsm;
					if(nT && nT>='0' && nT<='7')
					{
						nT-='0';
						nCode |= 1<<nT;
						nLBegin = nT;
						pAsm++;
					}
					if(!*pAsm) break;
					if((nLBegin!=-1 && *pAsm=='-'))
						continue;
					if(*pAsm!=',')
					{
						nCode=0;
						nSize=0;
						break;
					}
					pAsm++;
				}
			}
			break;

		case 0x696D646C:	// ldmi[a]
			pAsm+=4;
			if(*(WORD*)pAsm!=0x2061) {nSize=0;break;}	// a 
			pAsm+=2;
			{// ldmia [r0]!,r0-r1
				if(*(WORD*)pAsm!=0x725B) {nSize=0;break;}	// [r
				pAsm+=2;
				BYTE nRd = *pAsm;
				if(nRd<'0' || nRd>'7') {nSize=0;break;}
				nRd-='0';
				pAsm++;
				if(*(UINT*)pAsm!=0x722C215D) {nSize=0;break;}	// ]!,r
				// 处理完 ldmia [r0]!,r
				pAsm+=3;

				// list
				nCode = 0xC800 | nRd<<8;
				UINT nLBegin = -1;
				while(*pAsm=='r' || *pAsm=='-')
				{
					if(*pAsm=='-' && nLBegin!=-1)
					{// List
						pAsm++;
						if(*pAsm!='r')
						{
							nCode = 0;
							nSize=0;
							break;
						}
						pAsm++;

						UINT nT = *pAsm;
						if(nT && nT>='0' && nT<='7')
						for(UINT nF = nLBegin; nF<=nT-'0'; nF++)
							nCode |= 1<<nF;

						nLBegin = -1;
					}
					pAsm++;
					UINT nT = *pAsm;
					if(nT && nT>='0' && nT<='7')
					{
						nT-='0';
						nCode |= 1<<nT;
						nLBegin = nT;
						pAsm++;
					}
					if(!*pAsm) break;
					if((nLBegin!=-1 && *pAsm=='-'))
						continue;
					if(*pAsm!=',')
					{
						nCode=0;
						nSize=0;
						break;
					}
					pAsm++;
				}
			}
			break;

		case 0x20716562:	// beq
			nCode = 0xD0;	goto BCON;
		case 0x20656E62:	// bne
			nCode = 0xD1;	goto BCON;
		case 0x20736362:	// bcs
			nCode = 0xD2;	goto BCON;
		case 0x20636362:	// bcc
			nCode = 0xD3;	goto BCON;
		case 0x20696D62:	// bmi
			nCode = 0xD4;	goto BCON;
		case 0x206C7062:	// bpl
			nCode = 0xD5;	goto BCON;
		case 0x20737662:	// bvs
			nCode = 0xD6;	goto BCON;
		case 0x20637662:	// bvc
			nCode = 0xD7;	goto BCON;
		case 0x20696862:	// bhi
			nCode = 0xD8;	goto BCON;
		case 0x20736C62:	// bls
			nCode = 0xD9;	goto BCON;
		case 0x20656762:	// bge
			nCode = 0xDA;	goto BCON;
		case 0x20746C62:	// blt
			nCode = 0xDB;	goto BCON;
		case 0x20746762:	// bgt
			nCode = 0xDC;	goto BCON;
		case 0x20656C62:	// ble
			nCode = 0xDD;	//goto BCON;

			{
				BCON:
				pAsm+=4;
				int nOffset;
				if(sscanf(pAsm, "%x", &nOffset))
				{
					nOffset = (nOffset-nCurOffset-4)>>1;
					nCode = (nCode<<8) | (nOffset&0xFF);
				}else
					nSize = 0;
			}break;

		case 0x20697773:	// swi
			{
				pAsm+=4;
				int nSwi;
				if( sscanf(pAsm, "%x", &nSwi) &&
					nSwi>=0 && nSwi<=0x1F)
				{
					nCode = 0xDF00 | (nSwi&0x1F);
				}else
					nSize = 0;
			}break;

		default:
			UINT nAsm = *(UINT*)pAsm&0xFFFFFF;
			if( nAsm == 0x207862 )
			{// bx
				pAsm+=3;
				if(CheckAsm("r[0-14]", pAsm))
				{// bx r0
					nCode = nAVal1<<3 | 0x4700;
				}
			}else
			if( nAsm == 0x206C62 )
			{// bl
				pAsm+=3;
				int nOffset;
				if(sscanf(pAsm, "%x", &nOffset))
				{
					nSize = 4;
					nOffset = (nOffset-nCurOffset-4)>>1;
					WORD hi = (nOffset>>11)&0x7FF,
						lo = nOffset&0x7FF;
					nCode = 0xF800F000 | lo<<16 | hi;
				}
			}else
			if( (nAsm&0xFFFF) == 0x2062 )
			{// b 
				pAsm+=2;
				int nOffset;
				if(sscanf(pAsm, "%x", &nOffset))
				{
					nOffset = (nOffset-nCurOffset-4)>>1;
					nCode = 0xE000 | (nOffset&0x07FF);
				}
			}else
				nSize = 0;
		}
	}else
	{
		nSize = 4;
		switch( *(UINT*)pAsm )
		{// ARM
		case 0x2072646C:	// ldr
			pAsm+=4;
			break;

		case 0x00706F6E:	// nop
			if(strcmp(pAsm, "nop")==0)
			{
				nCode = 0xE1A00000;
			}else
				nSize = 0;
			break;

		default:
			//Hint(IDS_ASMERROR, MB_OK|MB_ICONERROR);
			//OnCancel();
			nSize = 0;
			break;
		}
	}

	return nCode;
}
/*
void InitMacro(void *pMacro, WORD wSize, WORD wActive)
{
	CT_MACRO *pM = (CT_MACRO*)pMacro;
	memset(pM, 0, wSize);
	pM->wSize = wSize;
	pM->wActive = wActive;
}
*/
void CreateImageList()
{
	if(g_hCTImageList) return;
	g_hCTImageList = ImageList_Create(16, 16, ILC_COLOR8, 0, 0);
	DWORD dwSize;
	HANDLE hHandle = LoadRes(IDR_NDSFSI, dwSize);
	LPPICTURE pPix= LoadPic(hHandle, dwSize, TRUE);
	HBITMAP hBitMap;
	pPix->get_Handle((OLE_HANDLE*)&hBitMap);

	ImageList_AddMasked(g_hCTImageList, hBitMap, (COLORREF)0xFFFFFF);

	pPix->Release();
}

int GetSFIIconIndex(CString name, BYTE *pFile)
{
	CString ext = PathFindExtension(name);
	ext.MakeLower();
	ext.Delete(0);

	if( ext.Left(2)==_T("lz") || ext==_T("lz77") || ext==_T("lzss") ||
		ext==_T("l") ||
		(name.Find(_T("_lz.bin"))!=-1) ||
		(pFile && ((*(UINT*)pFile)^0x37375A4C)==0) ||
		(pFile && (*pFile==0x10||*(UINT*)(pFile)==0x4C5A4F31) && ext.Right(1)==_T('_')) )
		return FSI_ICON_LZ;
	else
	if( ext==_T("font") || ext==_T("ncgr") || ext==_T("char") || ext==_T("ntft") ||
		ext==_T("fon") || ext==_T("bmp") || ext==_T("nftr") || ext==_T("nbfc"))
		return FSI_ICON_TILE;
	else
	if(ext==_T("sdat") || ext==_T("wav"))
		return FSI_ICON_SOUND;
	else
	if( ext==_T("mov") || ext==_T("avi") || ext==_T("ani") || ext==_T("nanr") )
		return FSI_ICON_ANIME;
	else
	if( ext==_T("srl") ||
		(pFile && (*(UINT*)(pFile)==0x544E494E &&	// NINTENDO
		*(UINT*)(pFile+0x0C+0x02)==0x31304A52 )) )	// NTRJ01
		return FSI_ICON_NDS;
	else
	if(ext==_T("nclr") || ext==_T("ntfp") || ext==_T("nbfp") || ext==_T("plt"))
		return FSI_ICON_PAL;
	else
	if( ext==_T("txt") || ext==_T("sadl") || ext==_T("h") || ext==_T("c") || ext==_T("cpp") ||
		ext==_T("inc") )
		return FSI_ICON_TEXT;
	else
	if(ext==_T("nscr"))
		return FSI_ICON_MAP;
	else
	if(ext==_T("pac") || ext==_T("sdat") || ext==_T("narc") || name.Find(_T("utility.bin"))!=-1)
		return FSI_ICON_CT;
	else
	if( name.Find(_T("scri"))!=-1 ||
		name.Find(_T("text"))!=-1 ||
		name.Find(_T("txt"))!=-1 ||
		name.Find(_T("fon"))!=-1 ||
		name.Find(_T("grap"))!=-1 ||
		name.Find(_T("face"))!=-1 ||
		name.Find(_T("imag"))!=-1 ||
		name.Find(_T("msg"))!=-1 ||
		name.Find(_T("titl"))!=-1 ||
		name.Find(_T("utf"))!=-1 )
		return FSI_ICON_HAN;
	return FSI_ICON_UNK;
}

UINT SFIExtractDirectory(CString& ParentDir, UINT nID, BYTE *pRom, UINT nFnt_Offset, UINT nFat_Offset, CListCtrl& FileList)
{
	CString strFilePathName, ext, name; NDSDIRREC *pDirRec;
	BYTE nRecLen; BOOL bIsDir; BYTE* pRec;
	CString Dir = ParentDir;

	UINT nPos = nFnt_Offset+(nID<<3);	// if(m_nRomSize<nPos) return;
	pDirRec = (NDSDIRREC*)(pRom+nPos);
	nPos = nFnt_Offset + pDirRec->entry_start;	// if(m_nRomSize<nPos) return;
	pRec  = pRom+nPos;
	UINT FileID = pDirRec->top_file_id;
	UINT nRomFitSize=0;
	while(1)
	{
		nRecLen = *(pRec++);	if(nRecLen==0) break;
		bIsDir = nRecLen&0x80; nRecLen&=0x7F;

		OemToCharBuff((char*)pRec, g_szBuffer, nRecLen);
		g_szBuffer[nRecLen]=0;
		pRec+=nRecLen;

		if(bIsDir)
		{
			WORD DirID = *(WORD*)(pRec);DirID&=0xFFF;
			pRec+=2;
			Dir.Format(_T("%s%s/"), ParentDir,g_szBuffer);
			nRomFitSize+=SFIExtractDirectory(Dir, DirID, pRom, nFnt_Offset, nFat_Offset, FileList); continue;
		}
		NDSFILEREC *pFileRec;
		pFileRec = (NDSFILEREC*)(pRom+nFat_Offset+(FileID<<3));
		nPos = pFileRec->bottom-pFileRec->top;
		nRomFitSize+=nPos;

		strFilePathName.Format(_T("%s%s"), ParentDir,g_szBuffer);

		nPos = FileList.GetItemCount();
		int nImage = GetSFIIconIndex(strFilePathName, pRom+pFileRec->top);// FSI_ICON_UNK;


		FileList.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE,
			nPos, strFilePathName, 0, 0,
			nImage, SFIMAKEPARAM(FileID, nPos));
		FileID++;
	}

	return nRomFitSize;
}
/*
UINT CRC32Table[256] = {
	0x00000000,0x77073096,0xee0e612c,0x990951ba,
	0x076dc419,0x706af48f,0xe963a535,0x9e6495a3,
	0x0edb8832,0x79dcb8a4,0xe0d5e91e,0x97d2d988,
	0x09b64c2b,0x7eb17cbd,0xe7b82d07,0x90bf1d91,
	0x1db71064,0x6ab020f2,0xf3b97148,0x84be41de,
	0x1adad47d,0x6ddde4eb,0xf4d4b551,0x83d385c7,
	0x136c9856,0x646ba8c0,0xfd62f97a,0x8a65c9ec,
	0x14015c4f,0x63066cd9,0xfa0f3d63,0x8d080df5,
	0x3b6e20c8,0x4c69105e,0xd56041e4,0xa2677172,
	0x3c03e4d1,0x4b04d447,0xd20d85fd,0xa50ab56b,
	0x35b5a8fa,0x42b2986c,0xdbbbc9d6,0xacbcf940,
	0x32d86ce3,0x45df5c75,0xdcd60dcf,0xabd13d59,
	0x26d930ac,0x51de003a,0xc8d75180,0xbfd06116,
	0x21b4f4b5,0x56b3c423,0xcfba9599,0xb8bda50f,
	0x2802b89e,0x5f058808,0xc60cd9b2,0xb10be924,
	0x2f6f7c87,0x58684c11,0xc1611dab,0xb6662d3d,
	0x76dc4190,0x01db7106,0x98d220bc,0xefd5102a,
	0x71b18589,0x06b6b51f,0x9fbfe4a5,0xe8b8d433,
	0x7807c9a2,0x0f00f934,0x9609a88e,0xe10e9818,
	0x7f6a0dbb,0x086d3d2d,0x91646c97,0xe6635c01,
	0x6b6b51f4,0x1c6c6162,0x856530d8,0xf262004e,
	0x6c0695ed,0x1b01a57b,0x8208f4c1,0xf50fc457,
	0x65b0d9c6,0x12b7e950,0x8bbeb8ea,0xfcb9887c,
	0x62dd1ddf,0x15da2d49,0x8cd37cf3,0xfbd44c65,
	0x4db26158,0x3ab551ce,0xa3bc0074,0xd4bb30e2,
	0x4adfa541,0x3dd895d7,0xa4d1c46d,0xd3d6f4fb,
	0x4369e96a,0x346ed9fc,0xad678846,0xda60b8d0,
	0x44042d73,0x33031de5,0xaa0a4c5f,0xdd0d7cc9,
	0x5005713c,0x270241aa,0xbe0b1010,0xc90c2086,
	0x5768b525,0x206f85b3,0xb966d409,0xce61e49f,
	0x5edef90e,0x29d9c998,0xb0d09822,0xc7d7a8b4,
	0x59b33d17,0x2eb40d81,0xb7bd5c3b,0xc0ba6cad,
	0xedb88320,0x9abfb3b6,0x03b6e20c,0x74b1d29a,
	0xead54739,0x9dd277af,0x04db2615,0x73dc1683,
	0xe3630b12,0x94643b84,0x0d6d6a3e,0x7a6a5aa8,
	0xe40ecf0b,0x9309ff9d,0x0a00ae27,0x7d079eb1,
	0xf00f9344,0x8708a3d2,0x1e01f268,0x6906c2fe,
	0xf762575d,0x806567cb,0x196c3671,0x6e6b06e7,
	0xfed41b76,0x89d32be0,0x10da7a5a,0x67dd4acc,
	0xf9b9df6f,0x8ebeeff9,0x17b7be43,0x60b08ed5,
	0xd6d6a3e8,0xa1d1937e,0x38d8c2c4,0x4fdff252,
	0xd1bb67f1,0xa6bc5767,0x3fb506dd,0x48b2364b,
	0xd80d2bda,0xaf0a1b4c,0x36034af6,0x41047a60,
	0xdf60efc3,0xa867df55,0x316e8eef,0x4669be79,
	0xcb61b38c,0xbc66831a,0x256fd2a0,0x5268e236,
	0xcc0c7795,0xbb0b4703,0x220216b9,0x5505262f,
	0xc5ba3bbe,0xb2bd0b28,0x2bb45a92,0x5cb36a04,
	0xc2d7ffa7,0xb5d0cf31,0x2cd99e8b,0x5bdeae1d,
	0x9b64c2b0,0xec63f226,0x756aa39c,0x026d930a,
	0x9c0906a9,0xeb0e363f,0x72076785,0x05005713,
	0x95bf4a82,0xe2b87a14,0x7bb12bae,0x0cb61b38,
	0x92d28e9b,0xe5d5be0d,0x7cdcefb7,0x0bdbdf21,
	0x86d3d2d4,0xf1d4e242,0x68ddb3f8,0x1fda836e,
	0x81be16cd,0xf6b9265b,0x6fb077e1,0x18b74777,
	0x88085ae6,0xff0f6a70,0x66063bca,0x11010b5c,
	0x8f659eff,0xf862ae69,0x616bffd3,0x166ccf45,
	0xa00ae278,0xd70dd2ee,0x4e048354,0x3903b3c2,
	0xa7672661,0xd06016f7,0x4969474d,0x3e6e77db,
	0xaed16a4a,0xd9d65adc,0x40df0b66,0x37d83bf0,
	0xa9bcae53,0xdebb9ec5,0x47b2cf7f,0x30b5ffe9,
	0xbdbdf21c,0xcabac28a,0x53b39330,0x24b4a3a6,
	0xbad03605,0xcdd70693,0x54de5729,0x23d967bf,
	0xb3667a2e,0xc4614ab8,0x5d681b02,0x2a6f2b94,
	0xb40bbe37,0xc30c8ea1,0x5a05df1b,0x2d02ef8d
};

UINT CalcCRC32(UINT crc, BYTE *buf, UINT len)
{
	crc=~crc;

	while(len--)
	crc=(crc>>8)^CRC32Table[(crc&0xFF)^(*buf++)];
	return(~crc);
}
*/

void GetTempFileName(LPTSTR pTempFileName)
{
	TCHAR path[_MAX_PATH];
	GetTempPath(_MAX_PATH, path);
	GetTempFileName(path,
		_T("Crystal"),
		0,
		pTempFileName);
}

int GetListImage(CListCtrl &lc, int iItem)
{
	LVITEM item;
	memset(&item, 0, sizeof(item));
	item.iItem=iItem;
	item.mask=LVIF_IMAGE;
	return item.iImage;
}
