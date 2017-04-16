// Author:	Ryan Westphal
// Desc:	Main view for AlciView; Shows the pretty pictures

#ifndef __AVVIEW_H__
#define __AVVIEW_H__

#include <View.h>
#include <Bitmap.h>
#include <StorageKit.h>
#include <String.h>
#include <Debug.h>
#include "BmEntry.h"

#define MAX_BMP	3

class AVView : public BView
{
public:
	AVView(BRect frame);
	virtual ~AVView(void);
	virtual void MessageReceived(BMessage *message);
	virtual void Draw(BRect updateRect);
	virtual void KeyDown(const char *bytes, int32 numBytes);
	virtual void MouseMoved(BPoint point, uint32 transit, const BMessage *message);
	virtual void MouseDown(BPoint point);
	virtual void MouseUp(BPoint point);
	
public:
	void NewBitmap(entry_ref ref);
	void Prev(bool key = false);
	void Next(bool key = false);
	void SetRatio(bool value){m_ratio = value;};
	void SetNosize(bool value);
	
protected:
	void Display(int direction); // move m_bitmap to the screen
	status_t SetBitmap(int32 index, int which);
	status_t SetPrevBitmap(void);
	status_t SetNextBitmap(void);
	static int32 SetPrevBitmap(void *data);
	static int32 SetNextBitmap(void *data);
	
	void PrintBms(void);
	
protected:
	// A circling array
	// m_cur is the index of the current image
	// the others are the previous and next images
	BmEntry		m_bitmaps[3];
	int			m_cur;

	bool		m_ratio; // should we maintain size ratio
	bool		m_nosize; // should we ignore frame changes
	BPoint		m_start; // where to start drawing the bitmap
	bool		m_moving; // someone's moving the image
	BPoint		m_old; // the old cursor point
	
	// Are there previous or next bitmaps to look at?
	bool		m_isPrev;
	bool		m_isNext;

	// Threads for loading the images
	// The threads are not used in this version as the code
	// was not stable.  I'll fix that in older versions
	thread_id	m_tPrev;
	thread_id	m_tNext;

	BList		m_list; // List of file entry_refs
};

#endif