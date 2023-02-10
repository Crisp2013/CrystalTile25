#ifndef _CTMACRO_H
#define _CTMACRO_H

#ifdef CRYSTALMACRO

#include "Common.h"

#pragma once

class CTMacro
{
public:
	CTMacro(void);
	virtual ~CTMacro(void);

	CT_MACRO_HEADER m_Header;
	CPtrArray m_Macro;

	BOOL IsEmpty();
	void Clear();
	BOOL Save(LPCTSTR lpszPathName, BOOL bSaveAllFile=FALSE);

	int GetCount();
	CT_MACRO* GetMacro(int iIndex);
	void SetMacro(int iIndex, CT_MACRO *pMacro);
	int AddMacro(void *pMacro);
	void DeleteMacro(int iIndex);
};

#endif

#endif	// _CTMACRO_H