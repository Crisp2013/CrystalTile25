#ifndef CT2_CT2CHILDFRM_H
#define CT2_CT2CHILDFRM_H

// CT2ChildFrm.h : CT2ChildFrm 类的接口
//
//#include "CT2View.h"
#include "NdsFileSystemInfo.h"
//#include ".\CTMacro.h"

#pragma once
#define CT2_ID_VIEW_FIRST ID_VIEW_TILEVIEW
#define CT2_ID_VIEW_LAST ID_VIEW_SCRIPTVIEW


class CT2ChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CT2ChildFrm)
public:
	CT2ChildFrm();

// 属性
public:

// 操作
public:

// 重写
	// virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CT2ChildFrm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	DECLARE_MESSAGE_MAP()
public:
	CT2View* m_pTileView;
	CT2View* m_pHexView;
	CT2View* m_pAsmView;
	CT2View* m_pTileEditorView;
	CT2View* m_pScriptView;
	UINT m_nCurView;
	UINT m_nOldView;
	CNdsFileSystemInfo m_NdsFSI;
	CDialog* m_pSFI;	// Sub File Info

	//CTMacro m_Macro;
	//int m_nMacroStatus;

	CT2View* GetCurView(UINT* pType=NULL);
	afx_msg void SetCurView(UINT nView);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateSetView(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	virtual void ActivateFrame(int nCmdShow = -1);
	//afx_msg void SetPalette();
	afx_msg void SetColor(UINT nID);
	afx_msg void OnToolsNdsfsi();
	afx_msg void OnUpdateToolsNdsfsi(CCmdUI *pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnViewBkBrush(UINT nID);
	afx_msg void OnUpdateViewBkBrush(CCmdUI *pCmdUI);
	afx_msg void OnSFI();
	afx_msg void OnUpdateSFI(CCmdUI *pCmdUI);
	afx_msg void OnFsiGoPC(UINT nID);
	/*afx_msg void OnToolsMacroStart();
	afx_msg void OnToolsMacroPause();
	afx_msg void OnToolsMacroCancel();
	afx_msg void OnToolsMacroSave(UINT nID);
	afx_msg void OnToolsMacroRun();
	afx_msg void OnUpdateToolsMacroStart(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsMacroPause(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsMacroCancel(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsMacroSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsMacroRun(CCmdUI *pCmdUI);*/
	void OnCreateSFI(CString& name);
	CString GetSubFileMenuText(CString& name);
	int GetFileType(LPCTSTR lpName);
};

#endif //CT2_CT2CHILDFRM_H