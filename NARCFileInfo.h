#ifndef _NARCFILEINFO_H
#define _NARCFILEINFO_H

#pragma once

#include ".\SubFileInfo.h"

// CNARCFileInfo 对话框

class CNARCFileInfo : public CSubFileInfo
{
	DECLARE_DYNAMIC(CNARCFileInfo)

public:
	CNARCFileInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNARCFileInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	virtual void Update();
	virtual BOOL OnInitDialog();
	virtual void SetHeader();
	afx_msg void OnLvnGetdispinfoSfiFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivateSfiFilelist(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif	// _NARCFILEINFO_H