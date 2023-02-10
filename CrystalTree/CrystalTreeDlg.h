// CrystalTreeDlg.h : ͷ�ļ�
//

#pragma once
#include "CrystalTreeCtrl.h"

// CCrystalTreeDlg �Ի���
class CCrystalTreeDlg : public CDialog
{
// ����
public:
	CCrystalTreeDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CRYSTALTREE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CCrystalTreeCtrl m_Tree;

	// ���ɵ���Ϣӳ�亯��
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
