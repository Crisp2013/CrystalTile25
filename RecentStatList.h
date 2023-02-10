#ifndef CT2_RECENTSTATLIST_H
#define CT2_RECENTSTATLIST_H

#pragma once

class CRecentStatList
{
public:
	CRecentStatList(UINT nMaxMRU);
	virtual ~CRecentStatList(void);

	void ReadList(void);
	void WriteList(void);

	UINT_PTR* m_pRecentStatList;
	UINT m_nMaxMRU;
	CString m_strSectionName;
	CString m_strEntryFormat;
	void Remove(UINT nIndex);
	BOOL IsOK(UINT nIndex);
	void SetOK(UINT nIndex);
	STATEDATA* GetStateData(UINT nIndex);
};

#endif // CT2_RECENTSTATLIST_H