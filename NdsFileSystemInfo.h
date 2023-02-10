#ifndef _NDSFILESYSTEMINFO_H
#define _NDSFILESYSTEMINFO_H

#pragma once
#include "afxcmn.h"
#include "CT2View.h"

// CNdsFileSystemInfo 对话框
#define OVERLAY_FMT		_T("FSI.CT/overlay%d_%04d.bin")
#define GetNDSSize(header) ( 1 << (17 + header.devicecap) )
#define HEADERCOUNT 8
#define GETFILEID(Param) (LOWORD(Param))
#define GETITEMID(Param) (HIWORD(Param))

class CNdsFileSystemInfo : public CDialog
{
	DECLARE_DYNAMIC(CNdsFileSystemInfo)

public:
	CNdsFileSystemInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNdsFileSystemInfo();

// 对话框数据
	enum { IDD = IDD_NDSFSI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_NdsFSI;
	NDSHEADER *m_pHeader;
	UINT m_nRomFitSize;
	UINT m_nRomSize;
	BYTE *m_pRom;
	UINT m_nOverlayFiles9;
	UINT m_nOverlayFiles7;
	UINT m_nOverlayFileSize;
	char m_FileName[0x80];
	CFile m_fNds;
	BOOL m_bReadOnly;
	CT2View *m_pTileView;
	NDSSPECREC m_SpecRec[HEADERCOUNT];
	CString m_strFind;

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void OnUpdateNds();
	void OnLoadFSI(void);
	// void ExtractDirectory(CString& ParentDir, UINT nID=0);
	afx_msg void OnLvnGetdispinfoFsilist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivateFsilist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnFsiGo();
	afx_msg void OnUpdateFsiGo(CCmdUI *pCmdUI);
	afx_msg void OnFsiRefalsh();
	afx_msg void OnFsiExport();
	afx_msg void OnFsiImport();
	void OnImport(CString path, UINT nOff, UINT nSize, WORD wFileID);
	afx_msg void OnUpdateFsiImport(CCmdUI *pCmdUI);
	afx_msg void OnFsiSplitrom();
	void CopyFromBin(CString& Path, UINT& nSize);
	afx_msg void OnFsiCreaterom();
	afx_msg void OnUpdateFsiCreaterom(CCmdUI *pCmdUI);
	afx_msg void OnFsiCheckcrc16(UINT nID);
	afx_msg void OnFsiGameinfo();
	afx_msg void OnUpdateFsiGameinfo(CCmdUI *pCmdUI);
	afx_msg void OnFsiGoFile();
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	static void LoadPal(WORD *pWPal, RGBQUAD *pPal, WORD nCount=256);
	afx_msg void OnFsiLoaddata();
	afx_msg void OnUpdateFsiLoaddata(CCmdUI *pCmdUI);
	afx_msg void OnFsiSetmap();
	afx_msg void OnUpdateFsiSetmap(CCmdUI *pCmdUI);
	UINT GetFilePos(CString& FileName, UINT nID=0);
	BOOL CheckSelItemExt(CString ext);
	afx_msg void OnSFI();
	afx_msg void OnUpdateSFI(CCmdUI *pCmdUI);
	afx_msg void OnFsiFind();
	afx_msg void OnFsiCopytoclipboard();
	afx_msg void OnUpdateFsiCopytoclipboard(CCmdUI *pCmdUI);
	afx_msg void OnFsiNext();
	afx_msg void OnLvnKeydownFsilist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFsiGohan();
	UINT ImportFile(CFile *File, UINT nOff, UINT nSize, WORD wFileID, UINT nNewSize=0, UINT nNewOffset=-1);
	afx_msg void OnFsiSetfilesize(UINT nID);
	void OnSetfilesize(UINT nNewSize, WORD wSub, WORD wFileID, UINT nNewOffset=-1);
	afx_msg void OnUpdateFsiSetfilesize(CCmdUI *pCmdUI);
	CString GetSubHeader(UINT nOffset, int iItem=-1);
	afx_msg void OnFsiGotoheader(UINT nID);
	afx_msg void OnUpdateFsiGotoheader(CCmdUI *pCmdUI);
	afx_msg void OnFsiExportex();
	void ExportFile(CString key);
	afx_msg void OnUpdateFsipopNtfr(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFsipopNtfrSetNftrWH(CCmdUI *pCmdUI);
	afx_msg void OnFsipopLoadntfrwh();
	void OnSetntfrwh(BYTE nWidth, BYTE nHeight, WORD wFileID);
	afx_msg void OnFsipopSetntfrwh();
	UINT GetSubHeaderOffset(int nItem, CString Header);
	afx_msg void OnFsipopExporttbl();
	int GetCurSel();
	int FileIDToIndex(WORD wFileID);

	/*void AddMacro_LoadPal(UINT nOffset);
	void AddMacro_Setntfrwh(BYTE nWidth, BYTE nHeight, WORD wFileID);
	void AddMacro_Setfilesize(UINT nNewSize, WORD wSub, WORD wFileID, UINT nNewOffset);
	void AddMacro_Import(CString path, UINT nOff, UINT nSize, WORD nFileID, UINT nNewOffset);
	void AddMacro_Checkcrc16();
	void AddMacro_NdsInfo();*/
	afx_msg void OnLvnEndScrollFsilist(NMHDR *pNMHDR, LRESULT *pResult);
	UINT AlignFile(UINT nAlignSize, BYTE* pData);
	afx_msg void OnFsipopShowicon();
	afx_msg void OnFsipopUpdateiconpal();
	afx_msg void OnFsiFix();
};

#endif	// _NDSFILESYSTEMINFO_H