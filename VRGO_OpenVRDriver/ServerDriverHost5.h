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
#include <map>
#include "TrackedDeviceServerDriver.h"
#include "DriverInstance.h"
#include "IVRGOVRServerDriverHost.h"

using namespace vr;

class CVRServerDriverHost5 : public IVRServerDriverHost, public IVRGOVRServerDriverHost
{
	IVRServerDriverHost *m_pServerDriverHost;
public:

	CVRServerDriverHost5(IVRServerDriverHost *pServerDriverHost, CDriverInstance *pDriverInstance) : IVRServerDriverHost(), IVRGOVRServerDriverHost(pDriverInstance)
	{
		m_pServerDriverHost = pServerDriverHost;
		m_nDriverHostVersion = 5;
	}

	~CVRServerDriverHost5()
	{
	}

	/** Notifies the server that a tracked device has been added. If this function returns true
	* the server will call Activate on the device. If it returns false some kind of error
	* has occurred and the device will not be activated. */
	virtual bool TrackedDeviceAdded(const char *pchDeviceSerialNumber, ETrackedDeviceClass eDeviceClass, ITrackedDeviceServerDriver *pDriver);

	/** Notifies the server that a tracked device's pose has been updated */
	virtual void TrackedDevicePoseUpdated(uint32_t unWhichDevice, const DriverPose_t & newPose, uint32_t unPoseStructSize);

	/** Notifies the server that vsync has occurred on the the display attached to the device. This is
	* only permitted on devices of the HMD class. */
	virtual void VsyncEvent(double vsyncTimeOffsetSeconds);

	/** Sends a vendor specific event (VREvent_VendorSpecific_Reserved_Start..VREvent_VendorSpecific_Reserved_End */
	virtual void VendorSpecificEvent(uint32_t unWhichDevice, vr::EVREventType eventType, const VREvent_Data_t & eventData, double eventTimeOffset);

	/** Returns true if SteamVR is exiting */
	virtual bool IsExiting();

	/** Returns true and fills the event with the next event on the queue if there is one. If there are no events
	* this method returns false. uncbVREvent should be the size in bytes of the VREvent_t struct */
	virtual bool PollNextEvent(VREvent_t *pEvent, uint32_t uncbVREvent);

	/** Provides access to device poses for drivers.  Poses are in their "raw" tracking space which is uniquely
	* defined by each driver providing poses for its devices.  It is up to clients of this function to correlate
	* poses across different drivers.  Poses are indexed by their device id, and their associated driver and
	* other properties can be looked up via IVRProperties. */
	virtual void GetRawTrackedDevicePoses(float fPredictedSecondsFromNow, TrackedDevicePose_t *pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount);

	/** Notifies the server that a tracked device's display component transforms have been updated. */
	virtual void TrackedDeviceDisplayTransformUpdated(uint32_t unWhichDevice, HmdMatrix34_t eyeToHeadLeft, HmdMatrix34_t eyeToHeadRight);

	virtual void SendTrackedDeviceButtonPressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false);
	virtual void SendTrackedDeviceButtonUnpressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false);
	virtual void SendTrackedDeviceButtonTouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false);
	virtual void SendTrackedDeviceButtonUntouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false);
	virtual void SendTrackedDeviceAxisUpdated(uint32_t unWhichDevice, uint32_t unWhichAxis, const VRControllerAxis_t & axisState);

	void TrackedDeviceButtonPressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false);
	void TrackedDeviceButtonUnpressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false);
	void TrackedDeviceButtonTouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false);
	void TrackedDeviceButtonUntouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false);
	void TrackedDeviceAxisUpdated(uint32_t unWhichDevice, uint32_t unWhichAxis, const VRControllerAxis_t & axisState);


};