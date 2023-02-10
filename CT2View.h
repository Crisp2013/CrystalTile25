#ifndef CT2_CT2VIEW_H
#define CT2_CT2VIEW_H
// CT2View.h : CT2View ��Ľӿ�
//
#include "CT2Doc.h"

#pragma once

// �����������
#define CTAV_CT2VIEW	3
#define CTAV_EDITORVIEW	18

class CT2View : public CScrollView
{
protected: // �������л�����
	CT2View();
	DECLARE_DYNCREATE(CT2View)

// ����
public:
	CT2Doc* GetDocument() const;

// ����
public:

// ��д
	public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CT2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // CT2View.cpp �ĵ��԰汾
inline CT2Doc* CT2View::GetDocument() const
   { return reinterpret_cast<CT2Doc*>(m_pDocument); }
#endif

#endif // CT2_CT2VIEW_H