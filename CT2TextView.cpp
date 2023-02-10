// CT2TextView.cpp : CT2TextView 类的实现
//

#include "stdafx.h"
#include "CrystalTile2.h"

#include "CT2Doc.h"
#include ".\CT2TextView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CT2TextView

IMPLEMENT_DYNCREATE(CT2TextView, CT2View)

BEGIN_MESSAGE_MAP(CT2TextView, CT2View)
	WM_PAINT()
END_MESSAGE_MAP()

// CT2TextView 构造/析构

CT2TextView::CT2TextView()
{
}

CT2TextView::~CT2TextView()
{
}

// CT2TextView 消息处理程序

void CT2TextView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc; GetClientRect(&rc);

	CBitmap memBM;
	memBM.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	memDC.SelectObject(&memBM);


	FillRect(memDC.GetSafeHdc(), &rc,
		((CT2TileView*)theApp.GetView(ID_VIEW_TILEVIEW))->m_hBkBrush);

	dc.BitBlt(0, 0, rc.Width(), rc.Height(),
		&memDC, 0, 0, SRCCOPY);

	memBM.DeleteObject();	memDC.DeleteDC();
}
