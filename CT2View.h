#ifndef CT2_CT2VIEW_H
#define CT2_CT2VIEW_H
// CT2View.h : CT2View 类的接口
//
#include "CT2Doc.h"

#pragma once

// 关联变量起点
#define CTAV_CT2VIEW	3
#define CTAV_EDITORVIEW	18

class CT2View : public CScrollView
{
protected: // 仅从序列化创建
	CT2View();
	DECLARE_DYNCREATE(CT2View)

// 属性
public:
	CT2Doc* GetDocument() const;

// 操作
public:

// 重写
	public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CT2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

public:
	UINT m_nOffset;
	POINT m_ptCursorPos;
	RGBQUAD* m_pPaintPanel;
	BITMAPINFOHEADER m_biPaintPanel;
	BYTE m_bUndo;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnUpdateData(void);
	afx_msg void OnDestroy();
	afx_msg void OnEditGo();
	afx_msg void OnUpdateOffset(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateViewOffset(CCmdUI *pCmdUI);
	void DoUndo(BYTE *pData, UINT nOffset, UINT nSize);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void ScrollTo(UINT nOffset, BOOL bForce=FALSE);
	void SetCurView(UINT nView);
	void Invalidate(BOOL bErase = TRUE);
	afx_msg void OnUpdateScriptViewBegin(CCmdUI *pCmdUI);
	afx_msg void OnEditReset();
	afx_msg void OnUpdateEditReset(CCmdUI *pCmdUI);
	UINT GetViewType();
	virtual void AppendVar();
	//int AddMacro(void *pMacro);
};

#ifndef _DEBUG  // CT2View.cpp 的调试版本
inline CT2Doc* CT2View::GetDocument() const
   { return reinterpret_cast<CT2Doc*>(m_pDocument); }
#endif

#endif // CT2_CT2VIEW_H