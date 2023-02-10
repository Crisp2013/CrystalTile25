#ifndef _UTILITYFILEINFO_H
#define _UTILITYFILEINFO_H

#pragma once
#include "SubFileInfo.h"

// CUtilityFileInfo 对话框

class CUtilityFileInfo : public CSubFileInfo
{
	DECLARE_DYNAMIC(CUtilityFileInfo)

public:
	CUtilityFileInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUtilityFileInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	virtual void Update();
	virtual void SetHeader();
	afx_msg void OnLvnGetdispinfoSfiFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivateSfiFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSfiExport();
	afx_msg void OnSfiImport();
};

#endif	// _UTILITYFILEINFO_H