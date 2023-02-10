#pragma once


// CRelativeDlg 对话框

class CRelativeDlg : public CDialog
{
	DECLARE_DYNAMIC(CRelativeDlg)

public:
	CRelativeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRelativeDlg();

// 对话框数据
	enum { IDD = IDD_RELATIVEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	//virtual void OnCancel();
	//virtual void OnOK();
	BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()

public:
	typedef struct tagFind
	{
		int nVal1;
		int nVal2;
	}CRYSTALFIND;
	CRYSTALFIND m_Find[100];

	int m_nValType;
	int m_nBytes;
	int m_nFindCount;
	CString m_strAdd;
	CListBox m_FindList;
	CListBox m_ResultList;

	BOOL m_bSwpCheck;
	BOOL m_bOffsetCheck;
	UINT m_nStart;
	UINT m_nEnd;
	CString m_strStart;
	CString m_strEnd;

	void OnRefresh(void);
	//void OnSavePos(void);
	void OnInitOffset(void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAddbtn();
	afx_msg void OnBnClickedDelbtn();
	afx_msg void OnBnClickedClrbtn();
	afx_msg void OnBnClickedFindbtn();
	afx_msg void OnBnClickedSavebtn();
	afx_msg void OnLbnDblclkResultlist();
	afx_msg void OnBnClicked1bitradio(UINT nID);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnBnClickedOffcheck();
};
