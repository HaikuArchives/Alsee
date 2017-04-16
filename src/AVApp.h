// Author:	Ryan Westphal
// Desc:	The AlciView application class

#include <Application.h>
#include "AVWnd.h"

class AVApp : public BApplication
{
public:
	AVApp(void);
	
	virtual void RefsReceived(BMessage *message);
protected:
	AVWnd	*m_wnd;
};