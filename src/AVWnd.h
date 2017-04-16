// Author:	Ryan Westphal
// Desc:	Main window for AlciView

#include <Window.h>
#include <MenuBar.h>
#include <FilePanel.h>
#include "AVView.h"

class AVWnd : public BWindow
{
public:
	AVWnd(BRect frame);
	virtual ~AVWnd(void);
	
	virtual void MessageReceived(BMessage *message);
	virtual bool QuitRequested(void);

public:
	void LoadImage(entry_ref ref);
	void RedrawMenu(void){m_menubar->Draw(Bounds());};
	
protected:
	void CreateMenus(void);
	void SetRes(BString which, bool on);
	bool GetRes(BString which);
	
protected:
	BMenuBar	*m_menubar;
	BMenuItem	*m_prevBtn;
	BMenuItem	*m_nextBtn;
	BMenuItem	*m_ratio;
	BMenuItem	*m_nosize; // do not change border or pos
	AVView		*m_view;
	BFilePanel	*m_panel;
};