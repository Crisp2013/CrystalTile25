#pragma once


// CToolBarEx

class CToolBarEx : public CToolBar
{
	DECLARE_DYNAMIC(CToolBarEx)

public:
	CToolBarEx();
	virtual ~CToolBarEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTbnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult);
};


