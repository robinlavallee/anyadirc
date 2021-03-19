// EDITOR.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EDITOR window

class EDITOR : public CEdit
{
// Construction
public:
	EDITOR();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EDITOR)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~EDITOR();

	// Generated message map functions
protected:
	//{{AFX_MSG(EDITOR)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
