#ifndef CT2_CT2TEXTVIEW_H
#define CT2_CT2TEXTVIEW_H
// CT2TextView.h : CT2TextView 类的接口
//
#include "CT2View.h"

#pragma once


class CT2TextView : public CT2View
{
protected: // 仅从序列化创建
	DECLARE_DYNCREATE(CT2TextView)

// 实现
public:
	CT2TextView();
	virtual ~CT2TextView();

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnPaint();
};

#endif // CT2_CT2TEXTVIEW_H
