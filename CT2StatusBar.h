#ifndef CT2_STATUSBAR_H
#define CT2_STATUSBAR_H

#pragma once

// CT2StatusBar

class CT2StatusBar : public CStatusBar
{
	DECLARE_DYNAMIC(CT2StatusBar)

public:
	CT2StatusBar();
	virtual ~CT2StatusBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	BYTE m_nPos;
	inline void SetProgressPos(BYTE nPos);
	void BeginProgress(UINT nIDs=0);
	void EndProgress();
	BOOL SetIndicators(const UINT* lpIDArray, int nIDCount);
	BOOL SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	int HitTest(CPoint pt);
	void GetItemRect(int nItem, CRect& rcItem);
	afx_msg void OnTimer(UINT nIDEvent);
};

#endif // CT2_STATUSBAR_H