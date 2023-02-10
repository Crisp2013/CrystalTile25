#ifndef CT2_CT2SCRIPTVIEW_H
#define CT2_CT2SCRIPTVIEW_H
// CT2ScriptView.h : CT2ScriptView 类的接口
//
#include "CT2View.h"

#pragma once

class CT2ScriptView : public CT2View
{
protected: // 仅从序列化创建
	DECLARE_DYNCREATE(CT2ScriptView)

// 属性
public:

// 操作
public:

// 重写
public:

protected:

// 实现
public:
	CT2ScriptView();
	virtual ~CT2ScriptView();

protected:
	//virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ScriptList;
	BYTE m_nTblMode;
	UINT m_nBegin;
	UINT m_nEnd;
	UINT m_nMinLen;
	UINT m_nMaxLen;
	BYTE m_nSwpCode;
	UINT m_nBaseAddress;
	UINT m_nPointBegin;
	UINT m_nPointEnd;
	BYTE m_nAlign2;
	BYTE m_nNoWord;
	UINT m_nNewAddress;
	//UINT m_nCodePage;
	BYTE m_nCtrlEndOnly;
	BYTE m_bNewAddress;
	BYTE m_bRelativePoint;

	afx_msg void OnPaint();
	void OnUpdateData();
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSearchText(UINT nID);
	afx_msg void OnUpdateViewCtrl(CCmdUI *pCmdUI);
	afx_msg void OnLvnItemActivateFsilist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTblMode();
	afx_msg void OnUpdateTblMode(CCmdUI *pCmdUI);
	afx_msg void OnSwpcode();
	afx_msg void OnUpdateSwpcode(CCmdUI *pCmdUI);
	afx_msg void OnScriptCopy();
	afx_msg void OnUpdateScriptCopy(CCmdUI *pCmdUI);
	afx_msg void OnScriptviewDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
	afx_msg void OnEditExport();
	afx_msg void OnUpdateEditExport(CCmdUI *pCmdUI);
	afx_msg void OnScriptviewFindpoint();
	afx_msg void OnScriptviewInvalidatedelete();
	afx_msg void OnSearchAlign2();
	afx_msg void OnUpdateSearchAlign2(CCmdUI *pCmdUI);
	afx_msg void OnSearchNoword();
	afx_msg void OnUpdateSearchNoword(CCmdUI *pCmdUI);
	afx_msg void OnEditImport();
	void OnImport(BYTE nNoDefHint, BYTE nNewCode, BYTE nTblMode, UINT nCodePage, BYTE nSaveTbl=FALSE);
	afx_msg void OnUpdateEditImport(CCmdUI *pCmdUI);
	afx_msg void OnScriptviewOpenscript();
	void OnOpenScript(CString path, UINT nCodePage);
	afx_msg void OnScriptFind();
	afx_msg void OnUpdateViewScriptFind(CCmdUI *pCmdUI);
	afx_msg void OnScriptviewFilterpoint();
	afx_msg void OnScriptviewFiltersitem();
	afx_msg void OnScriptviewFilterditem();
	afx_msg void OnScriptviewFiltermin();
	afx_msg void OnScriptviewFiltermax();
	afx_msg void OnScriptviewFiltersaddress();
	afx_msg void OnScriptviewFilterdaddress();
	afx_msg void OnScriptviewFiltercheckword();
	afx_msg void OnScriptviewFilterspecword();
	afx_msg void OnScriptviewFilteracheck();
	afx_msg void OnScriptviewCheckall();
	afx_msg void OnScriptviewFilteruncheck();
	afx_msg void OnScriptviewDelcheckitem();
	afx_msg void OnScriptviewExportcheckitem();
	afx_msg void OnEditFindtext();
	afx_msg void OnSearchCtrlendonly();
	afx_msg void OnUpdateSearchCtrlendonly(CCmdUI *pCmdUI);
	afx_msg void OnTblTotalword();
	//afx_msg void OnUpdateSearchText(CCmdUI *pCmdUI);
	virtual void ScrollTo(UINT nOffset, BOOL bForce=FALSE);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnScriptviewTblctrl();
	afx_msg void OnUpdateScriptviewTblctrl(CCmdUI *pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditExportEx();
	virtual void AppendVar();

	/*void AddMacro_OpenScript(CString path, UINT nCodePage);
	void AddMacro_Import();*/
};

#endif // CT2_CT2SCRIPTVIEW_H