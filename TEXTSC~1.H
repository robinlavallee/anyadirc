// TEXTSCROLLER.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TEXTSCROLLER window

class TEXTSCROLLER : public CEdit
{
// Construction
public:
	TEXTSCROLLER();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TEXTSCROLLER)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~TEXTSCROLLER();

	// Generated message map functions
protected:
	//{{AFX_MSG(TEXTSCROLLER)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
