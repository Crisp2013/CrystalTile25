#ifndef CT2_CT2TILEVIEW_H
#define CT2_CT2TILEVIEW_H
// CT2TileView.h : CT2TileView 类的接口
//
#include "CT2View.h"

#pragma once


class CT2TileView : public CT2View
{
protected: // 仅从序列化创建
	DECLARE_DYNCREATE(CT2TileView)

// 属性
public:

// 操作
public:

// 重写
public:

protected:

// 实现
public:
	CT2TileView();
	virtual ~CT2TileView();

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

public:
	BYTE m_nTileFormat;		// 颜色格式
	WORD m_nWidth;			// Tile宽度
	WORD m_nHeight;			// Tile高度
	UINT m_nTileSize;		// Tile大小
	BYTE m_nSkipSize;		// 跳过字节
	UINT m_nColorMask;		// 颜色遮罩
	BYTE m_nBytePixelCount;	// 一个字节包含多少象数
	BYTE m_nDrawMode;		// Tile显示模式(Tile/ObjH/ObjV)
	BYTE m_nBitCount;		// 位数
	WORD m_nScale;			// 缩放率
	BYTE m_nGrid;			// 是否显示网格
	WORD m_nColCount;		// Tile列数
	WORD m_nLineCount;		// Tile行数
	BYTE m_nPalIndex;		// 调色板索引号
	BYTE m_nL90Check;		// 左转九十度
	BYTE m_nR90Check;		// 右转九十度
	BYTE m_nHCheck;			// 水平翻转
	BYTE m_nVCheck;			// 垂直翻转
	BYTE m_nLRSwapCheck;		// 半边下移
	UINT m_nViewLineSize;	// 一行视图的Tile大小
	UINT m_nPageSize;		// 页大小
	BYTE m_bStartMove;		// 开始按下鼠标左键移动
	BYTE m_bShowFocus;		// 是否反色显示焦点
	UINT m_nScaleWidth;		// 缩放的Tile宽度
	UINT m_nScaleHeight;	// 缩放的Tile高度
	BYTE m_n24Check;		// 单Tile包含2/4层的Tile
	BYTE m_nNextSize;		// 相关偏移
	BYTE m_nAutoColFit;		// 是否自动调整Tile列数
	HBITMAP m_hImportBmp;	// 导入的位图
	CPoint m_ptImport;		// 导入位置
	CPoint m_ptMove;
	UINT m_nMoOffset;		// MAP或OAM地址
	BYTE m_nAlignMode;		// Tile对齐方式

	//ObjH/ObjV
	WORD m_nObjTileColCount;
	WORD m_nObjTileLineCount;
	UINT m_nTileSize1_4;

	HBRUSH m_hBkBrush;		// 底纹
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