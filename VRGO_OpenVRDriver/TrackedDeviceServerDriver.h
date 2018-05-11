#pragma once
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


#include "openvr_driver.h"
#include "DebugLog.h"
#include "DriverInstance.h"
#include "x86timer.h"

using namespace vr;

class CTrackedDeviceServerDriver : public ITrackedDeviceServerDriver//, public vr::IVRControllerComponent
{
public:
	ITrackedDeviceServerDriver *m_pTrackedDeviceServerDriver;
	TrackedDeviceIndex_t m_nID;
	PropertyContainerHandle_t m_ulPropertyContainer;
	ETrackedDeviceClass m_DeviceClass;
	CDriverInstance *m_pDriverInstance;
	DriverPose_t m_currentPose;
	x86timer m_TimeFrame;

	bool m_bPressed;
	bool m_bTouched;
	bool m_bUnPressed;
	bool m_bUnTouched;

	CTrackedDeviceServerDriver(ITrackedDeviceServerDriver *pTrackedDeviceServerDriver, ETrackedDeviceClass eDeviceClass = ETrackedDeviceClass::TrackedDeviceClass_Invalid, CDriverInstance *pDriverInstance=nullptr)
	{
		m_pTrackedDeviceServerDriver = pTrackedDeviceServerDriver;
		m_nID = k_unTrackedDeviceIndexInvalid;
		m_DeviceClass = eDeviceClass;
		m_ulPropertyContainer = k_ulInvalidPropertyContainer;
		m_pDriverInstance = pDriverInstance;
		m_pDriverInstance->m_bServerHostCreated = true;

		m_bPressed = false;
		m_bTouched = false;
		m_bUnPressed = false;
		m_bUnTouched = false;
	}

	/** This is called before an HMD is returned to the application. It will always be
	* called before any display or tracking methods. Memory and processor use by the
	* ITrackedDeviceServerDriver object should be kept to a minimum until it is activated.
	* The pose listener is guaranteed to be valid until Deactivate is called, but
	* should not be used after that point. */
	virtual EVRInitError Activate(uint32_t unObjectId);

	/** This is called when The VR system is switching from this Hmd being the active display
	* to another Hmd being the active display. The driver should clean whatever memory
	* and thread use it can when it is deactivated */
	virtual void Deactivate();

	/** Handles a request from the system to put this device into standby mode. What that means is defined per-device. */
	virtual void EnterStandby();

	/** Requests a component interface of the driver for device-specific functionality. The driver should return NULL
	* if the requested interface or version is not supported. */
	virtual void *GetComponent(const char *pchComponentNameAndVersion);

	/** A VR Watchdog has made this debug request of the driver. The set of valid requests is entirely
	* up to the driver and the Watchdog to figure out, as is the format of the response. Responses that
	* exceed the length of the supplied buffer should be truncated and null terminated */
	virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize);

	// ------------------------------------
	// Tracking Methods
	// ------------------------------------
	virtual DriverPose_t GetPose();

	//////IVRControllerComponent//////

	/** Gets the current state of a controller. */
	virtual VRControllerState_t GetControllerState();

	/** Returns a uint64 property. If the property is not available this function will return 0. */
	virtual bool TriggerHapticPulse(uint32_t unAxisId, uint16_t usPulseDurationMicroseconds);



};

