#include "StdAfx.h"
#include ".\CTMacro.h"

#ifdef CRYSTALMACRO

CTMacro::CTMacro(void)
{
	memset(&m_Header, 0, sizeof(m_Header));
	//StrCpyNA(m_Header.Flags, "CTM ", 5);
	*(UINT*)m_Header.Flags = 0x204D5443;
	m_Header.dwSize = sizeof(m_Header);
	m_Header.dwVer = CT_MACRO_VER;
	m_Header.nType = CT_MACRO_TYPE_NORMAL;
}

CTMacro::~CTMacro(void)
{
	Clear();
}

BOOL CTMacro::IsEmpty()
{
	return m_Macro.IsEmpty();
}

void CTMacro::Clear()
{
	for(int i=0; i<m_Macro.GetCount(); i++)
		delete[] m_Macro.GetAt(i);
	m_Macro.RemoveAll();
}

BOOL CTMacro::Save(LPCTSTR lpszPathName, BOOL bSaveAllFile)
{
	CFile f;
	if(!f.Open(lpszPathName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
	{
		ShowError();
		return FALSE;
	}

	f.Write(&m_Header, sizeof(m_Header));

	for(int i=0; i<m_Macro.GetCount(); i++)
	{
		CT_MACRO *pMacro = GetMacro(i);
		if(bSaveAllFile)
		{
			CString name, path = lpszPathName;
			int i = path.ReverseFind(_T('\\'));
			path = path.Left(i+1);
			LPTSTR szFile = NULL;
			switch(pMacro->wActive)
			{
			case CT2DOC_LOADTBL:
			case CT2HEXVIEW_IMPORTDATA:
			case CT2HEXVIEW_IMPORTTEXT:
			case CT2PALETTE_LOADPAL:
			case CT2SCRIPTVIEW_OPENSCRIPT:
			case CT2TILEVIEW_EDITIMPORT:
				{
					CT_MACRO_1VAL_1FILE *m = (CT_MACRO_1VAL_1FILE*)pMacro;
					szFile = m->szFile;
				}break;

			case CT2TILEEDITOR_TBL:
				{
					CT_MACRO_ONTBL *m = (CT_MACRO_ONTBL*)pMacro;
					szFile = m->szFile;
				}break;

			case CT2TILEEDITOR_SETTILE:
				{
					CT_MACRO_ONTBL *m = (CT_MACRO_ONTBL*)pMacro;
					szFile = m->szFile;
				}break;
			}
			if(szFile)
			{
				name = PathFindFileName(szFile);
				CopyFile(szFile, path+name, FALSE);
				lstrcpyn(szFile, name, _MAX_PATH);
			}
		}
		f.Write(pMacro, pMacro->wSize);
	}
	// Clear();

	return TRUE;
}

int CTMacro::GetCount()
{
	return (int)m_Macro.GetCount();
}

CT_MACRO* CTMacro::GetMacro(int iIndex)
{
	return (CT_MACRO*) m_Macro.GetAt(iIndex);
}

void CTMacro::SetMacro(int iIndex, CT_MACRO *pMacro)
{
	delete[] m_Macro.GetAt(iIndex);
	m_Macro.SetAt(iIndex, pMacro);
}

int CTMacro::AddMacro(void *pMacro)
{
	CT_MACRO *pMacro2 = (CT_MACRO*) pMacro;
	BYTE *pM = new BYTE[pMacro2->wSize];
	memcpy(pM, pMacro2, pMacro2->wSize);
	return (int)m_Macro.Add(pM);
}

void CTMacro::DeleteMacro(int iIndex)
{
	delete[] m_Macro.GetAt(iIndex);
	m_Macro.RemoveAt(iIndex);
}

#endif