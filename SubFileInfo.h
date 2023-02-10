#ifndef _SUBFILEINFO_H
#define _SUBFILEINFO_H

#include "CT2ChildFrm.h"

#pragma once


// CSubFileInfo 对话框

class CSubFileInfo : public CDialog
{
	DECLARE_DYNAMIC(CSubFileInfo)

public:
	CSubFileInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSubFileInfo();

// 对话框数据
	enum { IDD = IDD_SFI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CString m_strWTitle;
	CMenu	m_Menu;
	CListCtrl m_FileList;
	CString		m_strTitle;
	CT2ChildFrm*m_pChild;
	UINT		m_nOffset;
	UINT		m_nSize;
	char		m_nSubOffset;

	virtual void OnInit(CString strTitle, CT2ChildFrm* pChild, UINT nOffset, UINT nSize);
	virtual void Update()=0;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	void GoToFile(int iItem=-1, char cOffset=1);
	afx_msg void OnLvnItemActivateSdatlist(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void SetHeader();
	int GetCurSel();
	afx_msg void OnSfiExport();
	void OnExport(char cPos=0);
	afx_msg void OnUpdateSfiExport(CCmdUI *pCmdUI);
	afx_msg void OnSfiImport();
	void OnImport(char cPos=0);
	afx_msg void OnUpdateSfiImport(CCmdUI *pCmdUI);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnSfiReflash();
	afx_msg void OnLvnEndScrollFsilist(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL CheckSelItemExt(CString ext);
	afx_msg void OnFsiLoaddata();
	afx_msg void OnUpdateFsiLoaddata(CCmdUI *pCmdUI);
};

#endif	// _SUBFILEINFO_H