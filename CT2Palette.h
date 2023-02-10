#ifndef CT2_CT2PALETTE_H
#define CT2_CT2PALETTE_H

#include "CT2NormalDlg.h"

#pragma once

// CT2Palette 对话框

class CT2Palette : public CT2NormalDlg
{
	DECLARE_DYNAMIC(CT2Palette)

public:
	CT2Palette(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CT2Palette();

// 对话框数据
	enum { IDD = IDD_PALETTEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	//virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
public:
	BOOL LoadPal(RGBQUAD* pPal, UINT nType=-1);
	virtual BOOL OnInitDialog();
	//BOOL m_bRet;
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	int m_nSetColor;
	void SetColor(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedDefpalbtn();
	afx_msg void OnBnClickedLoadpalbtn();
	void OnLoadPal(CString path);
	BOOL IBmpPal(CFile& file);
	BOOL IPalPal(CFile& file);
	BOOL IActPal(CFile& file);
	BOOL INclrPal(CFile& file);
	afx_msg void OnBnClickedSavepalbtn();
	afx_msg void OnLoadDefPal(UINT nID);
	afx_msg void OnSetColor(UINT nIDC);

	/*void AddMacro_LoadPal(CString path);
	void AddMacro_LoadDefPal(UINT nType);*/
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

#endif // CT2_CT2PALETTE_H