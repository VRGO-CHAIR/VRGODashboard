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
#include "WatchdogDriver.h"
#include "DriverContext.h"
#include "DriverInstance.h"
#include "DebugLog.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWatchdogDriver
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWatchdogDriver::CWatchdogDriver(CDriverInstance *pDriverInstance)
{
	m_pDriverInstance = pDriverInstance;
	m_pDriverContext = nullptr;
}

CWatchdogDriver::~CWatchdogDriver()
{}

EVRInitError CWatchdogDriver::Init(IVRDriverContext *pDriverContext)
{
	EVRInitError nError = vr::VRInitError_Init_NotInitialized;

	m_pDriverContext = new CVRDriverContext(pDriverContext, m_pDriverInstance);
	VR_INIT_WATCHDOG_DRIVER_CONTEXT(pDriverContext);

	if (m_pDriverInstance->m_pChildWatchdogTrackedDeviceProvider)
		nError = m_pDriverInstance->m_pChildWatchdogTrackedDeviceProvider->Init(m_pDriverContext);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CWatchdogDriver::Init()=>%d", nError);

	return nError;
}

/** cleans up the driver right before it is unloaded */
void CWatchdogDriver::Cleanup()
{
	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CWatchdogDriver::Cleanup()");

	if (m_pDriverInstance->m_pChildWatchdogTrackedDeviceProvider)
		m_pDriverInstance->m_pChildWatchdogTrackedDeviceProvider->Cleanup();

	if (m_pDriverContext)
		delete m_pDriverContext;
	m_pDriverContext = NULL;
}