#ifndef _CCRYSTALMEMORY_H_
#define _CCRYSTALMEMORY_H_

#pragma once

class CCrystalMemory
{
public:
	CCrystalMemory(void);
	virtual ~CCrystalMemory(void);

	HWND m_hWnd;
	DWORD m_dwProcessId;
	HANDLE m_hProcess;
	SYSTEM_INFO m_si;
	MEMORY_BASIC_INFORMATION m_mbi;
	BYTE *m_pMemory;
	UINT m_nSize;
	UINT m_nOffset;

	void Init();

	BOOL Open(LPCTSTR lpszClass, LPCTSTR lpszWindowName=NULL);
	void Close();

	BOOL CheckOffset(UINT nOffset);

	BOOL Read(UINT nOffset, LPVOID lpBuffer, UINT nSize, UINT *lpOutSize=NULL);
	BOOL Write(UINT nOffset, LPVOID lpBuffer, UINT nSize, UINT *lpOutSize=NULL);

	UINT VirtualQuery(UINT nOffset, PMEMORY_BASIC_INFORMATION lpMemBaseInfo);

	UINT FindDataBase(UINT nBeginOffset, BYTE *pKeyData, UINT nKeySize, UINT nMinSize);

	UINT Search(UINT &nBeginOffset, BYTE *pKeyData, UINT nKeySize, BYTE nFlags=0);

	BOOL LoadMemory(UINT nOffset, UINT nSize);
	BOOL ReLoadMemory(UINT nPos=0, UINT nSize=0);
	void FreeMemory();

	void ModifyDsMenu(HMENU hMenu, LPSTR lpRes);
	void Menu2CHS();
	BOOL FindDSEmu(LPCTSTR lpClass);
	BOOL FindDsRom();

	void SuspendThread();
	void ResumeThread();

	BOOL Update(UINT nPos=0, UINT nSize=0);

	//UINT SearchCheats(UINT nPos, BYTE *pData, UINT nSize);

	UINT VirtualAlloc(UINT nSize);
	BOOL VirtualFree(UINT nOffset);
};

#endif // _CCRYSTALMEMORY_H_