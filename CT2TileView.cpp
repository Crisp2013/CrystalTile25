// CT2TileView.cpp : CT2TileView 类的实现
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include "CT2Doc.h"
#include ".\CT2TileView.h"
#include "CT2Palette.h"
#include "CT2MainFrm.h"
#include "CT2ChildFrm.h"
#include "CT2TileEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//#include "CT2ExplorerBar.h"

// CT2TileView

IMPLEMENT_DYNCREATE(CT2TileView, CT2View)

BEGIN_MESSAGE_MAP(CT2TileView, CT2View)
	ON_WM_PAINT()
	ON_COMMAND(ID_VIEW_GRID, OnViewGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, OnUpdateViewGrid)
	ON_COMMAND(ID_HEXVIEW_IMPORT, InsertText)
	ON_UPDATE_COMMAND_UI(ID_HEXVIEW_IMPORT, OnUpdateInsertText)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_EDIT_EXPORT, OnEditExport)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_IMPORT, OnEditImport)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_VIEW_AUTOFITCOL, OnViewAutofitcol)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AUTOFITCOL, OnUpdateViewAutofitcol)
	ON_WM_HSCROLL()
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ZOOMEXFIRST, OnUpdateToolZoomexfirst)
	ON_COMMAND_RANGE(ID_TOOLS_ZOOMEXFIRST, ID_TOOLS_ZOOMEXFIRST+9, OnToolsZoomexfirst)
	ON_COMMAND(ID_TILEEDITOR_TBL, OnTileeditorTbl)
	ON_UPDATE_COMMAND_UI(ID_TILEEDITOR_TBL, OnUpdateTileeditorTbl)
	ON_COMMAND(ID_TILEEDITOR_FONT, OnEditSetfont)
	ON_UPDATE_COMMAND_UI(ID_EDIT_IMPORT, OnUpdateTileeditorTbl)
END_MESSAGE_MAP()

// CT2TileView 构造/析构

CT2TileView::CT2TileView()
: m_nTileFormat(TF_1BPP)
, m_nWidth(8)
, m_nHeight(8)
, m_nTileSize(8)
, m_nSkipSize(0)
, m_nColorMask(1)
, m_nBytePixelCount(8)
, m_nDrawMode(CT2_DM_TILE)
, m_nBitCount(1)
, m_nScale(400)
, m_nGrid(TRUE)
, m_nColCount(16)
, m_nLineCount(16)
, m_nPalIndex(0)
, m_nL90Check(0)
, m_nR90Check(0)
, m_nHCheck(0)
, m_nVCheck(0)
, m_nLRSwapCheck(0)
, m_nViewLineSize(0)
, m_nPageSize(0)
, m_bStartMove(FALSE)
, m_bShowFocus(TRUE)
, m_nScaleWidth(0)
, m_nScaleHeight(0)
, m_n24Check(0)
, m_nNextSize(1)
, m_nAutoColFit(1)
, m_hImportBmp(NULL)
, m_nMoOffset(0)
, m_nAlignMode(0)
, m_hBkBrush(NULL)
, m_nBkPixType(IDR_BK1)
{
}

CT2TileView::~CT2TileView()
{
	UINT nIndex = theApp.GetMRUIndex(GetDocument()->GetPathName());
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	if(pSD)	Save_StateData(pSD);

	if(m_hImportBmp) DeleteObject(m_hImportBmp);
	if(m_hBkBrush) DeleteObject(m_hBkBrush);
}

// CT2TileView 消息处理程序

void CT2TileView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc; GetClientRect(&rc);

	//CBitmap memBM;
	//memBM.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	RGBQUAD *pBits;
	BITMAPINFO bi;memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biBitCount=32;
	bi.bmiHeader.biPlanes=1;
	bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth=rc.Width();
	bi.bmiHeader.biHeight=rc.Height();
	bi.bmiHeader.biSizeImage=bi.bmiHeader.biWidth*bi.bmiHeader.biHeight*4;
	HBITMAP hBM = CreateDIBSection(dc.GetSafeHdc(),
		&bi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	//memDC.SelectObject(&memBM);
	memDC.SelectObject(hBM);

	if(m_pPaintPanel)
	{
		int nViewScaleWidth = m_biPaintPanel.biWidth*m_nScale/100;
		int nViewScaleHeight = m_biPaintPanel.biHeight*m_nScale/100;
		SCROLLINFO si;
		GetScrollInfo(SB_HORZ, &si);
		int nLeft=si.nPos;

		FillRect(memDC.GetSafeHdc(), &rc, m_hBkBrush);

		RGBQUAD *pCryBits;
		CDC CryDC; CryDC.CreateCompatibleDC(&dc);
		HBITMAP hCryBM = CreateDIBSection(dc.GetSafeHdc(),
			(BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pCryBits, NULL, 0);
 		CryDC.SelectObject(hCryBM);
		FillRect(CryDC.GetSafeHdc(), &rc, m_hBkBrush);

		StretchDIBits(CryDC.GetSafeHdc(),
			0, 0,
			nViewScaleWidth-nLeft, nViewScaleHeight,
			nLeft*100/m_nScale, 0,
			m_biPaintPanel.biWidth-nLeft*100/m_nScale,
			m_biPaintPanel.biHeight,
			m_pPaintPanel, (BITMAPINFO*)&m_biPaintPanel,
			DIB_RGB_COLORS, SRCCOPY);

		UINT s=bi.bmiHeader.biSizeImage/4;
			while(s--)
			{
				if(m_nBitCount==16 || m_nBitCount==32)
				{// 不透明度
					pBits->rgbRed =
						(pCryBits->rgbRed*pCryBits->rgbReserved+
						pBits->rgbRed*(256-pCryBits->rgbReserved))/256;

					pBits->rgbGreen =
						(pCryBits->rgbGreen*pCryBits->rgbReserved+
						pBits->rgbGreen*(256-pCryBits->rgbReserved))/256;

					pBits->rgbBlue =
						(pCryBits->rgbBlue*pCryBits->rgbReserved+
						pBits->rgbBlue*(256-pCryBits->rgbReserved))/256;
				}else
					*pBits=*pCryBits;
				pBits++;pCryBits++;
			}
		DeleteObject(hCryBM);
		DeleteObject(hBM);

		if(m_hImportBmp)
		{
			BITMAP bm; GetObject(m_hImportBmp, sizeof(bm), &bm);
			CDC PasteDC; PasteDC.CreateCompatibleDC(&dc);
			PasteDC.SelectObject(m_hImportBmp);
			CT2TileEditorView *pEditor=(CT2TileEditorView*)theApp.GetView(ID_VIEW_TILEEDITORVIEW);
			if(pEditor->m_nTransparent)
			memDC.TransparentBlt(m_ptImport.x-nLeft, m_ptImport.y,
				bm.bmWidth*m_nScale/100, bm.bmHeight*m_nScale/100,
				&PasteDC, 0, 0, bm.bmWidth, bm.bmHeight, pEditor->m_clrBkColor);
			else
			memDC.StretchBlt(m_ptImport.x-nLeft, m_ptImport.y,
				bm.bmWidth*m_nScale/100, bm.bmHeight*m_nScale/100,
				&PasteDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			PasteDC.DeleteDC();
			memDC.Draw3dRect(m_ptImport.x-nLeft, m_ptImport.y,
				bm.bmWidth*m_nScale/100, bm.bmHeight*m_nScale/100,
				GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW));
		}else
		if(m_bShowFocus && ((m_nColCount>>1)||(m_nLineCount>>1)) &&
			(m_nDrawMode<CT2_DM_MAP))
		{	//Tile反色
			CRect rcFocus; GetTileRect(m_ptCursorPos, rcFocus);
			//焦点
			int c = rcFocus.Width(), l = rcFocus.Height();
			rcFocus.left=rcFocus.left*m_nScaleWidth-nLeft;
			rcFocus.top*=m_nScaleHeight;
			rcFocus.right=c*m_nScaleWidth+rcFocus.left;
			rcFocus.bottom=l*m_nScaleHeight+rcFocus.top;

			//memDC.DrawFocusRect(&rcFocus);
			rcFocus.InflateRect(1,1,2,2);
			memDC.DrawFocusRect(&rcFocus);
			rcFocus.InflateRect(-2,-2,-2,-2);
			memDC.DrawFocusRect(&rcFocus);
		}

		if(m_nGrid && (m_nScale>100))//绘制网格
		{
			CPen whiteGPen(PS_SOLID, 1, 0x9C9C9C);
			memDC.SelectObject(&whiteGPen);
			//memDC.SetBkMode(R2_NOT);
			for(int i=0; i<=m_nColCount; i++)
			{//纵线
				memDC.MoveTo(rc.left+m_nScaleWidth*i-nLeft, rc.top);
				memDC.LineTo(rc.left+m_nScaleWidth*i-nLeft, nViewScaleHeight);
			}
			for(int i=0; i<=m_nLineCount; i++)
			{//横线
				memDC.MoveTo(rc.left-nLeft, rc.top+m_nScaleHeight*i);
				memDC.LineTo(nViewScaleWidth-nLeft, rc.top+m_nScaleHeight*i);
			}
		}
	}

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.DeleteDC();
}

void CT2TileView::SetBits()
{
	CT2Doc* pDoc = GetDocument();
	RGBQUAD* pPal = pDoc->m_pPal+m_nPalIndex;
	if(!pPal) return;
	int nTileNO=0, nTilePixelNO, nPixelTop, nPixelLeft;
	BYTE* pTile;
	BYTE* pRom=pDoc->m_pRom+m_nOffset;
	BYTE* pEnd=(pDoc->m_pRom+pDoc->m_nRomSize);
	BYTE* pPixel, nPixel2, nPixel3, nPixel4;
	int nTilePixelCount = m_nDrawMode>=CT2_DM_MAP?8*8 : m_nWidth*m_nHeight;
	//size of per tile

	// 2/4层专用
	BYTE nCD=(m_n24Check?0x04:0x02);
	BYTE nCD2=4/nCD, nMask2=(1<<nCD2)-1;//m_nTileFormat=TF_GBA3XBPP

	//GB 2BPP用
	BYTE nTileLineSize = m_nDrawMode?1:m_nWidth/8;
	if(m_nWidth%8) nTileLineSize++; nTileLineSize*=2;
	UINT nOffset2 = (m_nTileFormat==TF_NES2BPP)?
		(m_nDrawMode==CT2_DM_TILE?m_nTileSize:m_nTileSize1_4)/2:
		// GB2 SNES
		(m_nDrawMode==CT2_DM_TILE?nTileLineSize:2)/2;

	WORD nScanLineSize = (WORD)m_biPaintPanel.biWidth;//*4;

	// 刷背景
	UINT nPixel = m_biPaintPanel.biWidth * m_biPaintPanel.biHeight;
	if(m_nBitCount>=16)
		memset(m_pPaintPanel, 0, m_biPaintPanel.biSizeImage);
	else
		while(nPixel--) *(UINT*)(m_pPaintPanel+nPixel)=*(UINT*)pPal;

	BYTE* pBmpTile = ((BYTE*)m_pPaintPanel)+ // 扫描线
		(m_biPaintPanel.biHeight-1)*(nScanLineSize*4); // 行

	int nTileCount = m_nDrawMode>=CT2_DM_MAP?
		(m_nWidth/8)*(m_nHeight/8) : m_nColCount*m_nLineCount;

	int nWidth=m_nDrawMode>=CT2_DM_MAP?8:m_nWidth,
		nHeight=m_nDrawMode>=CT2_DM_MAP?8:m_nHeight;

	WORD *pMoData = (WORD*)(GetDocument()->m_pRom+m_nMoOffset);
	WORD wMapData;

	while(nTileNO<nTileCount)//TileView的Tile循环
	{
		switch(m_nDrawMode)
		{
		case CT2_DM_MAP:
			wMapData = *pMoData++;
			pPal = GetDocument()->m_pPal+(m_nBitCount==4?(wMapData>>12)<<4:0);
			m_nVCheck=(wMapData>>11)&1; m_nHCheck=(wMapData>>10)&1;
			wMapData&=0x03FF;
			pTile=pRom+wMapData*m_nTileSize;
			break;
		case CT2_DM_TILE:
		case CT2_DM_OBJH:
		case CT2_DM_OBJV:
		default:
			if ((m_nTileFormat == TF_GBA3XBPP) || (m_nTileFormat == TF_CT0XBPP))
				pTile = pRom + (nTileNO / nCD) * m_nTileSize;
			else if (m_nTileFormat == TF_8C3BPP)
				pTile = pRom + (nTileNO * nWidth * nHeight * 3) / 8 + nTileNO + m_nSkipSize;
			else
				pTile=pRom+nTileNO*m_nTileSize;
			pTile+=m_nSkipSize;
			break;
		}

		nTilePixelNO=0;
		while(nTilePixelNO<nTilePixelCount)//Tile的象数循环
		{
			nPixelTop=nTilePixelNO/nWidth;
			nPixelLeft=nTilePixelNO%nWidth;

			int nTilePixelNO2 = m_nDrawMode?
						nPixelTop%8*8+nPixelLeft%8:nTilePixelNO;
			//翻转处理
			TileDirection(nTilePixelNO, nPixelTop, nPixelLeft);
	
			pPixel=pTile+(m_nBitCount>8?nTilePixelNO2*m_nBytePixelCount:nTilePixelNO2/m_nBytePixelCount);
			switch(m_nDrawMode)
			{
			case CT2_DM_OBJH:
				pPixel+=(nPixelTop/8*m_nObjTileColCount+nPixelLeft/8)*m_nTileSize1_4;
				break;
			case CT2_DM_OBJV:
				pPixel+=nPixelTop/8*m_nTileSize1_4+nPixelLeft/8*(m_nTileSize1_4*m_nObjTileLineCount);
				break;
			}
			if(pPixel>=pEnd) return; nPixel=*pPixel;
			switch(m_nTileFormat)
			{
			//MSB->LSB
			case TF_NDS1BPP:
			case TF_VB2BPP:
			// case TF_8CR3BPP:
			case TF_GBA4BPP:
			case TF_GBA8BPP:
				nPixel= nPixel>> (nTilePixelNO2%m_nBytePixelCount*m_nBitCount);
                break;
			//LSB->MSB
			case TF_1BPP:
                nPixel= nPixel>> (7- (nTilePixelNO2%m_nBytePixelCount/**m_nBitCount*/));
                break;//NO2%8 0??: >>7, 1??: >>6...
			case TF_4C2BPP:
			case TF_GBA24BPP:
				nPixel=( nPixel>> (6-nTilePixelNO2%m_nBytePixelCount*m_nBitCount));
				break;
			case TF_8C3BPP://m_nBytePixelCount = 8/3;
			//01201201
			//20120120
			//12012012
				// ((nTilePixelNO+nTileNO*nWidth*nHeight)/3)*3
				// nPixel=( nPixel>> (5-nTilePixelNO2%m_nBytePixelCount*m_nBitCount));
				// pPixel=(pTile+(nTilePixelNO2)*8)/3;
				//pPixel=(BYTE *)(((UINT)pTile*8+nTilePixelNO2*3)/8);
				pPixel=(BYTE *)(((UINT)pTile*8+nTilePixelNO2*3)/8);
				nPixel2= *pPixel;
				nPixel= *pPixel+1;
				nPixel |= nPixel2 << 8;
				//nPixel=( nPixel>> (5-nTilePixelNO2%m_nBytePixelCount*m_nBitCount));
				nPixel = (nPixel >> (8 - ((UINT)pTile + 8 + (nTilePixelNO2 + 1) * 3 ) % 8));
				break;
			
				break;
			case TF_N644BPP:
				nPixel= nPixel>> (4-nTilePixelNO2%m_nBytePixelCount*m_nBitCount);
                break;
			case TF_NGP2BPP:
				nPixelLeft = m_nWidth-1-nPixelLeft;
				pPixel=pTile+nTilePixelNO2/m_nBytePixelCount;

				switch(m_nDrawMode)
				{
				case CT2_DM_OBJH:
					pPixel+=(nPixelTop/8*m_nObjTileColCount+nPixelLeft/8)*m_nTileSize1_4;
					break;
				case CT2_DM_OBJV:
					pPixel+=nPixelTop/8*m_nTileSize1_4+nPixelLeft/8*(m_nTileSize1_4*m_nObjTileLineCount);
					break;
				}
				if(pPixel>=pEnd) return;
				nPixel = *pPixel;
				nPixel= nPixel>> (nTilePixelNO2%m_nBytePixelCount*m_nBitCount);
				break;
			case TF_SMS4BPP:
				pPixel+=(m_nDrawMode?nTilePixelNO2:nTilePixelNO)/8*3;
				if(pPixel>=pEnd) return;
				nPixel=*pPixel;
				nPixel = ((nPixel>> (7-(nTilePixelNO%m_nBytePixelCount)))&1)|
                    (((*(pPixel+1)>> (7-(nTilePixelNO%m_nBytePixelCount)))&1)<<1)|
					(((*(pPixel+2)>> (7-(nTilePixelNO%m_nBytePixelCount)))&1)<<2)|
					(((*(pPixel+3)>> (7-(nTilePixelNO%m_nBytePixelCount)))&1)<<3);
				break;
			case TF_GBA3XBPP:
			case TF_CT0XBPP://??OBJHV
				nPixel=(nTilePixelNO&1)?nPixel>>4:nPixel&0x0F;
				nPixel=(nPixel>>((nTileNO%nCD)*nCD2))&nMask2;
				break;
			
			case TF_SNES4BPP:
				switch(m_nDrawMode)
				{
				case CT2_DM_TILE:
					pPixel=(pTile+nPixelTop*nTileLineSize+nPixelLeft/m_nBytePixelCount);
					break;
				case CT2_DM_OBJH:
					pPixel=(pTile+nPixelTop%8*nTileLineSize+nPixelLeft%8/m_nBytePixelCount);
					pPixel+=(nPixelTop/8*m_nObjTileColCount+nPixelLeft/8)*m_nTileSize1_4;
					break;
				case CT2_DM_OBJV:
					pPixel=(pTile+nPixelTop%8*nTileLineSize+nPixelLeft%8/m_nBytePixelCount);
					pPixel+=nPixelTop/8*m_nTileSize1_4+nPixelLeft/8*(m_nTileSize1_4*m_nObjTileLineCount);
					break;
				}
					if(pPixel+nOffset2+(nTilePixelCount/4)>=pEnd) return;
					nPixel=*pPixel;
					nPixel=( nPixel>> (7- (nPixelLeft%8)) )&0x01;
					nPixel2=*(pPixel+nOffset2);
					nPixel2=( nPixel2>> (7- (nPixelLeft%8)) )&0x01;
					nPixel|=nPixel2<<1;
					nPixel2=*(pPixel+(nTilePixelCount/4));
					nPixel2=( nPixel2>> (7- (nPixelLeft%8)) )&0x01;
					nPixel|=nPixel2<<2;
					nPixel2=*(pPixel+nOffset2+(nTilePixelCount/4));
					nPixel2=( nPixel2>> (7- (nPixelLeft%8)) )&0x01;
					nPixel|=nPixel2<<3;
				break;
			case TF_GB2BPP:
				switch(m_nDrawMode)
				{
				case CT2_DM_TILE:
					pPixel=(pTile+nPixelTop*nTileLineSize+nPixelLeft/m_nBytePixelCount);
					break;
				case CT2_DM_OBJH:
					pPixel=(pTile+nPixelTop%8*nTileLineSize+nPixelLeft%8/m_nBytePixelCount);
					pPixel+=(nPixelTop/8*m_nObjTileColCount+nPixelLeft/8)*m_nTileSize1_4;
					break;
				case CT2_DM_OBJV:
					pPixel=(pTile+nPixelTop%8*nTileLineSize+nPixelLeft%8/m_nBytePixelCount);
					pPixel+=nPixelTop/8*m_nTileSize1_4+nPixelLeft/8*(m_nTileSize1_4*m_nObjTileLineCount);
					break;
				}
			case TF_NES2BPP:
					if(pPixel+nOffset2>=pEnd) return;
					nPixel=*pPixel;
					nPixel=( nPixel>> (7- (nPixelLeft%8)) )&0x01;
					nPixel2=*(pPixel+nOffset2);
					nPixel2=( nPixel2>> (7- (nPixelLeft%8)) )&0x01;
					nPixel|=nPixel2<<1;
				break;
			case TF_16BPP:
				nPixel = *(WORD*)pPixel;
				nPixel = WordToRgbQuad(nPixel);
				break;
			case TF_24BPP:
				nPixel = *(UINT*)pPixel;
				break;
			case TF_32BPP:
				nPixel = *(UINT*)pPixel;
				nPixel = ColorToRgbQuad(nPixel);
				break;
			case TF_RHYTHM:
				switch(m_nDrawMode)
				{
				case CT2_DM_TILE:
					pPixel = pTile+nPixelTop/4*(m_nWidth/2)+(nPixelLeft%m_nWidth)/8*4;
					break;
				case CT2_DM_OBJH:
					pPixel = pTile+nPixelTop%8/4*(8/2)+(nPixelLeft%8)/8*4;
					pPixel+=(nPixelTop/8*m_nObjTileColCount+nPixelLeft/8)*m_nTileSize1_4;
					break;
				case CT2_DM_OBJV:
					pPixel = pTile+nPixelTop%8/4*(8/2)+(nPixelLeft%8)/8*4;
					pPixel+=nPixelTop/8*m_nTileSize1_4+nPixelLeft/8*(m_nTileSize1_4*m_nObjTileLineCount);
					break;
				}
				if(pPixel>=pEnd) return;
				nPixel = (*(UINT*)pPixel >> nPixelTop%4)&0x11111111;
				nPixel = nPixel >> (((nPixelLeft%8/2))*8);
				nPixel = nPixelLeft&1?nPixel>>4:nPixel&0xF;
				break;
			default:
				nPixel=0; // 背景色
				break;
			}
			nPixel&=m_nColorMask;
			if(nPixel)
			{
				if(m_nBitCount>8)
					*((UINT*)pBmpTile- // 扫描线
					nPixelTop*nScanLineSize+ // 行
					nPixelLeft) = // 列
					nPixel;
				else
					*((RGBQUAD*)pBmpTile- // 扫描线
					nPixelTop*nScanLineSize+ // 行
					nPixelLeft) = // 列
					*(pPal+nPixel);
			}
			nTilePixelNO++;//增量
		}
		nTileNO++;//增量

		pBmpTile += nWidth*4;// Tile列
		if(!(nTileNO%m_nColCount))
			pBmpTile-=nScanLineSize*4*(nHeight+1);
	}
}

void CT2TileView::SetPixel(WORD nTilePixelNO, UINT nPixel)
{
	CT2Doc* pDoc = GetDocument();
	int nPixelTop, nPixelLeft;
	BYTE* pTile;
	BYTE* pRom=pDoc->m_pRom+m_nOffset;
	BYTE* pEnd=(pDoc->m_pRom+pDoc->m_nRomSize);
	BYTE* pPixel, nPixel1, nPixel2, nPixel3, nPixel4;

	// 2/4层专用
	BYTE nCD=(m_n24Check?0x04:0x02);
	BYTE nCD2=4/nCD, nMask2=(1<<nCD2)-1;//m_nTileFormat=TF_GBA3XBPP
	
	int nWidth=m_nDrawMode>=CT2_DM_MAP?8:m_nWidth,
		nHeight=m_nDrawMode>=CT2_DM_MAP?8:m_nHeight;
	UINT nTilePixelCount = nWidth*nHeight;
	//GB 2BPP用
	BYTE nTileLineSize = m_nDrawMode?1:m_nWidth/8;
	if(m_nWidth%8) nTileLineSize++; nTileLineSize*=2;
	UINT nOffset2 = (m_nTileFormat==TF_NES2BPP)?
		(m_nDrawMode==CT2_DM_TILE?m_nTileSize:m_nTileSize1_4)/2:
		// GB2 SNES
		(m_nDrawMode==CT2_DM_TILE?nTileLineSize:2)/2;

	if(m_nDrawMode>=CT2_DM_MAP)
	{
		int nMap = m_ptCursorPos.x*2;
		WORD wMapData = *(WORD*)(pDoc->m_pRom+m_nMoOffset+nMap);
		m_nVCheck=(wMapData>>11)&1; m_nHCheck=(wMapData>>10)&1;

		wMapData&=0x03FF;

		pTile=pRom+wMapData*m_nTileSize;
	}else
	{
		pTile=(m_nTileFormat!=TF_GBA3XBPP)&&(m_nTileFormat!=TF_CT0XBPP)?
			pRom+m_ptCursorPos.x*m_nTileSize:
			pRom+(m_ptCursorPos.x/nCD)*m_nTileSize;
		pTile+=m_nSkipSize;
	}
	if(m_nDrawMode>=CT2_DM_MAP)
	{
		nPixelTop=nTilePixelNO/8;
		nPixelLeft=nTilePixelNO%8;
	}else
	{
		nPixelTop=nTilePixelNO/m_nWidth;
		nPixelLeft=nTilePixelNO%m_nWidth;
	}
	//翻转处理
	TileDirection(nTilePixelNO, nPixelTop, nPixelLeft);
	nTilePixelNO = nPixelTop*m_nWidth+nPixelLeft;
	if(m_nL90Check||m_nR90Check)
	{
		nPixelLeft = m_nWidth-1-nPixelLeft;
		nPixelTop = m_nHeight-1-nPixelTop;
		nTilePixelNO = nPixelTop*m_nWidth+nPixelLeft;
	}

	int nTilePixelNO2 = m_nDrawMode?
				nPixelTop%8*8+nPixelLeft%8:nTilePixelNO;

	pPixel=pTile+(m_nBitCount>8?nTilePixelNO2*m_nBytePixelCount:nTilePixelNO2/m_nBytePixelCount);
	nPixel&=m_nColorMask;
	switch(m_nDrawMode)
	{
	case CT2_DM_OBJH:
		pPixel+=(nPixelTop/8*m_nObjTileColCount+nPixelLeft/8)*m_nTileSize1_4;
		if(m_nTileFormat!=TF_RHYTHM && m_nTileFormat!=TF_GB2BPP && m_nTileFormat!=TF_SNES4BPP && m_nTileFormat!=TF_NES2BPP && m_nTileFormat!=TF_NGP2BPP)
		{
		nPixelTop%=8; nPixelLeft%=8; nTilePixelNO=nPixelTop*8+nPixelLeft;
		}
		break;
	case CT2_DM_OBJV:
		pPixel+=nPixelTop/8*m_nTileSize1_4+nPixelLeft/8*(m_nTileSize1_4*m_nObjTileLineCount);
		if(m_nTileFormat!=TF_RHYTHM && m_nTileFormat!=TF_GB2BPP && m_nTileFormat!=TF_SNES4BPP && m_nTileFormat!=TF_NES2BPP && m_nTileFormat!=TF_NGP2BPP)
		{
		nPixelTop%=8; nPixelLeft%=8; nTilePixelNO=nPixelTop*8+nPixelLeft;
		}
		break;
	}
	if(pPixel>=pEnd) return;
	switch(m_nTileFormat)
	{
		//MSB->LSB
	case TF_NDS1BPP:
	case TF_VB2BPP:
	// case TF_8CR3BPP:
	case TF_GBA4BPP:
	case TF_GBA8BPP:
	    *pPixel&=~(m_nColorMask<<(nTilePixelNO%m_nBytePixelCount*m_nBitCount));
        *pPixel|=nPixel<<(nTilePixelNO%m_nBytePixelCount*m_nBitCount);
        break;
	//LSB->MSB
	case TF_1BPP:
        *pPixel&=~(m_nColorMask<<(7- (nTilePixelNO%m_nBytePixelCount)));
		*pPixel|=nPixel<<(7- (nTilePixelNO%m_nBytePixelCount));
        break;
	case TF_4C2BPP:
	case TF_GBA24BPP:
		*pPixel&=~(m_nColorMask<<(6-nTilePixelNO%m_nBytePixelCount*m_nBitCount));
		*pPixel|=nPixel<<(6-nTilePixelNO%m_nBytePixelCount*m_nBitCount);
		break;
	// case TF_8C3BPP:
		// *pPixel&=~(m_nColorMask<<(5-nTilePixelNO%m_nBytePixelCount*m_nBitCount));
		// *pPixel|=nPixel<<(5-nTilePixelNO%m_nBytePixelCount*m_nBitCount);
		// break;
	case TF_N644BPP:
        *pPixel&=~(m_nColorMask<<(4-nTilePixelNO%m_nBytePixelCount*m_nBitCount));
        *pPixel|=nPixel<<(4-nTilePixelNO%m_nBytePixelCount*m_nBitCount);
        break;
	case TF_NGP2BPP:
		nTilePixelNO2 = m_nDrawMode?(nPixelTop&7)*8+(7-(nPixelLeft&7)):nPixelTop*m_nWidth+m_nWidth-1-nPixelLeft;
		pPixel=pTile+nTilePixelNO2/m_nBytePixelCount;

		switch(m_nDrawMode)
		{
		case CT2_DM_OBJH:
			pPixel+=(nPixelTop/8*m_nObjTileColCount+nPixelLeft/8)*m_nTileSize1_4;
			break;
		case CT2_DM_OBJV:
			pPixel+=nPixelTop/8*m_nTileSize1_4+nPixelLeft/8*(m_nTileSize1_4*m_nObjTileLineCount);
			break;
		}
		if(pPixel>=pEnd) return;
		*pPixel &= ~(m_nColorMask<<nTilePixelNO2%m_nBytePixelCount*m_nBitCount);
		*pPixel |= nPixel<<(nTilePixelNO2%m_nBytePixelCount*m_nBitCount);
		break;
	case TF_SMS4BPP:
		pPixel+=(m_nDrawMode?nTilePixelNO2:nTilePixelNO)/8*3;
		if(pPixel>=pEnd) return;
		*pPixel &= ~(1<<(7-(nTilePixelNO%m_nBytePixelCount)));
		*pPixel++|= (nPixel&1)<<(7-(nTilePixelNO%m_nBytePixelCount));
		*pPixel &=~(1<<(7-(nTilePixelNO%m_nBytePixelCount)));
        *pPixel++|=((nPixel>>1)&1)<<(7-(nTilePixelNO%m_nBytePixelCount));
		*pPixel &=~(1<<(7-(nTilePixelNO%m_nBytePixelCount)));
		*pPixel++|=((nPixel>>2)&1)<<(7-(nTilePixelNO%m_nBytePixelCount));
		*pPixel &=~(1<<(7-(nTilePixelNO%m_nBytePixelCount)));
		*pPixel|=((nPixel>>3)&1)<<(7-(nTilePixelNO%m_nBytePixelCount));
		break;
	case TF_GBA3XBPP:
	case TF_CT0XBPP:
		*pPixel&=~((nTilePixelNO&1)?m_nColorMask>>4:m_nColorMask&0x0F);
		*pPixel|=nPixel<<((m_ptCursorPos.x%nCD)*nCD2);
		break;
	case TF_SNES4BPP:
		switch(m_nDrawMode)
		{
		case CT2_DM_TILE:
			pPixel=(pTile+nPixelTop*nTileLineSize+nPixelLeft/m_nBytePixelCount);
			break;
		case CT2_DM_OBJH:
			pPixel=(pTile+nPixelTop%8*nTileLineSize+nPixelLeft%8/m_nBytePixelCount);
			pPixel+=(nPixelTop/8*m_nObjTileColCount+nPixelLeft/8)*m_nTileSize1_4;
			break;
		case CT2_DM_OBJV:
			pPixel=(pTile+nPixelTop%8*nTileLineSize+nPixelLeft%8/m_nBytePixelCount);
			pPixel+=nPixelTop/8*m_nTileSize1_4+nPixelLeft/8*(m_nTileSize1_4*m_nObjTileLineCount);
			break;
		}
			if(pPixel+nOffset2+nTilePixelCount/4>=pEnd) return;
				nPixel1 = nPixel&0x01;
				nPixel2 = (nPixel>>1)&0x01;
				nPixel3 = (nPixel>>2)&0x01;
				nPixel4 = (nPixel>>3)&0x01;
				*pPixel&=~(1<<(7- (nPixelLeft%8)) );
				*pPixel|=nPixel1<<(7- (nPixelLeft%8));
				*(pPixel+nOffset2)&=~(1<<(7- (nPixelLeft%8)));
				*(pPixel+nOffset2)|=nPixel2<<(7- (nPixelLeft%8));
				*(pPixel+(nTilePixelCount/4))&=~(1<<(7- (nPixelLeft%8)) );
				*(pPixel+(nTilePixelCount/4))|=nPixel3<<(7- (nPixelLeft%8));
				*(pPixel+nOffset2+(nTilePixelCount/4))&=~(1<<(7- (nPixelLeft%8)));
				*(pPixel+nOffset2+(nTilePixelCount/4))|=nPixel4<<(7- (nPixelLeft%8));
		break;
	case TF_GB2BPP:
		switch(m_nDrawMode)
		{
		case CT2_DM_TILE:
			pPixel=(pTile+nPixelTop*nTileLineSize+nPixelLeft/m_nBytePixelCount);
			break;
		case CT2_DM_OBJH:
			pPixel=(pTile+nPixelTop%8*nTileLineSize+nPixelLeft%8/m_nBytePixelCount);
			pPixel+=(nPixelTop/8*m_nObjTileColCount+nPixelLeft/8)*m_nTileSize1_4;
			break;
		case CT2_DM_OBJV:
			pPixel=(pTile+nPixelTop%8*nTileLineSize+nPixelLeft%8/m_nBytePixelCount);
			pPixel+=nPixelTop/8*m_nTileSize1_4+nPixelLeft/8*(m_nTileSize1_4*m_nObjTileLineCount);
			break;
		}
	case TF_NES2BPP:
			if(pPixel+nOffset2>=pEnd) return;
			nPixel1 = nPixel&0x01;
			nPixel2 = (nPixel>>1)&0x01;
			*pPixel&=~(1<<(7- (nPixelLeft%8)) );
			*pPixel|=nPixel1<<(7- (nPixelLeft%8));
			*(pPixel+nOffset2)&=~(1<<(7- (nPixelLeft%8)));
			*(pPixel+nOffset2)|=nPixel2<<(7- (nPixelLeft%8));
		break;
	case TF_16BPP:
		nPixel = RgbQuadToWord(nPixel);
        *(WORD*)pPixel=nPixel;
		break;
	case TF_24BPP:
		*(WORD*)pPixel = nPixel;
		*(pPixel+2) = nPixel>>16;
		break;
	case TF_32BPP:
		nPixel = RgbQuadToColor(nPixel);
        *(UINT*)pPixel=nPixel;
		break;
	case TF_RHYTHM:
		{
			switch(m_nDrawMode)
			{
			case CT2_DM_TILE:
				pPixel = pTile+nPixelTop/4*(m_nWidth/2)+(nPixelLeft%m_nWidth)/8*4;
				break;
			case CT2_DM_OBJH:
				pPixel = pTile+nPixelTop%8/4*(8/2)+(nPixelLeft%8)/8*4;
				pPixel+=(nPixelTop/8*m_nObjTileColCount+nPixelLeft/8)*m_nTileSize1_4;
				break;
			case CT2_DM_OBJV:
				pPixel = pTile+nPixelTop%8/4*(8/2)+(nPixelLeft%8)/8*4;
				pPixel+=nPixelTop/8*m_nTileSize1_4+nPixelLeft/8*(m_nTileSize1_4*m_nObjTileLineCount);
				break;
			}
			if(pPixel>=pEnd) return;
			UINT nMask = nPixelLeft&1?0x10:01;

			nMask = nMask << (((nPixelLeft%8/2))*8);

			nMask = (nMask << nPixelTop%4);

			*(UINT*)pPixel &= ~nMask;
			//
			nPixel = nPixelLeft&1?nPixel<<4:nPixel;

			nPixel = nPixel << (((nPixelLeft%8/2))*8);

			nPixel = (nPixel << nPixelTop%4);

			*(UINT*)pPixel |= nPixel;
		}break;
	default:
		return;
	}
}

void CT2TileView::OnUpdateData()
{
	CT2Doc *pDoc = GetDocument();
	switch(m_nTileFormat)
	{
	default:
	case TF_1BPP:
	case TF_NDS1BPP:
	case TF_RHYTHM:
		m_nBitCount = 1;
		break;
	case TF_GB2BPP:
	case TF_4C2BPP:
	case TF_VB2BPP:
	case TF_NGP2BPP:
	case TF_NES2BPP:
		m_nBitCount = 2;
		break;
	case TF_8C3BPP:
	// case TF_8CR3BPP:
		m_nBitCount = 3;
		break;
	case TF_GBA4BPP:
	case TF_N644BPP:
	case TF_GBA24BPP:
	case TF_SNES4BPP:
	case TF_SMS4BPP:
		m_nBitCount = 4;
		break;
	case TF_GBA8BPP:
		m_nBitCount = 8;
		break;
	case TF_GBA3XBPP:
	case TF_CT0XBPP:
		m_nBitCount = m_n24Check?1:2;
		break;
	case TF_16BPP:
		m_nBitCount = 16;
		break;
	case TF_24BPP:
		m_nBitCount = 24;
		break;
	case TF_32BPP:
		m_nBitCount = 32;
		break;
	}

	if(m_nWidth<1) m_nWidth=1;
	if(m_nHeight<1) m_nHeight=1;
	if(m_nDrawMode)
	{
		if(m_nWidth&7) m_nWidth+=8-(m_nWidth&7);
		if(m_nHeight&7) m_nHeight+=8-(m_nHeight&7);
		if(m_nWidth*m_nHeight<128)// 至少8x16或16x8
			m_nWidth = m_nHeight = 16;
	}
	if(m_nTileFormat==TF_RHYTHM)
	{
		if(m_nWidth&7) m_nWidth+=8-(m_nWidth&7);
		if(m_nHeight&3) m_nHeight+=4-(m_nHeight&3);
	}

	int nWidth, nHeight;
	if(m_nDrawMode>=CT2_DM_MAP)
	{
		m_nAutoColFit = 1;
		m_ptCursorPos.x=m_ptCursorPos.y=0;
		nWidth = nHeight = 8;
	}else
	{
		nWidth = m_nWidth;
		nHeight = m_nHeight;
	}
	m_nScaleWidth = nWidth*m_nScale/100;
	m_nScaleHeight = nHeight*m_nScale/100;
	if(m_nBitCount>8)
		m_nColorMask=-1;
	else
		m_nColorMask = ((ULONGLONG)1<<m_nBitCount)-1;

	if(m_nTileFormat==TF_SMS4BPP || m_nTileFormat==TF_NES2BPP || m_nTileFormat==TF_GB2BPP || m_nTileFormat==TF_SNES4BPP)
		m_nBytePixelCount=8;
	else
	if(m_nTileFormat==TF_8C3BPP)
		m_nBytePixelCount=3;
	else
	if(m_nTileFormat==TF_GBA3XBPP)
		m_nBytePixelCount=2;
	else
	if(m_nTileFormat==TF_CT0XBPP)
		m_nBytePixelCount=1;
	else
	if(m_nBitCount>8)
		m_nBytePixelCount = m_nBitCount/8;
	else
		m_nBytePixelCount = 8/m_nBitCount;

	int nBits = m_nTileFormat==TF_GBA3XBPP?4:
				(m_nTileFormat==TF_CT0XBPP?8:m_nBitCount);

	int nSkipSize = (m_nDrawMode>=CT2_DM_MAP?0:m_nSkipSize);
	m_nTileSize=(nWidth*nHeight*nBits)/8 + nSkipSize;
	if(m_nTileSize<1) m_nTileSize=1;

	// ObjH/ObjV
	m_nObjTileColCount=m_nWidth/8;
	m_nObjTileLineCount=m_nHeight/8;
	if(m_nObjTileColCount<1) m_nObjTileColCount=1;
	if(m_nObjTileLineCount<1) m_nObjTileLineCount=1;
	m_nTileSize1_4=(m_nTileSize-nSkipSize)/(m_nObjTileColCount*m_nObjTileLineCount);

	CRect rc; GetClientRect(&rc);
	if(rc.IsRectEmpty()) return;
	if(m_nDrawMode>=CT2_DM_MAP)
		m_nColCount=m_nWidth/8;
	else
	{
		if(m_nAutoColFit) m_nColCount=rc.Width()/m_nScaleWidth;
		if(m_nColCount*m_nTileSize>=pDoc->m_nRomSize)
			m_nColCount=pDoc->m_nRomSize/m_nTileSize;
		if(m_nColCount<1) m_nColCount=1;
	}
	m_nViewLineSize = m_nTileSize*m_nColCount;

	if(m_nDrawMode>=CT2_DM_MAP)
		m_nLineCount=m_nHeight/8;
	else
	{
		if(m_nAutoColFit) m_nLineCount = rc.Height() / m_nScaleHeight;
		if(m_nLineCount*m_nColCount*m_nTileSize>=pDoc->m_nRomSize)
			m_nLineCount=pDoc->m_nRomSize/m_nViewLineSize;
		if(m_nLineCount<1) m_nLineCount=1;
	}
	if(m_ptCursorPos.x>m_nLineCount*m_nColCount)
		m_ptCursorPos.x=m_ptCursorPos.y=0;

	m_nPageSize = m_nViewLineSize*m_nLineCount;

	m_biPaintPanel.biWidth = m_nColCount*nWidth;
	m_biPaintPanel.biHeight = m_nLineCount*nHeight;

	int nSize = m_biPaintPanel.biWidth*m_biPaintPanel.biHeight*4;
	if( (m_biPaintPanel.biSizeImage != nSize) ||
		!m_pPaintPanel )
	{
		m_biPaintPanel.biSizeImage = nSize;
		if(m_pPaintPanel) delete[] m_pPaintPanel;
		m_pPaintPanel = (RGBQUAD*) new BYTE[nSize];
	}

	switch(m_nBitCount)
	{
	case 16:
	case 24:
	case 32:
	case 8:
		m_nPalIndex = 0;
		break;
	default:
		int nCount = 1<<m_nBitCount;
		if((m_nPalIndex%16+nCount)>16)
			m_nPalIndex=m_nPalIndex/16*16+16-nCount;
		break;
	}
	if(m_nLRSwapCheck)
		m_nLRSwapCheck = !(m_nWidth&1);

	SetBits();
	//设置滚动条
	CSize szVert;
	szVert.cx=m_biPaintPanel.biWidth*m_nScale/100;
	szVert.cy=pDoc->m_nRomType==CT2_RT_NSCR?0:(LONG)((pDoc->m_nRomSize/m_nViewLineSize+1)*(m_nScaleHeight));
	SetScrollSizes(MM_TEXT, szVert);
	SetScrollPos(SB_VERT, (m_nOffset/m_nViewLineSize)*(m_nScaleHeight));

	if(theApp.GetCurView()==this)
	{
		CT2StatusBar *pSb = theApp.GetStatusBar();
		CString lc,format;
		format.LoadString(IDS_LINECOL);
		lc.Format(format, m_nLineCount, m_nColCount, m_nLineCount*m_nColCount, m_nTileSize, m_nTileSize);
		pSb->SetPaneText(pSb->CommandToIndex(IDS_LINECOL), lc);

		pSb->SetPaneText(0, pDoc->OffsetToPathName(
				m_nOffset+m_ptCursorPos.x*m_nTileSize));

		theApp.GetPalDlg().Invalidate(FALSE);
	}
	CT2View::OnUpdateData();
}

void CT2TileView::OnViewGrid()
{
	m_nGrid=~m_nGrid;
	Invalidate(FALSE);
}

void CT2TileView::OnUpdateViewGrid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nGrid);
}

void CT2TileView::TileDirection(int nNo, int& nTop, int& nLeft)
{
	//翻转处理
	if(m_nHCheck)
	{
		if(m_nDrawMode)
			nLeft=(nLeft&0xFFFFFFF8)+(7-(nLeft&7));
		else
			nLeft=m_nWidth-1-nLeft;
	}
	if(m_nVCheck)
	{
		if(m_nDrawMode)
			nTop=(nTop&0xFFFFFFF8)+(7-(nTop&7));
		else
			nTop=m_nHeight-1-nTop;
	}
	if(m_nDrawMode>=CT2_DM_MAP) return;

	if(m_nL90Check)
	{
		nLeft=nNo/m_nWidth;
		nTop=nNo%m_nWidth;
		nTop=m_nHeight-1-nTop;
	}else
	if(m_nR90Check)
	{
		nLeft=nNo/m_nWidth;
		nTop=nNo%m_nWidth;
		nLeft=m_nWidth-1-nLeft;
	}

	if( m_nLRSwapCheck &&
		(m_nTileFormat!=TF_GBA3XBPP) && (m_nTileFormat!=TF_CT0XBPP) )
	{
		if(m_nL90Check||m_nR90Check)
		{
			int nTileHalf=m_nHeight/2;
			if(nTop>=nTileHalf)
				nTop-=nTileHalf;
			else
				nTop+=nTileHalf;
		}else
		{
			int nTileHalf=m_nWidth/2;
			if(nLeft>=nTileHalf)
				nLeft-=nTileHalf;
			else
				nLeft+=nTileHalf;
		}
	}
}

void CT2TileView::OnInitialUpdate()
{
	CT2View::OnInitialUpdate();

	CT2Doc *pDoc = GetDocument();
	CString Name = pDoc->GetPathName();
	UINT nIndex = theApp.GetMRUIndex(Name);
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	pDoc->m_pPal = pSD->pPal;
	if(theApp.m_pRecentStatList->IsOK(nIndex))
	{
		Load_StateData(pSD);
	}else
	{
		theApp.GetPalDlg().LoadPal(pSD->pPal);
		Name = PathFindExtension(Name);
		Name.MakeLower();
		if( pDoc->m_nRomType == CT2_RT_GBA ||
			pDoc->m_nRomType == CT2_RT_NDS )
			m_nTileFormat=TF_GBA4BPP;
		else
		if( (Name == _T(".gb")) || (Name == _T(".gbc")) )
			m_nTileFormat=TF_GB2BPP;
		else
		if( (Name == _T(".nes")) || (Name == _T(".fc")) )
			m_nTileFormat=TF_NES2BPP;
		else
		if( (Name == _T(".sfc")) || (Name == _T(".smc")) )
			m_nTileFormat=TF_SNES4BPP;
		else
		if( (Name == _T(".sms")) )
			m_nTileFormat=TF_SMS4BPP;
		else
		//if( (Name == _T(".n64")) || (Name == _T(".z64")) || (Name == _T(".md")))
		if(pDoc->m_nRomType == CT2_RT_N64)
			m_nTileFormat=TF_N644BPP;
		else
			SetCurView(ID_VIEW_HEXVIEW);

		m_nBkPixType = IDR_BK1+theApp.GetProfileInt(CT2Settings, CT2DefBk, 0);
	}

	while(pDoc->m_nRomType == CT2_RT_NSCR)
	{
		NDSFILEHEADER *header;
		UINT nPosNSCR = 0;
		header = (NDSFILEHEADER*)(pDoc->m_pRom + nPosNSCR);
		if(*(UINT*)header->falgs!=0x4E534352) break;
		nPosNSCR+=header->nSize;
		header = (NDSFILEHEADER*)(pDoc->m_pRom + nPosNSCR);
		if(*(UINT*)header->falgs!=0x5343524E) break;

		UINT nPosNCGR = pDoc->m_nMFileSize[0];
		header = (NDSFILEHEADER*)(pDoc->m_pRom + nPosNCGR);
		if(*(UINT*)header->falgs!=0x4E434752) break;
		nPosNCGR+=header->nSize;
		header = (NDSFILEHEADER*)(pDoc->m_pRom + nPosNCGR);
		if(*(UINT*)header->falgs!=0x43484152) break;

		UINT nPosNCLR = pDoc->m_nMFileSize[0]+pDoc->m_nMFileSize[1];
		header = (NDSFILEHEADER*)(pDoc->m_pRom + nPosNCLR);
		if(*(UINT*)header->falgs!=0x4E434C52) break;
		nPosNCLR+=header->nSize;
		header = (NDSFILEHEADER*)(pDoc->m_pRom + nPosNCLR);
		if(*(UINT*)header->falgs!=0x504C5454) break;

		m_nOffset = nPosNCGR+0x20;
		m_nMoOffset = nPosNSCR+0x14;

		m_nTileFormat = *(WORD*)(pDoc->m_pRom+nPosNCGR+0x0C)==4?TF_GBA8BPP:TF_GBA4BPP;
		m_nDrawMode = CT2_DM_MAP;
		m_nWidth = *(WORD*)(pDoc->m_pRom+nPosNSCR+0x08);
		m_nHeight = *(WORD*)(pDoc->m_pRom+nPosNSCR+0x0A);

		m_nScale = 100;

		if(pDoc->m_pPal)
			CNdsFileSystemInfo::LoadPal((WORD*)(pDoc->m_pRom+nPosNCLR+0x18), pDoc->m_pPal);
		theApp.GetPalDlg().Invalidate(FALSE);

		//CCrystalBar *pBar = theApp.GetExplorerBox();
		//pBar->GetDlgItem(IDC_TILEVIEW_DRAWMODE)->EnableWindow(FALSE);
		break;
	}

	LoadBkBmp(m_nBkPixType);

	OnUpdateData();
}

void CT2TileView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//if(GetDocument()->m_nRomType==CT2_RT_NSCR) return;
	char m_bUD=0;
	switch(nSBCode)
	{
	case SB_ENDSCROLL:
		return;
	case SB_LINEUP:
		if(m_nOffset<m_nViewLineSize)
			m_nOffset=0;
		else
			m_nOffset-=m_nViewLineSize;
		break;
	case SB_LINEDOWN:
		if(m_nOffset<(GetDocument()->m_nRomSize-m_nViewLineSize))
			m_nOffset+=m_nViewLineSize;
		break;
	case SB_PAGEUP:
		if(m_nOffset<m_nPageSize)
			m_nOffset=0;
		else
			m_nOffset-=m_nPageSize;
		break;
	case SB_PAGEDOWN:
		if(m_nOffset+m_nPageSize<GetDocument()->m_nRomSize)
			m_nOffset+=m_nPageSize;
		break;
	case SB_TOP:
		m_nOffset=0;
		break;
	case SB_BOTTOM:
		if(GetDocument()->m_nRomSize<m_nPageSize)
			m_nOffset=0;
		else
			m_nOffset=GetDocument()->m_nRomSize-m_nPageSize;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			SCROLLINFO SI;
			GetScrollInfo(SB_VERT, &SI, SIF_TRACKPOS);
			m_nOffset=(SI.nTrackPos/(m_nScaleHeight))*
				m_nViewLineSize+m_nOffset%m_nViewLineSize;
			SetScrollPos(SB_VERT, (m_nOffset/m_nViewLineSize)*(m_nScaleHeight));
		}break;
	default:
		return;
	}
	OnUpdateData();

	// CT2View::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CT2TileView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(GetDocument()->m_nRomType==CT2_RT_NSCR) return TRUE;

	if(zDelta<0)
		m_nOffset += m_nViewLineSize * (m_nLineCount/2);
	else
		m_nOffset -= m_nViewLineSize * (m_nLineCount/2);

	if( (m_nOffset>(GetDocument()->m_nRomSize-m_nPageSize)) ||
		(GetDocument()->m_nRomSize<m_nPageSize) )
	{
		if(zDelta<0)
		{
			if(GetDocument()->m_nRomSize>m_nPageSize)
				m_nOffset=GetDocument()->m_nRomSize-m_nPageSize;
		}else
			m_nOffset=0;
	}
	OnUpdateData();

	return TRUE;//CT2View::OnMouseWheel(nFlags, zDelta, pt);
}

void CT2TileView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	nChar = towlower(nChar);
	if( (m_nDrawMode>=CT2_DM_MAP) &&
		(nChar!=VK_ESCAPE) &&
		(nChar!=VK_RETURN) &&
		(nChar!=VK_TAB) )
		goto FKEY;

	switch(nChar)
	{
	case VK_LEFT:
		if(GetKeyState(VK_CONTROL)&0x80)
		{
			if(GetKeyState(VK_SHIFT)&0x80)
			{
				if(m_nOffset>=m_nBytePixelCount)
					m_nOffset-=m_nBytePixelCount;
			}else
			if(m_nOffset>=m_nNextSize)
			{
				m_nOffset-=m_nNextSize;
				OnUpdateData();
			}
		}else
		if(GetKeyState(VK_SHIFT)&0x80)
		{
			if(m_nColCount>1)
			{
				m_nColCount--;
				m_nAutoColFit=0;
			}
		}else
		if(m_nOffset && (m_nOffset>=m_nTileSize))
			m_nOffset-=m_nTileSize;
		else
		if(m_ptCursorPos.x)
		{
				m_ptCursorPos.x--;
				m_ptCursorPos.y=m_ptCursorPos.x;
		}
		OnUpdateData();
		SetCurTileOffset();
		break;
	case VK_RIGHT:
		if(GetKeyState(VK_CONTROL)&0x80)
		{
			if(GetKeyState(VK_SHIFT)&0x80)
			{
				if((m_nOffset+m_nBytePixelCount)<(GetDocument()->m_nRomSize-m_nBytePixelCount))
					m_nOffset+=m_nBytePixelCount;
			}else
			if((m_nOffset+m_ptCursorPos.x*m_nTileSize)<(GetDocument()->m_nRomSize-m_nNextSize))
			{
				m_nOffset+=m_nNextSize;
				OnUpdateData();
			}
		}else
		if(GetKeyState(VK_SHIFT)&0x80)
		{
			if(m_nColCount<0xFF)
			{
				m_nColCount++;
				m_nAutoColFit=0;
			}
		}else
		if( (GetDocument()->m_nRomSize>m_nTileSize) &&
			(m_nOffset<(GetDocument()->m_nRomSize-m_nViewLineSize*m_nLineCount)) )
		{
			m_nOffset+=m_nTileSize;
			OnUpdateData();
		}else
		if(m_ptCursorPos.x<(m_nColCount*m_nLineCount-1))
		{
			m_ptCursorPos.x++;
			m_ptCursorPos.y=m_ptCursorPos.x;
		}
		OnUpdateData();
		SetCurTileOffset();
		break;
	case VK_UP:
		if(GetKeyState(VK_CONTROL)&0x80)
		{
			if(GetKeyState(VK_SHIFT)&0x80)
			{
				UINT l = m_nWidth*m_nBitCount/8;
				if(m_nOffset>l)
					m_nOffset-=l;
			}else
			if(m_nTileFormat==TF_FIRST)
				m_nTileFormat=TF_LAST;
			else
				m_nTileFormat--;
		}else
		if(GetKeyState(VK_SHIFT)&0x80)
		{
			if(m_nLineCount>1)
			{
				m_nLineCount--;
				if(m_ptCursorPos.x>m_nColCount*m_nLineCount)
					m_ptCursorPos.x-=m_nColCount;
				if(m_ptCursorPos.y>m_nColCount*m_nLineCount)
					m_ptCursorPos.y-=m_nColCount;
				m_nAutoColFit=0;
			}
		}else
		if(m_nOffset)
		{
			if(m_nOffset<m_nViewLineSize)
				m_nOffset=0;
			else
				m_nOffset-=m_nViewLineSize;
		}else
		if(m_ptCursorPos.x>=m_nColCount)
		{
			m_ptCursorPos.x-=m_nColCount;
			m_ptCursorPos.y = m_ptCursorPos.x;
		}
		OnUpdateData();
		SetCurTileOffset();
		break;
	case VK_DOWN:
		if(GetKeyState(VK_CONTROL)&0x80)
		{
			if(GetKeyState(VK_SHIFT)&0x80)
			{
				UINT l = m_nWidth*m_nBitCount/8;
				if( (GetDocument()->m_nRomSize>l) &&
					((m_nOffset+m_nViewLineSize*m_nLineCount)<GetDocument()->m_nRomSize))
					m_nOffset+=l;
			}else
			if(m_nTileFormat==TF_LAST)
				m_nTileFormat=TF_FIRST;
			else
				m_nTileFormat++;
		}else
		if(GetKeyState(VK_SHIFT)&0x80)
		{
			CRect rc; GetClientRect(&rc);
			if(m_nLineCount<rc.Height()/m_nScaleHeight)
			{
				m_nLineCount++;
				m_nAutoColFit=0;
			}
		}else
		if( (GetDocument()->m_nRomSize>m_nViewLineSize) &&
			((m_nOffset+m_nViewLineSize*m_nLineCount)<GetDocument()->m_nRomSize))
		{
			m_nOffset+=m_nViewLineSize;
		}else
		if(m_ptCursorPos.x<(m_nColCount*(m_nLineCount-1)))
		{
			m_ptCursorPos.x+=m_nColCount;
			m_ptCursorPos.y = m_ptCursorPos.x;
		}
		OnUpdateData();
		SetCurTileOffset();
		break;
	case VK_HOME:
		m_nOffset=0;
		OnUpdateData();
		SetCurTileOffset();
		break;
	case VK_END:
		if(GetDocument()->m_nRomSize<m_nPageSize)
			m_nOffset=0;
		else
			m_nOffset=GetDocument()->m_nRomSize-m_nPageSize;
		OnUpdateData();
		SetCurTileOffset();
		break;
	case VK_PRIOR:
		if(m_nOffset<m_nPageSize)
			m_nOffset=0;
		else
			m_nOffset-=m_nPageSize;
		OnUpdateData();
		SetCurTileOffset();
		break;
	case VK_NEXT:
		if(GetDocument()->m_nRomSize<m_nPageSize)
			m_nOffset=0;
		else
		if(m_nOffset<(GetDocument()->m_nRomSize-m_nPageSize))
			m_nOffset+=m_nPageSize;
		else
			m_nOffset=GetDocument()->m_nRomSize-m_nPageSize;
		OnUpdateData();
		SetCurTileOffset();
	case VK_SPACE:
		m_bShowFocus=~m_bShowFocus;
		Invalidate(FALSE);
		break;
	case VK_ESCAPE:
		if(m_hImportBmp)
		{
			DeleteObject(m_hImportBmp);
			m_hImportBmp = NULL;
			Invalidate(FALSE);
		}
		break;
	case VK_RETURN:
		if(m_hImportBmp)
			OnImportBmp();
		break;
	case VK_TAB:
		SetCurView(ID_VIEW_TILEEDITORVIEW);
		break;

	default:
		{
			FKEY:
			//CCrystalBar *pBar = theApp.GetExplorerBox();
			switch(nChar)
			{
			case VK_OEM_MINUS:	//-
				if(m_nScale>25)
				{
					m_nScale-=25;
					OnUpdateData();
				}break;
			case VK_OEM_PLUS:	//+
				if(m_nScale<=(800-25))
				{
					m_nScale+=25;
					OnUpdateData();
				}break;
			case 0xBC:			//<
				if(m_nBitCount<8)
				{
					BYTE nCount = 1<<m_nBitCount;
					if(m_nPalIndex<nCount)
						m_nPalIndex = 256-nCount;
					else
						m_nPalIndex-=nCount;
					//SetBits();
					//Invalidate(FALSE);
					OnUpdateData();
				}break;
			case 0xBE:			//>
				if(m_nBitCount<8)
				{
					BYTE nCount = 1<<m_nBitCount;
					if(m_nPalIndex+nCount>=256)
						m_nPalIndex = 0;
					else
						m_nPalIndex+=nCount;
					//SetBits();
					//Invalidate(FALSE);
					OnUpdateData();
				}break;
			case '2':
			case '3':
				if(m_nWidth<=8 && m_nHeight<=8)
					break;
			case '1':
			case '4':
				m_nDrawMode = CT2_DM_TILE + (nChar-'1');
				OnUpdateData();
				break;
			case '8':
				m_nWidth = m_nHeight = 8;
				m_nDrawMode = CT2_DM_TILE;
				OnUpdateData();
				break;
			case '9':
				m_nWidth = m_nHeight = 16;
				OnUpdateData();
				break;
			}
		}break;
	}

	CT2View::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CT2TileView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	case VK_LEFT:
		if(m_nDrawMode)
		{
			if((m_nHeight>=16) || (m_nWidth>16)) m_nWidth-=8;
		}else
			m_nWidth--;
		OnUpdateData();
		break;
	case VK_RIGHT:
		m_nDrawMode?m_nWidth+=8:m_nWidth++;
		OnUpdateData();
		break;
	case VK_UP:
		if(m_nDrawMode)
		{
			if((m_nHeight>16) || (m_nWidth>=16)) m_nHeight-=8;
		}else
			m_nHeight--;
		OnUpdateData();
		break;
	case VK_DOWN:
		m_nDrawMode?m_nHeight+=8:m_nHeight++;
		OnUpdateData();
		break;
	}

	CT2View::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CT2TileView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);
	point.x += si.nPos;
	int x=m_nScaleWidth*m_nColCount,
        y=m_nScaleHeight*m_nLineCount;
	if((point.x>x) || (point.y>y)) return;

	if(m_hImportBmp)
	{
		BITMAP bm; GetObject(m_hImportBmp, sizeof(bm), &bm);
		CRect rc(m_ptImport.x, m_ptImport.y,
			m_ptImport.x+bm.bmWidth*m_nScale/100,
			m_ptImport.y+bm.bmHeight*m_nScale/100);
		if(rc.PtInRect(point))
		{
			m_bStartMove = TRUE;
			m_ptMove = point-m_ptImport;
		}else
		{
			OnImportBmp();
			return;
		}
	}else
	{
		m_ptCursorPos.y = point.y/m_nScaleHeight*
			m_nColCount+point.x/m_nScaleWidth;
		if(!(GetKeyState(VK_SHIFT)&0x80))
		{
			m_ptCursorPos.x=m_ptCursorPos.y;
			m_bStartMove=TRUE;
		}
		m_bShowFocus = TRUE;

		SetCurTileOffset();
		Invalidate(FALSE);
	}

	CT2View::OnLButtonDown(nFlags, point);
}

void CT2TileView::OnMouseMove(UINT nFlags, CPoint point)
{
	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);
	point.x += si.nPos;
	if( m_bStartMove && (nFlags&MK_LBUTTON) )
	{
		if(m_hImportBmp)
		{
			m_ptImport = point-m_ptMove;
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEALL)));
			Invalidate(FALSE);
		}else
		if( //(!m_bShow) &&
			(m_ptCursorPos.y!=point.y/(m_nScaleHeight)*
				m_nColCount+point.x/(m_nScaleWidth)) )
		{
			m_ptCursorPos.y=point.y/(m_nScaleHeight)*
				m_nColCount+point.x/(m_nScaleWidth);
			Invalidate(FALSE);
		}
	}else
	if(m_hImportBmp)
	{
		BITMAP bm; GetObject(m_hImportBmp, sizeof(bm), &bm);
		CRect rc(m_ptImport.x, m_ptImport.y,
			m_ptImport.x+bm.bmWidth*m_nScale/100,
			m_ptImport.y+bm.bmHeight*m_nScale/100);
		if(rc.PtInRect(point))
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEALL)));
	}
	CT2View::OnMouseMove(nFlags, point);
}

void CT2TileView::GetTileRect(POINT& ptPos, CRect& rcRect)
{
	int nBegin=ptPos.x>ptPos.y?ptPos.y:ptPos.x,
		nEnd=ptPos.x>ptPos.y?ptPos.x:ptPos.y;
	rcRect.top=nBegin/m_nColCount;
	rcRect.bottom=nEnd/m_nColCount;
    rcRect.left=nBegin%m_nColCount;
	rcRect.right=nEnd%m_nColCount;
	rcRect.NormalizeRect();
	rcRect.bottom++;
	rcRect.right++;
}

void CT2TileView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bStartMove = FALSE;
	if(m_hImportBmp)
	{
		m_ptImport.x=m_ptImport.x/m_nScaleWidth*m_nScaleWidth;
		m_ptImport.y=m_ptImport.y/m_nScaleHeight*m_nScaleHeight;
		Invalidate(FALSE);
	}
	CT2View::OnLButtonUp(nFlags, point);
}

void CT2TileView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int x=m_nScaleWidth*m_nColCount,
        y=m_nScaleHeight*m_nLineCount;
	if((point.x>x) || (point.y>y)) return;


	if(m_hImportBmp)
	{
		OnImportBmp();
	}else
	{
		CT2TileEditorView* pEv = (CT2TileEditorView*)
			theApp.GetView(ID_VIEW_TILEEDITORVIEW);
		pEv->m_nOffset = m_nOffset+
				m_ptCursorPos.y*m_nViewLineSize+
				m_ptCursorPos.x*m_nTileSize;
		SetCurView(ID_VIEW_TILEEDITORVIEW);
	}
	CT2View::OnLButtonDblClk(nFlags, point);
}

void CT2TileView::OnEditExport()
{
	static CFileDialog sfd(FALSE);
	CString strOpenDibFilter;
	strOpenDibFilter.LoadString(IDS_DIBFILTER);
	strOpenDibFilter.AppendChar(_T('\0'));
	sfd.m_pOFN->lpstrFilter=strOpenDibFilter;
	if(sfd.DoModal()!=IDOK) return;

	CFile bmFile;
	CString PathName=PathFindExtension(sfd.GetPathName());
	PathName.MakeLower();
	if(PathName != _T(".bmp"))
		PathName.Format(_T("%s.bmp"), sfd.GetPathName());
	else
		PathName=sfd.GetPathName();
	if(!bmFile.Open(PathName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)) return;

	CRect rc;
	int nWidth, nHeight;
	if(m_nDrawMode>=CT2_DM_MAP)
	{
		rc.SetRect(0,0,1,1);
		nWidth=m_nWidth;
		nHeight=m_nHeight;
	}else
	{
		GetTileRect(m_ptCursorPos, rc);
		nWidth=rc.Width()*m_nWidth;
		nHeight=rc.Height()*m_nHeight;
	}
	BITMAPINFOHEADER bi;
	memset(&bi, 0, sizeof(bi));
	bi.biSize=sizeof(bi);
	bi.biBitCount=32;
	bi.biPlanes=1;
	bi.biWidth=nWidth;
	bi.biHeight=nHeight;
	bi.biSizeImage=nWidth*nHeight*4;
	BITMAPFILEHEADER BmFileHeader; memset(&BmFileHeader, 0, sizeof(BITMAPFILEHEADER));
	BmFileHeader.bfType=0x4D42;//BM标记
	BmFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	BmFileHeader.bfSize=BmFileHeader.bfOffBits+bi.biSizeImage;

	bmFile.Write(&BmFileHeader, sizeof(BITMAPFILEHEADER));
	bmFile.Write(&bi, sizeof(BITMAPINFOHEADER));

	SetBits();

	RGBQUAD* pBmpTile = m_pPaintPanel +
		(m_biPaintPanel.biHeight-rc.bottom*m_nHeight) * m_biPaintPanel.biWidth +
		rc.left*m_nWidth;

	RGBQUAD bk = *(RGBQUAD*)&((CT2TileEditorView*)theApp.GetView(ID_VIEW_TILEEDITORVIEW))
		->m_rgbBkColor;
	while(nHeight--)
	{
		RGBQUAD *pBits = pBmpTile;
		for(int i=0; i<nWidth; i++)
		{
			if(m_nBitCount==16 || m_nBitCount==32)
			{// 不透明度
				pBits->rgbRed =
					(pBits->rgbRed*pBits->rgbReserved+
					bk.rgbRed*(256-pBits->rgbReserved))/256;

				pBits->rgbGreen =
					(pBits->rgbGreen*pBits->rgbReserved+
					bk.rgbGreen*(256-pBits->rgbReserved))/256;

				pBits->rgbBlue =
					(pBits->rgbBlue*pBits->rgbReserved+
					bk.rgbBlue*(256-pBits->rgbReserved))/256;
			}
			pBits++;
		}
		bmFile.Write((BYTE*)pBmpTile, nWidth*4);
		pBmpTile += m_biPaintPanel.biWidth;
	}
	SetBits();
}

void CT2TileView::OnEditSelectAll()
{
	m_ptCursorPos.x=0;
	m_ptCursorPos.y=m_nColCount*m_nLineCount-1;
	Invalidate(FALSE);
}

void CT2TileView::OnEditImport()
{
	//导入Tile
	static CFileDialog sfd(TRUE);
	CString strOpenDibFilter;
	strOpenDibFilter.LoadString(IDS_DIBFILTER);
	strOpenDibFilter.AppendChar(_T('\0'));
	sfd.m_pOFN->lpstrFilter=strOpenDibFilter;
	if(sfd.DoModal()!=IDOK) return;

	OnEditImport(sfd.GetPathName());
}

void CT2TileView::OnEditImport(CString path)
{
	CFile bmFile;
	if(!bmFile.Open(path, CFile::modeRead|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	DWORD nSize = (UINT)bmFile.GetLength();
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	BYTE *pBuffer = (BYTE*)GlobalLock(hGlobal);
	bmFile.Read(pBuffer, nSize);
	BITMAPFILEHEADER *pfHeader = (BITMAPFILEHEADER*)pBuffer;

	m_nOpenMapFile = pfHeader->bfType==0x4D42;
	BITMAPINFOHEADER *pbmHeader;
	if(m_nOpenMapFile)
	{
		pbmHeader = (BITMAPINFOHEADER*)(pBuffer+sizeof(BITMAPFILEHEADER));
		m_nOpenMapFile = pbmHeader->biBitCount==32;
	}

	CDC srcDC, dstDC, *pDC=GetDC();
	HBITMAP hsrcBM;LPPICTURE pPix;
	if(m_nOpenMapFile)
	{
		UINT *pBits;
		if(m_hImportBmp) DeleteObject(m_hImportBmp);
		m_hImportBmp = CreateDIBSection(pDC->GetSafeHdc(),
			(BITMAPINFO*)pbmHeader, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
		memcpy(pBits, pBuffer+pfHeader->bfOffBits, pbmHeader->biSizeImage);
		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal);
		goto ok;
	}else
	{
		GlobalUnlock(hGlobal);
		pPix = LoadPic(hGlobal, nSize, TRUE);
		if(!pPix) return;
		pPix->get_Handle((OLE_HANDLE*)&hsrcBM);
	}
	BITMAP srcbm; GetObject(hsrcBM, sizeof(srcbm), &srcbm);

	srcDC.CreateCompatibleDC(pDC);
	dstDC.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);

	srcDC.SelectObject(hsrcBM);

	if(m_hImportBmp) DeleteObject(m_hImportBmp);
	m_hImportBmp =
		CreateBitmap(srcbm.bmWidth, srcbm.bmHeight, 1, srcbm.bmBitsPixel, NULL);
	if(!dstDC.SelectObject(m_hImportBmp))
	{
		if(m_hImportBmp) DeleteObject(m_hImportBmp);
		m_hImportBmp =
			CreateBitmap(srcbm.bmWidth, srcbm.bmHeight, 1, 24, NULL);
		if(!dstDC.SelectObject(m_hImportBmp))
		{
			if(m_hImportBmp) DeleteObject(m_hImportBmp);
			m_hImportBmp =
				CreateBitmap(srcbm.bmWidth, srcbm.bmHeight, 1, 32, NULL);
			if(!dstDC.SelectObject(m_hImportBmp))
				Hint(IDS_IMBMPERROR, MB_OK|MB_ICONWARNING);
		}
	}

	dstDC.BitBlt(0, 0, srcbm.bmWidth, srcbm.bmHeight, &srcDC, 0, 0, SRCCOPY);

	dstDC.DeleteDC();
	DeleteObject(hsrcBM);
	srcDC.DeleteDC();

ok:
	if(!m_nOpenMapFile)
		pPix->Release();
	else
		m_nOpenMapFile=8;
	m_ptImport.x = m_ptCursorPos.x%m_nColCount*m_nScaleWidth;
	m_ptImport.y = m_ptCursorPos.y/m_nColCount*m_nScaleHeight;
	Invalidate(FALSE);

	//AddMacro_EditImport(path);
}

void CT2TileView::OnEditCopy()
{
	CWnd *pFocus = GetFocus();
	if( pFocus!=this )
	{
		pFocus->SendMessage(WM_COPY);
		return;
	}

	OnCopy();
}

void CT2TileView::OnCopy()
{
	//AddMacro_EditCopy();

	CRect rcSel;
	GetTileRect(m_ptCursorPos, rcSel);

	int nWidth = rcSel.Width()*m_nWidth,
		nHeight= rcSel.Height()*m_nHeight;
	HBITMAP hBm = CreateBitmap(nWidth, nHeight, 1, 24, NULL);
	if(hBm==NULL)
	{
		ShowError();
		return;
	}

	CDC memDC, srcDC, *pDC=GetDC();
	memDC.CreateCompatibleDC(pDC);
	srcDC.CreateCompatibleDC(pDC);
	ReleaseDC(pDC);
	if(!memDC.SelectObject(hBm))
	{
		DeleteObject(hBm);
		hBm = CreateBitmap(nWidth, nHeight, 1, 32, NULL);
		memDC.SelectObject(hBm);
	}

	RGBQUAD *pBits;
	BITMAPINFO bi;
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = m_nDrawMode>=CT2_DM_MAP?m_nWidth:m_nWidth*m_nColCount;
	bi.bmiHeader.biHeight= m_nDrawMode>=CT2_DM_MAP?m_nHeight:m_nHeight*m_nLineCount;
	bi.bmiHeader.biPlanes= 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biSizeImage = bi.bmiHeader.biWidth*bi.bmiHeader.biHeight*4;
	HBITMAP hsrcBm = CreateDIBSection(pDC->GetSafeHdc(), &bi,
			DIB_RGB_COLORS, (void**)&pBits, NULL, 0);

	srcDC.SelectObject(hsrcBm);

	StretchDIBits(srcDC.GetSafeHdc(),
		0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight,
		0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight,
		m_pPaintPanel, (BITMAPINFO*)&m_biPaintPanel, DIB_RGB_COLORS, SRCCOPY);

	RGBQUAD bk = *(RGBQUAD*)&((CT2TileEditorView*)theApp.GetView(ID_VIEW_TILEEDITORVIEW))
		->m_rgbBkColor;
	if(m_nBitCount==16 || m_nBitCount==32)
	while(bi.bmiHeader.biSizeImage-=4)
	{
		//不透明度
		pBits->rgbRed =
			(pBits->rgbRed*pBits->rgbReserved+
			bk.rgbRed*(256-pBits->rgbReserved))/256;

		pBits->rgbGreen =
			(pBits->rgbGreen*pBits->rgbReserved+
			bk.rgbGreen*(256-pBits->rgbReserved))/256;

		pBits->rgbBlue =
			(pBits->rgbBlue*pBits->rgbReserved+
			bk.rgbBlue*(256-pBits->rgbReserved))/256;
		pBits++;
	}

	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si);
	int nTop = si.nPos * 100 / m_nScale;
	GetScrollInfo(SB_HORZ, &si);
	int nLeft = si.nPos * 100 / m_nScale;

	memDC.BitBlt(0, 0, nWidth, nHeight, &srcDC,
		rcSel.left*m_nWidth, rcSel.top*m_nHeight, SRCCOPY);

	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBm);
	CloseClipboard();

	srcDC.DeleteDC();
	DeleteObject(hsrcBm);
	DeleteObject(hBm);
	memDC.DeleteDC();
	m_nOpenMapFile = 0;
}

void CT2TileView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	UINT nSel = 0;
	CWnd *pFocus = GetFocus();
	if(pFocus)
	nSel = (UINT)pFocus->SendMessage(EM_GETSEL);

	pCmdUI->Enable( (LOWORD(nSel)!=HIWORD(nSel)) || (pFocus==this) );
}

void CT2TileView::OnEditPaste()
{
	CWnd *pFocus = GetFocus();
	if( pFocus!=this )
	{
		pFocus->SendMessage(WM_PASTE);
	}else
	{
		OnPaste();
	}
}

void CT2TileView::OnPaste()
{
	if(m_hImportBmp) DeleteObject(m_hImportBmp);
	OpenClipboard();
	m_hImportBmp = (HBITMAP)GetClipboardData(CF_BITMAP);
	CloseClipboard();
	m_ptImport.x = m_ptCursorPos.x%m_nColCount*m_nScaleWidth;
	m_ptImport.y = m_ptCursorPos.y/m_nColCount*m_nScaleHeight;
	Invalidate(FALSE);

	//AddMacro_EditPaste();
}

void CT2TileView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	OpenClipboard();
	HANDLE hBmp = GetClipboardData(CF_BITMAP);
	pCmdUI->Enable( ((hBmp!=NULL) && !GetDocument()->m_nReadOnly)
		|| (GetFocus()!=this));
	DeleteObject(hBmp);
	CloseClipboard();
}

void CT2TileView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CT2View::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CT2TileView::OnViewAutofitcol()
{
	m_nAutoColFit=~m_nAutoColFit;
	if(m_nAutoColFit) OnUpdateData();
}

void CT2TileView::OnUpdateViewAutofitcol(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nAutoColFit);
}

void CT2TileView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Invalidate(FALSE);

	CT2View::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CT2TileView::OnImportBmp()
{
	if(!m_hImportBmp) return;
	BITMAP bm; GetObject(m_hImportBmp, sizeof(bm), &bm);
	CDC dstDC, *pDC=GetDC();
	dstDC.CreateCompatibleDC(pDC);
	if( m_nBitCount<=8 )
	{
		//int nPalSize = 0x400;//(1<<m_nBitCount)*4;
		BITMAPINFO *pBI = (BITMAPINFO*)
			new BYTE[sizeof(BITMAPINFOHEADER)+0x400];
		memset(pBI, 0, sizeof(BITMAPINFOHEADER)+0x400);

		if(m_nDrawMode==CT2_DM_MAP)
			memcpy(pBI->bmiColors, GetDocument()->m_pPal,
				256*4);
		else
			memcpy(pBI->bmiColors, GetDocument()->m_pPal+m_nPalIndex,
				(1<<m_nBitCount)*4);

		pBI->bmiHeader.biBitCount=8;//m_nBitCount;
		pBI->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		pBI->bmiHeader.biPlanes = 1;
		pBI->bmiHeader.biWidth=bm.bmWidth;
		pBI->bmiHeader.biHeight=bm.bmHeight;
		pBI->bmiHeader.biSizeImage=bm.bmWidth*bm.bmHeight;//*(8/m_nBitCount);

		LPVOID lpBits;
		HBITMAP hdstBM = ::CreateDIBSection(pDC->GetSafeHdc(),
			pBI, DIB_RGB_COLORS, (VOID**)&lpBits, NULL, 0);
		CDC srcDC;
		srcDC.CreateCompatibleDC(pDC);
		srcDC.SelectObject(m_hImportBmp);

		dstDC.SelectObject(hdstBM);
		dstDC.BitBlt(0,0,bm.bmWidth, bm.bmHeight, &srcDC, 0, 0, SRCCOPY);

		srcDC.DeleteDC();
		DeleteObject(m_hImportBmp);
		m_hImportBmp = hdstBM;
		delete[] pBI;
	}else
		dstDC.SelectObject(m_hImportBmp);
	ReleaseDC(pDC);

	int nWidth, nHeight;
	if(m_nDrawMode>=CT2_DM_MAP)
		nWidth = nHeight = 8;
	else
	{
		nWidth = m_nWidth;
		nHeight = m_nHeight;
	}
	int top=m_ptImport.y*100/m_nScale/nHeight;
	int left=m_ptImport.x*100/m_nScale/nWidth;
	int right=left+bm.bmWidth/nWidth;
	int bottom=top+bm.bmHeight/nHeight;
	int nCount = (top<0?abs(bottom-top)-abs(top):bottom-top) * m_nColCount;
	if(left>0) nCount-=left;
	if(right<m_nColCount) nCount-=m_nColCount-right;
	if(top<m_nLineCount && left<m_nColCount &&
		(right-abs(left) || bottom-abs(top)) )
	{
		BOOL bUndo = m_nDrawMode>=CT2_DM_MAP;
		if(!bUndo)
		{
			UINT nOffset = (top<0?0:top)*m_nColCount+(left<0?0:left);
			nOffset = m_nOffset+nOffset*m_nTileSize;
			UINT nSize = nCount*m_nTileSize;
			if((nOffset+nSize)>=GetDocument()->m_nRomSize)
			{
				if(GetDocument()->m_nRomSize<=nOffset)
					goto End;
				nSize = GetDocument()->m_nRomSize-nOffset;
			}
			DoUndo(GetDocument()->m_pRom+nOffset, nOffset, nSize);
			//AddMacro_ImprotBmp();
		}
		CT2TileEditorView *pEditor =
			(CT2TileEditorView*)theApp.GetView(ID_VIEW_TILEEDITORVIEW);
		pEditor->OnUpdateData();
		long ptx = m_ptCursorPos.x;
		if(m_nDrawMode>=CT2_DM_MAP && bottom>m_nLineCount)
			bottom=m_nLineCount;
		for(int y=top; y<bottom; y++)
		{
			if(y<0) y=0;
			m_ptCursorPos.x = y*m_nColCount+left;
			for(int x=left; x<right; x++)
			{
				if(x<0)
				{
					x=0;
					m_ptCursorPos.x+=abs(left);
				}else
				if(x>=m_nColCount)
					break;
				pEditor->m_TextDC.BitBlt(0, 0, nWidth, nHeight,
					&dstDC,
					(x-left)*nWidth, (y-top)*nHeight, SRCCOPY);
				pEditor->SetTile(bUndo);
				m_ptCursorPos.x++;
			}
		}
		m_ptCursorPos.x = ptx;
	}
End:
	m_bShowFocus = 0;
	dstDC.DeleteDC();
	DeleteObject(m_hImportBmp);
	m_hImportBmp = NULL;
	SetBits();
	Invalidate(FALSE);
}

void CT2TileView::SetCurTileOffset()
{
	CT2StatusBar *pSb = theApp.GetStatusBar();
	CString strOffset;
	strOffset.Format(_T("%08X"),
		m_nOffset+m_ptCursorPos.x*m_nTileSize);
	pSb->SetPaneText(pSb->CommandToIndex(ID_TILEEDITOR_OFFSET), strOffset);

	pSb->SetPaneText(0, GetDocument()->OffsetToPathName(
			m_nOffset+m_ptCursorPos.x*m_nTileSize));
}
/*
void CT2TileView::OnUpdateViewMapOam(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nDrawMode>=CT2_DM_MAP);
}
*/
void CT2TileView::ScrollTo(UINT nOffset, BOOL bForce)
{
	if( bForce ||
		(nOffset<m_nOffset) ||
		(nOffset>=(m_nOffset+m_nViewLineSize*m_nLineCount)) )
	{
		m_nOffset = nOffset+GetDocument()->m_nBlockOffset;
		GetDocument()->Seek(m_nOffset);
		//m_nOffset -= GetDocument()->m_nBlockOffset;
		OnUpdateData();
	}
}

void CT2TileView::OnUpdateToolZoomexfirst(CCmdUI *pCmdUI)
{
	CMultiDocTemplate *pDocTemp =
		(CMultiDocTemplate*)GetDocument()->GetDocTemplate();
	POSITION pos = pDocTemp->GetFirstDocPosition();
	CT2Doc *pThisDoc = GetDocument();

	UINT nIndex = GetParent()->IsZoomed()?1:0;
	HMENU hTile = GetSubMenu(theApp.m_hTileMenu, 3+nIndex);
	HMENU hZoomEx = GetSubMenu(hTile, 3);

	static CString DefMenu;
	if(DefMenu.IsEmpty())
	{
		GetMenuString(hZoomEx, ID_TOOLS_ZOOMEXFIRST,
			DefMenu.GetBuffer(_MAX_PATH), _MAX_PATH, MF_STRING);
		DefMenu.ReleaseBuffer();
	}
	for(int i=0; i<10; i++)
		RemoveMenu(hZoomEx, ID_TOOLS_ZOOMEXFIRST+i, MF_REMOVE);

	nIndex = 0;
	while(pos)
	{
		CT2Doc *pDoc = (CT2Doc*)pDocTemp->GetNextDoc(pos);
		CT2ChildFrm *pCf = (CT2ChildFrm*)pDoc->GetLastView()->GetParent();

		if( (pDoc==pThisDoc) || (pCf->m_nCurView!=ID_VIEW_TILEVIEW) )
			continue;

		CString path=pDoc->GetPathName();
		AppendMenu(hZoomEx, MF_STRING|MF_BYCOMMAND,
			ID_TOOLS_ZOOMEXFIRST+nIndex, path);

		nIndex++;
		if(nIndex>10) break;
	}
	if(!nIndex)
	{
		AppendMenu(hZoomEx, MF_STRING|MF_BYCOMMAND|MF_DISABLED,
			ID_TOOLS_ZOOMEXFIRST, DefMenu);
		pCmdUI->Enable(FALSE);
	}else
		pCmdUI->Enable(!pThisDoc->m_nReadOnly);
}

void CT2TileView::OnToolsZoomexfirst(UINT nID)
{
	CMultiDocTemplate *pDocTemp =
		(CMultiDocTemplate*)GetDocument()->GetDocTemplate();
	POSITION pos = pDocTemp->GetFirstDocPosition();
	CT2Doc *pThisDoc = GetDocument();

	UINT nIndex = GetParent()->IsZoomed()?1:0;
	HMENU hTile = GetSubMenu(theApp.m_hTileMenu, 3+nIndex);
	HMENU hZoomEx = GetSubMenu(hTile, 3);

	CString PathName;
	GetMenuString(hZoomEx, nID, PathName.GetBuffer(_MAX_PATH),
		_MAX_PATH, MF_STRING);
	PathName.ReleaseBuffer();

	CT2Doc *pDoc;
	while(pos)
	{
		pDoc = (CT2Doc*)pDocTemp->GetNextDoc(pos);
		CString path=pDoc->GetPathName();
		if(path==PathName)
			goto OK;
	}
	return;
OK:

	CT2ChildFrm *pCf = (CT2ChildFrm*)pDoc->GetLastView()->GetParent();
	CT2TileView *pTView = (CT2TileView*)pCf->m_pTileView;

	CT2TileEditorView* pEditor =
		(CT2TileEditorView*)theApp.GetView(ID_VIEW_TILEEDITORVIEW);
	pEditor->OnUpdateData();

	int nStart = pTView->m_ptCursorPos.y>pTView->m_ptCursorPos.x?
		pTView->m_ptCursorPos.x:pTView->m_ptCursorPos.y;
	int nCount = pTView->m_ptCursorPos.x!=pTView->m_ptCursorPos.y?
		abs(pTView->m_ptCursorPos.x-pTView->m_ptCursorPos.y)+1:
		pTView->m_nColCount*pTView->m_nLineCount-nStart;
	UINT nOffset = m_nOffset+m_ptCursorPos.x*m_nTileSize;
	UINT nSize = nCount*m_nTileSize;
	if(nOffset+nSize>GetDocument()->m_nRomSize)
	{
		nSize = GetDocument()->m_nRomSize-nOffset;
		nCount = nSize/m_nTileSize;
	}
	DoUndo(GetDocument()->m_pRom+nOffset,
		nOffset, nSize);

	int nAlignLeft=0, nAlignTop=0;
	switch(m_nAlignMode)
	{
	case 0:// 0 1 2
	case 3:// 3 4 5
	case 6:// 6 7 8
		nAlignLeft = 0;
		break;

	case 2:
	case 5:
	case 8:
		nAlignLeft = m_nWidth-pTView->m_nWidth;
		break;

	case 1:
	case 4:
	case 7:
		nAlignLeft = (m_nWidth-pTView->m_nWidth)/2;
		break;
	}
	WORD nTWidth = pTView->m_nWidth,
		nTHeight = pTView->m_nHeight;
	int OldBltMode = pEditor->m_TextDC.GetStretchBltMode();
	switch(m_nAlignMode)
	{
	case 0:
	case 1:
	case 2:
		nAlignTop = 0;
		break;
	case 6:
	case 7:
	case 8:
		nAlignTop = m_nHeight-pTView->m_nHeight;
		break;
	case 3:
	case 4:
	case 5:
		nAlignTop = (m_nHeight-pTView->m_nHeight)/2;
		break;
	case 9:	// WHITEONBLACK
	case 10:// BLACKONWHITE
	case 11:// COLORONCOLOR
	case 12:// HALFTONE
		nAlignLeft = nAlignTop = 0;
		nTWidth = m_nWidth;
		nTHeight = m_nHeight;
		if(m_nAlignMode==9)
			pEditor->m_TextDC.SetStretchBltMode(WHITEONBLACK);
		else
		if(m_nAlignMode==10)
			pEditor->m_TextDC.SetStretchBltMode(BLACKONWHITE);
		else
		if(m_nAlignMode==11)
			pEditor->m_TextDC.SetStretchBltMode(COLORONCOLOR);
		else
		if(m_nAlignMode==12)
			pEditor->m_TextDC.SetStretchBltMode(HALFTONE);
		break;
	}
	//
	CDC dstDC;
	dstDC.CreateCompatibleDC(&pEditor->m_TextDC);
	HBITMAP hdstBM;

	if(m_nBitCount<=8)
	{
		BITMAPINFO *pBI = (BITMAPINFO*)
			new BYTE[sizeof(BITMAPINFOHEADER)+0x400];
		memset(pBI, 0, sizeof(BITMAPINFOHEADER)+0x400);

		memcpy(pBI->bmiColors, GetDocument()->m_pPal+m_nPalIndex,
			(1<<m_nBitCount)*4);

		pBI->bmiHeader.biBitCount=8;//m_nBitCount;
		pBI->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		pBI->bmiHeader.biPlanes = 1;
		pBI->bmiHeader.biWidth=pTView->m_biPaintPanel.biWidth;
		pBI->bmiHeader.biHeight=pTView->m_biPaintPanel.biHeight;
		pBI->bmiHeader.biSizeImage=pBI->bmiHeader.biWidth*pBI->bmiHeader.biHeight;

		LPVOID lpBits;
		hdstBM = CreateDIBSection(pEditor->m_TextDC.GetSafeHdc(),
			pBI, DIB_RGB_COLORS, (VOID**)&lpBits, NULL, 0);
		delete[] pBI;
	}else
		hdstBM = CreateCompatibleBitmap(pEditor->m_TextDC.GetSafeHdc(),
		pTView->m_biPaintPanel.biWidth, pTView->m_biPaintPanel.biHeight);

	dstDC.SelectObject(hdstBM);
	StretchDIBits(dstDC.GetSafeHdc(),
		0, 0, pTView->m_biPaintPanel.biWidth, pTView->m_biPaintPanel.biHeight,
		0, 0, pTView->m_biPaintPanel.biWidth, pTView->m_biPaintPanel.biHeight,
		pTView->m_pPaintPanel,
		(BITMAPINFO*)&pTView->m_biPaintPanel,
        DIB_RGB_COLORS, SRCCOPY);
	//dstDC.BitBlt(0,0,bm.bmWidth, bm.bmHeight, &srcDC, 0, 0, SRCCOPY);
	DeleteObject(hdstBM);

	//
	POINT ptOld = m_ptCursorPos;
	CRect rc(0, 0, m_nWidth, m_nHeight);
	CBrush Brush(pEditor->m_rgbBkColor);
	nCount+=nStart;
	for(int nTileNo=nStart; nTileNo<nCount; nTileNo++)
	{
		int nLeft = nTileNo%pTView->m_nColCount*pTView->m_nWidth,
			nTop = (nTileNo/pTView->m_nColCount)*pTView->m_nHeight;
		pEditor->m_TextDC.StretchBlt(nAlignLeft, nAlignTop,
			nTWidth, nTHeight, &dstDC,
			nLeft, nTop, pTView->m_nWidth, pTView->m_nHeight, SRCCOPY);
		int nSaveDC = pEditor->m_TextDC.SaveDC();
		pEditor->m_TextDC.ExcludeClipRect(nAlignLeft, nAlignTop,
			nAlignLeft+nTWidth, nAlignTop+nTHeight);
		pEditor->m_TextDC.FillRect(&rc, &Brush);
		pEditor->m_TextDC.RestoreDC(nSaveDC);

		//pEditor->m_TextDC.BitBlt(0, 0, m_nWidth, m_nHeight,
		pEditor->SetTile(FALSE);
		m_ptCursorPos.x++;
	}
	pEditor->m_TextDC.SetStretchBltMode(OldBltMode);
	dstDC.DeleteDC();
	m_ptCursorPos = ptOld;
	OnUpdateData();
}

void CT2TileView::OnTileeditorTbl()
{
	CT2TileEditorView *pEditor =
		(CT2TileEditorView*)
		((CT2ChildFrm*)GetParent())->m_pTileEditorView;

	// pEditor->OnUpdateData();
	pEditor->OnTbl();
}

void CT2TileView::OnUpdateTileeditorTbl(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_nReadOnly);
}

void CT2TileView::LoadBkBmp(WORD nBkPixType)
{
	m_nBkPixType = nBkPixType;
	DWORD dwSize;
	HANDLE bk = LoadRes(m_nBkPixType, dwSize);
	LPPICTURE pPix = LoadPic(bk, dwSize, TRUE);
	HBITMAP hBitmap;
	pPix->get_Handle((OLE_HANDLE*)&hBitmap);
	m_hBkBrush = CreatePatternBrush(hBitmap);
	pPix->Release();
}

void CT2TileView::OnEditSetfont()
{
	((CT2TileEditorView*)theApp.GetView(ID_VIEW_TILEEDITORVIEW))->SetFont();
}

void CT2TileView::InsertText()
{
	((CT2TileEditorView*)theApp.GetView(ID_VIEW_TILEEDITORVIEW))
		->InsertText();
}

void CT2TileView::OnUpdateInsertText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_nReadOnly);
}

void CT2TileView::AppendVar()
{
	CT2ChildFrm *pChild = (CT2ChildFrm*)GetParent();
	if(pChild->m_nCurView==ID_VIEW_TILEVIEW)
		pChild->m_pTileEditorView->AppendVar();

	CCrystalTreeCtrl &tc = ((CT2MainFrm*)theApp.GetMainWnd())->m_TreeBar.m_Tree;
	int i=CTAV_CT2VIEW;
	int nMax=GetDocument()->m_nRomSize-1;
	tc.AppendVal(i++, &m_nScale, CTCS_WORD, 25, 4000);
	tc.AppendVal(i++, &m_nNextSize, CTCS_BYTE);
	tc.AppendVal(i++, &m_nMoOffset, CTCS_UINT, 0, nMax);
	tc.AppendVal(i++, &m_nSkipSize, CTCS_BYTE);
	tc.AppendVal(i++, &m_nWidth, CTCS_WORD, 1, 2048);
	tc.AppendVal(i++, &m_nHeight, CTCS_WORD, 1, 2048);
	tc.AppendVal(i++, &m_nTileFormat, CTCS_BYTE);
	tc.AppendVal(i++, &m_nDrawMode, CTCS_BYTE);
	tc.AppendVal(i++, &m_nL90Check, CTCS_BYTE);
	tc.AppendVal(i++, &m_nR90Check, CTCS_BYTE);
	tc.AppendVal(i++, &m_nLRSwapCheck, CTCS_BYTE);
	tc.AppendVal(i++, &m_nHCheck, CTCS_BYTE);
	tc.AppendVal(i++, &m_nVCheck, CTCS_BYTE);
	tc.AppendVal(i, &m_n24Check, CTCS_BYTE);
	i=CTAV_EDITORVIEW;
	tc.AppendVal(i, &m_nAlignMode, CTCS_BYTE);
}

void CT2TileView::Save_StateData(STATEDATA *pSD)
{
	pSD->nTileViewOffset= m_nOffset;
	pSD->nTileFormat	= m_nTileFormat;
	pSD->nWidth			= m_nWidth;
	pSD->nHeight		= m_nHeight;
	pSD->nScale			= m_nScale;
	pSD->nDrawMode		= m_nDrawMode;
	pSD->nHCheck		= m_nHCheck;
	pSD->nVCheck		= m_nVCheck;
	pSD->nL90Check		= m_nL90Check;
	pSD->nR90Check		= m_nR90Check;
	pSD->nTileViewGrid	= m_nGrid;
	pSD->nSkipSize		= m_nSkipSize;
	pSD->n24Check		= m_n24Check;
	pSD->nNextSize		= m_nNextSize;
	pSD->nPalIndex		= m_nPalIndex;
	pSD->nColCount		= m_nAutoColFit?(WORD)-1:m_nColCount;
	pSD->nLineCount		= m_nLineCount;
	pSD->nMoOffset		= m_nMoOffset;
	pSD->nBkPixType		= m_nBkPixType;
}

void CT2TileView::Load_StateData(STATEDATA *pSD)
{
	m_nOffset = pSD->nTileViewOffset>=GetDocument()->m_nRomSize?0:pSD->nTileViewOffset;
	m_nTileFormat = pSD->nTileFormat;
	m_nWidth = pSD->nWidth;
	m_nHeight = pSD->nHeight;
	m_nScale = pSD->nScale;
	m_nDrawMode = pSD->nDrawMode;
	m_nHCheck = pSD->nHCheck&1;
	m_nVCheck = pSD->nVCheck&1;
	m_nL90Check = pSD->nL90Check&1;
	m_nR90Check = pSD->nR90Check&1;
	m_nGrid = pSD->nTileViewGrid&1;
	m_nSkipSize = pSD->nSkipSize;
	m_n24Check = pSD->n24Check&1;
	m_nNextSize = pSD->nNextSize;
	m_nPalIndex = pSD->nPalIndex;
	if(pSD->nColCount!=(WORD)-1)
	{
		m_nColCount=pSD->nColCount;
		m_nLineCount=pSD->nLineCount;
		m_nAutoColFit=0;
	}
	m_nMoOffset = pSD->nMoOffset;
	m_nBkPixType = pSD->nBkPixType;
}
/*
void CT2TileView::AddMacro_ImprotBmp()
{
	CT_MACRO_CT2TILEVIEW_IMPROTBMP m;
	INITMACRO(m, CT2TILEVIEW_IMPROTBMP);
	m.ptImport = m_ptImport;

	Save_StateData(&m.StateData);
	m.StateData.nColCount = m_nColCount;

	AddMacro(&m);
}

void CT2TileView::AddMacro_EditImport(CString path)
{
	CT_MACRO_1VAL_1FILE m;
	INITMACRO(m, CT2TILEVIEW_EDITIMPORT);
	lstrcpyn(m.szFile, path, _MAX_PATH);

	AddMacro(&m);
}

void CT2TileView::AddMacro_SetPixel(WORD nTilePixelNO, UINT nPixel)
{
	CT_MACRO_CT2TILEVIEW_SETPIXEL m;
	INITMACRO(m, CT2TILEVIEW_SETPIXEL);

	m.nOffset = m_nOffset;
	m.nPixel = nPixel;
	m.ptPos = m_ptCursorPos;
	m.nTilePixelNO = nTilePixelNO;

	Save_StateData(&m.StateData);

	AddMacro(&m);
}

void CT2TileView::AddMacro_EditCopy()
{
	CT_MACRO_CT2TILEVIEW_IMPROTBMP m;
	INITMACRO(m, CT2TILEVIEW_EDITCOPY);

	m.ptImport = m_ptCursorPos;

	Save_StateData(&m.StateData);
	m.StateData.nColCount = m_nColCount;

	AddMacro(&m);
}

void CT2TileView::AddMacro_EditPaste()
{
	CT_MACRO m;
	INITMACRO(m, CT2TILEVIEW_EDITPASTE);

	AddMacro(&m);
}
*/