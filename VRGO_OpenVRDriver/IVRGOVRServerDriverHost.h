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
#include <string>

#include "TrackedDeviceServerDriver.h"
#include "DriverInstance.h"

using namespace vr;

typedef std::map <ITrackedDeviceServerDriver *, CTrackedDeviceServerDriver*> DriverServerMap;

class IVRGOVRServerDriverHost
{
protected:
	bool m_bCalibrated;
	CDriverInstance *m_pDriverInstance;
public:
	IVRGOVRServerDriverHost(CDriverInstance *pDriverInstance);
	~IVRGOVRServerDriverHost();


	virtual void SendTrackedDeviceButtonPressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false) = 0;
	virtual void SendTrackedDeviceButtonUnpressed(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false) = 0;
	virtual void SendTrackedDeviceButtonTouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false) = 0;
	virtual void SendTrackedDeviceButtonUntouched(uint32_t unWhichDevice, EVRButtonId eButtonId, double eventTimeOffset, bool bInputCall = false) = 0;
	virtual void SendTrackedDeviceAxisUpdated(uint32_t unWhichDevice, uint32_t unWhichAxis, const VRControllerAxis_t & axisState) = 0;

	virtual CTrackedDeviceServerDriver* GetDeviceIndex(int nWhichDevice);

	virtual void HandleTrackedDevicePoseUpdated(uint32_t unWhichDevice, DriverPose_t & newPose, uint32_t unPoseStructSize);

	virtual void Calibration(CMovement *pMovement, const DriverPose_t& pos);

	virtual void ComputeMovement(float fDeltaTime);

	virtual bool IsCalibrated()
	{
		return m_bCalibrated;
	}

	int m_nDriverHostVersion;

	DriverServerMap m_MapDeviceServerDriver;
	bool m_bTrackpadTouched[2];

	bool m_bStatusTrackedDevice[2 * 4];
	x86timer m_Time;

};