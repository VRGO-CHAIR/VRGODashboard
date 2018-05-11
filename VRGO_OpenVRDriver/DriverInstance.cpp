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
#include "DriverInstance.h"
#include "ServerDriver.h"
#include "DriverContext.h"
#include "WatchdogDriver.h"
#include "openvr_driver.h"
#include "IVRGOVRServerDriverHost.h"

CDriverInstance::CDriverInstance()
{
	m_pSharedData = nullptr;
	m_pSharedDataDebug = nullptr;
	m_pSharedMessageHandleIn = nullptr;
	m_pSharedMessageHandleOut = nullptr;

	m_pServerTrackedDeviceProvider = nullptr;
	m_pWatchdogTrackedDeviceProvider = nullptr;

	m_pChildWatchdogTrackedDeviceProvider = nullptr;
	m_pChildServerTrackedDeviceProvider = nullptr;

	m_hChildDll = nullptr;
	m_hParentModule = nullptr;

	m_bServerHostCreated = false;
	m_bServerHostActivated = false;
	m_bActivated = false;
	m_bOpenVrError = false;

	m_pVRSettings = nullptr;
	m_pVRProperties = nullptr;
	m_pVRPropertyHelpers = nullptr;
	m_pVRHiddenAreaHelpers = nullptr;
	m_pVRDriverLog = nullptr;
	m_pVRServerDriverHost = nullptr;
	m_pVRWatchdogHost = nullptr;
	m_pVRDriverManager = nullptr;
	m_pVRResources = nullptr;
	m_pVRDriverInput = nullptr;

}

CDriverInstance::~CDriverInstance()
{
	if (m_pServerTrackedDeviceProvider)
		delete m_pServerTrackedDeviceProvider;
	if (m_pWatchdogTrackedDeviceProvider)
		delete m_pWatchdogTrackedDeviceProvider;
	if (m_pVRPropertyHelpers)
		delete m_pVRPropertyHelpers;
	if (m_pVRHiddenAreaHelpers)
		delete m_pVRHiddenAreaHelpers;
}

void *CDriverInstance::GetContextInterface(const char *pchInterfaceVersion, EVRInitError *peError)
{
	return m_pServerTrackedDeviceProvider->m_pDriverContext->GetGenericInterface(pchInterfaceVersion, peError);
}

IVRSettings * CDriverInstance::VRSettings()
{
	EVRInitError eError;
	if (!m_pVRSettings) m_pVRSettings = (IVRSettings *)GetContextInterface(IVRSettings_Version, &eError);
	return m_pVRSettings;
}

IVRProperties * CDriverInstance::VRPropertiesRaw()
{
	EVRInitError eError;
	if (!m_pVRProperties) m_pVRProperties = (IVRProperties *)GetContextInterface(IVRProperties_Version, &eError);
	m_pVRPropertyHelpers = new CVRPropertyHelpers(m_pVRProperties);
	m_pVRHiddenAreaHelpers = new CVRHiddenAreaHelpers(m_pVRProperties);
	return m_pVRProperties;
}
CVRPropertyHelpers * CDriverInstance::VRProperties()
{
	VRPropertiesRaw();
	return m_pVRPropertyHelpers;
}

CVRHiddenAreaHelpers * CDriverInstance::VRHiddenArea()
{
	VRPropertiesRaw();
	return m_pVRHiddenAreaHelpers;
}

IVRDriverLog * CDriverInstance::VRDriverLog()
{
	EVRInitError eError;
	if (!m_pVRDriverLog) m_pVRDriverLog = (IVRDriverLog *)GetContextInterface(IVRDriverLog_Version, &eError);
	return m_pVRDriverLog;
}

IVRServerDriverHost * CDriverInstance::VRServerDriverHost()
{
	EVRInitError eError;
	if (!m_pVRServerDriverHost) m_pVRServerDriverHost = (IVRServerDriverHost *)GetContextInterface(IVRServerDriverHost_Version, &eError);
	return m_pVRServerDriverHost;
}

IVRGOVRServerDriverHost *CDriverInstance::VRVRGOServerDriverHost()
{
	VRServerDriverHost();
	return m_pVRGOVRServerDriverHost;
}

IVRWatchdogHost * CDriverInstance::VRWatchdogHost()
{
	EVRInitError eError;
	if (!m_pVRWatchdogHost) m_pVRWatchdogHost = (IVRWatchdogHost *)GetContextInterface(IVRWatchdogHost_Version, &eError);
	return m_pVRWatchdogHost;
}

DriverHandle_t  CDriverInstance::VRDriverHandle()
{
	return m_pServerTrackedDeviceProvider->m_pDriverContext->GetDriverHandle();
}

IVRDriverManager * CDriverInstance::VRDriverManager()
{
	EVRInitError eError;
	if (!m_pVRDriverManager) m_pVRDriverManager = (IVRDriverManager *)GetContextInterface(IVRDriverManager_Version, &eError);
	return m_pVRDriverManager;
}

IVRResources * CDriverInstance::VRResources()
{
	EVRInitError eError;
	if (!m_pVRResources) m_pVRResources = (IVRResources *)GetContextInterface(IVRResources_Version, &eError);
	return m_pVRResources;
}

IVRDriverInput * CDriverInstance::VRDriverInput()
{
	EVRInitError eError;
	if (!m_pVRDriverInput) m_pVRDriverInput = (IVRDriverInput *)GetContextInterface(IVRDriverInput_Version, &eError);
	return m_pVRDriverInput;
}



bool CheckInterface(const char *sAskedVerion, const char *sCurVersion, bool &bVersionOK)
{
	int nVerIndex = (int)strlen(sAskedVerion) - 4;
	if (strncmp(sAskedVerion, sCurVersion, nVerIndex) == 0)
	{
		if (strcmp(sAskedVerion + nVerIndex, sCurVersion + nVerIndex) == 0)
			bVersionOK = true;
		else
			bVersionOK = false;
		return true;
	}
	return false;
}