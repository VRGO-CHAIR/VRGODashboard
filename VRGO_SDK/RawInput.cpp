#include "stdafx.h"
#include "RawInput.h"
#include <hidsdi.h>

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


// Static Members
BOOL RawInput::bButtonStates[MAX_BUTTONS];
LONG RawInput::lAxisX = 0;
LONG RawInput::lAxisY = 0;
LONG RawInput::lAxisZ = 0;
LONG RawInput::lAxisRz = 0;
LONG RawInput::lHat = 0;
INT  RawInput::g_NumberOfButtons = 0;
VRGOAxis RawInput::axisInfo;
VRGOAxis RawInput::rawAxisInfo;
BOOL RawInput::bSuccessfulHook = false;

RawInput::RawInput()
{

}

float RawInput::Clamp(float x, float a, float b)
{

	return x < a ? a : (x > b ? b : x);
}


void RawInput::ParseRawInput(PRAWINPUT pRawInput)
{
	PHIDP_PREPARSED_DATA pPreparsedData;
	HIDP_CAPS            Caps;
	PHIDP_BUTTON_CAPS    pButtonCaps;
	PHIDP_VALUE_CAPS     pValueCaps;
	USHORT               capsLength;
	UINT                 bufferSize;
	HANDLE               hHeap;
	USAGE                usage[MAX_BUTTONS];
	ULONG                i, usageLength;
	LONG				 value;

	pPreparsedData = NULL;
	pButtonCaps = NULL;
	pValueCaps = NULL;
	hHeap = GetProcessHeap();

	//
	// Get the preparsed data block
	//

	CHECK(GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize) == 0);
	CHECK(pPreparsedData = (PHIDP_PREPARSED_DATA)HeapAlloc(hHeap, 0, bufferSize));
	CHECK((int)GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize) >= 0);

	//
	// Get the joystick's (VRGO) capabilities
	//

	// Button caps
	CHECK(HidP_GetCaps(pPreparsedData, &Caps) == HIDP_STATUS_SUCCESS)
		CHECK(pButtonCaps = (PHIDP_BUTTON_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_BUTTON_CAPS) * Caps.NumberInputButtonCaps));

	capsLength = Caps.NumberInputButtonCaps;
	CHECK(HidP_GetButtonCaps(HidP_Input, pButtonCaps, &capsLength, pPreparsedData) == HIDP_STATUS_SUCCESS)
		g_NumberOfButtons = pButtonCaps->Range.UsageMax - pButtonCaps->Range.UsageMin + 1;

	// Value caps
	CHECK(pValueCaps = (PHIDP_VALUE_CAPS)HeapAlloc(hHeap, 0, sizeof(HIDP_VALUE_CAPS) * Caps.NumberInputValueCaps));
	capsLength = Caps.NumberInputValueCaps;
	CHECK(HidP_GetValueCaps(HidP_Input, pValueCaps, &capsLength, pPreparsedData) == HIDP_STATUS_SUCCESS)

		//
		// Get the pressed buttons
		//

		usageLength = g_NumberOfButtons;
	CHECK(
		HidP_GetUsages(
			HidP_Input, pButtonCaps->UsagePage, 0, usage, &usageLength, pPreparsedData,
			(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
		) == HIDP_STATUS_SUCCESS);

	ZeroMemory(bButtonStates, sizeof(bButtonStates));
	for (i = 0; i < usageLength; i++)
		bButtonStates[usage[i] - pButtonCaps->Range.UsageMin] = TRUE;

	//
	// Get the state of discrete-valued-controls
	//

	for (i = 0; i < Caps.NumberInputValueCaps; i++)
	{
		/*CHECK(
			HidP_GetUsageValue(
				HidP_Input, pValueCaps[i].UsagePage, 0, pValueCaps[i].Range.UsageMin, &value, pPreparsedData,
				(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid
			) == HIDP_STATUS_SUCCESS);*/

		CHECK(HidP_GetScaledUsageValue(HidP_Input, pValueCaps[i].UsagePage, 0, pValueCaps[i].Range.UsageMin, &value, pPreparsedData,
			(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid) == HIDP_STATUS_SUCCESS);

		switch (pValueCaps[i].Range.UsageMin)
		{
		case 0x30:	// X-axis
			lAxisX = (LONG)value;// -128;
			break;

		case 0x31:	// Y-axis
			lAxisY = (LONG)value;// -128;
			break;

		case 0x32: // Z-axis
			lAxisZ = (LONG)value;// -128;
			break;

		case 0x35: // Rotate-Z
			lAxisRz = (LONG)value;// -128;
			break;

		case 0x39:	// Hat Switch
			lHat = value;
			break;
		}
	}

	// convert the inputs into a -1 to 1 range.
	// NOTE: this requires them to be converted into floats as they are LONG (int)
	axisInfo.lAxisX = Clamp((float)lAxisX / 120.0f, -1.f, 1.f);
	axisInfo.lAxisY = -Clamp((float)lAxisY / 120.0f, -1.f, 1.f);
	axisInfo.lAxisRz = Clamp((float)lAxisRz / 120.0f, -1.f, 1.f);
	axisInfo.lAxisZ = Clamp((float)lAxisZ / 120.0f, -1.f, 1.f);

	rawAxisInfo.lAxisX = Clamp((float)lAxisX / 120.0f, -1.f, 1.f);
	rawAxisInfo.lAxisY = -Clamp((float)lAxisY / 120.0f, -1.f, 1.f);
	rawAxisInfo.lAxisRz = Clamp((float)lAxisRz / 120.0f, -1.f, 1.f);
	rawAxisInfo.lAxisZ = Clamp((float)lAxisZ / 120.0f, -1.f, 1.f);


/*
	axisInfo.lAxisX = lAxisX;
	axisInfo.lAxisY = lAxisY;
	axisInfo.lAxisZ = lAxisZ;
	axisInfo.lAxisRz = lAxisRz;*/



	//
	// Clean up
	//

Error:
	SAFE_FREE(pPreparsedData);
	SAFE_FREE(pButtonCaps);
	SAFE_FREE(pValueCaps);
}