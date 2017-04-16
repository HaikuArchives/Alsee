// Author:	Ryan Westphal
// Desc:	Defined messages

#ifndef __AVDEFS_H__
#define __AVDEFS_H__

#define AV_MESSAGE_BASE	'AVMg'

enum {	// Menu messages
	AV_FILE_OPEN = AV_MESSAGE_BASE + 1,
	AV_NO_DIRECTION,
	AV_PREV,
	AV_NEXT,
	AV_NO_PREV, // The next 4 are for turning on/off the buttons
	AV_NO_NEXT,
	AV_YES_PREV,
	AV_YES_NEXT,
	AV_RATIO,
	AV_NOSIZE
	};

#endif