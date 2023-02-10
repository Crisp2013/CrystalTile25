/*
使用步骤：
	1. Open("窗口类", "窗口标题（可忽略）")
	2. FindNoDS / FindDeSmuMe 返回DS内存0200:0000的PC内存地址
	3. LoadMemory

搜索关键数据: FindDataBase、Search
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

// 按窗口类打开进程
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

// 读进程指定内存中的数据
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

// 写数据到进程指定内存中
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

// 地址有效性检查
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

// 在进程整个内存中搜索关键字
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
		case 0:	// 以存在的可读写内存区
			if(m_mbi.AllocationProtect != PAGE_READWRITE) continue;
			if(m_mbi.State != MEM_COMMIT) continue;
			if(m_mbi.Type != MEM_PRIVATE) continue;
			break;
		case 1:	// 可用内存空间、申请内存时用
			{
			//if(m_mbi.AllocationProtect != PAGE_READWRITE) continue;
			if(m_mbi.State != MEM_FREE) continue;
			//if(m_mbi.Type != MEM_PRIVATE) continue;

			UINT s = (nBeginOffset-(UINT)(UINT_PTR)m_mbi.AllocationBase);
			if( ((UINT)(UINT_PTR)m_mbi.AllocationBase - s) < nKeySize ) continue;

			nBeginOffset += (UINT)(UINT_PTR)m_mbi.RegionSize;
			return s;
			}
		case 3:	// 不对内存类型做检查
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

// 基址搜索
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

// 加载进程内的内存
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

// 刷新LoadMemory时载入的数据
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

// 模拟器菜单汉化
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

// 以窗口类名查找模拟器进程、并将模拟器对应的02000000内存载入m_pMemory
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
	默认调用：更新LoadMemory时载入的整个内存区
	传递 nPos、nSize 参数时更新载入内存指定偏移的内存区
	这里的nPos是相对载入的内存，而不是进程的地址
	操作是m_nOffset + nPos
	m_nOffset才是进程地址
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
	在打开进程内申请内存，如果要给进程传递内存地址的话
	必须使用进程内的虚拟地址
	否则会发生访问内存错误
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
