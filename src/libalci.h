// Author:		Ryan Westphal
// Description:	Header file for global functions usable by all alci programs

#ifndef __LIBALCI_H__
#define __LIBALCI_H__

#include <InterfaceKit.h>
#include <StorageKit.h>
#include <SupportKit.h>

#define CRYPTO_KEY	16

// App info
BPath GetLaunchPath(void);

// Retrieval
BBitmap* FetchBitmap(const char* filename);
rgb_color* GetPixel32(BBitmap *pBitmap, int x, int y);
rgb_color* GetPixel32(uint8 *pBits, int32 Bpr, int x, int y);

// Conversion	
BRect PointToRect(int x, int y, int extrude = 31);
BRect PointToRect(BPoint point);

// Comparison
bool AreEqual(rgb_color *pOne, rgb_color *pAnother, bool checkAlpha = false);

// File stuff	
status_t CopyFile(const entry_ref& source, const entry_ref& dest,
	void* buffer = NULL, size_t bufferSize = 0, bool preflight = false,
	bool createIndices = false);

// Attribute stuff
status_t SetAttrString(const BString &file, const BString &name, const BString &value);
status_t SetAttrInt32(const BString &file, const BString &name, const int32 &value);

BString GetAttrString(const BString &file, const BString &name, BString def = "");
int32 GetAttrInt32(const BString &file, const BString &name, int32 def = B_ERROR);

// Crypography
BString sn_encrypt(BString src);
BString sn_decrypt(BString src);

#endif