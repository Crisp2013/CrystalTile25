#ifndef _CT2FAVORITE_H
#define _CT2FAVORITE_H

#pragma once

#include "CT2NormalDlg.h"

// CT2Favorite 对话框

class CT2Favorite : public CT2NormalDlg
{
	DECLARE_DYNAMIC(CT2Favorite)

public:
	CT2Favorite(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CT2Favorite();

// 对话框数据
	enum { IDD = IDD_FAVORITESBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemActivateFavoritesList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditFavoritesList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedFavoritesDelbtn();
	void Goto(int nItem);
};

#endif // _CT2FAVORITE_H