// PacFileInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "CrystalTile2.h"
#include ".\PacFileInfo.h"

// CPacFileInfo 对话框

IMPLEMENT_DYNAMIC(CPacFileInfo, CSubFileInfo)
CPacFileInfo::CPacFileInfo(CWnd* pParent /*=NULL*/)
	: CSubFileInfo(pParent)
{
}

CPacFileInfo::~CPacFileInfo()
{
}

void CPacFileInfo::DoDataExchange(CDataExchange* pDX)
{
	CSubFileInfo::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPacFileInfo, CSubFileInfo)
	ON_COMMAND(ID_SFI_IMPORT, OnSfiImport)
END_MESSAGE_MAP()


// CPacFileInfo 消息处理程序

void CPacFileInfo::Update()
{
	m_FileList.DeleteAllItems();

	BYTE *pFile = (((CT2ChildFrm*)m_pChild)->m_pTileView->GetDocument()->m_pRom+m_nOffset);
	BYTE *pEnd = pFile + m_nSize;

	int nFileCount=GetFileCount();
	int item=0, nFNTCount=0;

	PACFILE_HEADER *h = (PACFILE_HEADER*)pFile;

	CString text;
	PACFILE_FNT *fnt = (PACFILE_FNT*)(pFile+sizeof(PACFILE_HEADER));

	UINT nBase = h->nSize?h->nSize:fnt->nFNTCount*PAC_FILEALIGN;

	PACFILE_FAT *fat = (PACFILE_FAT*)(pFile+nBase);
	PACFILE_FAT_REC *rec = (PACFILE_FAT_REC*)((BYTE*)fat+sizeof(PACFILE_FAT));

	if(!h->nSize)
		nBase += (nFileCount+PAC_MAXREC) / PAC_MAXREC * PAC_FILEALIGN;
	else
		nBase = (nBase + 0x1FF) / PAC_FILEALIGN * PAC_FILEALIGN;

	WORD nRecCount=fat->wFileCount>PAC_MAXREC?PAC_MAXREC:fat->wFileCount;;

	while( item<nFileCount )
	{
		WORD nSize=0;
		while(nSize<PAC_FILEALIGN && item<nFileCount)
		{
			if(*fnt->FileName && fnt->nFileNameLen)
			{
				MultiByteToWideChar(CP_SHIFTJIS, 0, fnt->FileName, fnt->nFileNameLen,
					g_szBuffer, _MAX_PATH);
				g_szBuffer[fnt->nFileNameLen]=0;

				int iImage=GetSFIIconIndex(g_szBuffer);

				if(!nRecCount)
				{
					fat = (PACFILE_FAT*)((BYTE*)fat + PAC_FILEALIGN);
					rec = (PACFILE_FAT_REC*)((BYTE*)fat+sizeof(PACFILE_FAT));
					nRecCount=fat->wFileCount>PAC_MAXREC?PAC_MAXREC:fat->wFileCount;
				}
				nRecCount--;

				char sub=1;
				char i=item%PAC_MAXREC;

				m_FileList.InsertItem(LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM, item, g_szBuffer,
					0, 0, iImage, (LPARAM)&rec[i]);
				// off1
				text.Format(_T("%08X"), rec[i].nOffset+nBase+m_nOffset);
				m_FileList.SetItemText(item, sub++, text);
				// off2
				if(m_nOffset)
				{
					text.Format(_T("%08X"), rec[i].nOffset+nBase);
					m_FileList.SetItemText(item, sub++, text);
				}
				// size
				text.Format(_T("%d"), rec[i].nSize);
				m_FileList.SetItemText(item, sub, text);

				item++;
			}else break;

			nSize += fnt->nFileNameLen+2;
			fnt = (PACFILE_FNT*)((BYTE*)fnt + fnt->nFileNameLen + 2);
		}
		fnt = (PACFILE_FNT*)(pFile+sizeof(PACFILE_HEADER)+(++nFNTCount)*PAC_FILEALIGN);
	}

	CString t;
	t.Format(m_strWTitle, m_strTitle, item, nFileCount);
	SetWindowText(t);
}

void CPacFileInfo::OnOK()
{
	GoToFile();
}

BOOL CPacFileInfo::OnInitDialog()
{
	CSubFileInfo::OnInitDialog();

	// SetHeader();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

int CPacFileInfo::GetFileCount()
{
	BYTE *pFile = (((CT2ChildFrm*)m_pChild)->m_pTileView->GetDocument()->m_pRom+m_nOffset);
	BYTE *pEnd = pFile + m_nSize;

	int item=0, nFNTCount=0;

	PACFILE_HEADER *h = (PACFILE_HEADER*)pFile;
	PACFILE_FNT *fnt = (PACFILE_FNT*)(pFile+sizeof(PACFILE_HEADER));

	if(h->nSize)
	{
		//pEnd = pFile+h->nSize;
		UINT nBase = h->nSize?h->nSize:fnt->nFNTCount*PAC_FILEALIGN;
		PACFILE_FAT *fat = (PACFILE_FAT*)(pFile+nBase);
		return fat->wFileCount;
	}

	while( ((nFNTCount<fnt->nFNTCount && !h->nSize)||(h->nSize)) &&
		((BYTE*)fnt+sizeof(PACFILE_FNT))<pEnd && fnt->nFileNameLen && *fnt->FileName)
	{
		WORD nSize=0;
		while(nSize<PAC_FILEALIGN)
		{
			if(*fnt->FileName && fnt->nFileNameLen)
				item++;
			else break;

			nSize += fnt->nFileNameLen+2;
			fnt = (PACFILE_FNT*)((BYTE*)fnt + fnt->nFileNameLen + 2);
		}
		fnt = (PACFILE_FNT*)(pFile+sizeof(PACFILE_HEADER)+(++nFNTCount)*PAC_FILEALIGN);
	}
	return item;
}

void CPacFileInfo::OnSfiImport()
{
	int nSel = GetCurSel();
	m_FileList.GetItemText(nSel, 0, g_szBuffer, _MAX_PATH);
	lstrcpy(g_szBuffer, PathFindFileName(g_szBuffer));

	static CFileDialog fd(TRUE);
	fd.m_pOFN->lpstrFile = g_szBuffer;
	fd.m_pOFN->nMaxFile = _MAX_PATH;
	if(fd.DoModal()!=IDOK) return;

	CT2View *pView = ((CT2ChildFrm*)m_pChild)->m_pTileView;
	CT2Doc *pDoc = pView->GetDocument();
	BYTE *pFile = pDoc->m_pRom+m_nOffset;

	CFile imfile;
	if(!imfile.Open(g_szBuffer, CFile::modeRead|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	PACFILE_FAT_REC *rec = (PACFILE_FAT_REC*)m_FileList.GetItemData(nSel);

	UINT nSize = (UINT)imfile.GetLength();

	PACFILE_HEADER *h = (PACFILE_HEADER*)pFile;
	PACFILE_FNT *fnt = (PACFILE_FNT*)(pFile+sizeof(PACFILE_HEADER));
	UINT nBase = h->nSize?h->nSize:fnt->nFNTCount*PAC_FILEALIGN;

	if(!h->nSize)
		nBase += (GetFileCount()+PAC_MAXREC) / PAC_MAXREC * PAC_FILEALIGN;
	else
		nBase = (nBase + 0x1FF) / PAC_FILEALIGN * PAC_FILEALIGN;

	UINT nOffset = rec->nOffset+nBase;
	if(m_nOffset || nSize==rec->nSize)
	{
		if(nSize>rec->nSize)
		{
			Hint(IDS_SFI_IMPORTERR);
			return;
		}else
		{
			pView->DoUndo(pFile+nOffset, nOffset+m_nOffset, rec->nSize);
			pView->DoUndo((BYTE*)&rec->nSize, (UINT)((BYTE*)&rec->nSize-pFile)+m_nOffset, 4);
			imfile.Read(pFile+nOffset, nSize);
			UINT zlen = rec->nSize-nSize;
			if(zlen)
				memset(pFile+nOffset+nSize, 0, zlen);
			rec->nSize = nSize;
			return;
		}
	}

	CString name=pDoc->GetPathName();
	CFile file;
	if(!file.Open(name, CFile::modeReadWrite|CFile::typeBinary))
	{
		ShowError();
		return;
	}

	int iPos = nSize-rec->nSize;
	char zlen=4-(nSize&3);
	if(zlen!=4)
		iPos+=zlen;
	for(int i=nSel+1; i<m_FileList.GetItemCount(); i++)
	{
		PACFILE_FAT_REC *rec2 = (PACFILE_FAT_REC*)m_FileList.GetItemData(i);
		rec2->nOffset += iPos;
	}

	UINT nOff = nOffset+rec->nSize;

	rec->nSize = nSize;
	file.Write(pFile, nOffset);
	BYTE *pBuffer = new BYTE[nSize];
	imfile.Read(pBuffer, nSize);
	file.Write(pBuffer, nSize);
	delete[] pBuffer;

	TCHAR zero[3]={0};
	if(zlen!=4)
		file.Write(zero, zlen);
	file.Write(pFile+nOff, m_nSize-nOff);

	pDoc->DeleteContents();
	file.SetLength(file.GetPosition());
	file.Close();

	pDoc->OpenRom(name);

	Update();
	m_FileList.EnsureVisible(nSel, TRUE);
}
