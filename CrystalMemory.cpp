/*
ʹ�ò��裺
	1. Open("������", "���ڱ��⣨�ɺ��ԣ�")
	2. FindNoDS / FindDeSmuMe ����DS�ڴ�0200:0000��PC�ڴ��ַ
	3. LoadMemory

�����ؼ�����: FindDataBase��Search
*/
#include "StdAfx.h"
#include ".\CrystalMemory.h"
#include ".\Common.h"
#include ".\resource.h"

CCrystalMemory::CCrystalMemory(void)
{
	Init();
}

void CCrystalMemory::Init()
{
	m_hWnd = NULL;
	m_hProcess = NULL;
	m_pMemory = NULL;
	m_nSize = 0;
	m_nOffset = 0;
}

CCrystalMemory::~CCrystalMemory(void)
{
	Close();
}

// ��������򿪽���
BOOL CCrystalMemory::Open(LPCTSTR lpszClass, LPCTSTR lpszWindowName)
{
	Close();

	m_hWnd = FindWindow(lpszClass, lpszWindowName);

	if(m_hWnd)
	{
		GetWindowThreadProcessId(m_hWnd, &m_dwProcessId);

		m_hProcess = OpenProcess(PROCESS_ALL_ACCESS|PROCESS_SUSPEND_RESUME, FALSE, m_dwProcessId);

		GetSystemInfo(&m_si);
	}

	return m_hWnd!=NULL && m_dwProcessId && m_hProcess!=NULL;
}

void CCrystalMemory::Close()
{
	if(m_hProcess)
	{
		CloseHandle(m_hProcess);
		FreeMemory();
		Init();
	}
}

// ������ָ���ڴ��е�����
BOOL CCrystalMemory::Read(UINT nOffset, LPVOID lpBuffer, UINT nSize, UINT *lpOutSize)
{
	if(!CheckOffset(nOffset)) return 0;

	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery(nOffset, &mbi);

	if(mbi.AllocationProtect != PAGE_READWRITE) return 0;
	if(mbi.State != MEM_COMMIT) return 0;
	if(mbi.Type != MEM_PRIVATE) return 0;

	SIZE_T nOutSize;
	BOOL bRet = ReadProcessMemory(m_hProcess, (LPCVOID)(UINT_PTR)nOffset,
			lpBuffer, nSize, &nOutSize);

	if(lpOutSize)
		*lpOutSize = (UINT)nOutSize;
	return bRet;
}

// д���ݵ�����ָ���ڴ���
BOOL CCrystalMemory::Write(UINT nOffset, LPVOID lpBuffer, UINT nSize, UINT *lpOutSize)
{
	if(!CheckOffset(nOffset)) return 0;

	SIZE_T nOutSize;
	BOOL bRet = WriteProcessMemory(m_hProcess, (LPVOID)(UINT_PTR)nOffset,
			lpBuffer, nSize, &nOutSize);

	if(lpOutSize)
		*lpOutSize = (UINT)nOutSize;
	return bRet;
}

// ��ַ��Ч�Լ��
BOOL CCrystalMemory::CheckOffset(UINT nOffset)
{
	if( !m_hProcess ||
		nOffset<(UINT)(UINT_PTR)m_si.lpMinimumApplicationAddress ||
		nOffset>(UINT)(UINT_PTR)m_si.lpMaximumApplicationAddress )
		return FALSE;

	return TRUE;
}

UINT CCrystalMemory::VirtualQuery(UINT nOffset, PMEMORY_BASIC_INFORMATION lpMemBaseInfo)
{
	if(!CheckOffset(nOffset)) return 0;

	return (UINT)VirtualQueryEx(m_hProcess, (LPCVOID)(UINT_PTR)nOffset, lpMemBaseInfo, sizeof(MEMORY_BASIC_INFORMATION));
}

// �ڽ��������ڴ��������ؼ���
UINT CCrystalMemory::Search(UINT &nBeginOffset, BYTE *pKeyData, UINT nKeySize, BYTE nFlags)
{
	// ASSERT((m_hProcess==FALSE));

	// UINT nKeyOff = 0;

	// FreeMemory();

	for(;
		nBeginOffset<(UINT)(UINT_PTR)m_si.lpMaximumApplicationAddress;
		nBeginOffset+=(UINT)m_mbi.RegionSize)
	{
		VirtualQuery(nBeginOffset, &m_mbi);

		switch(nFlags)
		{
		default:
		case 0:	// �Դ��ڵĿɶ�д�ڴ���
			if(m_mbi.AllocationProtect != PAGE_READWRITE) continue;
			if(m_mbi.State != MEM_COMMIT) continue;
			if(m_mbi.Type != MEM_PRIVATE) continue;
			break;
		case 1:	// �����ڴ�ռ䡢�����ڴ�ʱ��
			{
			//if(m_mbi.AllocationProtect != PAGE_READWRITE) continue;
			if(m_mbi.State != MEM_FREE) continue;
			//if(m_mbi.Type != MEM_PRIVATE) continue;

			UINT s = (nBeginOffset-(UINT)(UINT_PTR)m_mbi.AllocationBase);
			if( ((UINT)(UINT_PTR)m_mbi.AllocationBase - s) < nKeySize ) continue;

			nBeginOffset += (UINT)(UINT_PTR)m_mbi.RegionSize;
			return s;
			}
		case 3:	// �����ڴ����������
			break;
		}

		if(!LoadMemory((UINT)(UINT_PTR)m_mbi.AllocationBase, (UINT)m_mbi.RegionSize)) continue;

		for(UINT nKeyOff=nBeginOffset-(UINT)(UINT_PTR)m_mbi.AllocationBase; nKeyOff <= (m_mbi.RegionSize-nKeySize); nKeyOff++)
		{
			if(memcmp(m_pMemory+nKeyOff, pKeyData, nKeySize)==0)
			{
				nBeginOffset+=(UINT)m_mbi.RegionSize;
				// FreeMemory();
                return (UINT)(UINT_PTR)m_mbi.AllocationBase+nKeyOff;
			}
		}

		// break;
	}
	// FreeMemory();
	nBeginOffset+=(UINT)m_mbi.RegionSize;
	return 0;
}

// ��ַ����
UINT CCrystalMemory::FindDataBase(UINT nBeginOffset, BYTE *pKeyData, UINT nKeySize, UINT nMinSize)
{
	if( nBeginOffset<(UINT)(UINT_PTR)m_si.lpMinimumApplicationAddress ||
		(nBeginOffset+nKeySize)>(UINT)(UINT_PTR)m_si.lpMaximumApplicationAddress )
		nBeginOffset = (UINT)(UINT_PTR)m_si.lpMinimumApplicationAddress;

	UINT nOff;
	g_bBreak=FALSE;
	while(nBeginOffset<(UINT)(UINT_PTR)m_si.lpMaximumApplicationAddress && !g_bBreak)
	{
		if(nOff=Search(nBeginOffset, pKeyData, nKeySize))
		{
			UINT s = (UINT)m_mbi.RegionSize-(nOff-(UINT)(UINT_PTR)m_mbi.AllocationBase);
			if(s < nMinSize)
				continue;
			// LoadMemory(nOff, s);
			FreeMemory();
			return nOff;
		}
	}
	FreeMemory();
	return 0;
}

// ���ؽ����ڵ��ڴ�
BOOL CCrystalMemory::LoadMemory(UINT nOffset, UINT nSize)
{
	if(!CheckOffset(nOffset)) return FALSE;

	UINT nOutSize=0;
	if(!m_pMemory || m_nSize!=nSize)
	{
		delete[] m_pMemory;
		m_nSize = nSize;
        m_pMemory = new BYTE[nSize];
	}
	if( !Read((UINT)(UINT_PTR)nOffset, m_pMemory, nSize, &nOutSize) ||
		nOutSize!=nSize)
	{
		// FreeMemory();
		return FALSE;
	}
	m_nOffset = nOffset;
	return TRUE;
}

// ˢ��LoadMemoryʱ���������
BOOL CCrystalMemory::ReLoadMemory(UINT nPos, UINT nSize)
{
	if(nSize==0)
		return LoadMemory(m_nOffset, m_nSize);
	else
		return LoadMemory(m_nOffset+nPos, m_nSize-nPos);
}

void CCrystalMemory::FreeMemory()
{
	if(m_pMemory)
	{
		delete[] m_pMemory;
		m_pMemory = NULL;
		m_nOffset = 0;
		m_nSize = 0;
	}
}

void CCrystalMemory::ModifyDsMenu(HMENU hMenu, LPSTR lpRes)
{
	int c = GetMenuItemCount(hMenu);
	for(int i=0; i<c; i++)
	{
		HMENU hSub = GetSubMenu(hMenu, i);
		if(hSub) ModifyDsMenu(hSub, lpRes);

		GetMenuStringA(hMenu, i, (char*)m_pMemory, m_si.dwPageSize, MF_BYPOSITION);
		if(!*m_pMemory) continue;

		LPSTR p = lpRes;
		while(*p)
		{
			if( strcmp((char*)m_pMemory, p)== 0 )
			{
				strcpy((char*)m_pMemory, p+strlen(p)+1);
				ModifyMenuA(hMenu, i, MF_BYPOSITION, GetMenuItemID(hMenu, i), (char*)m_pMemory);
				break;
			}
			p += strlen(p)+1;
			p += strlen(p)+1;
		}
	}
}

// ģ�����˵�����
void CCrystalMemory::Menu2CHS()
{
	if(Hint(IDS_DSMENU2CHS, MB_ICONQUESTION|MB_YESNO)==IDNO) return;
	UINT nRes = VirtualAlloc(m_si.dwPageSize);
	if(nRes)
	{
		if(m_hWnd && LoadMemory(nRes, m_si.dwPageSize))
		{
			HMENU hMenu = GetMenu(m_hWnd);
			DWORD dwSize;
			HANDLE hRes = LoadRes(IDR_DSEMU_MENURES, dwSize);
			if(hMenu && hRes)
			{
				LPSTR lpRes = (LPSTR)GlobalLock(hRes);
				ModifyDsMenu(hMenu, lpRes);
				GlobalUnlock(hRes);
				GlobalFree(hRes);
			}
		}
		VirtualFree(nRes);
	}
}

// �Դ�����������ģ�������̡�����ģ������Ӧ��02000000�ڴ�����m_pMemory
BOOL CCrystalMemory::FindDSEmu(LPCTSTR lpClass)
{
	//TCHAR Class[] = _T("No$dlgClass");
	if(!Open(lpClass)) return FALSE;
	UINT Arm9[] = {0xE7FFDEFF, 0xE7FFDEFF, 0xE7FFDEFF};
	UINT nOffset = FindDataBase(0x00400000, (BYTE*)Arm9, sizeof(Arm9), 0x00400000);
	if(!nOffset) return FALSE;
	//
	Menu2CHS();
	//
	return LoadMemory(nOffset, 0x00400000);
}

void CCrystalMemory::SuspendThread()
{
	::SuspendThread(m_hProcess);
}

void CCrystalMemory::ResumeThread()
{
	::ResumeThread(m_hProcess);
}

/*
	Ĭ�ϵ��ã�����LoadMemoryʱ����������ڴ���
	���� nPos��nSize ����ʱ���������ڴ�ָ��ƫ�Ƶ��ڴ���
	�����nPos�����������ڴ棬�����ǽ��̵ĵ�ַ
	������m_nOffset + nPos
	m_nOffset���ǽ��̵�ַ
*/
BOOL CCrystalMemory::Update(UINT nPos, UINT nSize)
{
	if(nSize==0)
	{
		BOOL bRet = Write(m_nOffset, m_pMemory, m_nSize);
		ReLoadMemory();
		return bRet;
	}else
	{
		if(nPos>=(m_nOffset+m_nSize))
			return FALSE;
		if(nPos+nSize > m_nSize)
			nSize = m_nSize-nPos;
		BOOL bRet = Write(m_nOffset+nPos, m_pMemory+nPos, nSize);
		ReLoadMemory();
		return bRet;
	}
}

BOOL CCrystalMemory::FindDsRom()
{
	DWORD dwSize;
	HANDLE logo = LoadRes(IDR_NINTENDOLOGO, dwSize);
	BOOL bret = FALSE;
	if(logo)
	{
		BYTE *pLogo = (BYTE*)GlobalLock(logo);

		if(m_nOffset = FindDataBase(0x00400000, pLogo, dwSize, sizeof(NDSHEADER)))
		{
			m_nOffset -= 0xC0;
			bret = LoadMemory(m_nOffset, sizeof(NDSHEADER));
		}

		GlobalUnlock(logo);
		GlobalFree(logo);
	}
	return bret;
}

/*
	�ڴ򿪽����������ڴ棬���Ҫ�����̴����ڴ��ַ�Ļ�
	����ʹ�ý����ڵ������ַ
	����ᷢ�������ڴ����
*/
UINT CCrystalMemory::VirtualAlloc(UINT nSize)
{
	return (UINT)(UINT_PTR)
		::VirtualAllocEx(m_hProcess, NULL, nSize, MEM_COMMIT, PAGE_READWRITE);
}

BOOL CCrystalMemory::VirtualFree(UINT nOffset)
{
	return ::VirtualFreeEx(m_hProcess, (LPVOID)(UINT_PTR)nOffset, 0, MEM_RELEASE);
}
