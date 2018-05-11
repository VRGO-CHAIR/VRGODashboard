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
#include "x86timer.h"
#include "DriverInstance.h"

using namespace vr;

class CServerDriver : public vr::IServerTrackedDeviceProvider
{
public:
	CDriverInstance *m_pDriverInstance;
	class CVRDriverContext *m_pDriverContext;

	CServerDriver(CDriverInstance *pDriverInstance)
	{
		m_pDriverContext = nullptr;
		m_bHoldInUse = false;
		m_pDriverInstance = pDriverInstance;
	}
	~CServerDriver() {}

	bool m_bHoldInUse;
	x86timer m_TimeFrame;
	virtual EVRInitError Init(IVRDriverContext *pDriverContext);

	/** cleans up the driver right before it is unloaded */
	virtual void Cleanup();

	/** Returns the version of the ITrackedDeviceServerDriver interface used by this driver */
	virtual const char * const *GetInterfaceVersions();

	/** Allows the driver do to some work in the main loop of the server. */
	virtual void RunFrame();

	// ------------  Power State Functions ----------------------- //

	/** Returns true if the driver wants to block Standby mode. */
	virtual bool ShouldBlockStandbyMode();

	/** Called when the system is entering Standby mode. The driver should switch itself into whatever sort of low-power
	* state it has. */
	virtual void EnterStandby();

	/** Called when the system is leaving Standby mode. The driver should switch itself back to
	full operation. */
	virtual void LeaveStandby();



};

