#include "StdAfx.h"
#include "CrystalTile2.h"

#include ".\recentstatlist.h"

CRecentStatList::CRecentStatList(UINT nMaxMRU)
: m_strSectionName(_T("Recent File List"))
, m_strEntryFormat(_T("Stat%d"))
{
	m_nMaxMRU = nMaxMRU;
}

CRecentStatList::~CRecentStatList(void)
{
	if(m_pRecentStatList)
	{
		for(UINT i=0; i<m_nMaxMRU; i++)
		{
			if(m_pRecentStatList[i])
			{
				delete (UINT_PTR*)m_pRecentStatList[i];
				m_pRecentStatList[i]=NULL;
			}
		}
		delete[] m_pRecentStatList;
		m_pRecentStatList=NULL;
	}
}

void CRecentStatList::ReadList(void)
{
	m_pRecentStatList = new UINT_PTR[m_nMaxMRU];
	memset(m_pRecentStatList, 0, sizeof(UINT_PTR)*m_nMaxMRU);

	CString strEntry;
	BYTE* pStatus;
	UINT nSize;
	CT2App* pApp = (CT2App*)AfxGetApp();
	for(UINT iMRU=0; iMRU<m_nMaxMRU; iMRU++)
	{
		strEntry.Format(m_strEntryFormat, iMRU+1);
		if(pApp->
			GetProfileBinary(m_strSectionName, strEntry, &pStatus, &nSize))
		{
			m_pRecentStatList[iMRU] = (UINT_PTR)pStatus;
		}
	}
}

void CRecentStatList::WriteList(void)
{
	LPTSTR pszEntry = new TCHAR[m_strEntryFormat.GetLength()+10];
	CWinApp* pApp = AfxGetApp();
	// pApp->WriteProfileString(m_strSectionName, NULL, NULL);
	for(UINT iMRU = 0; iMRU < m_nMaxMRU; iMRU++)
	{
		wsprintf(pszEntry, m_strEntryFormat, iMRU + 1);
		//if(m_pRecentStatList[iMRU])
		if(IsOK(iMRU))
		{
			pApp->WriteProfileBinary(m_strSectionName, pszEntry,
                (BYTE*)m_pRecentStatList[iMRU], sizeof(STATEDATA));
		}
	}
	delete[] pszEntry;
}

void CRecentStatList::Remove(UINT nIndex)
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < m_nMaxMRU);

	if(m_pRecentStatList[nIndex])
	{
		delete[] (UINT_PTR*)m_pRecentStatList[nIndex];
		m_pRecentStatList[nIndex] = 0;
	}
	UINT iMRU;
	for (iMRU = nIndex; iMRU < m_nMaxMRU-1; iMRU++)
		m_pRecentStatList[iMRU] = m_pRecentStatList[iMRU+1];

	ASSERT(iMRU < m_nMaxMRU);

	if(m_pRecentStatList[iMRU])
	{
		delete[] (UINT_PTR*)m_pRecentStatList[iMRU];
		m_pRecentStatList[iMRU] = 0;
	}
}

BOOL CRecentStatList::IsOK(UINT nIndex)
{
//to prevent that program not work on 64bit
//it maybe a little buggy?
#ifdef _WIN64
	STATEDATA* pSD;
	if (nIndex != -1)
	{
		pSD = (STATEDATA*)m_pRecentStatList[nIndex];
		return pSD && (pSD->nSize == sizeof(STATEDATA));
	}
	else
		return 0;
#else
	STATEDATA* pSD = (STATEDATA*)m_pRecentStatList[nIndex];
	return (nIndex != -1) && pSD && (pSD->nSize == sizeof(STATEDATA));
#endif
}

void CRecentStatList::SetOK(UINT nIndex)
{
	if(!(STATEDATA*)m_pRecentStatList[nIndex])
		m_pRecentStatList[nIndex] = (UINT_PTR)new STATEDATA;
	STATEDATA* pSd = (STATEDATA*)m_pRecentStatList[nIndex];
	pSd->nSize=sizeof(STATEDATA);
}

STATEDATA* CRecentStatList::GetStateData(UINT nIndex)
{
	return nIndex!=-1?
		(STATEDATA*)m_pRecentStatList[nIndex]:NULL;
}
