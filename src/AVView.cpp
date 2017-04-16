// Author:	Ryan Westphal
// Desc:	Code for the view

#include <Debug.h>
#include <GameKit.h>
#include "AVView.h"
#include "AVWnd.h"
#include "libalci.h"
#include "avdefs.h"

AVView::AVView(BRect frame) : BView(frame, "AVView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW), m_cur(0)
{
	m_moving = m_ratio = m_nosize = m_isPrev = m_isNext = false;
	SetViewColor(B_TRANSPARENT_COLOR); // Avoids flicker as we do our own drawing
	SetHighColor(255, 255, 255); // Fill in dead space white
	m_tNext = m_tPrev = B_ERROR;
}

AVView::~AVView()
{

}

void AVView::MessageReceived(BMessage *message)
{
	BMessage *temp(NULL);
	
	if(message->WasDropped())
	{
		// For when someone drops a file on us
		temp = new BMessage(message);
		temp->what = B_REFS_RECEIVED;
		be_app->PostMessage(temp);
	}
	
	switch(message->what)
	{
	case AV_NO_NEXT:
		PRINT(("no next.\n"));
		BMessenger(Window()).SendMessage(AV_NO_NEXT);
		m_isNext = false;
		break;
	}
}

void AVView::Draw(BRect updateRect)
{
	BRect bmBounds;
	BRegion fakeBg;
	
	if(m_bitmaps[m_cur].Bitmap() != NULL)
	{ // The current image is ok
		if(m_nosize)
		{
			// Regions are glorious for flicker free clearing
			// of space where the image is not
			fakeBg.Include(updateRect);
			bmBounds = m_bitmaps[m_cur].Bitmap()->Bounds();
			bmBounds.OffsetBy(m_start.x, m_start.y);
			fakeBg.Exclude(bmBounds);
			
			DrawBitmap(m_bitmaps[m_cur].Bitmap(), m_start);
			FillRegion(&fakeBg);
		}
		else
		{
			bmBounds = m_bitmaps[m_cur].Bitmap()->Bounds();
			if(bmBounds.right < Bounds().right)
			{ // The image is smaller then Next in the menubar
				// Draw the image and kill the extra space (remember,
				// we do all our own drawing here
				DrawBitmap(m_bitmaps[m_cur].Bitmap(), bmBounds, bmBounds);
				FillRect(BRect(bmBounds.right, 0, Bounds().right, Bounds().bottom));
			}
			else
				DrawBitmap(m_bitmaps[m_cur].Bitmap(), bmBounds, Bounds());
		}
	}
	else
		FillRect(updateRect); // Fills space not drawn on by us
}

void AVView::MouseMoved(BPoint point, uint32 transit, const BMessage *message)
{
	BPoint center, oldStart(m_start);
	BRect bmBounds;
	
	if(m_moving)
	{
		// The mouse button is currently down and the user
		// is dragging the image aroudn
		bmBounds = m_bitmaps[m_cur].Bitmap()->Bounds();
		
		if(point.x > m_old.x)
		{
			if(m_start.x < -7)
				m_start.x += 7;
			else if(m_start.x < 0)
				m_start.x = 0;
		}
		else if(point.x < m_old.x)
		{
			if(bmBounds.right + m_start.x > Bounds().right + 7)
				m_start.x -= 7;
			else if(bmBounds.right + m_start.x > Bounds().right)
				m_start.x = -(bmBounds.right - Bounds().right);
		}
		
		if(point.y > m_old.y)
		{
			if(m_start.y < -7)
				m_start.y += 7;
			else if(m_start.y < 0)
				m_start.y = 0;
		}
		else if(point.y < m_old.y)
		{
			if(bmBounds.bottom + m_start.y > Bounds().bottom + 7)
				m_start.y -= 7;
			else if(bmBounds.bottom + m_start.x > Bounds().bottom)
				m_start.y = -(bmBounds.bottom - Bounds().bottom);
		}
	
		if(point.x < 15 || point.x > Bounds().right - 15 ||
			point.y < 15 || point.y > Bounds().bottom - 15)
		{
			center.Set(Bounds().Width() / 2, Bounds().Height() / 2);
			m_old = center;
			center = ConvertToScreen(center);
			set_mouse_position((int32)center.x, (int32)center.y);
		}
		else
			m_old = point;

		if(m_start != oldStart)
		{
			// the start pos of the bitmap has changed
			// therefore we need to redraw
			if(LockLooper())
			{
				Draw(Bounds());
				((AVWnd*)Window())->RedrawMenu();
				UnlockLooper();
			}
		}
	}
}

void AVView::MouseDown(BPoint point)
{
	m_moving = true;
	m_old = point;
	be_app->HideCursor();
}

void AVView::MouseUp(BPoint point)
{
	m_moving = false;
	be_app->ShowCursor();
}

void AVView::NewBitmap(entry_ref ref)
{
	entry_ref buf;
	int32 cur_index(0);
	bool increment(true); // should we be incrementing the counter
	
	// Setup directory
	BPath path;
	BEntry(&ref).GetPath(&path);
	path.GetParent(&path);
	BDirectory dir(path.Path());
	
	// Kill old entries
	while(m_list.CountItems() > 0)
		delete m_list.RemoveItem((int32)0);

	// Get all the entries in the directory	
	while(dir.GetNextRef(&buf) != B_ENTRY_NOT_FOUND)
	{
		m_list.AddItem(new entry_ref(buf));
		if(buf == ref)
			increment = false;
		if(increment)
			cur_index++;
	}
	
	// Set and display the current image
	if(SetBitmap(cur_index, m_cur) == B_OK)
		Display(AV_NO_DIRECTION);
	
	// Set the next and prevous images
	if(SetNextBitmap() == B_OK)
	{ // If there is a next image, tell the window to enable the button
		BMessenger(Window()).SendMessage(AV_YES_NEXT);
		m_isNext = true;
	}
	else
		BMessenger(Window()).SendMessage(AV_NO_NEXT);
		
	if(SetPrevBitmap() == B_OK)
	{
		BMessenger(Window()).SendMessage(AV_YES_PREV);
		m_isPrev = true;
	}
	else
		BMessenger(Window()).SendMessage(AV_NO_PREV);
		
	m_start.Set(0,0);
}

void AVView::Display(int direction)
{
	BRect bitmap_rect;
	BScreen screen;
	BPath path;
	float width_ratio(1), height_ratio(1), lesser(1); // a lesser of 1 == no scaling
	
	if(m_bitmaps[m_cur].Bitmap() != NULL)
	{
		if(!m_nosize)
		{
			//
			// BEGIN LOTS OF CALCULATION
			//
			
			bitmap_rect = m_bitmaps[m_cur].Bitmap()->Bounds();
			
			// Width fix
			if(bitmap_rect.Width() >= screen.Frame().Width() - 20)
			{
				if(m_ratio)
					width_ratio = (screen.Frame().Width() - 20) / bitmap_rect.Width();
				else
					bitmap_rect.right = bitmap_rect.left + screen.Frame().Width() - 20;
			}
			else if(bitmap_rect.Width() < 125)
				bitmap_rect.right = bitmap_rect.left + 125;
			
			// Height fix
			if(bitmap_rect.Height() >= screen.Frame().Height() - 100)
			{
				if(m_ratio)
					height_ratio = (screen.Frame().Height() - 100) / bitmap_rect.Height();
				else
					bitmap_rect.bottom = bitmap_rect.top + screen.Frame().Height() - 100;
			}
			
			if(m_ratio)
			{
				if(height_ratio <= width_ratio && height_ratio > 0)
					lesser = height_ratio;
				else if(width_ratio <= height_ratio && width_ratio > 0)
					lesser = width_ratio;
					
				bitmap_rect.right *= lesser;
				bitmap_rect.bottom *= lesser;
			}
			
			//
			// END LOTS OF CALCULATION
			// BEGIN MOVING STUFF AROUND
			//
			
			Window()->ResizeTo(bitmap_rect.Width(), bitmap_rect.Height() + 20);
			Window()->MoveTo(screen.Frame().Width()/2 - bitmap_rect.Width()/2,
				screen.Frame().Height()/2 - bitmap_rect.Height()/2);
			Window()->Activate();
			
			if(direction == AV_NEXT)
				set_mouse_position(int32(Window()->Frame().left + 95), int32(Window()->Frame().top + 10));
			else if(direction == AV_PREV)
				set_mouse_position(int32(Window()->Frame().left + 55), int32(Window()->Frame().top + 10));
		}
		
		// Set the title		
		Window()->SetTitle(m_bitmaps[m_cur].Title().String());
		
		if(LockLooper())
		{
			Draw(Bounds());
			((AVWnd*)Window())->RedrawMenu();
			UnlockLooper();
		}
	}
}

status_t AVView::SetBitmap(int32 index, int which)
{
	entry_ref *ref = (entry_ref*)m_list.ItemAt(index);
	BPath path;
	BEntry(ref).GetPath(&path);
	BBitmap *bitmap = FetchBitmap(path.Path());
	BmEntry *target = &m_bitmaps[which];
	
	if(bitmap != NULL)
	{
		target->SetIndex(index);
		target->SetBitmap(bitmap);
		target->SetTitle(path.Leaf());
		return B_OK;
	}
	else
	{
		PRINT(("Not an image file.\n"));
		return B_ERROR;
	}
	return B_ERROR;
}

status_t AVView::SetPrevBitmap()
{
	for(int32 i = m_bitmaps[m_cur].Index() - 1; i >= 0; i--)
	{
		if(SetBitmap(i, (m_cur+2)%MAX_BMP) == B_OK)
			return B_OK;
	}
	return B_ERROR;
}

int32 AVView::SetPrevBitmap(void *data)
{
	// Threadable function
	// See int32 SetNextBitmap for info
	AVView *view = (AVView*)data;
	BMessenger toView(view);
	
	for(int32 i = view->m_bitmaps[view->m_cur].Index() - 1; i >= 0; i--)
	{
		if(view->SetBitmap(i, (view->m_cur+2)%MAX_BMP) == B_OK)
		{
			view->m_tPrev = B_ERROR;
			return 0;
		}
	}
	toView.SendMessage(AV_NO_PREV);
	return 0;
}
	
status_t AVView::SetNextBitmap()
{
	for(int32 i = m_bitmaps[m_cur].Index() + 1; i < m_list.CountItems(); i++)
	{
		if(SetBitmap(i, (m_cur+1)%MAX_BMP) == B_OK)
			return B_OK;
	}
	return B_ERROR;
}

int32 AVView::SetNextBitmap(void *data)
{
	// This is a threadable function so the user can move the images
	// around while the bitmaps are loading.  This is not
	// part of the current distribution
	
	AVView *view = (AVView*)data;
	BMessenger toView(view);
	
	for(int32 i = view->m_bitmaps[view->m_cur].Index() + 1; i < view->m_list.CountItems(); i++)
	{
		if(view->SetBitmap(i, (view->m_cur+1)%MAX_BMP) == B_OK)
		{
			view->m_tNext = B_ERROR;
			return 0;
		}
	}
	toView.SendMessage(AV_NO_NEXT);
	return 0;
}
	
void AVView::Prev(bool key)
{
	m_start.Set(0,0);
	
	if(m_isPrev)
	{	// we're not at the beginning
		if(m_bitmaps[m_cur].Bitmap() != NULL)
		{
			BMessenger(Window()).SendMessage(AV_YES_NEXT);
			m_isNext = true;
		}
		m_cur = (m_cur+2)%MAX_BMP;
		
		if(key)
			Display(AV_NO_DIRECTION);
		else
			Display(AV_PREV);
			
		if(SetPrevBitmap() != B_OK)
		{
			BMessenger(Window()).SendMessage(AV_NO_PREV);
			m_isPrev = false;
		}
	}
}

void AVView::Next(bool key)
{
	m_start.Set(0,0);
	
	if(m_isNext)
	{	// we're not at the end
		if(m_bitmaps[m_cur].Bitmap() != NULL)
		{
			BMessenger(Window()).SendMessage(AV_YES_PREV);
			m_isPrev = true;
		}
		m_cur = (m_cur+1)%MAX_BMP;
		
		if(key)
			Display(AV_NO_DIRECTION);
		else
			Display(AV_NEXT);

#if 0 // This is for when I add threaded bitmap loading
		if(m_tNext == B_ERROR)
		{
			m_tNext = spawn_thread(SetNextBitmap, "t>next", B_NORMAL_PRIORITY, (void*)this);
			resume_thread(m_tNext);
		}
#endif
		if(SetNextBitmap() != B_OK)
		{
			BMessenger(Window()).SendMessage(AV_NO_NEXT);
			m_isNext = false;
		}
	}
}

void AVView::KeyDown(const char *bytes, int32 numBytes)
{
	switch(bytes[0])
	{
	case B_PAGE_UP:
		Prev(true);
		break;
	case B_PAGE_DOWN:
		Next(true);
		break;
	}
}

void AVView::PrintBms()
{
	// Spit out some info
	PRINT(("current = %i\n", m_cur));
	for(int i = 0; i < MAX_BMP; i++)
	{
		PRINT(("%i ", i));
		m_bitmaps[i].PrintToStream();
	}
	PRINT(("\n"));
}

void AVView::SetNosize(bool value)
{
	// They don't want the border to resize
	// Make sure we draw white in the back
	m_nosize = value;
	if(value == true)
		SetViewColor(255,255,255,255);
	else
		SetViewColor(B_TRANSPARENT_COLOR);
}