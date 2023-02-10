#ifndef CT2_CT2TILEVIEW_H
#define CT2_CT2TILEVIEW_H
// CT2TileView.h : CT2TileView ��Ľӿ�
//
#include "CT2View.h"

#pragma once


class CT2TileView : public CT2View
{
protected: // �������л�����
	DECLARE_DYNCREATE(CT2TileView)

// ����
public:

// ����
public:

// ��д
public:

protected:

// ʵ��
public:
	CT2TileView();
	virtual ~CT2TileView();

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

public:
	BYTE m_nTileFormat;		// ��ɫ��ʽ
	WORD m_nWidth;			// Tile���
	WORD m_nHeight;			// Tile�߶�
	UINT m_nTileSize;		// Tile��С
	BYTE m_nSkipSize;		// �����ֽ�
	UINT m_nColorMask;		// ��ɫ����
	BYTE m_nBytePixelCount;	// һ���ֽڰ�����������
	BYTE m_nDrawMode;		// Tile��ʾģʽ(Tile/ObjH/ObjV)
	BYTE m_nBitCount;		// λ��
	WORD m_nScale;			// ������
	BYTE m_nGrid;			// �Ƿ���ʾ����
	WORD m_nColCount;		// Tile����
	WORD m_nLineCount;		// Tile����
	BYTE m_nPalIndex;		// ��ɫ��������
	BYTE m_nL90Check;		// ��ת��ʮ��
	BYTE m_nR90Check;		// ��ת��ʮ��
	BYTE m_nHCheck;			// ˮƽ��ת
	BYTE m_nVCheck;			// ��ֱ��ת
	BYTE m_nLRSwapCheck;		// �������
	UINT m_nViewLineSize;	// һ����ͼ��Tile��С
	UINT m_nPageSize;		// ҳ��С
	BYTE m_bStartMove;		// ��ʼ�����������ƶ�
	BYTE m_bShowFocus;		// �Ƿ�ɫ��ʾ����
	UINT m_nScaleWidth;		// ���ŵ�Tile���
	UINT m_nScaleHeight;	// ���ŵ�Tile�߶�
	BYTE m_n24Check;		// ��Tile����2/4���Tile
	BYTE m_nNextSize;		// ���ƫ��
	BYTE m_nAutoColFit;		// �Ƿ��Զ�����Tile����
	HBITMAP m_hImportBmp;	// �����λͼ
	CPoint m_ptImport;		// ����λ��
	CPoint m_ptMove;
	UINT m_nMoOffset;		// MAP��OAM��ַ
	BYTE m_nAlignMode;		// Tile���뷽ʽ

	//ObjH/ObjV
	WORD m_nObjTileColCount;
	WORD m_nObjTileLineCount;
	UINT m_nTileSize1_4;

	HBRUSH m_hBkBrush;		// ����
	WORD m_nBkPixType;

	afx_msg void OnPaint();
	void SetBits();
	void SetPixel(WORD nTilePixelNO, UINT nPixel);
	void OnUpdateData();
	void TileDirection(int nNo, int& nTop, int& nLeft);
	void GetTileRect(POINT& ptPos, CRect& rcRect);
	void OnImportBmp();
	afx_msg void OnViewGrid();
	afx_msg void OnUpdateViewGrid(CCmdUI *pCmdUI);
	virtual void OnInitialUpdate();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEditExport();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditImport();
	void OnEditImport(CString path);
	afx_msg void OnEditCopy();
	void OnCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	void OnPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnViewAutofitcol();
	afx_msg void OnUpdateViewAutofitcol(CCmdUI *pCmdUI);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void SetCurTileOffset();
	//afx_msg void OnUpdateViewMapOam(CCmdUI *pCmdUI);
	virtual void ScrollTo(UINT nOffset, BOOL bForce=FALSE);
	afx_msg void OnUpdateToolZoomexfirst(CCmdUI *pCmdUI);
	afx_msg void OnToolsZoomexfirst(UINT nID);
	afx_msg void OnTileeditorTbl();
	afx_msg void OnUpdateTileeditorTbl(CCmdUI *pCmdUI);
	void LoadBkBmp(WORD nBkPixType);
	afx_msg void OnEditSetfont();
	afx_msg void InsertText();
	afx_msg void OnUpdateInsertText(CCmdUI *pCmdUI);
	virtual void AppendVar();

	void Save_StateData(STATEDATA *pSD);
	void Load_StateData(STATEDATA *pSD);
	/*void AddMacro_ImprotBmp();
	void AddMacro_EditImport(CString path);
	void AddMacro_SetPixel(WORD nTilePixelNO, UINT nPixel);
	void AddMacro_EditCopy();
	void AddMacro_EditPaste();*/
};

#endif // CT2_CT2TILEVIEW_H