#ifndef CT2_CT2TILEEDITORVIEW_H
#define CT2_CT2TILEEDITORVIEW_H
// CT2TileEditorView.h : CT2TileEditorView 类的接口
//
#include "CT2View.h"

#pragma once

class CT2TileEditorView : public CT2View
{
protected: // 仅从序列化创建
	DECLARE_DYNCREATE(CT2TileEditorView)

// 属性
public:

// 操作
public:

// 重写
public:

protected:

// 实现
public:
	CT2TileEditorView();
	virtual ~CT2TileEditorView();

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

public:
	COLORREF* m_pTileEditor;
	UINT m_nScaleWidth;
	UINT m_nScaleHeight;
	WORD m_nScale;
	UINT m_nTextColor;
	UINT m_nBkColor;
	UINT m_nShadowColor;
	COLORREF m_clrTextColor;
	COLORREF m_clrBkColor;
	COLORREF m_clrShadowColor;
	COLORREF m_rgbTextColor;
	COLORREF m_rgbBkColor;
	COLORREF m_rgbShadowColor;
	BYTE m_nTransparent;
	int m_nAlignLeft;
	int m_nAlignTop;

	BYTE m_nInsertText;
	CRect m_rcScaleText;
	CRect m_rcText;
	CPoint m_ptStartMoveText;
	BYTE m_nStartMoveText;
	CString m_strText;
	LOGFONT m_lfFont;
	CDC m_TextDC;
	CDC m_ScaleDC;
	WORD m_nSWidth;
	WORD m_nSHeight;
	RGBQUAD* m_pPal;
	WORD m_nWidth;
	WORD m_nHeight;
	HFONT m_hFont;
	HBITMAP m_hBmText;
	BYTE m_nShadowType;
	BYTE m_nGrid;

	afx_msg void OnPaint();
	void OnUpdateData();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnInitialUpdate();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnViewGrid();
	afx_msg void OnUpdateViewGrid(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void SetPixel(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTileeditorText();
	afx_msg void OnUpdateTileeditorText(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTileeditorTextCtrl(CCmdUI *pCmdUI);
	afx_msg void SetFont();
	void OnSetFont();
	afx_msg void OnTbl();
	BOOL OnTbl(CString path, UINT nCodePage);
	afx_msg void OnUpdateTbl(CCmdUI *pCmdUI);
	void DrawText();
	void SetTile(BOOL bUndo=TRUE);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void DoImportTbl(LPCTSTR lpTblList, UINT nCount, UINT nRes=0);
	void OnCreateDC();
	afx_msg void InsertText();
	virtual void AppendVar();

	/*void AddMacro_Tbl(CString& path, UINT nCodePage);
	void AddMacro_SetTile();*/
};

#endif // CT2_CT2TILEEDITORVIEW_H