/********************************************************************************
MIT License

Copyright(c) 3dRudder 2017

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
********************************************************************************/

// Modifications made by Samuel Pike for VRGO ltd.
// Copyright(c) VRGO ltd 2018


#include "stdafx.h"
#include "ServerDriverHost5.h"
#include "..\Common\MathTools.h"
#include "..\Common\SharedData.h"
#include "ServerDriver.h"
#include "DriverContext.h"
#include "DriverInput.h"
#define _USE_MATH_DEFINES
#include <math.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVRServerDriverHost5
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CVRServerDriverHost5::TrackedDeviceAdded(const char *pchDeviceSerialNumber, ETrackedDeviceClass eDeviceClass, ITrackedDeviceServerDriver *pDriver)
{
	char *sType[] = { "Invalid","HMD","Controller","GenericTracker","TrackingReference" };

	bool bRet = false;

	// add pTrackedDeviceServerDriver !!
	CTrackedDeviceServerDriver *pPout;

	DriverServerMap::iterator it = m_MapDeviceServerDriver.find(pDriver);
	if (it == m_MapDeviceServerDriver.end())
	{
		pPout = new CTrackedDeviceServerDriver(pDriver, eDeviceClass, m_pDriverInstance);
		m_MapDeviceServerDriver.insert(std::pair <ITrackedDeviceServerDriver *, CTrackedDeviceServerDriver*>(pDriver, pPout));
	}
	else
		pPout = (it->second);

	if (m_pServerDriverHost)
		bRet = m_pServerDriverHost->TrackedDeviceAdded(pchDeviceSerialNumber, eDeviceClass, pPout);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CVRServerDriverHost5::TrackedDeviceAdded(%s:%s,%p)=%d", pchDeviceSerialNumber, sType[eDeviceClass], pDriver, bRet);

	return bRet;
}


/** Notifies the server that a tracked device's pose has been updated */
void CVRServerDriverHost5::TrackedDevicePoseUpdated(uint32_t unWhichDevice, const DriverPose_t & newPose, uint32_t unPoseStructSize)
{
	DriverPose_t pos = newPose;

	HandleTrackedDevicePoseUpdated(unWhichDevice, pos, unPoseStructSize);

	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDevicePoseUpdated(unWhichDevice, pos, unPoseStructSize);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDevicePoseUpdated(%d)", unWhichDevice);
}

/** Notifies the server that vsync has occurred on the the display attached to the device. This is
* only permitted on devices of the HMD class. */
void CVRServerDriverHost5::VsyncEvent(double vsyncTimeOffsetSeconds)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->VsyncEvent(vsyncTimeOffsetSeconds);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(3, "CVRServerDriverHost5::VsyncEvent(%d)", vsyncTimeOffsetSeconds);
}
#if 0
/** notifies the server that the button was pressed */
void CVRServerDriverHost5::TrackedDeviceButtonPressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDeviceButtonPressed(unWhichDevice, eButtonId, eventTimeOffset);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceButtonPressed(%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset);
}

/** notifies the server that the button was unpressed */
void CVRServerDriverHost5::TrackedDeviceButtonUnpressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDeviceButtonUnpressed(unWhichDevice, eButtonId, eventTimeOffset);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceButtonUnpressed(%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset);
}

/** notifies the server that the button was pressed */
void CVRServerDriverHost5::TrackedDeviceButtonTouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset)
{
	if (eButtonId == EVRButtonId::k_EButton_SteamVR_Touchpad)
	{
		CSISharedData *pSharedData = m_pDriverInstance->m_pSharedData;
		if (pSharedData->m_nPadIndex[0] == unWhichDevice)  // Left controller
			m_bTrackpadTouched[0] = true;
		if (pSharedData->m_nPadIndex[1] == unWhichDevice) // Right controller
			m_bTrackpadTouched[1] = true;
	}
	else if (eButtonId & 0x8000)
		eButtonId = EVRButtonId::k_EButton_SteamVR_Touchpad;

	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDeviceButtonTouched(unWhichDevice, eButtonId, eventTimeOffset);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_PURPLE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceButtonTouched(%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset);
}

/** notifies the server that the button was unpressed */
void CVRServerDriverHost5::TrackedDeviceButtonUntouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset)
{
	if (eButtonId == EVRButtonId::k_EButton_SteamVR_Touchpad)
	{
		CSISharedData *pSharedData = m_pDriverInstance->m_pSharedData;
		if (pSharedData->m_nPadIndex[0] == unWhichDevice)  // Left controller
			m_bTrackpadTouched[0] = false;
		if (pSharedData->m_nPadIndex[1] == unWhichDevice) // Right controller
			m_bTrackpadTouched[1] = false;
	}
	else if (eButtonId & 0x8000)
		eButtonId = EVRButtonId::k_EButton_SteamVR_Touchpad;

	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDeviceButtonUntouched(unWhichDevice, eButtonId, eventTimeOffset);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_PURPLE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceButtonUntouched(%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset);
}

/** notifies the server than a controller axis changed */
void CVRServerDriverHost5::TrackedDeviceAxisUpdated(uint32_t unWhichDevice, uint32_t unWhichAxis, const VRControllerAxis_t & axisState)
{
	/// PATCH !!!!! 

	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDeviceAxisUpdated(unWhichDevice, unWhichAxis, axisState);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_GRAY_BLUE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceAxisUpdated(%d,%d,(%f,%f))", unWhichDevice, unWhichAxis, axisState.x, axisState.y);

}

/** Notifies the server that the proximity sensor on the specified device  */
void CVRServerDriverHost5::ProximitySensorState(uint32_t unWhichDevice, bool bProximitySensorTriggered)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->ProximitySensorState(unWhichDevice, bProximitySensorTriggered);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::ProximitySensorState(%d,%d)", unWhichDevice, bProximitySensorTriggered);
}
#endif 

void CVRServerDriverHost5::SendTrackedDeviceButtonPressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	TrackedDeviceButtonPressed(unWhichDevice, eButtonId, eventTimeOffset, bInputCall);
}
void CVRServerDriverHost5::SendTrackedDeviceButtonUnpressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	TrackedDeviceButtonUnpressed(unWhichDevice, eButtonId, eventTimeOffset, bInputCall);
}
void CVRServerDriverHost5::SendTrackedDeviceButtonTouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	TrackedDeviceButtonTouched(unWhichDevice, eButtonId, eventTimeOffset, bInputCall);
}
void CVRServerDriverHost5::SendTrackedDeviceButtonUntouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	TrackedDeviceButtonUntouched(unWhichDevice, eButtonId, eventTimeOffset, bInputCall);
}
void CVRServerDriverHost5::SendTrackedDeviceAxisUpdated(uint32_t unWhichDevice, uint32_t unWhichAxis, const VRControllerAxis_t & axisState)
{
	TrackedDeviceAxisUpdated(unWhichDevice, unWhichAxis, axisState);
}



/** notifies the server that the button was pressed */
void CVRServerDriverHost5::TrackedDeviceButtonPressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	if (!bInputCall)
	{
		CSISharedData *pSharedData = m_pDriverInstance->m_pSharedData;
		int nIndex = 0;
		if (pSharedData->m_nPadIndex[0] == unWhichDevice)  // Left controller
			nIndex = 0;
		if (pSharedData->m_nPadIndex[1] == unWhichDevice) // Right controller
			nIndex = 1;

		m_bStatusTrackedDevice[nIndex] = true;

		CVRDriverInput *pInput = (CVRDriverInput *)m_pDriverInstance->VRDriverInput();
		CVRInputInfo *pKeyInfo = pInput->GetInputInfo(unWhichDevice, CVRDriverInput::ruTrackpadClick);
		if (pKeyInfo)
			pInput->UpdateBooleanComponentInternal(pKeyInfo->m_nKeyHandle, true, 0);
	}

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceButtonPressed5(%d,%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset, bInputCall);
}

/** notifies the server that the button was unpressed */
void CVRServerDriverHost5::TrackedDeviceButtonUnpressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	if (!bInputCall)
	{
		CSISharedData *pSharedData = m_pDriverInstance->m_pSharedData;
		int nIndex = 0;
		if (pSharedData->m_nPadIndex[0] == unWhichDevice)  // Left controller
			nIndex = 0;
		if (pSharedData->m_nPadIndex[1] == unWhichDevice) // Right controller
			nIndex = 1;

		m_bStatusTrackedDevice[nIndex + 2] = true;

		CVRDriverInput *pInput = (CVRDriverInput *)m_pDriverInstance->VRDriverInput();
		CVRInputInfo *pKeyInfo = pInput->GetInputInfo(unWhichDevice, CVRDriverInput::ruTrackpadClick);
		if (pKeyInfo)
			pInput->UpdateBooleanComponentInternal(pKeyInfo->m_nKeyHandle, false, 0);
	}

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceButtonUnpressed5(%d,%d,%d,)", unWhichDevice, eButtonId, eventTimeOffset, bInputCall);
}

/** notifies the server that the button was pressed */
void CVRServerDriverHost5::TrackedDeviceButtonTouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	CSISharedData *pSharedData = m_pDriverInstance->m_pSharedData;

	if (bInputCall)
	{
		if (pSharedData->m_nPadIndex[0] == unWhichDevice)  // Left controller
			m_bTrackpadTouched[0] = true;

		if (pSharedData->m_nPadIndex[1] == unWhichDevice) // Right controller
			m_bTrackpadTouched[1] = true;

		DEBUG_LOG(2, "TrackedDeviceButtonTouched %d %d", m_bTrackpadTouched[0], m_bTrackpadTouched[1]);
	}
	else
	{
		int nIndex = 0;
		if (pSharedData->m_nPadIndex[0] == unWhichDevice)  // Left controller
			nIndex = 0;
		if (pSharedData->m_nPadIndex[1] == unWhichDevice) // Right controller
			nIndex = 1;

		m_bStatusTrackedDevice[nIndex + 4] = true;

		CVRDriverInput *pInput = (CVRDriverInput *)m_pDriverInstance->VRDriverInput();
		CVRInputInfo *pKeyInfo = pInput->GetInputInfo(unWhichDevice, CVRDriverInput::ruTrackpadTouch);
		if (pKeyInfo)
			pInput->UpdateBooleanComponentInternal(pKeyInfo->m_nKeyHandle, true, 0);
	}

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_PURPLE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceButtonTouched5(%d,%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset, bInputCall);
}

/** notifies the server that the button was unpressed */
void CVRServerDriverHost5::TrackedDeviceButtonUntouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	CSISharedData *pSharedData = m_pDriverInstance->m_pSharedData;

	if (bInputCall)
	{
		if (pSharedData->m_nPadIndex[0] == unWhichDevice)  // Left controller
			m_bTrackpadTouched[0] = false;

		if (pSharedData->m_nPadIndex[1] == unWhichDevice) // Right controller
			m_bTrackpadTouched[1] = false;
		DEBUG_LOG(2, "TrackedDeviceButtonUntouched %d %d", m_bTrackpadTouched[0], m_bTrackpadTouched[1]);
	}
	else
	{
		int nIndex = 0;

		if (pSharedData->m_nPadIndex[0] == unWhichDevice)  // Left controller
			nIndex = 0;
		if (pSharedData->m_nPadIndex[1] == unWhichDevice) // Right controller
			nIndex = 1;

		m_bStatusTrackedDevice[nIndex + 6] = true;

		CVRDriverInput *pInput = (CVRDriverInput *)m_pDriverInstance->VRDriverInput();
		CVRInputInfo *pKeyInfo = pInput->GetInputInfo(unWhichDevice, CVRDriverInput::ruTrackpadTouch);
		if (pKeyInfo)
			pInput->UpdateBooleanComponentInternal(pKeyInfo->m_nKeyHandle, false, 0);
	}

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_PURPLE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceButtonUntouched5(%d,%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset, bInputCall);
}

/** notifies the server than a controller axis changed */
void CVRServerDriverHost5::TrackedDeviceAxisUpdated(uint32_t unWhichDevice, uint32_t unWhichAxis, const VRControllerAxis_t & axisState)
{
	int nIndex = 0;
	CSISharedData *pSharedData = m_pDriverInstance->m_pSharedData;
	if (pSharedData->m_nPadIndex[0] == unWhichDevice)  // Left controller
		nIndex = 0;
	if (pSharedData->m_nPadIndex[1] == unWhichDevice) // Right controller
		nIndex = 1;

	CVRDriverInput *pInput = (CVRDriverInput *)m_pDriverInstance->VRDriverInput();

	if (unWhichAxis == 0)
	{
		CVRInputInfo *pKeyInfo = pInput->GetInputInfo(unWhichDevice, CVRDriverInput::ruTrackpadX);
		if (pKeyInfo)
			pInput->UpdateScalarComponentInternal(pKeyInfo->m_nKeyHandle, axisState.x, 0);
		pKeyInfo = pInput->GetInputInfo(unWhichDevice, CVRDriverInput::ruTrackpadY);
		if (pKeyInfo)
			pInput->UpdateScalarComponentInternal(pKeyInfo->m_nKeyHandle, axisState.y, 0);
	}
	else if (unWhichAxis == 1)
	{
		CVRInputInfo *pKeyInfo = pInput->GetInputInfo(unWhichDevice, CVRDriverInput::ruTriggerValue);
		if (pKeyInfo)
			pInput->UpdateScalarComponentInternal(pKeyInfo->m_nKeyHandle, axisState.x, 0);
		pKeyInfo = pInput->GetInputInfo(unWhichDevice, CVRDriverInput::ruTriggerClick);
		if (pKeyInfo)
			pInput->UpdateBooleanComponentInternal(pKeyInfo->m_nKeyHandle, axisState.x > 0.2f, 0);
	}


	DEBUG_LOG_SETCOLOR(DEBUG_LOG_GRAY_BLUE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceAxisUpdated5(%d,%d,(%f,%f))", unWhichDevice, unWhichAxis, axisState.x, axisState.y);
}



/** Sends a vendor specific event (VREvent_VendorSpecific_Reserved_Start..VREvent_VendorSpecific_Reserved_End */
void CVRServerDriverHost5::VendorSpecificEvent(uint32_t unWhichDevice, vr::EVREventType eventType, const VREvent_Data_t & eventData, double eventTimeOffset)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->VendorSpecificEvent(unWhichDevice, eventType, eventData, eventTimeOffset);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CVRServerDriverHost5::VendorSpecificEvent(%d,%d,%d,%f)", unWhichDevice, eventType, eventData, eventTimeOffset);
}

/** Returns true if SteamVR is exiting */
bool CVRServerDriverHost5::IsExiting()
{
	bool bRet = false;
	if (m_pServerDriverHost)
		bRet = m_pServerDriverHost->IsExiting();

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CVRServerDriverHost5::IsExiting()=%d", bRet);

	return bRet;
}

bool CVRServerDriverHost5::PollNextEvent(VREvent_t *pEvent, uint32_t uncbVREvent)
{
	bool bRet = false;
	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::PollNextEvent([%d],%d,%d,%f...){", uncbVREvent, pEvent->eventType, pEvent->trackedDeviceIndex, pEvent->eventAgeSeconds);

	bool bFinish = true;
	CSISharedData *pSharedData = m_pDriverInstance->m_pSharedData;

	for (int i = 2 * 2; i < 2 * 4; i++)
	{
		if (m_bStatusTrackedDevice[i])
		{
			m_bStatusTrackedDevice[i] = false;
			bFinish = false;
			bRet = true;
			pEvent->eventType = VREvent_ButtonPress + i / 2;
			pEvent->trackedDeviceIndex = pSharedData->m_nPadIndex[i % 2];
			float fDeltaTime = float(m_Time.stop() / 1000000.0) / 1000.0f;
			pEvent->eventAgeSeconds = fDeltaTime;
			pEvent->data.controller.button = EVRButtonId::k_EButton_SteamVR_Touchpad;
			break;
		}
	}

	for (int i = 0; i < 2 * 2; i++)
	{
		if (m_bStatusTrackedDevice[i])
		{
			m_bStatusTrackedDevice[i] = false;
			bFinish = false;
			bRet = true;
			pEvent->eventType = VREvent_ButtonPress + i / 2;
			pEvent->trackedDeviceIndex = pSharedData->m_nPadIndex[i % 2];
			float fDeltaTime = float(m_Time.stop() / 1000000.0) / 1000.0f;
			pEvent->eventAgeSeconds = fDeltaTime;
			pEvent->data.controller.button = EVRButtonId::k_EButton_SteamVR_Touchpad;

			break;
		}
	}

	if (bFinish)
		if (m_pServerDriverHost)
			bRet = m_pServerDriverHost->PollNextEvent(pEvent, uncbVREvent);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::PollNextEvent([%d],%d,%d,%f...)=%d}", uncbVREvent, pEvent->eventType, pEvent->trackedDeviceIndex, pEvent->eventAgeSeconds, bRet);

	m_Time.start();
	return bRet;
}

void CVRServerDriverHost5::GetRawTrackedDevicePoses(float fPredictedSecondsFromNow, TrackedDevicePose_t *pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->GetRawTrackedDevicePoses(fPredictedSecondsFromNow, pTrackedDevicePoseArray, unTrackedDevicePoseArrayCount);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::GetRawTrackedDevicePoses(%f,%d)", fPredictedSecondsFromNow, unTrackedDevicePoseArrayCount);
}

void CVRServerDriverHost5::TrackedDeviceDisplayTransformUpdated(uint32_t unWhichDevice, HmdMatrix34_t eyeToHeadLeft, HmdMatrix34_t eyeToHeadRight)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDeviceDisplayTransformUpdated(unWhichDevice, eyeToHeadLeft, eyeToHeadRight);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost5::TrackedDeviceDisplayTransformUpdated(%d)", unWhichDevice);
}