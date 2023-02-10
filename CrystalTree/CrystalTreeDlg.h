// CrystalTreeDlg.h : 头文件
//

#pragma once
#include "CrystalTreeCtrl.h"

// CCrystalTreeDlg 对话框
class CCrystalTreeDlg : public CDialog
{
// 构造
public:
	CCrystalTreeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CRYSTALTREE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CCrystalTreeCtrl m_Tree;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	HTREEITEM SaveItem(HTREEITEM hItem, CStdioFile& file);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad(UINT nID);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDwon();
	afx_msg void OnBnClickedButtonChange();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonAddparent();
	afx_msg void OnBnClickedButtonAppend();
};
