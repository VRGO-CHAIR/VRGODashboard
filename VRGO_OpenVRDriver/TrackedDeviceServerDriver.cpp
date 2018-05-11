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
#include "TrackedDeviceServerDriver.h"



/** This is called before an HMD is returned to the application. It will always be
* called before any display or tracking methods. Memory and processor use by the
* ITrackedDeviceServerDriver object should be kept to a minimum until it is activated.
* The pose listener is guaranteed to be valid until Deactivate is called, but
* should not be used after that point. */
EVRInitError CTrackedDeviceServerDriver::Activate(uint32_t unObjectId)
{
	EVRInitError nRet = vr::VRInitError_Init_NotInitialized;
	if (m_DeviceClass == TrackedDeviceClass_HMD)
		m_pDriverInstance->m_bServerHostActivated = true;

	m_nID = unObjectId;
	m_ulPropertyContainer = m_pDriverInstance->VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
	ETrackedPropertyError Error = ETrackedPropertyError::TrackedProp_Success;
	if (m_DeviceClass == ETrackedDeviceClass::TrackedDeviceClass_Invalid)
	{
		m_DeviceClass = (ETrackedDeviceClass)m_pDriverInstance->VRProperties()->GetInt32Property(m_ulPropertyContainer, ETrackedDeviceProperty::Prop_DeviceClass_Int32, &Error);
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_DARK_BLUE, DEBUG_LOG_GRAY);
		DEBUG_LOG(1, "Prop_DeviceClass_Int32  :%d", m_DeviceClass);
	}

	if (m_pTrackedDeviceServerDriver)
		nRet = m_pTrackedDeviceServerDriver->Activate(unObjectId);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CTrackedDeviceServerDriver::Activate(%d,%d(%016llx)=>%d/%d", m_DeviceClass, unObjectId, m_ulPropertyContainer, nRet, m_pDriverInstance->m_bServerHostActivated);
	return nRet;
}


/** This is called when The VR system is switching from this Hmd being the active display
* to another Hmd being the active display. The driver should clean whatever memory
* and thread use it can when it is deactivated */
void CTrackedDeviceServerDriver::Deactivate()
{
	if (m_DeviceClass == TrackedDeviceClass_HMD)
		m_pDriverInstance->m_bServerHostActivated = false;

	m_nID = k_unTrackedDeviceIndexInvalid;
	if (m_pTrackedDeviceServerDriver)
		m_pTrackedDeviceServerDriver->Deactivate();

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CTrackedDeviceServerDriver::Deactivate()");

}

/** Handles a request from the system to put this device into standby mode. What that means is defined per-device. */
void CTrackedDeviceServerDriver::EnterStandby()
{
	if (m_pTrackedDeviceServerDriver)
		m_pTrackedDeviceServerDriver->EnterStandby();

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CTrackedDeviceServerDriver::EnterStandby()");

}

/** Requests a component interface of the driver for device-specific functionality. The driver should return NULL
* if the requested interface or version is not supported. */
void *CTrackedDeviceServerDriver::GetComponent(const char *pchComponentNameAndVersion)
{
	void *pRet = NULL;
	if (m_pTrackedDeviceServerDriver)
		pRet = m_pTrackedDeviceServerDriver->GetComponent(pchComponentNameAndVersion);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CTrackedDeviceServerDriver::GetComponent(%s)=>%p", pchComponentNameAndVersion, pRet);
	return pRet;

}

/** A VR Watchdog has made this debug request of the driver. The set of valid requests is entirely
* up to the driver and the Watchdog to figure out, as is the format of the response. Responses that
* exceed the length of the supplied buffer should be truncated and null terminated */
void CTrackedDeviceServerDriver::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (m_pTrackedDeviceServerDriver)
		m_pTrackedDeviceServerDriver->DebugRequest(pchRequest, pchResponseBuffer, unResponseBufferSize);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_PURPLE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CTrackedDeviceServerDriver::DebugRequest(%s,%s,%d)", pchRequest, pchResponseBuffer, unResponseBufferSize);
}

// ------------------------------------
// Tracking Methods
// ------------------------------------
DriverPose_t CTrackedDeviceServerDriver::GetPose()
{
	DriverPose_t dRet = DriverPose_t();
	if (m_pTrackedDeviceServerDriver)
		dRet = m_pTrackedDeviceServerDriver->GetPose();

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CTrackedDeviceServerDriver::GetPose()=>");
	return dRet;
}


//////IVRControllerComponent//////

/** Gets the current state of a controller. */
VRControllerState_t CTrackedDeviceServerDriver::GetControllerState()
{
	DEBUG_LOG_SETCOLOR(DEBUG_LOG_BROWN, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CTrackedDeviceServerDriver::GetControllerState()");
	return VRControllerState_t();
}

/** Returns a uint64 property. If the property is not available this function will return 0. */
bool CTrackedDeviceServerDriver::TriggerHapticPulse(uint32_t unAxisId, uint16_t usPulseDurationMicroseconds)
{
	DEBUG_LOG_SETCOLOR(DEBUG_LOG_BROWN, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CTrackedDeviceServerDriver::TriggerHapticPulse(%d,%d)", unAxisId, usPulseDurationMicroseconds);
	return false;
}
