#ifndef _PACFILEINFO_H
#define _PACFILEINFO_H

#pragma once

#include ".\SubFileInfo.h"

// CPacFileInfo �Ի���
#define PAC_MAXREC	0x3F

class CPacFileInfo : public CSubFileInfo
{
	DECLARE_DYNAMIC(CPacFileInfo)

public:
	CPacFileInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPacFileInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	virtual void Update();
	virtual BOOL OnInitDialog();
	int GetFileCount();
	afx_msg void OnSfiImport();
};

#endif	// _PACFILEINFO_H