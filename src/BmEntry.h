// Author:	Ryan Westphal
// Desc:	Bitmap storage class

#ifndef __BMENTRY_H__
#define __BMENTRY_H__

#include <Bitmap.h>
#include <SupportKit.h>

class BmEntry
{
public:
	BmEntry();
	BmEntry(BmEntry &other);
	~BmEntry();

public:
	const BBitmap* Bitmap(){return m_data;};
	void SetBitmap(BBitmap *bitmap);
	
	void SetTitle(BString title){m_title = title;};
	BString Title(){return m_title;};
	
	void SetIndex(int32 index){m_index = index;};
	int32 Index(){return m_index;};
	
	void PrintToStream();

protected:
	BBitmap		*m_data;
	BString		m_title;
	int32		m_index;
};

#endif