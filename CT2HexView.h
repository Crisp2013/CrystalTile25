#ifndef CT2_CT2HEXVIEW_H
#define CT2_CT2HEXVIEW_H
// CT2HexView.h : CT2HexView 类的接口
//
#include "CT2View.h"
#include ".\LZ77Search.h"
#include ".\RelativeDlg.h"

#pragma once


class CT2HexView : public CT2View
{
protected: // 仅从序列化创建
	DECLARE_DYNCREATE(CT2HexView)

// 属性
public:

// 操作
public:

// 重写
public:

protected:

// 实现
public:
	CT2HexView();
	virtual ~CT2HexView();

protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdateData(void);

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	BYTE m_nHeight;
	BYTE m_nWidth;
	BYTE m_nAsciiMode;
	BYTE m_nTblMode;
	BYTE m_nBeginSel;
	BYTE m_nByteLR;
	WORD m_nLines;
	BYTE m_nAlign;
	BYTE m_nColorText;
	BYTE m_nSwpCode;
	CFindReplaceDialog *m_pFrd;
	CString m_strFind;
	CString m_strReplace;
	BYTE *m_pFindData;
	char *m_pFindText;
	BYTE *m_pReplaceData;
	UINT m_nFindSize;
	UINT m_nFindTextSize;
	CLZ77Search m_Lz77Search;
	CRelativeDlg m_RelativeDlg;
	LOGFONT m_lf;

	afx_msg void OnPaint();
	virtual void OnInitialUpdate();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void SetCaretPos();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void SetData(UINT nOff, UINT nData, BYTE nSize);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void ScrollTo(UINT nOffset, BOOL bForce=FALSE);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnEnableTbl();
	afx_msg void OnUpdateEnableTbl(CCmdUI *pCmdUI);
	afx_msg void OnViewalign(UINT nID);
	afx_msg void OnUpdateViewalign(CCmdUI *pCmdUI);
	afx_msg void OnViewColortext();
	afx_msg void OnUpdateViewColortext(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void SetBits(int nBits);
	afx_msg void OnSwpCode();
	afx_msg void OnUpdateViewSwpCode(CCmdUI *pCmdUI);
	void DoFindReplace(BOOL bReplace=FALSE);
	LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSearchFind();
	afx_msg void OnSearchReplace();
	void OnFind();
	void OnReplace(BOOL bAll=FALSE);
	afx_msg void OnSearchFindnext();
	afx_msg void OnEditData2pal();
	void OnEditData2pal(UINT nOffset, BYTE nIndex, BYTE nBitCount);
	void OnEditPal2data(UINT nOffset, BYTE nIndex, BYTE nBitCount);
	afx_msg void OnUpdateEditData2pal(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditPal2data(CCmdUI *pCmdUI);
	afx_msg void OnEditPal2data();
	afx_msg void OnSearchLz77();
	afx_msg void OnToolsLz77uncomp();
	afx_msg void OnUpdateToolsLz77uncomp(CCmdUI *pCmdUI);
	afx_msg void OnSearchRelative();
	afx_msg void OnExportData();
	afx_msg void OnUpdateExportData(CCmdUI *pCmdUI);
	afx_msg void OnImportData();
	void OnImportData(CString path, UINT nOffset);
	void HitTest(UINT &nOffset, CPoint &pt, BOOL bOff=TRUE);
	afx_msg void OnImportText();
	void OnImportText(CString Text, UINT nOffset);
	afx_msg void OnUpdateImportText(CCmdUI *pCmdUI);
	afx_msg void OnHexviewTblctrl();
	afx_msg void OnUpdateHexviewTblctrl(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditImport(CCmdUI *pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnFontList(UINT nID);
	afx_msg void OnFontList();
	afx_msg void OnUpdateImport(CCmdUI *pCmdUI);
	virtual void AppendVar();

	/*void AddMacro_SetData(UINT nOffset, UINT nData, BYTE nSize);
	void AddMacro_ImportData(CString path, UINT nOffset);
	void AddMacro_ImportText(CString Text, UINT nOffset);
	void AddMacro_Pal2data(UINT nOffset, BYTE nIndex, BYTE nBitCount, BYTE nT);*/
	afx_msg void OnToolsGetcrc16();
};

#endif // CT2_CT2HEXVIEW_H