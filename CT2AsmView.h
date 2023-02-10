#ifndef CT2_CT2ASMVIEW_H
#define CT2_CT2ASMVIEW_H
// CT2AsmView.h : CT2AsmView 类的接口
//
#include "CT2View.h"

#pragma once


class CT2AsmView : public CT2View
{
protected: // 仅从序列化创建
	DECLARE_DYNCREATE(CT2AsmView)

// 属性
public:

// 操作
public:

// 重写
public:

protected:

// 实现
public:
	CT2AsmView();
	virtual ~CT2AsmView();

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdateData(void);
public:
	int m_nHeight;
	int m_nLines;
	BYTE m_nSize;
	BYTE m_nCpuType;
	char m_strAsm[_MAX_PATH];
	TCHAR m_wstrAsm[_MAX_PATH];
	CEdit m_BaseEdit;
	UINT m_nBaseAddress;
	CUIntArray m_Follow;

	afx_msg void OnPaint();
	virtual void OnInitialUpdate();
	afx_msg void OnArmList(UINT nID);
	afx_msg void OnUpdateArmList(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBaseChange();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void ScrollTo(UINT nOffset, BOOL bForce=FALSE);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditExport();
	int GetAsm(CString &strAsm, UINT nOffset, BOOL bType);
	afx_msg void OnAsmFollow();
	afx_msg void OnAsmUnfollow();
	afx_msg void OnUpdateAsmUnfollow(CCmdUI *pCmdUI);
};

#endif // CT2_CT2ASMVIEW_H