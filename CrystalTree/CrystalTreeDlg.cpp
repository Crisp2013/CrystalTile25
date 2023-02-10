// CrystalTreeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTree.h"
#include ".\CrystalTreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCrystalTreeDlg 对话框



CCrystalTreeDlg::CCrystalTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCrystalTreeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCrystalTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCrystalTreeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_COMMAND_RANGE(IDC_BUTTON_LOAD, IDC_BUTTON_APPEND, OnBnClickedButtonLoad)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DWON, OnBnClickedButtonDwon)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, OnBnClickedButtonChange)
	ON_BN_CLICKED(IDC_BUTTON_ADDPARENT, OnBnClickedButtonAddparent)
END_MESSAGE_MAP()


// CCrystalTreeDlg 消息处理程序

BOOL CCrystalTreeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	RECT rc;
	GetClientRect(&rc);
	rc.bottom-=(rc.bottom-rc.top)/3;
	m_Tree.Create(WS_CHILD|WS_VISIBLE, rc, this, 1111);
	m_Tree.LoadTree();

	((CComboBox*)GetDlgItem(IDC_COMBO_CTRLTYPE))->SetCurSel(0);

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CCrystalTreeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCrystalTreeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CCrystalTreeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCrystalTreeDlg::OnBnClickedButtonAdd()
{
	CString ln, hint, data;
	GetDlgItemText(IDC_EDIT_LEFTNAME, ln);
	GetDlgItemText(IDC_EDIT_HINT, hint);
	hint.Replace(_T("\\n"), _T("\n"));
	GetDlgItemText(IDC_EDIT_DATA, data);

	if(ln.IsEmpty()) return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();

	if(hItem)
	{
		CTC_PARAM *dt = new CTC_PARAM;
		memset(dt, 0, sizeof(CTC_PARAM));

		if(!hint.IsEmpty())
		{
			int l=hint.GetLength()+1;
			dt->lpHint = new TCHAR[l];
			lstrcpyn(dt->lpHint, hint, l);
		}

		dt->nStyle|=GetDlgItem(IDC_CHECK_HEXMODE)->SendMessage(BM_GETCHECK) ? CTCS_HEXMODE : 0;
		dt->nStyle|=GetDlgItem(IDC_CHECK_READONLY)->SendMessage(BM_GETCHECK) ? CTCS_READONLY : 0;

		int ct=((CComboBox*)GetDlgItem(IDC_COMBO_CTRLTYPE))->GetCurSel();

		if(ct==CTCS_COMBOBOX || ct==CTCS_COMBOLBOX)
		{
		dt->nStyle|=GetDlgItem(IDC_CHECK_CMDRANGE)->SendMessage(BM_GETCHECK) ? CTCS_EX_CMDRANGE : 0;
		dt->nStyle|=GetDlgItem(IDC_CHECK_GETTXT)->SendMessage(BM_GETCHECK) ? CTCS_EX_GETTXT : 0;
		}

		dt->nStyle|=ct;
		if(!data.IsEmpty())
		{
			int l=data.GetLength()+1;
			dt->pData = new TCHAR[l];
			lstrcpyn(dt->pData, data, l);
		}
		CTC_PARAM *pData = (CTC_PARAM*)m_Tree.GetItemData(hItem);
		if(pData)	hItem=m_Tree.GetParentItem(hItem);
		HTREEITEM h=m_Tree.InsertItem(TVIF_TEXT|TVIF_PARAM,
			ln, 0, 0, 0, 0, (LPARAM)dt, hItem, TVI_LAST);

		m_Tree.EnsureVisible(h);
	}else
		m_Tree.InsertItem(TVIF_TEXT|TVIF_PARAM,
			ln, 0, 0, 0, 0, 0, TVI_ROOT, TVI_LAST);
}

void CCrystalTreeDlg::OnBnClickedButtonDel()
{
	HTREEITEM h=m_Tree.GetSelectedItem();
	if(h) m_Tree.DeleteItem(h);
}

HTREEITEM CCrystalTreeDlg::SaveItem(HTREEITEM hItem, CStdioFile& file)
{
	CString text;
	while(hItem)
	{
		CTC_PARAM *pData = (CTC_PARAM*)m_Tree.GetItemData(hItem);
		text=m_Tree.GetItemText(hItem);

		CTC t; memset(&t,0,sizeof(t));
		t.nLeftNameLen=text.GetLength()&0x7F;

		if(pData)
		{
			if(pData->lpHint)
			{
				t.nHintLen = lstrlen(pData->lpHint);
			}
			if(pData->pData)
			{
				t.nDataLen = lstrlen(pData->pData);
			}

			t.nStyle=(BYTE)pData->nStyle;
			t.nExStyle=pData->nStyle>>8;
		}else
			t.nLeftNameLen|=0x80;	//tree

		file.Write(&t, sizeof(t));
		file.WriteString(text);
		if(t.nHintLen)
			file.WriteString(pData->lpHint);
		if(t.nDataLen)
			file.WriteString(pData->pData);

		if(!pData)
		{
			SaveItem(m_Tree.GetChildItem(hItem), file);
			BYTE Nul=0;
			file.Write(&Nul, 1);
		}

		hItem=m_Tree.GetNextItem(hItem, 1);
	}
	return hItem;
}

void CCrystalTreeDlg::OnBnClickedButtonSave()
{
	CFileDialog fd(FALSE);
	HTREEITEM hItem=m_Tree.GetRootItem();

	if(hItem==NULL || fd.DoModal()!=IDOK) return;


	CStdioFile file;
	if(!file.Open(fd.GetPathName(), CStdioFile::modeCreate|CStdioFile::typeBinary|CStdioFile::modeWrite))
		return;

	SaveItem(hItem, file);
	BYTE Nul=0;
	file.Write(&Nul, 1);
}

void CCrystalTreeDlg::OnBnClickedButtonLoad(UINT nID)
{
	CFileDialog fd(TRUE);
	if(fd.DoModal()!=IDOK) return;

	CStdioFile file;
	if(!file.Open(fd.GetPathName(), CStdioFile::typeBinary|CStdioFile::modeRead))
		return;

	UINT s=(UINT)file.GetLength();
	CTC *c = (CTC*)new BYTE[s];
	file.Read(c, s);

	m_Tree.LoadTree(c, CTC_ACT_EXPANDALL|(nID==IDC_BUTTON_APPEND?0:CTC_ACT_REMOVEALL));

	delete[] c;
}

LRESULT CCrystalTreeDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_COMMAND:
		{
			WORD id=LOWORD(wParam);
			WORD Act=HIWORD(wParam);
			HWND h=(HWND)lParam;
			if(id==1111 && Act==(WORD)TVN_SELCHANGED && h==m_Tree.m_hWnd && !(GetKeyState(VK_CONTROL)&0x80))
			{
				HTREEITEM hItem = m_Tree.GetSelectedItem();
				if(hItem==NULL) break;
				CTC_PARAM *p = (CTC_PARAM*)m_Tree.GetItemData(hItem);
				if(p)
				{
					CString h=p->lpHint;
					h.Replace(_T("\n"), _T("\\n"));
					SetDlgItemText(IDC_EDIT_HINT, h);
					SetDlgItemText(IDC_EDIT_DATA, p->pData);

					int i=p->nStyle&CTRLTYPE_MASK;
					((CComboBox*)GetDlgItem(IDC_COMBO_CTRLTYPE))->SetCurSel(i);

					GetDlgItem(IDC_CHECK_READONLY)->
						SendMessage(BM_SETCHECK, p->nStyle&CTCS_READONLY?1:0, 0);
					GetDlgItem(IDC_CHECK_HEXMODE)->
						SendMessage(BM_SETCHECK, p->nStyle&CTCS_HEXMODE?1:0, 0);
					GetDlgItem(IDC_CHECK_GETTXT)->
						SendMessage(BM_SETCHECK, p->nStyle&CTCS_EX_GETTXT?1:0, 0);
					GetDlgItem(IDC_CHECK_CMDRANGE)->
						SendMessage(BM_SETCHECK, p->nStyle&CTCS_EX_CMDRANGE?1:0, 0);
				}
				SetDlgItemText(IDC_EDIT_LEFTNAME, m_Tree.GetItemText(hItem));
			}
		}break;
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CCrystalTreeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	RECT rc;
	::GetClientRect(m_Tree.m_hWnd, &rc);
	::SetWindowPos(m_Tree.m_hWnd, HWND_TOP, 0, 0,
		cx, rc.bottom-rc.top, SWP_NOMOVE|SWP_NOACTIVATE);
}

void CCrystalTreeDlg::OnBnClickedButtonUp()
{
	HTREEITEM hItem=m_Tree.GetSelectedItem();
	if(hItem==NULL) return;
	HTREEITEM hUP = m_Tree.GetPrevSiblingItem(hItem);
	if(hUP)
	{
		DWORD_PTR hitem = m_Tree.GetItemData(hItem);
		DWORD_PTR hup = m_Tree.GetItemData(hUP);

		m_Tree.SetItemData(hItem, 0);
		m_Tree.SetItemData(hUP, 0);

		CString t1=m_Tree.GetItemText(hItem);
		m_Tree.SetItemText(hItem, m_Tree.GetItemText(hUP));
		m_Tree.SetItemText(hUP, t1);

		m_Tree.SelectItem(hUP);

		m_Tree.SetItemData(hItem, hup);
		m_Tree.SetItemData(hUP, hitem);
		m_Tree.InitEditCtrl();
		m_Tree.MoveEditCtrl();
	}
}

void CCrystalTreeDlg::OnBnClickedButtonDwon()
{
	HTREEITEM hItem=m_Tree.GetSelectedItem();
	if(hItem==NULL) return;
	HTREEITEM hDown = m_Tree.GetNextSiblingItem(hItem);
	if(hDown)
	{
		DWORD_PTR hitem = m_Tree.GetItemData(hItem);
		DWORD_PTR hdown = m_Tree.GetItemData(hDown);

		m_Tree.SetItemData(hItem, 0);
		m_Tree.SetItemData(hDown, 0);

		CString t1=m_Tree.GetItemText(hItem);
		m_Tree.SetItemText(hItem, m_Tree.GetItemText(hDown));
		m_Tree.SetItemText(hDown, t1);

		m_Tree.SelectItem(hDown);

		m_Tree.SetItemData(hItem, hdown);
		m_Tree.SetItemData(hDown, hitem);
		m_Tree.InitEditCtrl();
		m_Tree.MoveEditCtrl();
	}
}

void CCrystalTreeDlg::OnBnClickedButtonChange()
{
	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem==NULL) return;

	CString ln, hint, data;
	GetDlgItemText(IDC_EDIT_LEFTNAME, ln);
	GetDlgItemText(IDC_EDIT_HINT, hint);
	hint.Replace(_T("\\n"), _T("\n"));
	GetDlgItemText(IDC_EDIT_DATA, data);

	if(ln.IsEmpty()) return;

	CTC_PARAM *dt = (CTC_PARAM*)m_Tree.GetItemData(hItem);

	if(dt)
	{
		if((!dt->lpHint||hint.Compare(dt->lpHint)!=0))
		{
			if(dt->lpHint) delete[] dt->lpHint;
			dt->lpHint=NULL;
			if(!hint.IsEmpty())
			{
				int l=hint.GetLength()+1;
				dt->lpHint = new TCHAR[l];
				lstrcpyn(dt->lpHint, hint, l);
			}
		}

		dt->nStyle&=DATATYPE_MASK;
		dt->nStyle|=GetDlgItem(IDC_CHECK_HEXMODE)->SendMessage(BM_GETCHECK) ? CTCS_HEXMODE : 0;
		dt->nStyle|=GetDlgItem(IDC_CHECK_READONLY)->SendMessage(BM_GETCHECK) ? CTCS_READONLY : 0;

		int ct=((CComboBox*)GetDlgItem(IDC_COMBO_CTRLTYPE))->GetCurSel();

		if(ct==CTCS_COMBOBOX || ct==CTCS_COMBOLBOX)
		{
		dt->nStyle|=GetDlgItem(IDC_CHECK_CMDRANGE)->SendMessage(BM_GETCHECK) ? CTCS_EX_CMDRANGE : 0;
		dt->nStyle|=GetDlgItem(IDC_CHECK_GETTXT)->SendMessage(BM_GETCHECK) ? CTCS_EX_GETTXT : 0;
		}
		dt->nStyle|=ct;

		if((!dt->pData||data.Compare(dt->pData)!=0))
		{
			if(dt->pData) delete[] dt->pData;
			dt->pData=NULL;
			if(!data.IsEmpty())
			{
				int l=data.GetLength()+1;
				dt->pData = new TCHAR[l];
				lstrcpyn(dt->pData, data, l);
			}
		}
		m_Tree.SelectItem(NULL);
		m_Tree.SelectItem(hItem);
	}
	m_Tree.SetItemText(hItem, ln);
}

BOOL CCrystalTreeDlg::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		{
			WORD wKey=LOWORD(pMsg->wParam);
			if(wKey==VK_RETURN || wKey==VK_ESCAPE)
				return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CCrystalTreeDlg::OnBnClickedButtonAddparent()
{
	CString ln;
	GetDlgItemText(IDC_EDIT_LEFTNAME, ln);
	if(ln.IsEmpty()) return;

	HTREEITEM hItem = m_Tree.GetSelectedItem();
	if(hItem==NULL) hItem=TVI_ROOT;
	else
	{
		hItem = m_Tree.GetParentItem(hItem);
		if(hItem==NULL) hItem=TVI_ROOT;
	}

	m_Tree.InsertItem(TVIF_TEXT|TVIF_PARAM,
		ln, 0, 0, 0, 0, 0, hItem, TVI_LAST);
}
