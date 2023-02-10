// CT2Palette.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include ".\CT2Palette.h"
#include "CT2Doc.h"
#include "CT2TileView.h"
#include "CT2TileEditorView.h"
#include "NdsFileSystemInfo.h"
#include "CT2ChildFrm.h"

// CT2Palette 对话框
#define PALSIZE 8

IMPLEMENT_DYNAMIC(CT2Palette, CT2NormalDlg)
CT2Palette::CT2Palette(CWnd* pParent /*=NULL*/)
	//: CDialog(CT2Palette::IDD, pParent)
	: m_nSetColor(-1)
{
}

CT2Palette::~CT2Palette()
{
}

void CT2Palette::DoDataExchange(CDataExchange* pDX)
{
	CT2NormalDlg::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_SETCOLORBTN, m_nSetColor);
}


BEGIN_MESSAGE_MAP(CT2Palette, CT2NormalDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	//ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_DEFPALBTN, OnBnClickedDefpalbtn)
	ON_BN_CLICKED(IDC_LOADPALBTN, OnBnClickedLoadpalbtn)
	ON_BN_CLICKED(IDC_SAVEPALBTN, OnBnClickedSavepalbtn)
	ON_NOTIFY_EX(TTN_GETDISPINFO, 0, OnToolTipText)
	ON_COMMAND_RANGE(ID_PALETTE_PAL1, ID_PALETTE_PAL7, OnLoadDefPal)
	ON_COMMAND_RANGE(IDC_SETCOLORBTN, IDC_SETSHADOWCOLORBTN, OnSetColor)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CT2Palette 消息处理程序

BOOL CT2Palette::LoadPal(RGBQUAD* pPal, UINT nType)
{
	if(nType==-1)
	{
		nType = IDR_PALETTE+theApp.GetProfileInt(CT2Settings, CT2DefPal, 0);
	}

	HRSRC hRsrc=FindResource(NULL,
		MAKEINTRESOURCE(nType), _T("CRYSTAL"));
	if(hRsrc)
	{
		HGLOBAL hPal=LoadResource(NULL, hRsrc);
		BYTE* pPalette=(BYTE*)LockResource(hPal);
		int nCount = SizeofResource(NULL, (HRSRC)hRsrc)/3;
		if(nCount>256) nCount=256;
		while(nCount--)
			*(UINT*)&pPal[nCount] = RGB(*(pPalette+nCount*3+2),
										*(pPalette+nCount*3+1),
                                        *(pPalette+nCount*3));
		UnlockResource(hPal);
		FreeResource(hPal);

		return TRUE;
	}
	return FALSE;
}

BOOL CT2Palette::OnInitDialog()
{
	CT2NormalDlg::OnInitDialog();

	UpdateData(FALSE);

	EnableToolTips();

	//SetCapture();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CT2Palette::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CT2TileView* pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	if(pView==NULL) return;

	CT2Doc* pDoc = (CT2Doc*)pView->GetDocument();
	RGBQUAD* pPal = pDoc->m_pPal;
	CRect rcColor, rcClient; GetClientRect(&rcClient);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bm;
	bm.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	memDC.SelectObject(&bm);
	memDC.FillRect(&rcClient, &CBrush(::GetSysColor(COLOR_BTNFACE)));
	memDC.SetBkMode(TRANSPARENT);
	for(int y=0; y<16; y++)
	{
		rcColor.top = y*PALSIZE;
		rcColor.bottom = rcColor.top+PALSIZE;
		for(int x=0; x<16; x++)
		{
			rcColor.left = x*PALSIZE;
			rcColor.right = rcColor.left+PALSIZE;
			RGBQUAD rq=*(RGBQUAD*)(pPal+y*16+x);
			COLORREF clrColor=RGB(rq.rgbRed, rq.rgbGreen, rq.rgbBlue);
			memDC.FillRect(&rcColor, &CBrush(clrColor));
		}
		if( (y==(pView->m_nPalIndex/16)) && (pView->m_nBitCount<8) )
		{
			CRect rc;
			rc.left = pView->m_nPalIndex%16*PALSIZE;
			rc.top	= y*PALSIZE;
			rc.right = rc.left+(1<<pView->m_nBitCount)*PALSIZE;
			rc.bottom = rc.top+PALSIZE;
			memDC.Draw3dRect(&rc,
				::GetSysColor(COLOR_BTNHILIGHT),
				::GetSysColor(COLOR_BTNFACE));
		}
	}
	CRect rc(0, 0, 16*PALSIZE, 16*PALSIZE);
	CPen whiteGPen(PS_SOLID, 1, (COLORREF)0x9C9C9C);
	memDC.SelectObject(&whiteGPen);
	for(int i=0; i<=16; i++)
	{//纵线
		memDC.MoveTo(rc.left+i*PALSIZE, rc.top);
		memDC.LineTo(rc.left+i*PALSIZE, rc.bottom);
	}
	for(int i=0; i<=16; i++)
	{//横线
		memDC.MoveTo(rc.left, rc.top+i*PALSIZE);
		memDC.LineTo(rc.right, rc.top+i*PALSIZE);
	}

	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(), &memDC, 0,0,SRCCOPY);
	bm.DeleteObject(), memDC.DeleteDC();
}

void CT2Palette::OnLButtonDown(UINT nFlags, CPoint point)
{
	CT2NormalDlg::OnLButtonDown(nFlags, point);
	SetColor(nFlags, point);
}

void CT2Palette::OnRButtonDown(UINT nFlags, CPoint point)
{
	CT2NormalDlg::OnRButtonDown(nFlags, point);
	SetColor(nFlags, point);
}

void CT2Palette::SetColor(UINT nFlags, CPoint point)
{
	UpdateData();
	CT2TileView* pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	CRect rc(0, 0, PALSIZE*16, PALSIZE*16);
	if(!rc.PtInRect(point) || !pView/* || (pView->m_nBitCount>8)*/) return;

	int nIndex=point.x/PALSIZE;
	int nCount = 1<<pView->m_nBitCount;
	if((nIndex+nCount)>16) nIndex=16-nCount;
	BYTE nPalIndex = (BYTE) (point.y/PALSIZE*16 + nIndex);
	if((pView->m_nBitCount<=8) && ((nPalIndex<pView->m_nPalIndex) || (nPalIndex>=pView->m_nPalIndex+nCount)))
	{
		pView->m_nPalIndex = nPalIndex;
		pView->SetBits();
	}
	if(m_nSetColor!=-1)
	{
		CT2TileEditorView* pEditorView =
			(CT2TileEditorView*)theApp.GetView(ID_VIEW_TILEEDITORVIEW);
		CT2Doc *pDoc = pEditorView->GetDocument();

		UINT nColor = (BYTE)(point.y/PALSIZE*16+point.x/PALSIZE);
		if(nColor>=pView->m_nPalIndex)
			nColor-=pView->m_nPalIndex;
		switch(m_nSetColor)
		{
		case 0:
			pEditorView->m_nTextColor = nColor;
			break;
		case 1:
			pEditorView->m_nBkColor = nColor;
			break;
		case 2:
			pEditorView->m_nShadowColor = nColor;
			break;
		}
		m_nSetColor=-1;
		UpdateData(FALSE);
		pEditorView->OnUpdateData();
	}
	Invalidate(FALSE);

	theApp.GetCurView()->OnUpdateData();
	return;
}

HBRUSH CT2Palette::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	CT2TileEditorView *pEditorView =
		(CT2TileEditorView*)theApp.GetView(ID_VIEW_TILEEDITORVIEW);
	CT2TileView *pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	if(pView==NULL) return CT2NormalDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	CT2Doc *pDoc = pEditorView->GetDocument();
	RGBQUAD *pcqText = (RGBQUAD*)(pDoc->m_pPal+pView->m_nPalIndex+pEditorView->m_nTextColor);
	pDC->SetTextColor(RGB(pcqText->rgbRed, pcqText->rgbGreen, pcqText->rgbBlue));

	LOGBRUSH lb;
	pcqText = (RGBQUAD*)(pDoc->m_pPal+pView->m_nPalIndex+pEditorView->m_nBkColor);
	lb.lbColor = RGB(pcqText->rgbRed, pcqText->rgbGreen, pcqText->rgbBlue);
	lb.lbHatch = 0;
	lb.lbStyle = BS_SOLID;

	return CreateBrushIndirect(&lb);
}

void CT2Palette::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CT2NormalDlg::OnLButtonDblClk(nFlags, point);

	CT2TileView* pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	CRect rc(0, 0, PALSIZE*16, PALSIZE*16);
	if(!rc.PtInRect(point) || !pView/* || (pView->m_nBitCount>=8)*/) return;
	CT2Doc *pDoc=pView->GetDocument();

	static CColorDialog cd(0, CC_RGBINIT);
	//static COLORREF lpCustColor[16];
	//cd.m_cc.lpCustColors=lpCustColor;
	RGBQUAD *rq = pDoc->m_pPal+point.y/PALSIZE*16+point.x/PALSIZE;
	cd.m_cc.rgbResult=RGB(rq->rgbRed, rq->rgbGreen, rq->rgbBlue);
	if(cd.DoModal()!=IDOK) return;

	rq->rgbRed=(BYTE)cd.m_cc.rgbResult;
	rq->rgbGreen=(BYTE)(cd.m_cc.rgbResult>>8);
	rq->rgbBlue=(BYTE)(cd.m_cc.rgbResult>>16);
	Invalidate(FALSE);
	theApp.GetCurView()->OnUpdateData();
}

void CT2Palette::OnBnClickedDefpalbtn()
{
	if(!theApp.GetCurView()) return;

	CMenu menu;
	menu.LoadMenu(IDR_CT2POPMENU);
	CMenu *pSub = menu.GetSubMenu(1);
	CRect rc; GetDlgItem(IDC_DEFPALBTN)->GetWindowRect(&rc);
	pSub->TrackPopupMenu(TPM_LEFTBUTTON, rc.left, rc.bottom, this);
}

void CT2Palette::OnBnClickedLoadpalbtn()
{
	static CFileDialog fd(TRUE);
	CString strOpenDibFilter;
	strOpenDibFilter.LoadString(IDS_PALFILTER);
	strOpenDibFilter.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter=strOpenDibFilter;
	if(!theApp.GetCurView() || fd.DoModal()!=IDOK) return;

	OnLoadPal(fd.GetPathName());
}

void CT2Palette::OnLoadPal(CString path)
{
	CFile file;
	if(!file.Open(path, CFile::modeRead|CFile::typeBinary))
		return;
	CString Ext=PathFindExtension(path);
	Ext.MakeLower();
	BOOL bOk=FALSE;
	if(Ext==_T(".bmp"))
		bOk = IBmpPal(file);
	else
	if(Ext==_T(".act"))
		bOk = IActPal(file);
	else
	if(Ext==_T(".pal"))
		bOk = IPalPal(file);
	else
	if(Ext==_T(".nclr"))
		bOk = INclrPal(file);
	if(bOk)
	{
		Invalidate(FALSE);
		//AddMacro_LoadPal(path);
	}
}

BOOL CT2Palette::IBmpPal(CFile& file)
{
	file.SeekToBegin();
	BITMAPFILEHEADER BmFileHeader;
	BITMAPINFOHEADER BmInfoHeader;
	file.Read((BYTE*)&BmFileHeader, sizeof(BITMAPFILEHEADER));
	file.Read((BYTE*)&BmInfoHeader, sizeof(BITMAPINFOHEADER));
	if((BmFileHeader.bfType!=0x4D42) || (BmInfoHeader.biBitCount>8))
		return FALSE;

	CT2View* pView = theApp.GetCurView();
	CString Name = pView->GetDocument()->GetPathName();
	UINT nIndex = theApp.GetMRUIndex(Name);
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	file.Read((BYTE*)pSD->pPal, sizeof(RGBQUAD)*(1<<BmInfoHeader.biBitCount));
	pView->OnUpdateData();

	return TRUE;
}

BOOL CT2Palette::IPalPal(CFile& file)
{
	PALETTEINFO Pal;
	file.Read(&Pal, sizeof(Pal));
	if(Pal.Palette.Entries>256 ||
		Pal.FileHeader.Signature!=0x46464952 ||//RIFF
		Pal.FileHeader.FileSize!=(sizeof(PALETTEINFOHEADER)+Pal.Palette.Entries*sizeof(COLORREF))||
		Pal.Palette.RIFF!=0x204C4150 ||//----Pal data
		Pal.Palette.Signature!=0x61746164)//
		return FALSE;
	
	CT2View* pView = theApp.GetCurView();
	CString Name = pView->GetDocument()->GetPathName();
	UINT nIndex = theApp.GetMRUIndex(Name);
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	for(int i=0; i<Pal.Palette.Entries; i++)
	{
		COLORREF clrColor;
		file.Read(&clrColor, 4);
		pSD->pPal[i].rgbRed = (BYTE)clrColor;
		pSD->pPal[i].rgbGreen = (BYTE)(clrColor>>8);
		pSD->pPal[i].rgbBlue = (BYTE)(clrColor>>16);
	}
	pView->OnUpdateData();

	return TRUE;
}

BOOL CT2Palette::IActPal(CFile& file)
{
	CT2View* pView = theApp.GetCurView();
	CString Name = pView->GetDocument()->GetPathName();
	UINT nIndex = theApp.GetMRUIndex(Name);
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	for(int i=0; i<256; i++)
	{
		COLORREF clrColor;
		if(file.Read(&clrColor, 3)!=3) break;
		pSD->pPal[i].rgbRed = (BYTE)clrColor;
		pSD->pPal[i].rgbGreen = (BYTE)(clrColor>>8);
		pSD->pPal[i].rgbBlue = (BYTE)(clrColor>>16);
	}
	pView->OnUpdateData();

	return TRUE;
}

BOOL CT2Palette::INclrPal(CFile& file)
{
	HANDLE hMapFile = ::CreateFileMapping(file.m_hFile, NULL,
		PAGE_READONLY, 0, 0, NULL);

	BYTE *pNclr = (BYTE*)::MapViewOfFile(hMapFile,
		FILE_MAP_READ, 0, 0, 0);

	WORD wSize = ((NDSFILEHEADER*)pNclr)->nSize;
	BYTE *pPal = pNclr + wSize;

	if(file.GetLength()<(256*2+wSize+0x18))
		return FALSE;

	CT2View* pView = theApp.GetCurView();
	CString Name = pView->GetDocument()->GetPathName();
	UINT nIndex = theApp.GetMRUIndex(Name);
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);

	CNdsFileSystemInfo::LoadPal((WORD*)(pPal+0x18), pSD->pPal);
	pView->OnUpdateData();

	return TRUE;
}

void CT2Palette::OnBnClickedSavepalbtn()
{
	static CFileDialog fd(FALSE);
	CString strOpenDibFilter;
	strOpenDibFilter.LoadString(IDS_PAL_EXPORTPAL);
	strOpenDibFilter.AppendChar(_T('\0'));
	fd.m_pOFN->lpstrFilter = strOpenDibFilter;
	fd.m_pOFN->lpstrDefExt = _T("pal");

	CT2TileView* pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);

	if(!pView || fd.DoModal()!=IDOK) return;

	CString path = fd.GetPathName();
	CString ext = PathFindExtension(path); ext.MakeLower();
	CFile file;
	if(ext==_T(".pal"))
		fd.m_pOFN->nFilterIndex = 1;
	else
	if(ext==_T(".act"))
		fd.m_pOFN->nFilterIndex = 2;
	else
	if(ext==_T(".bmp"))
		fd.m_pOFN->nFilterIndex = 3;
	else
	if(ext==_T(".bin"))
		fd.m_pOFN->nFilterIndex = 4;

	CString Name = pView->GetDocument()->GetPathName();
	UINT nIndex = theApp.GetMRUIndex(Name);
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);

	switch(fd.m_pOFN->nFilterIndex)
	{
	default:
	case 1:// .pal
		{
			if(ext!=_T(".pal")) path.Append(_T(".pal"));
			PALETTEINFO Pal;
			Pal.FileHeader.Signature = 0x46464952;	// RIFF
			Pal.Palette.Entries = 256;
			Pal.Palette.RIFF = 0x204C4150;			// Pal
			Pal.Palette.Signature = 0x61746164;		// data
			Pal.Palette.ChunkSize = 0;
			Pal.Palette.Version = 0x0300;
			Pal.FileHeader.FileSize = sizeof(PALETTEINFOHEADER)+Pal.Palette.Entries*sizeof(COLORREF);
			if(!file.Open(path, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)) return;
			file.Write(&Pal, sizeof(Pal));
			for(int i=0; i<Pal.Palette.Entries; i++)
			{
				COLORREF clrColor = RGB(pSD->pPal[i].rgbRed,
					pSD->pPal[i].rgbGreen, pSD->pPal[i].rgbBlue);
				file.Write((BYTE*)&clrColor, 4);
			}
		}break;
	case 2:// .act
		if(ext!=_T(".act")) path.Append(_T(".act"));
		if(!file.Open(path, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)) return;
		for(int i=0; i<256; i++)
		{
			RGBQUAD *rq = pSD->pPal+i;
			COLORREF clr = *(UINT*)rq;
			clr = RgbQuadToColor(clr);
			file.Write(&clr, 3);
		}break;
	case 3:// .bmp
		{
			if(ext!=_T(".bmp")) path.Append(_T(".bmp"));
			if(!file.Open(path, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)) return;
			BITMAPINFOHEADER bmInfoHeader; memset(&bmInfoHeader, 0, sizeof(bmInfoHeader));
			bmInfoHeader.biSize = sizeof(bmInfoHeader);
			bmInfoHeader.biWidth = 256;
			bmInfoHeader.biHeight = 256;
			bmInfoHeader.biPlanes = 1;
			bmInfoHeader.biBitCount = 8;
			bmInfoHeader.biSizeImage =
				((((bmInfoHeader.biWidth* 8) + 31) & ~31) >> 3)*bmInfoHeader.biHeight;
			BITMAPFILEHEADER bmFileHeader; memset(&bmFileHeader, 0, sizeof(bmFileHeader));
			bmFileHeader.bfType = 0x4D42;	// BM
			bmFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256;
			bmFileHeader.bfSize = bmFileHeader.bfOffBits+bmInfoHeader.biSizeImage;
			file.Write(&bmFileHeader, sizeof(bmFileHeader));
			file.Write(&bmInfoHeader, sizeof(bmInfoHeader));
			
			file.Write(pSD->pPal, 256*sizeof(RGBQUAD));

			BYTE Index[256];
			for(int i=255; i>=0; i--)
			{
				memset(Index, i, 256);	file.Write(Index, 256);
			}
		}break;
	case 4:// .bin
		if(ext!=_T(".bin")) path.Append(_T(".bin"));
		if(!file.Open(path, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)) return;
		int nCount = 1<<pView->m_nBitCount;
		WORD wcolor;
		for(int i=0; i<nCount; i++)
		{
			RGBQUAD *rq = pSD->pPal+pView->m_nPalIndex+i;
			COLORREF clrColor = RGB(rq->rgbRed, rq->rgbGreen, rq->rgbBlue);
			wcolor =(WORD) ( ((clrColor&0xFF)>>3) |
					(((clrColor&0xFF00)>>11)<<5) |
					(((clrColor&0xFF0000)>>19)<<10) );
			file.Write(&wcolor, 2);
		}break;
	}
}

BOOL CT2Palette::OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	TOOLTIPTEXT *pTTT=(TOOLTIPTEXT*)pNMHDR;
	UINT nID=(UINT)pNMHDR->idFrom;
	if(pTTT->uFlags&TTF_IDISHWND)
		nID=::GetDlgCtrlID((HWND)pNMHDR->idFrom);

	if(nID)
	{
		pTTT->lpszText=MAKEINTRESOURCE(nID);
		pTTT->hinst=AfxGetResourceHandle();
		return TRUE;
	}
	return FALSE;
}

void CT2Palette::OnLoadDefPal(UINT nID)
{
	CT2View* pView = theApp.GetCurView();
	UINT nType = IDR_PALETTE+nID-ID_PALETTE_PAL1;
	if(LoadPal(pView->GetDocument()->m_pPal, nType))
	{
		Invalidate(FALSE);
		pView->OnUpdateData();

		//AddMacro_LoadDefPal(nID);
	}
}

void CT2Palette::OnSetColor(UINT nIDC)
{
	int nSelColor = m_nSetColor;
	UpdateData();
	if(nSelColor==m_nSetColor)
	{
		m_nSetColor=-1;
		UpdateData(FALSE);
	}
}
/*
void CT2Palette::AddMacro_LoadPal(CString path)
{
	CT_MACRO_1VAL_1FILE m;
	INITMACRO(m, CT2PALETTE_LOADPAL);
	lstrcpyn(m.szFile, path, _MAX_PATH);

	CT2ChildFrm* pChild = (CT2ChildFrm*)theApp.GetCurView()->GetParent();
	pChild->m_Macro.AddMacro(&m);
}

void CT2Palette::AddMacro_LoadDefPal(UINT nType)
{
	CT_MACRO_1VAL_1FILE m;
	INITMACRO(m, CT2PALETTE_LOADDEFPAL);
	m.nVal = nType;

	CT2ChildFrm* pChild = (CT2ChildFrm*)theApp.GetCurView()->GetParent();
	pChild->m_Macro.AddMacro(&m);
}
*/
void CT2Palette::OnDropFiles(HDROP hDropInfo)
{
	if(!theApp.GetCurView()) return;

	::DragQueryFile(hDropInfo, 0, g_szBuffer, _MAX_PATH);
	OnLoadPal(g_szBuffer);

	CT2NormalDlg::OnDropFiles(hDropInfo);
}
