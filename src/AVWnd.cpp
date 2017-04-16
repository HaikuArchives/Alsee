// Author:	Ryan Westphal
// Desc:	Code for AVWnd class

#include <Debug.h>
#include <Application.h>
#include <MenuItem.h>
#include <FilePanel.h>
#include <Alert.h>
#include "AVWnd.h"
#include "avdefs.h"
#include "libalci.h"

AVWnd::AVWnd(BRect frame) : BWindow(frame, "AVWnd", B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	SetTitle("AlciView");
	CreateMenus();
	
	// Make the view start lower than the menubar
	// for easy drawing later
	BRect view_bounds = Bounds();
	view_bounds.top = 20;
	m_view = new AVView(view_bounds);
	AddChild(m_view);

	// here set m_ratio
	if(GetRes("ASPECT") == true)
	{
		m_ratio->SetMarked(true);
		m_view->SetRatio(true);
	}
	
	if(GetRes("NOSIZE") == true)
	{
		m_nosize->SetMarked(true);
		m_view->SetNosize(true);
		SetFlags(0);
	}
	
	m_panel = new BFilePanel();
}

AVWnd::~AVWnd()
{
	delete m_panel;
}

void AVWnd::MessageReceived(BMessage *message)
{
	
	switch(message->what)
	{
	case B_KEY_DOWN:
		DispatchMessage(message, m_view);
		break;
	case AV_FILE_OPEN:
		m_panel->Show();
		break;
	
	// Previous/Next button was clicked
	case AV_PREV:
		m_view->Prev();
		break;
	case AV_NEXT:
		m_view->Next();
		break;
	
	// There is no previous/next image
	// Figured out when there is a problem loading
	case AV_NO_PREV:
		m_prevBtn->SetEnabled(false);
		break;
	case AV_NO_NEXT:
		m_nextBtn->SetEnabled(false);
		break;
	
	// There is a previous/next image
	// but the buttons are disabled
	case AV_YES_PREV:
		m_prevBtn->SetEnabled(true);
		break;
	case AV_YES_NEXT:
		m_nextBtn->SetEnabled(true);
		break;
	
	// Maintian ratio was clicked
	case AV_RATIO:
		m_ratio->SetMarked(!m_ratio->IsMarked());
		m_view->SetRatio(m_ratio->IsMarked());
		SetRes("ASPECT", m_ratio->IsMarked());
		break;
	case AV_NOSIZE:
		m_nosize->SetMarked(!m_nosize->IsMarked());
		m_view->SetNosize(m_nosize->IsMarked());
		SetRes("NOSIZE", m_nosize->IsMarked());
		if(m_nosize->IsMarked())
			SetFlags(0);
		else
			SetFlags(B_NOT_ZOOMABLE | B_NOT_RESIZABLE);
		break;
	default:
		BWindow::MessageReceived(message);
		break;
	}
}

bool AVWnd::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void AVWnd::CreateMenus()
{
	BMenuItem *item(NULL);
	BMenu *menu = new BMenu("File");
	m_menubar = new BMenuBar(BRect(0,0,0,0), "AlciViewMenu");
	
	// File menu
	item = new BMenuItem("Open...", new BMessage(AV_FILE_OPEN));
	menu->AddItem(item);
	
	m_nosize = new BMenuItem("No Resize", new BMessage(AV_NOSIZE));
	menu->AddItem(m_nosize);
	
	m_ratio = new BMenuItem("Maintain Ratio", new BMessage(AV_RATIO));
	menu->AddItem(m_ratio);
	
	item = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED));
	menu->AddItem(item);
	m_menubar->AddItem(menu);
	
	// Temp prev/next button
	m_prevBtn = new BMenuItem("Prev", new BMessage(AV_PREV));
	m_prevBtn->SetEnabled(false);
	m_menubar->AddItem(m_prevBtn);
	m_nextBtn = new BMenuItem("Next", new BMessage(AV_NEXT));
	m_nextBtn->SetEnabled(false);
	m_menubar->AddItem(m_nextBtn);
	
	AddChild(m_menubar);
}

void AVWnd::LoadImage(entry_ref ref)
{
	// Only called when a file was opened
	// Next/Prev do not get here
	m_view->NewBitmap(ref);
}

bool AVWnd::GetRes(BString which)
{
	// return whether they left maintain aspect checked
	BPath settingsFile;
	find_directory(B_USER_SETTINGS_DIRECTORY, &settingsFile);
	settingsFile.Append("AlSee");
	
	int aspect = GetAttrInt32(settingsFile.Path(), which.String());

	if(aspect == B_ERROR)
	{
		SetAttrInt32(settingsFile.Path(), which.String(), 0);
		return false;
	}
	else
		return aspect;
}

void AVWnd::SetRes(BString which, bool on)
{
	// Save the change to maintain check to a settings file
	BPath settingsFile;
	find_directory(B_USER_SETTINGS_DIRECTORY, &settingsFile);
	settingsFile.Append("AlSee");
	
	BFile(settingsFile.Path(), B_READ_ONLY | B_CREATE_FILE).Unset();
	SetAttrInt32(settingsFile.Path(), which.String(), on);
}