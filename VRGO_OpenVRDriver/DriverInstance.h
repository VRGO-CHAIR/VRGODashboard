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


#include "Movement.h"


using namespace vr;
typedef void *(*ChildHmdDriverFactory)(const char *pInterfaceName, int *pReturnCode);


class CDriverInstance
{
public:
	CSISharedData *m_pSharedData;
	CSISharedDataDebug *m_pSharedDataDebug;
	CSISharedMessageHandle *m_pSharedMessageHandleIn;
	CSISharedMessageHandle *m_pSharedMessageHandleOut;



	class CServerDriver *m_pServerTrackedDeviceProvider;
	class CWatchdogDriver *m_pWatchdogTrackedDeviceProvider;

	IVRWatchdogProvider *m_pChildWatchdogTrackedDeviceProvider;
	IServerTrackedDeviceProvider *m_pChildServerTrackedDeviceProvider;

	HMODULE m_hChildDll;
	HMODULE m_hParentModule;

	bool m_bServerHostCreated;
	bool m_bServerHostActivated;
	bool m_bActivated;
	bool m_bOpenVrError;

	ChildHmdDriverFactory m_pChildHmdDriverFactory;
	ChildHmdDriverFactory m_pChildHmdDriverFactoryCopy;

	char m_LogFilePath[_MAX_PATH];
	TCHAR m_sModuleName[_MAX_PATH];

	CMovement m_pMovement;

	CDriverInstance();
	~CDriverInstance();

	IVRSettings *m_pVRSettings;
	IVRProperties *m_pVRProperties;
	CVRPropertyHelpers *m_pVRPropertyHelpers;
	CVRHiddenAreaHelpers *m_pVRHiddenAreaHelpers;
	IVRDriverLog *m_pVRDriverLog;
	IVRServerDriverHost *m_pVRServerDriverHost;
	class IVRGOVRServerDriverHost *m_pVRGOVRServerDriverHost;
	IVRWatchdogHost *m_pVRWatchdogHost;
	IVRDriverManager *m_pVRDriverManager;
	IVRResources *m_pVRResources;
	IVRDriverInput *m_pVRDriverInput;

	void *GetContextInterface(const char *pchInterfaceVersion, EVRInitError *peError);

	IVRSettings * VRSettings();
	IVRProperties * VRPropertiesRaw();
	CVRPropertyHelpers * VRProperties();
	CVRHiddenAreaHelpers * VRHiddenArea();
	IVRDriverLog * VRDriverLog();
	IVRServerDriverHost * VRServerDriverHost();
	IVRGOVRServerDriverHost *VRVRGOServerDriverHost();

	IVRWatchdogHost * VRWatchdogHost();
	DriverHandle_t  VRDriverHandle();
	IVRDriverManager * VRDriverManager();
	IVRResources * VRResources();
	IVRDriverInput * VRDriverInput();
};

bool CheckInterface(const char *sAskedVerion, const char *sCurVersion, bool &bVersionOK);

