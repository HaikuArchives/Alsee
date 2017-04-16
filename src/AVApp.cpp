// Author:	Ryan Westphal
// Desc:	Code for AVApp

#include <Debug.h>
#include <StorageKit.h>
#include "AVApp.h"

AVApp::AVApp() : BApplication("application/x-vnd.Alci-AlSee")
{
	m_wnd = new AVWnd(BRect(100, 100, 400, 300));
	m_wnd->Show();
}

void AVApp::RefsReceived(BMessage *message)
{
	entry_ref ref;
	
	if(message->FindRef("refs", &ref) == B_OK)
		m_wnd->LoadImage(ref); // Dropped or associated file
}