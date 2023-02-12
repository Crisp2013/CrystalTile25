// CT2TileEditorView.cpp : CT2TileEditorView 类的实现
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include "CT2Doc.h"
#include ".\CT2TileEditorView.h"
#include "CT2ChildFrm.h"
#include "CT2TileView.h"
#include "CT2StatusBar.h"
#include "TextFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct TBLDATA {
	CT2StatusBar* pPro;
	CT2TileView* pView;
	CT2TileEditorView* pEditor;
	LPCTSTR lpTblList;
	UINT nCount;
};

UINT ImportTbl(LPVOID lParam)
{
	TBLDATA& td = *(TBLDATA*)lParam;
	CT2TileView *pView = td.pView;
	CT2TileEditorView *pEditor = td.pEditor;
	LPCTSTR lpTblList = td.lpTblList;
	UINT nCount = td.nCount;
	CT2StatusBar& pro = *td.pPro;

	CT2Doc* pDoc = pView->GetDocument();
	RGBQUAD *m_pPaintPanel = pEditor->m_pPaintPanel;
	UINT m_nTextColor = pEditor->m_nTextColor;
	UINT m_nBkColor = pEditor->m_nBkColor;
	UINT m_nShadowColor = pEditor->m_nShadowColor;
	BYTE m_nTransparent = pEditor->m_nTransparent;

	UINT m_rgbTextColor = pEditor->m_rgbTextColor;;
	UINT m_rgbBkColor = pEditor->m_rgbBkColor;
	UINT m_rgbShadowColor = pEditor->m_rgbShadowColor;

	UINT nPixel;
	int nPixelTop, nPixelLeft;
	BYTE* pTile;
	BYTE* pRom=pDoc->m_pRom+pView->m_nOffset;
	BYTE* pEnd=(pDoc->m_pRom+pDoc->m_nRomSize);
	BYTE* pPixel, nPixel1, nPixel2, nPixel3, nPixel4;
	WORD nWidth = pView->m_nWidth;
	WORD nHeight = pView->m_nHeight;
	BYTE nTileFormat = pView->m_nTileFormat;
	UINT nTileNO = pView->m_ptCursorPos.x;
	UINT nTileSize = pView->m_nTileSize;
	BYTE nBytePixelCount = pView->m_nBytePixelCount;
	UINT nColorMask = pView->m_nColorMask;
	BYTE nBitCount = pView->m_nBitCount;
	WORD nSkipSize = pView->m_nSkipSize;
	BYTE nDrawMode = pView->m_nDrawMode;
	UINT nTilePixelCount = nWidth*nHeight;
	BYTE nL90Check = pView->m_nL90Check;
	BYTE nR90Check = pView->m_nR90Check;

	// ObjH/V
	WORD nObjTileColCount = pView->m_nObjTileColCount;
	WORD nObjTileLineCount = pView->m_nObjTileLineCount;
	UINT nTileSize1_4 = pView->m_nTileSize1_4;

	// 2/4层专用
	BYTE nCD=(pView->m_n24Check?0x04:0x02);
	BYTE nCD2=4/nCD, nMask2=(1<<nCD2)-1;//m_nTileFormat=TF_GBA3XBPP

	//GB 2BPP用
	BYTE nTileLineSize = nDrawMode?1:nWidth/8;
	if(nWidth%8) nTileLineSize++; nTileLineSize*=2;
	UINT nOffset2 = (nTileFormat==TF_NES2BPP)?
		(nDrawMode==CT2_DM_TILE?nTileSize:nTileSize1_4)/2:
		// GB2 SNES
		(nDrawMode==CT2_DM_TILE?nTileLineSize:2)/2;

	UINT nOffset = pView->m_nOffset+nTileNO*nTileSize;
	if((nOffset+nCount*nTileSize) >= pDoc->m_nRomSize)
		nCount=(pDoc->m_nRomSize-nOffset)/nTileSize;
	pView->DoUndo(pRom+nTileNO*nTileSize, nOffset, nCount*nTileSize);
	memset(pRom+nTileNO*nTileSize, 0, nCount*nTileSize);

	UINT nTile = 0;
	while(nTile<nCount)
	{
		pEditor->m_strText = *lpTblList++;

		pEditor->DrawText();
		//SetTile();

		pTile=(nTileFormat!=TF_GBA3XBPP)&&(nTileFormat!=TF_CT0XBPP)?
			pRom+nTileNO*nTileSize:
			pRom+(nTileNO/nCD)*nTileSize;
		pTile+=nSkipSize;

		UINT nTilePixelNO=0;
		while(nTilePixelNO<nTilePixelCount)
		{
			UINT rgb= *(UINT*)(m_pPaintPanel+(nHeight-1-nTilePixelNO/nWidth)*nWidth
				+nTilePixelNO%nWidth);
			//COLORREF clrColor = RGB(rgb->rgbRed, rgb->rgbGreen, rgb->rgbBlue);
			if(nBitCount>8)
			{
				if( (m_nTransparent) && (rgb==m_rgbBkColor) )
				{
					nTilePixelNO++;
					continue;
				}
				nPixel = rgb;
			}else
			if(rgb==m_rgbBkColor)
			{
				if(m_nTransparent)
				{
					gTransparent:
					nTilePixelNO++;
					continue;
				}
				nPixel = m_nBkColor;
			}else
			if(rgb==m_rgbTextColor)
				nPixel = m_nTextColor;
			else
			if(rgb==m_rgbShadowColor)
				nPixel = m_nShadowColor;
			else
			{
				//ASSERT(FALSE);
				goto gTransparent;
			}
			nPixelTop=nTilePixelNO/nWidth;
			nPixelLeft=nTilePixelNO%nWidth;
			//翻转处理
			pView->TileDirection(nTilePixelNO, nPixelTop, nPixelLeft);
			if(nL90Check||nR90Check)
			{
				nPixelLeft = nWidth-1-nPixelLeft;
				nPixelTop = nHeight-1-nPixelTop;
				nTilePixelNO = nPixelTop*nWidth+nPixelLeft;
			}

			int nPixelNO = nDrawMode?
						nPixelTop%8*8+nPixelLeft%8:nTilePixelNO;
			pPixel=pTile+(nBitCount>8?nPixelNO*nBytePixelCount:nPixelNO/nBytePixelCount);

			int nPixelTop2=nPixelTop,
				nPixelLeft2=nPixelLeft,
				nTilePixelNO2=nTilePixelNO;
			switch(nDrawMode)
			{
			case CT2_DM_OBJH:
				pPixel+=(nPixelTop2/8*nObjTileColCount+nPixelLeft2/8)*nTileSize1_4;
				nPixelTop2%=8; nPixelLeft2%=8; nTilePixelNO2=nPixelTop2*8+nPixelLeft2;
				break;
			case CT2_DM_OBJV:
				pPixel+=nPixelTop2/8*nTileSize1_4+nPixelLeft2/8*(nTileSize1_4*nObjTileLineCount);
				nPixelTop2%=8; nPixelLeft2%=8; nTilePixelNO2=nPixelTop2*8+nPixelLeft2;
				break;
			}
			if(pPixel>=pEnd) goto End;
			switch(nTileFormat)
			{
			//MSB->LSB
			case TF_NDS1BPP:
			case TF_VB2BPP:
			// case TF_8CR3BPP:
			case TF_GBA4BPP:
			case TF_GBA8BPP:
				*pPixel&=~(nColorMask<<(nTilePixelNO2%nBytePixelCount*nBitCount));
				*pPixel|=nPixel<<(nTilePixelNO2%nBytePixelCount*nBitCount);
				break;
			//LSB->MSB
			case TF_1BPP:
				*pPixel&=~(nColorMask<<(7- (nTilePixelNO2%nBytePixelCount*nBitCount)));
				*pPixel|=nPixel<<(7- (nTilePixelNO2%nBytePixelCount*nBitCount));
				break;
			case TF_4C2BPP:
			case TF_GBA24BPP:
				*pPixel&=~(nColorMask<<(6-nTilePixelNO2%nBytePixelCount*nBitCount));
				*pPixel|=nPixel<<(6-nTilePixelNO2%nBytePixelCount*nBitCount);
				break;
			// case TF_8C3BPP:
				// *pPixel&=~(nColorMask<<(5-nTilePixelNO2%nBytePixelCount*nBitCount));
				// *pPixel|=nPixel<<(5-nTilePixelNO2%nBytePixelCount*nBitCount);
				// break;
			case TF_N644BPP:
				*pPixel&=~(nColorMask<<(4-nTilePixelNO2%nBytePixelCount*nBitCount));
				*pPixel|=nPixel<<(4-nTilePixelNO2%nBytePixelCount*nBitCount);
				break;
			case TF_NGP2BPP:
				nTilePixelNO2 = nDrawMode?(nPixelTop&7)*8+(7-(nPixelLeft&7)):nPixelTop*nWidth+nWidth-1-nPixelLeft;
				pPixel=pTile+nTilePixelNO2/nBytePixelCount;

				switch(nDrawMode)
				{
				case CT2_DM_OBJH:
					pPixel+=(nPixelTop/8*nObjTileColCount+nPixelLeft/8)*nTileSize1_4;
					break;
				case CT2_DM_OBJV:
					pPixel+=nPixelTop/8*nTileSize1_4+nPixelLeft/8*(nTileSize1_4*nObjTileLineCount);
					break;
				}
				if(pPixel>=pEnd) goto End;
				*pPixel &= ~(nColorMask<<nTilePixelNO2%nBytePixelCount*nBitCount);
				*pPixel |= nPixel<<(nTilePixelNO2%nBytePixelCount*nBitCount);
				break;

			
			case TF_SMS4BPP:
				pPixel+=(nDrawMode?nTilePixelNO2:nTilePixelNO2)/8*3;
				if(pPixel>=pEnd) goto End;
				*pPixel &= ~(1<<(7-(nTilePixelNO2%nBytePixelCount)));
				*pPixel++ |= (nPixel&1)<<(7-(nTilePixelNO2%nBytePixelCount));
				*pPixel &=~(1<<(7-(nTilePixelNO2%nBytePixelCount)));
				*pPixel++|=((nPixel>>1)&1)<<(7-(nTilePixelNO2%nBytePixelCount));
				*pPixel&=~(1<<(7-(nTilePixelNO2%nBytePixelCount)));
				*pPixel++|=((nPixel>>2)&1)<<(7-(nTilePixelNO2%nBytePixelCount));
				*pPixel&=~(1<<(7-(nTilePixelNO2%nBytePixelCount)));
				*pPixel|=((nPixel>>3)&1)<<(7-(nTilePixelNO2%nBytePixelCount));
				break;
			case TF_GBA3XBPP:
			case TF_CT0XBPP:
				*pPixel&=~((nTilePixelNO2&1)?nColorMask>>4:nColorMask&0x0F);
				*pPixel|=nPixel<<((nTileNO%nCD)*nCD2);
				break;
	
			case TF_SNES4BPP:
				switch(nDrawMode)
				{
				case CT2_DM_TILE:
					pPixel=(pTile+nPixelTop*nTileLineSize+nPixelLeft/nBytePixelCount);
					break;
				case CT2_DM_OBJH:
					pPixel=(pTile+nPixelTop2%8*nTileLineSize+nPixelLeft2%8/nBytePixelCount);
					pPixel+=(nPixelTop/8*nObjTileColCount+nPixelLeft/8)*nTileSize1_4;
					break;
				case CT2_DM_OBJV:
					pPixel=(pTile+nPixelTop2%8*nTileLineSize+nPixelLeft2%8/nBytePixelCount);
					pPixel+=nPixelTop/8*nTileSize1_4+nPixelLeft/8*(nTileSize1_4*nObjTileLineCount);
					break;
				}
				if(pPixel+nOffset2+(nTilePixelCount/4)>=pEnd) goto End;
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
				switch(nDrawMode)
				{
				case CT2_DM_TILE:
					pPixel=(pTile+nPixelTop*nTileLineSize+nPixelLeft/nBytePixelCount);
					break;
				case CT2_DM_OBJH:
					pPixel=(pTile+nPixelTop2%8*nTileLineSize+nPixelLeft2%8/nBytePixelCount);
					pPixel+=(nPixelTop/8*nObjTileColCount+nPixelLeft/8)*nTileSize1_4;
					break;
				case CT2_DM_OBJV:
					pPixel=(pTile+nPixelTop2%8*nTileLineSize+nPixelLeft2%8/nBytePixelCount);
					pPixel+=nPixelTop/8*nTileSize1_4+nPixelLeft/8*(nTileSize1_4*nObjTileLineCount);
					break;
				}
			case TF_NES2BPP:
					if(pPixel+nOffset2>=pEnd) goto End;
					nPixel1 = nPixel&0x01;
					nPixel2 = (nPixel>>1)&0x01;
					*pPixel&=~(1<<(7- (nPixelLeft%8)) );
					*pPixel|=nPixel1<<(7- (nPixelLeft%8));
					*(pPixel+nOffset2)&=~(1<<(7- (nPixelLeft%8)));
					*(pPixel+nOffset2)|=nPixel2<<(7- (nPixelLeft%8));
				break;
			case TF_RGBA16BPP:
				nPixel = RgbQuadToWord(nPixel|0xFF<<24);
				*(WORD*)pPixel=nPixel;
				break;
			case TF_RGB24BPP:
				*(WORD*)pPixel=nPixel;
				*(pPixel+2)=nPixel>>16;
				//memcpy(pPixel, &nPixel, 3);
				break;
			case TF_RGBA32BPP:
				nPixel = RgbQuadToColor(nPixel) | 0xFF<<24;
				*(UINT*)pPixel=nPixel;
				break;
			case TF_RHYTHM:
				{
					switch(nDrawMode)
					{
					case CT2_DM_TILE:
						pPixel = pTile+nPixelTop/4*(nWidth/2)+(nPixelLeft%nWidth)/8*4;
						break;
					case CT2_DM_OBJH:
						pPixel = pTile+nPixelTop%8/4*(8/2)+(nPixelLeft%8)/8*4;
						pPixel+=(nPixelTop/8*nObjTileColCount+nPixelLeft/8)*nTileSize1_4;
						break;
					case CT2_DM_OBJV:
						pPixel = pTile+nPixelTop%8/4*(8/2)+(nPixelLeft%8)/8*4;
						pPixel+=nPixelTop/8*nTileSize1_4+nPixelLeft/8*(nTileSize1_4*nObjTileLineCount);
						break;
					}
					if(pPixel>=pEnd) goto End;
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
				goto End;
			}
			nTilePixelNO++;
		}
		nTileNO++;nTile++;
		pro.SetProgressPos(nTile*100/nCount);
		//
	}
End:
	pro.EndModalLoop(IDCANCEL);
	return 0;
}

// CT2TileEditorView

IMPLEMENT_DYNCREATE(CT2TileEditorView, CT2View)

BEGIN_MESSAGE_MAP(CT2TileEditorView, CT2View)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_VIEW_GRID, OnViewGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRID, OnUpdateViewGrid)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SYSKEYDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_TILEEDITOR_TEXT, OnTileeditorText)
	ON_UPDATE_COMMAND_UI(ID_TILEEDITOR_TEXT, OnUpdateTileeditorText)
	ON_COMMAND(ID_HEXVIEW_IMPORT, InsertText)
	ON_UPDATE_COMMAND_UI(ID_HEXVIEW_IMPORT, OnUpdateTileeditorTextCtrl)
	ON_COMMAND(ID_TILEEDITOR_FONT, SetFont)
	ON_COMMAND(ID_TILEEDITOR_TBL, OnTbl)
	ON_UPDATE_COMMAND_UI(ID_TILEEDITOR_TBL, OnUpdateTbl)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CT2TileEditorView 构造/析构

CT2TileEditorView::CT2TileEditorView()
: m_pTileEditor(NULL)
, m_nScaleWidth(8)
, m_nScaleHeight(8)
, m_nWidth(8)
, m_nHeight(8)
, m_nScale(800)
, m_nTextColor(1)
, m_nBkColor(0)
, m_nShadowColor(2)
, m_nInsertText(0)
, m_nStartMoveText(0)
, m_strText(_T(""))
, m_hFont(NULL)
, m_hBmText(NULL)
, m_pPal(NULL)
, m_nShadowType(0)
, m_nTransparent(0)
, m_nAlignLeft(0)
, m_nAlignTop(0)
, m_nGrid(1)
{
}

CT2TileEditorView::~CT2TileEditorView()
{
	UINT nIndex = theApp.GetMRUIndex(GetDocument()->GetPathName());
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	if(pSD)
	{
		pSD->nTileEditorViewScale = m_nScale;
	}
	DeleteObject(m_hFont);
	DeleteObject(m_hBmText);
	m_pPaintPanel=NULL;

	m_TextDC.DeleteDC();
	m_ScaleDC.DeleteDC();
}

// CT2TileEditorView 消息处理程序

void CT2TileEditorView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc; GetClientRect(&rc);

	//CBitmap memBM;
	//memBM.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	//memDC.SelectObject(&memBM);
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
	memDC.SelectObject(hBM);

	CT2TileView* pTileView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);

	CRect rcSel;
	pTileView->GetTileRect(pTileView->m_ptCursorPos, rcSel);

	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si);
	int nTop = si.nPos * 100 / m_nScale;
	//if(nTop<0 || (UINT)nTop>m_nScaleHeight) nTop=0;
	GetScrollInfo(SB_HORZ, &si);
	int nLeft = si.nPos * 100 / m_nScale;
	//if(nLeft<0 || (UINT)nLeft>m_nScaleWidth) nLeft=0;

	int nWidth = m_nScaleWidth-(nLeft*m_nScale/100);
	int nHeight= m_nScaleHeight-(nTop*m_nScale/100);
	{
		FillRect(memDC.GetSafeHdc(), &rc, pTileView->m_hBkBrush);

		RGBQUAD *pCryBits;
		CDC CryDC; CryDC.CreateCompatibleDC(&dc);
		HBITMAP hCryBM = CreateDIBSection(dc.GetSafeHdc(),
			(BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pCryBits, NULL, 0);
 		CryDC.SelectObject(hCryBM);
		FillRect(CryDC.GetSafeHdc(), &rc, pTileView->m_hBkBrush);
		StretchDIBits(CryDC.GetSafeHdc(),
			0, 0, nWidth, nHeight,

			(pTileView->m_nDrawMode>=CT2_DM_MAP?0:rcSel.left)*m_nWidth+nLeft,
			pTileView->m_nDrawMode>=CT2_DM_MAP?0:((pTileView->m_nLineCount-1-rcSel.top)*
				m_nHeight), m_nWidth-nLeft, m_nHeight-nTop,

			pTileView->m_pPaintPanel,
			(BITMAPINFO*)&pTileView->m_biPaintPanel,
			DIB_RGB_COLORS, SRCCOPY);

		UINT s=bi.bmiHeader.biSizeImage/4;
			while(s--)
			{
				if(pTileView->m_nBitCount==16 || pTileView->m_nBitCount==32)
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
	}

	if(m_pPaintPanel && m_nInsertText) {
		DrawText();
		if(m_nTransparent)
			memDC.TransparentBlt(0, 0,
				m_nScaleWidth, m_nScaleHeight,
				&m_TextDC, 0, 0,
				m_nWidth, m_nHeight, m_clrBkColor);
		else
			memDC.StretchBlt(0, 0,
				m_nScaleWidth, m_nScaleHeight,
				&m_TextDC, 0, 0,
				m_nWidth,
				m_nHeight, SRCCOPY);

		int l=m_rcText.left*m_nScale/100;
		int t=m_rcText.top*m_nScale/100;
		// Track
		memDC.Draw3dRect(l+1, t+1, m_nScaleWidth-1, m_nScaleHeight-1,
			0xFF00FF, 0xFF00FF);
		CBrush b((COLORREF)0xFF00FF);
		CRect rc2(l-2, t-2, l+4, t+4);
		memDC.FillRect(&rc2, &b);
		rc2.MoveToX(l+m_nScaleWidth-4);
		memDC.FillRect(&rc2, &b);
		rc2.MoveToY(t+m_nScaleHeight-4);
		memDC.FillRect(&rc2, &b);
		rc2.MoveToX(l-2);
		memDC.FillRect(&rc2, &b);
	}

	if(m_nGrid && (m_nScale>100))//绘制网格
	{
		CPen whiteGPen(PS_SOLID, 1, (COLORREF)0x9C9C9C);
		memDC.SelectObject(&whiteGPen);
		memDC.SetBkMode(R2_NOT);
		for(int i=0; i<=m_nWidth; i++)
		{//纵线
			memDC.MoveTo(rc.left+i*m_nScale/100, rc.top);
			memDC.LineTo(rc.left+i*m_nScale/100, m_nScaleHeight);
		}
		for(int i=0; i<=m_nHeight; i++)
		{//横线
			memDC.MoveTo(rc.left, rc.top+i*m_nScale/100);
			memDC.LineTo(m_nScaleWidth, rc.top+i*m_nScale/100);
		}
	}

	dc.BitBlt(0, 0, rc.Width(), rc.Height(),
		&memDC, 0, 0, SRCCOPY);

	memDC.DeleteDC();
}

void CT2TileEditorView::OnUpdateData()
{
	CT2TileView* pTileView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	if(!pTileView) return;
	pTileView->OnUpdateData();

	m_pPal = pTileView->GetDocument()->m_pPal+pTileView->m_nPalIndex;
	m_nWidth = pTileView->m_nWidth;
	m_nHeight = pTileView->m_nHeight;
	m_nScaleWidth=m_nWidth*m_nScale/100;
	m_nScaleHeight=m_nHeight*m_nScale/100;

	m_rcScaleText.SetRect(0,0,m_nScaleWidth,m_nScaleHeight);
	m_rcText.SetRect(m_rcScaleText.left*100/m_nScale,
			m_rcScaleText.top*100/m_nScale,
			m_nWidth, m_nHeight);

	pTileView->SetBits();

	m_nSWidth = m_nWidth; m_nSHeight = m_nHeight;
	OnCreateDC();
	if( (m_nWidth!=m_biPaintPanel.biWidth) ||
		(m_nHeight!=m_biPaintPanel.biHeight) )
	{
		m_biPaintPanel.biWidth = m_nWidth;
		m_biPaintPanel.biHeight= m_nHeight;
		m_biPaintPanel.biSizeImage = m_nWidth*m_nHeight*4;

		//OnCreateDC();

		if(m_hBmText) DeleteObject(m_hBmText);
		m_hBmText = CreateDIBSection(m_TextDC.GetSafeHdc(),
			(BITMAPINFO*)&m_biPaintPanel,
			DIB_RGB_COLORS, (void**)&m_pPaintPanel, NULL, 0);
		m_TextDC.SelectObject(m_hBmText);

		CBitmap bm; int w = m_nSWidth, h = m_nSHeight;
		if(pTileView->m_nAlignMode>8) w=h=abs(m_lfFont.lfHeight);

		bm.CreateCompatibleBitmap(&m_TextDC, w, h);
		m_ScaleDC.SelectObject(&bm)->DeleteObject();
	}

	switch(pTileView->m_nAlignMode)
	{
	case 0:// 0 1 2
	case 3:// 3 4 5
	case 6:// 6 7 8
		m_nAlignLeft = 0;
		break;

	case 2:
	case 5:
	case 8:
		m_nAlignLeft = m_nWidth-abs(m_lfFont.lfHeight);
		break;

	case 1:
	case 4:
	case 7:
		m_nAlignLeft = (m_nWidth-abs(m_lfFont.lfHeight))/2;
		break;
	}
	//m_TextDC.SetStretchBltMode(BLACKONWHITE);
	switch(pTileView->m_nAlignMode)
	{
	case 0:
	case 1:
	case 2:
		m_nAlignTop = 0;
		break;
	case 6:
	case 7:
	case 8:
		m_nAlignTop = m_nHeight-abs(m_lfFont.lfHeight);
		break;
	case 3:
	case 4:
	case 5:
		m_nAlignTop = (m_nHeight-abs(m_lfFont.lfHeight))/2;
		break;

	case 9:
	case 10:
	case 11:
	case 12:
		m_nAlignLeft = m_nAlignTop = 0;
		m_nSWidth = m_nSHeight = (WORD)abs(m_lfFont.lfHeight);

		if(pTileView->m_nAlignMode==9)
			m_TextDC.SetStretchBltMode(WHITEONBLACK);
		else
		if(pTileView->m_nAlignMode==10)
			m_TextDC.SetStretchBltMode(BLACKONWHITE);
		else
		if(pTileView->m_nAlignMode==11)
			m_TextDC.SetStretchBltMode(COLORONCOLOR);
		else
		if(pTileView->m_nAlignMode==12)
			m_TextDC.SetStretchBltMode(HALFTONE);
		break;
	}

	if(pTileView->m_nBitCount>8)
	{
		RGBQUAD *pPal = pTileView->GetDocument()->m_pPal;
		RGBQUAD rgbColor = *(pPal+m_nTextColor);
		m_clrTextColor = RGB(rgbColor.rgbRed,rgbColor.rgbGreen,rgbColor.rgbBlue);;
		m_rgbTextColor = *(UINT*)&rgbColor;
		rgbColor = *(pPal+m_nBkColor);
		m_clrBkColor = RGB(rgbColor.rgbRed,rgbColor.rgbGreen,rgbColor.rgbBlue);;;
		m_rgbBkColor = *(UINT*)&rgbColor;
		rgbColor = *(pPal+m_nShadowColor);
		m_clrShadowColor = RGB(rgbColor.rgbRed,rgbColor.rgbGreen,rgbColor.rgbBlue);;;
		m_rgbShadowColor = *(UINT*)&rgbColor;
	}else
	{
		m_nTextColor&=pTileView->m_nColorMask;
		RGBQUAD *rgb = m_pPal+m_nTextColor;
		m_rgbTextColor = *(UINT*)rgb;
		m_clrTextColor = RGB(rgb->rgbRed, rgb->rgbGreen, rgb->rgbBlue);
		m_nBkColor&=pTileView->m_nColorMask;
		rgb = m_pPal+m_nBkColor;
		m_rgbBkColor = *(UINT*)rgb;
		m_clrBkColor = RGB(rgb->rgbRed, rgb->rgbGreen, rgb->rgbBlue);
		m_nShadowColor&=pTileView->m_nColorMask;
		rgb = m_pPal+m_nShadowColor;
		m_rgbShadowColor = *(UINT*)rgb;
		m_clrShadowColor = RGB(rgb->rgbRed, rgb->rgbGreen, rgb->rgbBlue);
	}
	m_rcText.SetRect(m_rcText.left, m_rcText.top, m_nWidth, m_nHeight);
	//if(m_nInsertText)
		DrawText();

	SetScrollSizes(MM_TEXT, CSize(m_nScaleWidth, m_nScaleHeight));

	CT2View::OnUpdateData();
}

void CT2TileEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	case VK_ESCAPE:
	case VK_TAB:
		SetCurView(ID_VIEW_TILEVIEW);
		break;
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
		if(!(GetKeyState(VK_SHIFT)&0x80))
		{
			CT2TileView* pView=(CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
			pView->OnKeyDown(nChar, nRepCnt, nFlags);
			OnUpdateData();
		}break;
	case VK_RETURN:
		if(m_nInsertText)
		{
			//AddMacro_SetTile();
			SetTile();
			OnUpdateData();
		}break;
	}

	CT2View::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CT2TileEditorView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CT2View::OnActivateView(bActivate, pActivateView, pDeactiveView);

	CT2TileView* pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	if(pView)
		pView->SetBits();
}

void CT2TileEditorView::OnInitialUpdate()
{
	CT2View::OnInitialUpdate();

	CString Name = GetDocument()->GetPathName();
	UINT nIndex = theApp.GetMRUIndex(Name);
	STATEDATA* pSD = theApp.m_pRecentStatList->GetStateData(nIndex);
	if(theApp.m_pRecentStatList->IsOK(nIndex))
	{
		m_nScale = pSD->nTileEditorViewScale;
	}

	OnCreateDC();

	//m_rcScaleText.SetRect(0,0,m_nScaleWidth,m_nScaleHeight);
	//m_rcText.SetRect(m_rcScaleText.left*100/m_nScale,
	//		m_rcScaleText.top*100/m_nScale,
	//		m_nWidth, m_nHeight);

	::GetObject(GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), &m_lfFont);
	GetCodePageDescription(GetDocument()->m_nTblCodePage, &Name);
	//m_lfFont.lfHeight = m_nHeight>m_nWidth?m_nWidth:m_nHeight;
	lstrcpy(m_lfFont.lfFaceName, Name);
	m_hFont = CreateFontIndirect(&m_lfFont);
	m_ScaleDC.SelectObject(m_hFont);

	OnUpdateData();
}

void CT2TileEditorView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Invalidate(FALSE);
	CT2View::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CT2TileEditorView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Invalidate(FALSE);
	CT2View::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CT2TileEditorView::OnViewGrid()
{
	m_nGrid=!m_nGrid;
	Invalidate(FALSE);
}

void CT2TileEditorView::OnUpdateViewGrid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nGrid);
}

void CT2TileEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CT2View::OnLButtonDown(nFlags, point);

	if(m_nInsertText)
	{
		if(m_rcScaleText.PtInRect(point))
		{
			m_ptStartMoveText=point;
			m_nStartMoveText=1;
		}
	}else
		SetPixel(nFlags, point);
}

void CT2TileEditorView::SetPixel(UINT nFlags, CPoint point)
{
	CT2TileView *pTileView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	if(GetDocument()->m_nReadOnly || pTileView->m_nTileFormat==TF_CT0XBPP || pTileView->m_nTileFormat==TF_GBA3XBPP) return;

	UINT Pixel;
	if(nFlags&MK_LBUTTON)
		Pixel=pTileView->m_nBitCount>8?m_rgbTextColor:m_nTextColor;
	else
	if(nFlags&MK_RBUTTON)
		Pixel=pTileView->m_nBitCount>8?m_rgbBkColor:m_nBkColor;
	else
		return;
	SCROLLINFO si;
	GetScrollInfo(SB_HORZ, &si);
	int x=point.x+si.nPos;
	GetScrollInfo(SB_VERT, &si);
	int y=point.y+si.nPos;
	if( (x>=(int)m_nScaleWidth) ||
		(y>=(int)m_nScaleHeight) ||
		(x<0) ||
		(y<0) )
		return;
	SetCapture();

	CRect rc; GetClientRect(&rc);
	int nTilePixelNO=(y-rc.top)*100/m_nScale*m_nWidth+
		(x-rc.left)*100/m_nScale;
	static int nOldTilePixelNO=-1, nOldPixel=-1;
	// 修改
	if((nOldTilePixelNO!=nTilePixelNO) || (nOldPixel!=Pixel))
	{
		nOldTilePixelNO=nTilePixelNO; nOldPixel=Pixel;
		int x=pTileView->m_ptCursorPos.x;
		if(pTileView->m_nDrawMode>=CT2_DM_MAP)
		{
			int l = nTilePixelNO%m_nWidth/8;
			int t = nTilePixelNO/m_nWidth/8*pTileView->m_nColCount;
			int nMap = (t+l)*2;
			x = *(WORD*)(GetDocument()->m_pRom+pTileView->m_nMoOffset+nMap);
			x&=0x03FF;
			pTileView->m_ptCursorPos.x=t+l;
			nTilePixelNO = nTilePixelNO/m_nWidth%8*8+nTilePixelNO%8;
		}
		if(!m_bUndo)
		{
			UINT nOffset = pTileView->m_nOffset+
				x*pTileView->m_nTileSize;
			UINT nSize=pTileView->m_nTileSize;
			if((nOffset+nSize)>=GetDocument()->m_nRomSize)
				nSize=GetDocument()->m_nRomSize-nOffset;

			DoUndo(GetDocument()->m_pRom+nOffset,
				nOffset, nSize);
			m_bUndo=pTileView->m_nDrawMode>=CT2_DM_MAP?0:1;
		}
		if(pTileView->m_nBitCount==16||pTileView->m_nBitCount==32)
			Pixel|=0xFF<<24;;

		//pTileView->AddMacro_SetPixel(nTilePixelNO, Pixel);
		pTileView->SetPixel(nTilePixelNO, Pixel);
	}

	pTileView->SetBits(); Invalidate(FALSE);
}

void CT2TileEditorView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CT2View::OnRButtonDown(nFlags, point);

	if(!m_nInsertText)
		SetPixel(nFlags, point);
}

void CT2TileEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_nInsertText)
	{
		CRect rc(0, 0, m_nScaleWidth, m_nScaleHeight);
		if(m_nStartMoveText && rc.PtInRect(m_ptStartMoveText))
		{
			m_rcScaleText.left+=point.x-m_ptStartMoveText.x;
			m_rcScaleText.top+=point.y-m_ptStartMoveText.y;
			m_rcScaleText.right=m_rcScaleText.left+m_nScaleWidth;
			m_rcScaleText.bottom=m_rcScaleText.top+m_nScaleHeight;
			m_ptStartMoveText=point;
			m_rcText.SetRect(m_rcScaleText.left*100/m_nScale,
				m_rcScaleText.top*100/m_nScale,
				m_nWidth, m_nHeight);
			Invalidate(FALSE);
		}
	}else
		SetPixel(nFlags, point);

	CT2View::OnMouseMove(nFlags, point);
}

void CT2TileEditorView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if((nChar==VK_LEFT)||(nChar==VK_RIGHT)||(nChar==VK_UP)||(nChar==VK_DOWN))
	{
		CT2TileView* pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
		pView->OnSysKeyDown(nChar, nRepCnt, nFlags);
		OnUpdateData();
	}
	CT2View::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CT2TileEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bUndo=0;
	m_nStartMoveText=0;
	ReleaseCapture();

	CT2View::OnLButtonUp(nFlags, point);
}

void CT2TileEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bUndo=0;
	ReleaseCapture();

	CT2View::OnRButtonUp(nFlags, point);
}

void CT2TileEditorView::OnTileeditorText()
{
	m_nInsertText = !m_nInsertText;
	if(m_nInsertText)
	{
		m_rcScaleText.SetRect(0,0,m_nScaleWidth,m_nScaleHeight);
		m_rcText.SetRect(m_rcScaleText.left*100/m_nScale,
				m_rcScaleText.top*100/m_nScale,
				m_nWidth, m_nHeight);
		//theApp.GetExplorerBar()->GetDlgItem(IDC_TILEEDITOR_TEXT)->SetFocus();
	}
	Invalidate(FALSE);
}

void CT2TileEditorView::OnUpdateTileeditorText(CCmdUI *pCmdUI)
{
	CT2TileView *pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	pCmdUI->SetCheck(m_nInsertText);
	pCmdUI->Enable((!GetDocument()->m_nReadOnly) &&
		(pView->m_nDrawMode<CT2_DM_MAP));
}

void CT2TileEditorView::OnUpdateTileeditorTextCtrl(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nInsertText && !GetDocument()->m_nReadOnly);
}

void CT2TileEditorView::SetFont()
{
	CFontDialog fd(&m_lfFont);
	if(fd.DoModal()!=IDOK) return;

	OnSetFont();
}

void CT2TileEditorView::OnSetFont()
{
	if(m_hFont) DeleteObject(m_hFont);
	m_hFont = CreateFontIndirect(&m_lfFont);
	m_ScaleDC.SelectObject(m_hFont);

	m_biPaintPanel.biWidth=0;
	OnUpdateData();
}

void CT2TileEditorView::DrawText()
{
	m_ScaleDC.SetBkColor(m_clrBkColor);
	m_ScaleDC.FillRect(&CRect(0, 0, m_nSWidth, m_nSHeight), &CBrush(m_clrBkColor));

	m_ScaleDC.SetTextColor(m_clrShadowColor);

	//CRect rcText(m_rcText);
	CRect rcText(0, 0, m_nSWidth, m_nSHeight);

	rcText.OffsetRect(m_nAlignLeft+m_rcText.left, m_nAlignTop+m_rcText.top);

	CRect rcShadow(rcText);
	switch(m_nShadowType)
	{
	case 3://ID_TILEEDITOR_PATH
		rcShadow.MoveToY(rcText.top-1);
		m_ScaleDC.DrawText(m_strText, &rcShadow, DT_LEFT);

		rcShadow.MoveToX(rcText.left-1);
		m_ScaleDC.DrawText(m_strText, &rcShadow, DT_LEFT);

		rcShadow.MoveToY(rcText.top);
		m_ScaleDC.DrawText(m_strText, &rcShadow, DT_LEFT);

		rcShadow.MoveToY(rcText.top+1);
		m_ScaleDC.DrawText(m_strText, &rcShadow, DT_LEFT);

		rcShadow.MoveToX(rcText.left);
		m_ScaleDC.DrawText(m_strText, &rcShadow, DT_LEFT);

		rcShadow.MoveToX(rcText.left+1);
		m_ScaleDC.DrawText(m_strText, &rcShadow, DT_LEFT);

		rcShadow.MoveToY(rcText.top);
		m_ScaleDC.DrawText(m_strText, &rcShadow, DT_LEFT);

		rcShadow.MoveToY(rcText.top-1);
		m_ScaleDC.DrawText(m_strText, &rcShadow, DT_LEFT);
		break;
	case 1://ID_TILEEDITOR_SHADOWRB
		rcShadow.MoveToY(rcText.top+1);
		m_ScaleDC.DrawText(m_strText, &rcShadow, DT_LEFT);
	case 2://ID_TILEEDITOR_SHADOWR
		rcShadow.MoveToX(rcText.left+1);
		m_ScaleDC.DrawText(m_strText, &rcShadow, DT_LEFT);
		break;
	}
	m_ScaleDC.SetTextColor(m_clrTextColor);
	m_ScaleDC.DrawText(m_strText, &rcText, DT_LEFT);

	m_TextDC.StretchBlt(0, 0, m_nWidth, m_nHeight,
		&m_ScaleDC,
		0, 0, m_nSWidth, m_nSHeight, SRCCOPY);
}

void CT2TileEditorView::SetTile(BOOL bUndo)
{
	if(GetDocument()->m_nReadOnly) return;
	CT2TileView *pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	BYTE *pRom = pView->GetDocument()->m_pRom+pView->m_nOffset;
	BYTE nCD=(pView->m_n24Check?0x04:0x02);

	UINT nOffset; int nMap; WORD wMapData;
	RGBQUAD *pPalette = m_pPal;

	if(pView->m_nDrawMode>=CT2_DM_MAP)
	{
		nMap = pView->m_ptCursorPos.x<<1;
		wMapData = *(WORD*)(GetDocument()->m_pRom+pView->m_nMoOffset+nMap);

		pPalette = pView->GetDocument()->m_pPal+ ((wMapData>>12)<<4);

		nOffset = (wMapData&0x03FF)*pView->m_nTileSize;
	}else
	{
		nOffset = (pView->m_nTileFormat!=TF_GBA3XBPP)&&
		(pView->m_nTileFormat!=TF_CT0XBPP)?
		pView->m_ptCursorPos.x*pView->m_nTileSize:
		(pView->m_ptCursorPos.x/nCD)*pView->m_nTileSize;
	}
	if( (nOffset+pView->m_nTileSize) >= GetDocument()->m_nRomSize ) return;
	if(bUndo)
		DoUndo(pRom+nOffset,
			nOffset+pView->m_nOffset, pView->m_nTileSize);
	int nWidth, nHeight;
	if(pView->m_nDrawMode>=CT2_DM_MAP)
		nWidth = nHeight = 8;
	else
	{
		nWidth = m_nWidth;
		nHeight = m_nHeight;
	}
	int nCount = nWidth*nHeight;
	UINT nPixel; WORD nColorCount = /*pView->m_nDrawMode>=CT2_DM_MAP?256:*/1<<pView->m_nBitCount;
	for(int i=0; i<nCount; i++)
	{
		int nNo = pView->m_nDrawMode>=CT2_DM_MAP ? (i>>3)*m_nWidth+(i&7) : i;
		UINT rgb = *(UINT*)(m_pPaintPanel+
			(m_nHeight-1-nNo/m_nWidth)*m_nWidth+nNo%m_nWidth);
		if(pView->m_nBitCount>8)
		{
			if(rgb==m_rgbBkColor)
			{
				if(m_nTransparent)
					continue;
				if(!m_nOpenMapFile)
				{
					if(pView->m_nBitCount==16)
						rgb&=0x7FFF;
					if(pView->m_nBitCount==32)
						rgb&=0xFFFFFF;
				}
			}else
			if(!m_nOpenMapFile)
			{
				if(pView->m_nBitCount==16||pView->m_nBitCount==32)
					rgb|=0xFF<<24;
			}
			nPixel = rgb;
		}else
		if(pView->m_nDrawMode<CT2_DM_MAP)
		{
			if(rgb==m_rgbBkColor)
			{
				if(m_nTransparent) continue;
				nPixel=m_nBkColor;
			}else
			if(rgb==m_rgbTextColor)
				nPixel=m_nTextColor;
			else
			if(rgb==m_rgbShadowColor)
				nPixel=m_nShadowColor;
			else
			{
				goto pal;
			}
		}else
		{
			pal:
			nPixel=m_nBkColor;
			RGBQUAD *pPal=pPalette;
			for(int p=0; p<nColorCount; p++)
			{
				if(*(UINT*)pPal++==rgb)
				{
					nPixel=p;
					break;
				}
			}
		}
		pView->SetPixel(i, nPixel);
	}
}

void CT2TileEditorView::OnTbl()
{
	static CTextFileDialog fd(TRUE, IDS_TBLFILTER);
	if(fd.DoModal()!=IDOK) return;

	OnTbl(fd.GetPathName(), fd.m_nCodePage);
}

BOOL CT2TileEditorView::OnTbl(CString path, UINT nCodePage)
{
	LPTSTR lpUnicodeBuffer = GetUText(path, nCodePage);
	if(!lpUnicodeBuffer) return FALSE;

	if(theApp.GetCurView()!=this)
		OnUpdateData();

	LPTSTR lpTbl = lpUnicodeBuffer;
	LPTSTR lpTblList = lpTbl;

	if(*(WORD*)lpTbl==0xFEFF) lpTbl = (LPTSTR)((BYTE*)lpTbl+2);

	UINT nCount=0;
	while(*lpTbl)
	{
		if(*lpTbl==_T(';'))
			goto skip;
		if(*lpTbl!=_T('='))
		{
			lpTbl++;
			continue;
		}
		lpTbl++;
		if(*lpTbl) {*lpTblList++=*lpTbl++; nCount++;}
		skip:
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
	CString strOldText = m_strText;
	lpTblList=lpUnicodeBuffer;

	DoImportTbl(lpTblList, nCount, IDS_IMPROTTBL);

	delete[] lpUnicodeBuffer;

	m_strText = strOldText;

	//AddMacro_Tbl(path, nCodePage);
	return TRUE;
}

void CT2TileEditorView::OnUpdateTbl(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!GetDocument()->m_nReadOnly);
}

BOOL CT2TileEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	Invalidate(FALSE);

	return CT2View::OnMouseWheel(nFlags, zDelta, pt);
}

void CT2TileEditorView::DoImportTbl(LPCTSTR lpTblList, UINT nCount, UINT nRes)
{
	TBLDATA td;
	td.pPro = (CT2StatusBar*) theApp.GetStatusBar();
	td.pPro->BeginProgress(nRes);

	td.pView = (CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW);
	td.pEditor = this;
	td.nCount = nCount;
	td.lpTblList = lpTblList;

	CWinThread *pOpenThread = AfxBeginThread(ImportTbl,
		&td, THREAD_PRIORITY_IDLE, 1024, 0, NULL);
	td.pPro->RunModalLoop();

	td.pPro->EndProgress();
}

void CT2TileEditorView::OnCreateDC()
{
	if(!m_TextDC.GetSafeHdc())
	{
		CDC *pDC=GetDC();
		m_TextDC.CreateCompatibleDC(pDC);
		ReleaseDC(pDC);

		m_TextDC.SetBkMode(TRANSPARENT);
	}
	if(!m_ScaleDC.GetSafeHdc())
	{
		m_ScaleDC.CreateCompatibleDC(&m_TextDC);
		m_ScaleDC.SetBkMode(TRANSPARENT);
	}
}

void CT2TileEditorView::InsertText()
{
	OnUpdateData();
	//AddMacro_SetTile();
	SetTile();
	OnUpdateData();

	if(GetViewType()==ID_VIEW_TILEVIEW)
	{
		CT2ChildFrm *pChild = (CT2ChildFrm*)GetParent();
		CT2TileView *pTV = (CT2TileView*)pChild->m_pTileView;
		if(pTV->m_ptCursorPos.x<(pTV->m_nColCount*pTV->m_nLineCount-1))
		{
			pTV->m_ptCursorPos.y=++pTV->m_ptCursorPos.x;
		}else
		{
			theApp.GetCurView()->SendMessage(WM_KEYDOWN, VK_DOWN);
			pTV->m_ptCursorPos.y=pTV->m_ptCursorPos.x-=pTV->m_nColCount-1;
		}
	}
}

void CT2TileEditorView::AppendVar()
{
	CT2ChildFrm *pChild = (CT2ChildFrm*)GetParent();

	if(pChild->m_nCurView==ID_VIEW_TILEEDITORVIEW)
		pChild->m_pTileView->AppendVar();

	CCrystalTreeCtrl &tc = ((CT2MainFrm*)theApp.GetMainWnd())->m_TreeBar.m_Tree;
	//tc.AppendVal(1, &pChild->m_pTileView->m_nOffset, CTCS_UINT, 0, pChild->m_pTileView->GetDocument()->m_nRomSize-1);
	tc.AppendVal(3, &m_nScale, CTCS_WORD);

	int i=CTAV_EDITORVIEW+1;
	tc.AppendVal(i++, &m_nTransparent, CTCS_BYTE);
	tc.AppendVal(i++, &m_nShadowType, CTCS_BYTE);
	tc.AppendVal(i++, &m_lfFont.lfFaceName, CTCS_LPTSTR, 0, LF_FACESIZE);
	tc.AppendVal(i++, &m_nInsertText, CTCS_BYTE);
	tc.AppendVal(i++, &m_strText, CTCS_CSTRING);
}
/*
void CT2TileEditorView::AddMacro_Tbl(CString& path, UINT nCodePage)
{
	CT_MACRO_ONTBL m;
	INITMACRO(m, CT2TILEEDITOR_TBL);

	m.nVal = nCodePage;
	lstrcpyn(m.szFile, path, _MAX_PATH);

	m.nTextColor	= m_nTextColor;
	m.nBkColor		= m_nBkColor;
	m.nShadowColor	= m_nShadowColor;
	m.nAlignLeft	= m_nAlignLeft;
	m.nAlignTop		= m_nAlignTop;
	m.rcText		= m_rcText;
	m.nTransparent	= m_nTransparent;
	m.nShadowType	= m_nShadowType;
	m.lfFont		= m_lfFont;

	CT2TileView *pTV = (CT2TileView*) ((CT2ChildFrm*)GetParent())->m_pTileView;
	pTV->Save_StateData(&m.StateData);
	m.StateData.nColCount = pTV->m_nColCount;
	m.ptPos			= pTV->m_ptCursorPos;

	AddMacro(&m);
}

void CT2TileEditorView::AddMacro_SetTile()
{
	CT_MACRO_ONTBL m;
	INITMACRO(m, CT2TILEEDITOR_SETTILE);

	lstrcpyn(m.szFile, m_strText, _MAX_PATH);

	m.nTextColor	= m_nTextColor;
	m.nBkColor		= m_nBkColor;
	m.nShadowColor	= m_nShadowColor;
	m.nAlignLeft	= m_nAlignLeft;
	m.nAlignTop		= m_nAlignTop;
	m.rcText		= m_rcText;
	m.nTransparent	= m_nTransparent;
	m.nShadowType	= m_nShadowType;
	m.lfFont		= m_lfFont;

	CT2TileView *pTV = (CT2TileView*) ((CT2ChildFrm*)GetParent())->m_pTileView;
	pTV->Save_StateData(&m.StateData);
	m.StateData.nColCount = pTV->m_nColCount;
	m.ptPos			= pTV->m_ptCursorPos;

	AddMacro(&m);
}
*/