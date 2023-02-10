#ifndef _SDATINFO_H
#define _SDATINFO_H

#pragma once
#include "afxcmn.h"

#include "SubFileInfo.h"
#include "CrystalPlayer.h"

// CSDatInfo 对话框

class CSDatInfo : public CSubFileInfo
{
	DECLARE_DYNAMIC(CSDatInfo)

public:
	CSDatInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSDatInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CCrystalPlayer m_Player;

	virtual BOOL OnInitDialog();
	void Update();
	afx_msg void OnPlayerChannels(UINT nID);
	afx_msg void OnUpdatePlayerChannels(CCmdUI *pCmdUI);
	afx_msg void OnPlayer4_16bit(UINT nID);
	afx_msg void OnUpdatePlayer4_16bit(CCmdUI *pCmdUI);
	afx_msg void OnPlayerNkhz(UINT nID);
	afx_msg void OnUpdatePlayerNkhz(CCmdUI *pCmdUI);
	afx_msg void OnPlayerPlay();
	afx_msg void OnPlayerStop();
	virtual void SetHeader();
};

#endif	// _SDATINFO_H