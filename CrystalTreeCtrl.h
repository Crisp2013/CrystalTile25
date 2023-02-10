#ifndef _CCRYSTALTREECTRL_H
#define _CCRYSTALTREECTRL_H

#pragma once

#define CTCN_UPDATE_T (TVN_LAST)
#define CTCN_UPDATE_F (TVN_LAST+1)

struct CTC_PARAM
{
	void	*pValue;	// 关联指针变量
	LPTSTR	pData;		// 控件自身数据
	LPTSTR	lpText;		// 控件文本
	LPTSTR	lpHint;		// 控件提示
	UINT	nMin;
	UINT	nMax;
	WORD	nStyle;		// 控件风格
};

struct CTC
{
	BYTE	nLeftNameLen;
	BYTE	nHintLen;
	BYTE	nDataLen;
	BYTE	nStyle;
	BYTE	nExStyle;
};

enum CTC_STYLE
{
	CTCS_EDIT		= 0x00,	// 附加控件类型
	CTCS_COMBOBOX	= 0x01,
	CTCS_COMBOLBOX	= 0x02,
	CTCS_PATHBOX	= 0x03,
	CTCS_LINK		= 0x04,
	CTCS_BUTTON		= 0x05,

	CTCS_HEXMODE	= 0x08,
	CTCS_CHECKED	= 0x08,

	CTCS_THIS		= 0x00,	// 附加数据类型
	CTCS_LPTSTR		= 0x10,
	CTCS_CSTRING	= 0x20,
	CTCS_BYTE		= 0x30,
	CTCS_WORD		= 0x40,
	CTCS_UINT		= 0x50,

	CTCS_READONLY	= 0x80
};

enum CTC_EX_STYLE
{
	CTCS_EX_GETTXT	= 0x0100,	// 关联变量的值转换为文本
	CTCS_EX_CMDRANGE= 0x0200,	// Cmd Range
	CTCS_EX_DISIBLE = 0x0400,	// Cmd Range
};

#define	DATATYPE_MASK	(0x70)
#define CTRLTYPE_MASK	(0x07)

#define IDC_CTC_EDIT		0xF000
#define IDC_CTC_EDITBTN		0xF001
#define CTC_ACT_EXPANDALL	1
#define CTC_ACT_REMOVEALL	2

// CCrystalTreeCtrl

class CCrystalTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CCrystalTreeCtrl)

public:
	CCrystalTreeCtrl();
	virtual ~CCrystalTreeCtrl();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()

public:
	CComboBox	m_Edit;
	HWND	m_hwndList;
	HWND	m_hwndItem;
	HWND	m_hEditBtn;
	int		m_nSpliter;
	char	m_bSpliter;
	HFONT	m_hSymbolFont;
	char	m_nBtn;

	CTC* InsertTree(CTC *pCtc, HTREEITEM hParent=TVI_ROOT);
	BOOL LoadTree(CTC *pCtc=NULL, UINT nAct=CTC_ACT_REMOVEALL|CTC_ACT_EXPANDALL);
	BOOL LoadTree(CString strPath, HTREEITEM hParent=TVI_ROOT);
	BOOL AppendVal(int i, void *pVal, int nType, UINT nMin=0, UINT nMax=0);
	void DrawItem(HDC hdc);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	HTREEITEM IndexToItem(int i, HTREEITEM hItem, int &iCur);
	CString ParamToString(HTREEITEM hItem);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void MoveEditCtrl();
	void InitEditCtrl();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void UpdateData(HTREEITEM hItem);
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void ShowDropDown();
	BOOL ActiveLink(HTREEITEM hItem);
	UINT IsCmdItem(HTREEITEM hItem);
	void OnClear(HTREEITEM hItem);
	void ClearVar();
	void Reflash(BOOL bErase=FALSE);
	void SetVarData(HTREEITEM hItem, CString &strData);
	CString GetPathName(HTREEITEM hItem);
};

#endif // _CCRYSTALTREECTRL_H