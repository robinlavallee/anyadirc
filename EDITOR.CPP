// EDITOR.cpp : implementation file
//

#include "stdafx.h"
#include "menu.h"
#include "EDITOR.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// EDITOR

EDITOR::EDITOR()
{
}

EDITOR::~EDITOR()
{
}


BEGIN_MESSAGE_MAP(EDITOR, CEdit)
	//{{AFX_MSG_MAP(EDITOR)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EDITOR message handlers

BOOL EDITOR::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CEdit::DestroyWindow();
}
