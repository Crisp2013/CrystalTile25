// CrystalTree.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CCrystalTreeApp:
// �йش����ʵ�֣������ CrystalTree.cpp
//

class CCrystalTreeApp : public CWinApp
{
public:
	CCrystalTreeApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCrystalTreeApp theApp;
