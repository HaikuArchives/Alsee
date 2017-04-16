// Author:	Ryan Westphal
// Desc:	Implementation of Bitmap wrapper class

#include "BmEntry.h"

BmEntry::BmEntry() : m_data(NULL), m_index(-1)
{

}

BmEntry::BmEntry(BmEntry &other)
{
	SetBitmap((BBitmap*)other.Bitmap());
	SetTitle(other.Title());
	SetIndex(other.Index());
}

BmEntry::~BmEntry()
{
	if(m_data != NULL)
		delete m_data;
}

void BmEntry::SetBitmap(BBitmap *bitmap)
{
	if(m_data != NULL)
		delete m_data;
	m_data = bitmap;
}

void BmEntry::PrintToStream()
{
	PRINT(("BmEntry:\n"));
	PRINT(("\tdata\t%x\n", m_data));
	PRINT(("\ttitle\t%s\n", m_title.String()));
	PRINT(("\tindex\t%i\n", m_index));
}