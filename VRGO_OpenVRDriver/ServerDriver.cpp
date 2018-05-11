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
#include "ServerDriver.h"
#include "IVRGOVRServerDriverHost.h"
#include "DriverContext.h"
//#include <process.h>
#include "VRGO_OpenVRDriver.h"
#include "../Common/SharedData.h"
#include "../Common/MathTools.h"
#include <map>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CServerDriver
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EVRInitError CServerDriver::Init(IVRDriverContext *pDriverContext)
{
	EVRInitError nError = vr::VRInitError_Init_NotInitialized;

	m_pDriverContext = new CVRDriverContext(pDriverContext, m_pDriverInstance);
	//VR_INIT_SERVER_DRIVER_CONTEXT(m_pDriverContext);

	if (m_pDriverInstance->m_pChildServerTrackedDeviceProvider)
		nError = m_pDriverInstance->m_pChildServerTrackedDeviceProvider->Init(m_pDriverContext);

	m_TimeFrame.start();
	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CServerDriver::Init()=>%d", nError);

	CreateMemoryMapManagerThread();
	DEBUG_LOG(1, "Memory Map Manager Created");
	//_beginthread(OnMemoryMapManager, 0, nullptr);

	return nError;
}

/** cleans up the driver right before it is unloaded */
void CServerDriver::Cleanup()
{
	ExitMemoryMapManagerThread();


	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CServerDriver::Cleanup()");

	if (m_pDriverInstance->m_pChildServerTrackedDeviceProvider)
		m_pDriverInstance->m_pChildServerTrackedDeviceProvider->Cleanup();

	if (m_pDriverContext)
		delete m_pDriverContext;
	m_pDriverContext = NULL;
}

/** Returns the version of the ITrackedDeviceServerDriver interface used by this driver */
const char * const *CServerDriver::GetInterfaceVersions()
{
	const char *const *pRet = NULL;
	if (m_pDriverInstance->m_pChildServerTrackedDeviceProvider)
		pRet = m_pDriverInstance->m_pChildServerTrackedDeviceProvider->GetInterfaceVersions();

#if DEBUG_LOG_LEVEL >0
	char sText[2048];
	memset(sText, 0, 2048);
	int i = 0;
	if (pRet)
		while (pRet[i])
		{
			strcat_s(sText, pRet[i]);
			strcat_s(sText, ",");
			i++;
		}

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_BLUE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CServerDriver::GetInterfaceVersions()=>");
	DEBUG_LOG(1, sText);
#endif

	return pRet;
}

void ButtonPressed(IVRGOVRServerDriverHost *pDriverHost, CTrackedDeviceServerDriver *pTrackedDevice, bool pressed)
{
	if (pressed)
		pDriverHost->SendTrackedDeviceButtonPressed(pTrackedDevice->m_nID, EVRButtonId::k_EButton_Axis0, 0);
	else
		pDriverHost->SendTrackedDeviceButtonUnpressed(pTrackedDevice->m_nID, EVRButtonId::k_EButton_Axis0, 0);
	pTrackedDevice->m_bPressed = pressed;
	pTrackedDevice->m_bUnPressed = !pressed;
}

void ButtonTouched(IVRGOVRServerDriverHost *pDriverHost, CTrackedDeviceServerDriver *pTrackedDevice, bool pressed)
{
	if (pressed)
		pDriverHost->SendTrackedDeviceButtonTouched(pTrackedDevice->m_nID, EVRButtonId::k_EButton_Axis0, 0);
	else
		pDriverHost->SendTrackedDeviceButtonUntouched(pTrackedDevice->m_nID, EVRButtonId::k_EButton_Axis0, 0);
	pTrackedDevice->m_bTouched = pressed;
	pTrackedDevice->m_bUnTouched = !pressed;
}

/** Allows the driver do to some work in the main loop of the server. */
bool activeLastFrame[2] = { false };

void CServerDriver::RunFrame()
{
	// NOTE: Function of interest. This is where the driver runs the frame and places our VRGO input into the appropriate trackpad.

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CServerDriver::RunFrame() Start");
	if (m_pDriverInstance->m_pChildServerTrackedDeviceProvider)
		m_pDriverInstance->m_pChildServerTrackedDeviceProvider->RunFrame();

	//if (!m_pDriverInstance->m_bActivated)
	//	return;

	CSISharedData *pSharedData = m_pDriverInstance->m_pSharedData;

	IVRGOVRServerDriverHost *pDriverHost = m_pDriverInstance->VRVRGOServerDriverHost();
	CSISharedDataDebug *pSharedDataDebug = m_pDriverInstance->m_pSharedDataDebug;


	if (pDriverHost == nullptr)
	{
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
		DEBUG_LOG(1, "CServerDriver::RunFrame() PDRIVERHOST IS NULL");
	}

	// if we have activated controls in VRGO config/dashboard
	if (pSharedData->m_bActivateCtrls)
	{
		// Loop through devices to find the controllers.
		for (std::pair <ITrackedDeviceServerDriver*, CTrackedDeviceServerDriver*> element : pDriverHost->m_MapDeviceServerDriver)
		{
			CTrackedDeviceServerDriver* pTrackedDevice = (element.second);//(it->second);
			ETrackedPropertyError Error = ETrackedPropertyError::TrackedProp_Success;

			if (pTrackedDevice->m_nID != k_ulOverlayHandleInvalid && pTrackedDevice->m_DeviceClass == ETrackedDeviceClass::TrackedDeviceClass_Controller)
			{
				// If this device is a controller, check if it's the controller we expected (left or right based on when it registered with the dashboard.
				int nDataIndex = -1;
				if (pSharedData->m_nPadIndex[0] == pTrackedDevice->m_nID)  // Left controller
					nDataIndex = 0;
				if (pSharedData->m_nPadIndex[1] == pTrackedDevice->m_nID) // Right controller
					nDataIndex = 1;

				DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
				DEBUG_LOG(2, "CServerDriver::RunFrame() - Is a controller: pad index(%d), index (%d)", pSharedData->m_nPadIndex, pTrackedDevice->m_nID);

				if (nDataIndex >= 0)
				{
					// pass if dont override trackpad
					if (!pSharedData->m_bOverridePad[nDataIndex] && activeLastFrame[nDataIndex])
					{
						// If this trackpad/controller is not active in VRGO dashboard, but it was last frame, then our user has disabled it. Ensure we stop touching and pressing the trackpad.
						pTrackedDevice->m_bTouched = false;
						pTrackedDevice->m_bPressed = false;
						pTrackedDevice->m_bUnTouched = true;
						pTrackedDevice->m_bUnPressed = true;
						//DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
						//DEBUG_LOG(2, "CServerDriver::RunFrame() - continue");

						// Now ensure that the trackpad is at 0,0 and not left at wherever VRGO was before disabling.
						VRControllerAxis_t resetState;
						resetState.x = 0;
						resetState.y = 0;
						pDriverHost->SendTrackedDeviceAxisUpdated(pTrackedDevice->m_nID, 0, resetState);
						activeLastFrame[nDataIndex] = pSharedData->m_bOverridePad[nDataIndex];
						continue;
					}
					if (!pSharedData->m_bOverridePad[nDataIndex])
						continue;

					// Update our active last frame variable to handle the frame after it's disabled in dashboard.
					activeLastFrame[nDataIndex] = pSharedData->m_bOverridePad[nDataIndex];

					// Based on dashboard, is this trackpad just touched or is it pressed down?
					bool trackpadPressed = !pSharedData->m_bAxisOnlyTouched[nDataIndex];


					VRControllerAxis_t MyAxisState;
					bool bMove = false;
					int indexPad = nDataIndex * 2;

					// If we aren't using the default functionality of the trackpad.
					// NOTE: this should probably be changed as we no longer expose the avDefault variable to the dashboard so it's always the same value.
					if (pSharedData->m_nAxis[0 + indexPad] != CSISharedData::avDefault || pSharedData->m_nAxis[1 + indexPad] != CSISharedData::avDefault)
					{
						// Get the input from the VRGO SDK shared data.
						float inputX = (float)pSharedDataDebug->m_TranslationXYZAngleW.m_fx;
						float inputY = (float)pSharedDataDebug->m_TranslationXYZAngleW.m_fy;

						// Map this input to a circle (VRGO is mapped to a square by default).
						// NOTE: Might be worth moving this into the Dashboard or SDK.
						MapToCircle(inputX, inputY);

						// Multiply it by speed.
						// NOTE: The speed variable is also no exposed to the dashboard any longer, should probably remove but left it in JIC.
						MyAxisState.x = pSharedData->m_fSpeed[0] * inputX;
						MyAxisState.y = pSharedData->m_fSpeed[1] * inputY;

						DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
						DEBUG_LOG(2, "CServerDriver::RunFrame() - Idle. Axis: ( %d, %d ) incoming data (%d, %d)", MyAxisState.x, MyAxisState.y, pSharedDataDebug->m_TranslationXYZAngleW.m_fx, pSharedDataDebug->m_TranslationXYZAngleW.m_fy);
						if (fabs(MyAxisState.x) > 0.0 || fabs(MyAxisState.y) > 0.0)
						{
							// if we have anything more than 0.0 then our VRGO has moved.
							// NOTE: Deadzone is already accounted for by this point.
							DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
							DEBUG_LOG(2, "CServerDriver::RunFrame() - Move. Axis: ( %d, %d )", MyAxisState.x, MyAxisState.y);
							bMove = true;
						}
						if (bMove)
						{
							// Update touch and press values if required.
							if (!pTrackedDevice->m_bTouched)
								ButtonTouched(pDriverHost, pTrackedDevice, true);
							if (!pTrackedDevice->m_bPressed && trackpadPressed)
								ButtonPressed(pDriverHost, pTrackedDevice, true);
						}
						else
						{
							if (!pTrackedDevice->m_bUnPressed && trackpadPressed)
								ButtonPressed(pDriverHost, pTrackedDevice, false);
							if (!pTrackedDevice->m_bUnTouched)
								ButtonTouched(pDriverHost, pTrackedDevice, false);
						}

						// update axis trackpad
						// This is where our VRGO values are actually send into Open VR.
						pDriverHost->SendTrackedDeviceAxisUpdated(pTrackedDevice->m_nID, 0, MyAxisState);
					}
				}
			}
		}
	}
	else if (activeLastFrame[0] || activeLastFrame[1])
	{
		// Before we turned off the vr locomotion, one of the controllers was still tracking and will continue to if we don't reset here.
		for (std::pair <ITrackedDeviceServerDriver*, CTrackedDeviceServerDriver*> element : pDriverHost->m_MapDeviceServerDriver)
		{
			CTrackedDeviceServerDriver* pTrackedDevice = (element.second);
			ETrackedPropertyError Error = ETrackedPropertyError::TrackedProp_Success;

			if (pTrackedDevice->m_nID != k_ulOverlayHandleInvalid && pTrackedDevice->m_DeviceClass == ETrackedDeviceClass::TrackedDeviceClass_Controller)
			{
				int nDataIndex = -1;
				if (pSharedData->m_nPadIndex[0] == pTrackedDevice->m_nID)  // Left controller
					nDataIndex = 0;
				if (pSharedData->m_nPadIndex[1] == pTrackedDevice->m_nID) // Right controller
					nDataIndex = 1;


				if (nDataIndex >= 0)
				{
					// pass if dont override trackpad
					if (activeLastFrame[nDataIndex])
					{
						pTrackedDevice->m_bTouched = false;
						pTrackedDevice->m_bPressed = false;
						pTrackedDevice->m_bUnTouched = true;
						pTrackedDevice->m_bUnPressed = true;

						// update axis trackpad
						VRControllerAxis_t resetState;
						resetState.x = 0;
						resetState.y = 0;
						pDriverHost->SendTrackedDeviceAxisUpdated(pTrackedDevice->m_nID, 0, resetState);
						// no longer active.
						activeLastFrame[nDataIndex] = false;
						continue;
					}
				}
			}
		}
	}
	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CServerDriver::RunFrame() End");
}

// ------------  Power State Functions ----------------------- //

/** Returns true if the driver wants to block Standby mode. */
bool CServerDriver::ShouldBlockStandbyMode()
{
	bool bRet = false;
	if (m_pDriverInstance->m_pChildServerTrackedDeviceProvider)
		bRet = m_pDriverInstance->m_pChildServerTrackedDeviceProvider->ShouldBlockStandbyMode();

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CServerDriver::RunFrame()=>%d", bRet);

	return false;
}


/** Called when the system is entering Standby mode. The driver should switch itself into whatever sort of low-power
* state it has. */
void CServerDriver::EnterStandby()
{
	if (m_pDriverInstance->m_pChildServerTrackedDeviceProvider)
		m_pDriverInstance->m_pChildServerTrackedDeviceProvider->EnterStandby();

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CServerDriver::EnterStandby()");
}


/** Called when the system is leaving Standby mode. The driver should switch itself back to
full operation. */
void CServerDriver::LeaveStandby()
{
	if (m_pDriverInstance->m_pChildServerTrackedDeviceProvider)
		m_pDriverInstance->m_pChildServerTrackedDeviceProvider->LeaveStandby();
	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CServerDriver::LeaveStandby()");
}
