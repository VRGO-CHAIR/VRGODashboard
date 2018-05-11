#pragma once

//#include "stdafx.h"

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif


#include "resource.h"


// VRGOApp:
// See .cpp for the implementation of this class
//

class CVRGOConfigApp : public CWinApp
{
public:
	CVRGOConfigApp();

	// Overrides
public:
	virtual BOOL InitInstance();

	// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CVRGOConfigApp theApp;