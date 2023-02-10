#ifndef CT2_CT2DOC_H
#define CT2_CT2DOC_H
// CT2Doc.h :  CT2Doc ��Ľӿ�
//
#include <afxtempl.h>
#include ".\CrystalMemory.h"
#include ".\CT2CheatsSearch.h"

#pragma once

class CT2Doc : public CDocument
{
protected: // �������л�����
	CT2Doc();
	DECLARE_DYNCREATE(CT2Doc)

// ����
public:

// ����
public:

// ��д
public:
	// virtual BOOL OnNewDocument();
	// virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CT2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	BYTE* m_pRom;
	UINT m_nRomSize;
	HANDLE m_hMapFile;
	RGBQUAD *m_pPal;
	CString m_strRomInfo;
	BYTE m_nReadOnly;
	BYTE m_nOpenMapFile;
	FILETIME m_ftTime;
	//CFile m_UndoFile;
	CList<UNDO, UNDO&> m_UndoList;
	CString m_strTblName;
	UINT m_nTblCodePage;
	UINT m_nScriptCodePage;
	WORD m_nDefaultChar;
	CString m_DTBL[0x10000];
	CString m_STBL[0x10000];
	BYTE m_nEndCodeLen;	// �������Ʒ��Ƿ�˫�ֽ�
	UINT m_nEndCode;
	int m_wLastCode;
	char m_nRomType;
	BYTE m_nCurCp;

	CString m_strScriptBegin;
	CString m_strCHN;
	CString m_strScriptEnd;
	CList<VISUALSCRIPT, VISUALSCRIPT&> m_VisualScript;

	//BYTE *m_pEnd;
	UINT m_nBlockOffset;
	UINT m_nFileSize;

	//���ļ����ĵ�
	CString m_strMFileName[3];
	UINT m_nMFileSize[3];

	CCrystalMemory m_Memory;
	CT2CheatsSearch m_Cheats;

	BOOL OpenRom(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	CView* GetLastView(void);
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	void DoOpenSaveFile(CArchive *pAr, BYTE* pRom, UINT nSize, UINT nRes=0, BOOL bOpen=TRUE);
	BOOL LoadTbl(CString strTblName, BYTE nImTbl=0);
	afx_msg void OnTblSeltbl();
	afx_msg void OnCodepageAcp(UINT nID);
	afx_msg void OnUpdateCodepageAcp(CCmdUI *pCmdUI);
	virtual void DeleteContents();
	CString OffsetToPathName(UINT nOffset, POINT *pPoint=NULL);
	void Seek(UINT &nOffset);
	afx_msg void OnBnClickedFavoritesAddbtn();
	afx_msg void OnEditMemrefalsh();
	afx_msg void OnUpdateEditMemrefalsh(CCmdUI *pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnToolsCheatsserach();
	afx_msg void OnUpdateToolsCheatsserach(CCmdUI *pCmdUI);

	//void AddMacro_LoadTbl(CString path, BYTE nImTbl);
};

#endif // CT2_CT2DOC_H