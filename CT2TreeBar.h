#ifndef _CT2TREEBAR_H
#define _CT2TREEBAR_H

#pragma once

#include ".\CrystalTree\CrystalTreeCtrl.h"
#include "CT2Favorite.h"
#include "CT2Palette.h"
#include "CT2NormalDlg.h"

// CT2TreeBar

class CT2TreeBar : public CControlBar
{
	DECLARE_DYNAMIC(CT2TreeBar)

public:
	CT2TreeBar();
	virtual ~CT2TreeBar();
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	virtual BOOL CT2TreeBar::Create(CWnd* pParentWnd, UINT nStyle, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()

public:
	CCrystalTreeCtrl m_Tree;
	POINT m_ptSizing;
	int m_nSizing;
	int m_nSpliter;
	CString m_strHint;
	HBRUSH m_hBrush;
	BOOL m_bUpdate;

	CTabCtrl m_Tab;
	CT2Favorite m_wndFav;
	CT2Palette m_wndPal;
	CT2NormalDlg m_wndDefOption;
	CCrystalTreeCtrl m_MyRes;

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	void DrawHint(CDC &dc);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTabSel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonRespath();
	int CanSplitte(POINT pt);
};

#endif // _CT2TREEBAR_H