#ifndef CT2_CT2TEXTVIEW_H
#define CT2_CT2TEXTVIEW_H
// CT2TextView.h : CT2TextView ��Ľӿ�
//
#include "CT2View.h"

#pragma once


class CT2TextView : public CT2View
{
protected: // �������л�����
	DECLARE_DYNCREATE(CT2TextView)

// ʵ��
public:
	CT2TextView();
	virtual ~CT2TextView();

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnPaint();
};

#endif // CT2_CT2TEXTVIEW_H
