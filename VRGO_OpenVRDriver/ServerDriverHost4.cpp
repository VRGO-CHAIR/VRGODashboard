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
#include "ServerDriverHost4.h"
#include "../Common/MathTools.h"
#include "../Common/SharedData.h"
#include "ServerDriver.h"
#include "DriverContext.h"
// include driverinput


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVRServerDriverHost 4
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CVRServerDriverHost4::TrackedDeviceAdded(const char *pchDeviceSerialNumber, ETrackedDeviceClass eDeviceClass, ITrackedDeviceServerDriver *pDriver)
{
	char *sType[] = { "Invalid","HMD","Controller","GenericTracker","TrackingReference" };

	bool bRet = false;

	// add pTrackedDeviceServerDriver !!
	CTrackedDeviceServerDriver *pPout;

	DriverServerMap::iterator it = m_MapDeviceServerDriver.find(pDriver);
	if (it == m_MapDeviceServerDriver.end())
	{
		pPout = new CTrackedDeviceServerDriver(pDriver, eDeviceClass);
		m_MapDeviceServerDriver.insert(std::pair <ITrackedDeviceServerDriver *, CTrackedDeviceServerDriver*>(pDriver, pPout));
	}
	else
		pPout = (it->second);

	if (m_pServerDriverHost)
		bRet = m_pServerDriverHost->TrackedDeviceAdded(pchDeviceSerialNumber, eDeviceClass, pPout);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CVRServerDriverHost::TrackedDeviceAdded(%s:%s,%p)=%d", pchDeviceSerialNumber, sType[eDeviceClass], pDriver, bRet);

	return bRet;
}

//CTrackedDeviceServerDriver* CVRServerDriverHost4::GetDeviceIndex(int nWhichDevice)
//{
//	CTrackedDeviceServerDriver* pTrackedDevice = nullptr;
//	DriverServerMap::iterator it = m_MapDeviceServerDriver.begin();
//	while (it != m_MapDeviceServerDriver.end())
//	{
//		if ((it->second)->m_nID == nWhichDevice)
//		{
//			pTrackedDevice = (it->second);
//			break;
//		}
//		it++;
//	}
//	return pTrackedDevice;
//}
//
//void CVRServerDriverHost4::Calibration(CMovement *pMovement, const DriverPose_t& pos)
//{
//	// Get orientation of HMD
//	Mat_33 HmdRotationMat;
//	for (int i = 0; i < 3; ++i)
//		for (int j = 0; j < 3; ++j)
//			HmdRotationMat[i][j] = pMovement->m_mMatrix34Hmd.m[i][j];
//	// init position
//	for (int i = 0; i < 3; ++i)
//		pMovement->m_v3PositionHmd.v[i] = pos.vecPosition[i];
//
//	HmdQuaternion_t HmdRotationQuat;
//	MatToQuat(HmdRotationMat, HmdRotationQuat);
//	// rotation x -180
//	HmdRotationQuat.y *= -1.0;
//	HmdRotationQuat.z *= -1.0;
//
//	HmdQuaternion_t HmdTransposedQuat = pos.qRotation;;
//	Inverse(HmdTransposedQuat);
//	// Applique la rotation au repere du driver dans le repere du casque
//	HmdQuaternion_t driverRotationQuat = HmdTransposedQuat * HmdRotationQuat;
//	Inverse(driverRotationQuat);
//	// Transform en vecteur X Y Z
//	QuatToVectors(driverRotationQuat, pMovement->m_v3AxisX, pMovement->m_v3AxisY, pMovement->m_v3AxisZ);
//	// Save initial rotation
//	pMovement->m_qRotationInitial = driverRotationQuat;
//	pMovement->m_bAskCalibration = false;
//	// Restore delta movement
//	pMovement->Restore();
//	m_bCalibrated = true;
//}

/** Notifies the server that a tracked device's pose has been updated */
void CVRServerDriverHost4::TrackedDevicePoseUpdated(uint32_t unWhichDevice, const DriverPose_t & newPose, uint32_t unPoseStructSize)
{
	DriverPose_t pos = newPose;

	HandleTrackedDevicePoseUpdated(unWhichDevice, pos, unPoseStructSize);

	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDevicePoseUpdated(unWhichDevice, pos, unPoseStructSize);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost4::TrackedDevicePoseUpdated(%d)", unWhichDevice);
	//DriverPose_t pos = newPose;
	//CTrackedDeviceServerDriver* pTrackedDevice = GetDeviceIndex(unWhichDevice);
	//CMovement *pMovement = m_pDriverInstance->m_pMovement;

	//if (pMovement->m_bInCalibration && pos.poseIsValid && pTrackedDevice->m_DeviceClass == ETrackedDeviceClass::TrackedDeviceClass_HMD)
	//{
	//	pMovement->m_bInCalibration = false;
	//	// send message to ask matrix
	//	CSI_MessageCalibration msgCalib;
	//	m_pDriverInstance->m_pSharedMessageHandleOut->PushMessage(&msgCalib);
	//}
	//else
	//{
	//	// Init position and orientation of HMD (look forward)
	//	if (pMovement->m_bAskCalibration && pos.poseIsValid && pTrackedDevice->m_DeviceClass == ETrackedDeviceClass::TrackedDeviceClass_HMD)
	//	{
	//		// Calibration
	//		Calibration(pMovement, pos);

	//		/******DEBUG******/
	//		CSISharedDataDebug *pSharedDataDebug = m_pDriverInstance->m_pSharedDataDebug;
	//		pSharedDataDebug->m_PositionInitial.m_fx = pMovement->m_v3PositionHmd.v[0];
	//		pSharedDataDebug->m_PositionInitial.m_fy = pMovement->m_v3PositionHmd.v[1];
	//		pSharedDataDebug->m_PositionInitial.m_fz = pMovement->m_v3PositionHmd.v[2];

	//		pSharedDataDebug->m_RotationInitial.m_fx = pMovement->m_qRotationInitial.x;
	//		pSharedDataDebug->m_RotationInitial.m_fy = pMovement->m_qRotationInitial.y;
	//		pSharedDataDebug->m_RotationInitial.m_fz = pMovement->m_qRotationInitial.z;
	//		pSharedDataDebug->m_RotationInitial.m_fw = pMovement->m_qRotationInitial.w;
	//		/******END DEBUG******/
	//	}

	//	if (m_bCalibrated)
	//	{
	//		// Active VR Locomotion
	//		if (m_pDriverInstance->m_pSharedData->m_bActivateVRLocomotion)
	//		{
	//			// Compute Rotation
	//			pMovement->m_qRotation = FromAngleAxis(pMovement->m_fAngle, pMovement->m_v3AxisY);
	//			// Apply Rotation
	//			pos.qRotation = pos.qRotation * pMovement->m_qRotation;
	//			// Compute the difference between the hmd and the controller
	//			HmdVector3d_t controllerFromHead;
	//			for (int i = 0; i < 3; ++i)
	//				controllerFromHead.v[i] = pos.vecPosition[i] - pMovement->m_v3PositionHmd.v[i];
	//			// Rotate by angle
	//			Transform(pMovement->m_qRotation, controllerFromHead);
	//			// Apply Movement
	//			for (int i = 0; i < 3; ++i)
	//				pos.vecPosition[i] = pMovement->m_v3PositionHmd.v[i] + pMovement->m_v3Translation.v[i] + controllerFromHead.v[i];

	//			// Save current pose to use on ServerDriver::RunFrame
	//			pTrackedDevice->m_currentPose = newPose;
	//		}

	//		// Height offset
	//		for (int i = 0; i < 3; ++i)
	//			pos.vecPosition[i] += pMovement->m_v3OffsetY.v[i];
	//	}
	//}
	//if (m_pServerDriverHost)
	//	m_pServerDriverHost->TrackedDevicePoseUpdated(unWhichDevice, pos, unPoseStructSize);

	//DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	//DEBUG_LOG(5, "CVRServerDriverHost::TrackedDevicePoseUpdated(%d)", unWhichDevice);
}

/** Notifies the server that vsync has occurred on the the display attached to the device. This is
* only permitted on devices of the HMD class. */
void CVRServerDriverHost4::VsyncEvent(double vsyncTimeOffsetSeconds)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->VsyncEvent(vsyncTimeOffsetSeconds);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(3, "CVRServerDriverHost4::VsyncEvent(%d)", vsyncTimeOffsetSeconds);
}

/** notifies the server that the button was pressed */
void CVRServerDriverHost4::TrackedDeviceButtonPressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDeviceButtonPressed(unWhichDevice, eButtonId, eventTimeOffset);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost4::TrackedDeviceButtonPressed(%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset);
}

/** notifies the server that the button was unpressed */
void CVRServerDriverHost4::TrackedDeviceButtonUnpressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDeviceButtonUnpressed(unWhichDevice, eButtonId, eventTimeOffset);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost4::TrackedDeviceButtonUnpressed(%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset);
}

/** notifies the server that the button was pressed */
void CVRServerDriverHost4::TrackedDeviceButtonTouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset)
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
	DEBUG_LOG(5, "CVRServerDriverHost4::TrackedDeviceButtonTouched(%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset);
}

/** notifies the server that the button was unpressed */
void CVRServerDriverHost4::TrackedDeviceButtonUntouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset)
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
	DEBUG_LOG(5, "CVRServerDriverHost4::TrackedDeviceButtonUntouched(%d,%d,%d)", unWhichDevice, eButtonId, eventTimeOffset);
}

/** notifies the server than a controller axis changed */
void CVRServerDriverHost4::TrackedDeviceAxisUpdated(uint32_t unWhichDevice, uint32_t unWhichAxis, const VRControllerAxis_t & axisState)
{
	/// PATCH !!!!! 

	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDeviceAxisUpdated(unWhichDevice, unWhichAxis, axisState);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_GRAY_BLUE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost4::TrackedDeviceAxisUpdated(%d,%d,(%f,%f))", unWhichDevice, unWhichAxis, axisState.x, axisState.y);

}

/** Notifies the server that the proximity sensor on the specified device  */
void CVRServerDriverHost4::ProximitySensorState(uint32_t unWhichDevice, bool bProximitySensorTriggered)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->ProximitySensorState(unWhichDevice, bProximitySensorTriggered);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost4::ProximitySensorState(%d,%d)", unWhichDevice, bProximitySensorTriggered);
}

void CVRServerDriverHost4::SendTrackedDeviceButtonPressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	TrackedDeviceButtonPressed(unWhichDevice, eButtonId, eventTimeOffset);
}
void CVRServerDriverHost4::SendTrackedDeviceButtonUnpressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	TrackedDeviceButtonUnpressed(unWhichDevice, eButtonId, eventTimeOffset);
}
void CVRServerDriverHost4::SendTrackedDeviceButtonTouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	TrackedDeviceButtonTouched(unWhichDevice, EVRButtonId(int(eButtonId) | 0x8000), eventTimeOffset);
}
void CVRServerDriverHost4::SendTrackedDeviceButtonUntouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall)
{
	TrackedDeviceButtonUntouched(unWhichDevice, EVRButtonId(int(eButtonId) | 0x8000), eventTimeOffset);
}
void CVRServerDriverHost4::SendTrackedDeviceAxisUpdated(uint32_t unWhichDevice, uint32_t unWhichAxis, const VRControllerAxis_t & axisState)
{
	TrackedDeviceAxisUpdated(unWhichDevice, unWhichAxis, axisState);
}

/** Sends a vendor specific event (VREvent_VendorSpecific_Reserved_Start..VREvent_VendorSpecific_Reserved_End */
void CVRServerDriverHost4::VendorSpecificEvent(uint32_t unWhichDevice, vr::EVREventType eventType, const VREvent_Data_t & eventData, double eventTimeOffset)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->VendorSpecificEvent(unWhichDevice, eventType, eventData, eventTimeOffset);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CVRServerDriverHost::VendorSpecificEvent(%d,%d,%d,%f)", unWhichDevice, eventType, eventData, eventTimeOffset);
}

/** Returns true if SteamVR is exiting */
bool CVRServerDriverHost4::IsExiting()
{
	bool bRet = false;
	if (m_pServerDriverHost)
		bRet = m_pServerDriverHost->IsExiting();

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CVRServerDriverHost4::IsExiting()=%d", bRet);

	return bRet;
}

bool CVRServerDriverHost4::PollNextEvent(VREvent_t *pEvent, uint32_t uncbVREvent)
{
	bool bRet = false;
	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost4::PollNextEvent([%d],%d,%d,%f...){", uncbVREvent, pEvent->eventType, pEvent->trackedDeviceIndex, pEvent->eventAgeSeconds);

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
	DEBUG_LOG(5, "CVRServerDriverHost4::PollNextEvent([%d],%d,%d,%f...)=%d}", uncbVREvent, pEvent->eventType, pEvent->trackedDeviceIndex, pEvent->eventAgeSeconds, bRet);

	m_Time.start();
	return bRet;
}

void CVRServerDriverHost4::GetRawTrackedDevicePoses(float fPredictedSecondsFromNow, TrackedDevicePose_t *pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->GetRawTrackedDevicePoses(fPredictedSecondsFromNow, pTrackedDevicePoseArray, unTrackedDevicePoseArrayCount);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost4::GetRawTrackedDevicePoses(%f,%d)", fPredictedSecondsFromNow, unTrackedDevicePoseArrayCount);
}

void CVRServerDriverHost4::TrackedDeviceDisplayTransformUpdated(uint32_t unWhichDevice, HmdMatrix34_t eyeToHeadLeft, HmdMatrix34_t eyeToHeadRight)
{
	if (m_pServerDriverHost)
		m_pServerDriverHost->TrackedDeviceDisplayTransformUpdated(unWhichDevice, eyeToHeadLeft, eyeToHeadRight);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRServerDriverHost4::TrackedDeviceDisplayTransformUpdated(%d)", unWhichDevice);
}