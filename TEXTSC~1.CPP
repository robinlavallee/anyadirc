// TEXTSCROLLER.cpp : implementation file
//

#include "stdafx.h"
#include "menu.h"
#include "TEXTSCROLLER.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TEXTSCROLLER

TEXTSCROLLER::TEXTSCROLLER()
{
}

TEXTSCROLLER::~TEXTSCROLLER()
{
}


BEGIN_MESSAGE_MAP(TEXTSCROLLER, CEdit)
	//{{AFX_MSG_MAP(TEXTSCROLLER)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TEXTSCROLLER message handlers

BOOL TEXTSCROLLER::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CEdit::DestroyWindow();
}

BOOL TEXTSCROLLER::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
}

LRESULT TEXTSCROLLER::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CEdit::DefWindowProc(message, wParam, lParam);
}

void TEXTSCROLLER::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CEdit::CalcWindowRect(lpClientRect, nAdjustType);
}
