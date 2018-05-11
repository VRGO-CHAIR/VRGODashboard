#pragma once

///////////////////////////////////////////////////////////////////////////////
//
// Raw Input API sample showing joystick support
//
// Author: Alexander Böcken
// Date:   04/22/2011
//
// Copyright 2011 Alexander Böcken
//
///////////////////////////////////////////////////////////////////////////////

// Modifications by Samuel Pike for VRGO Ltd.


struct VRGOAxis
{
	float lAxisX = 0;
	float lAxisY = 0;
	float lAxisZ = 0;
	float lAxisRz = 0;
};

#define CHECK(exp)		{ if(!(exp)) goto Error; }
#define MAX_BUTTONS		128
#define SAFE_FREE(p)	{ if(p) { HeapFree(hHeap, 0, p); (p) = NULL; } }
class RawInput
{
private:
	RawInput();
public:
	static void ParseRawInput(PRAWINPUT pRawInput);




	static BOOL bButtonStates[MAX_BUTTONS];
	static LONG lAxisX;
	static LONG lAxisY;
	static LONG lAxisZ;
	static LONG lAxisRz;
	static LONG lHat;
	static INT  g_NumberOfButtons;

	static BOOL bSuccessfulHook;

	static VRGOAxis axisInfo;
	static VRGOAxis rawAxisInfo;

	static float Clamp(float x, float a, float b);
};

//BOOL RawInput::bButtonStates[MAX_BUTTONS];
//LONG RawInput::lAxisX = 0;
//LONG RawInput::lAxisY = 0;
//LONG RawInput::lAxisZ = 0;
//LONG RawInput::lAxisRz = 0;
//LONG RawInput::lHat = 0;
//INT  RawInput::g_NumberOfButtons = 0;
