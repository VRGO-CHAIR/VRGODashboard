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

//
#include "stdafx.h"
//#include <tchar.h>
////#include "3dRudderUnleashedVRDriver.h"
//#include <time.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string>
//#include <process.h>
//
//#include "ServerDriverHost.h"
//#include "ServerDriver.h"
//#include "WatchdogDriver.h"
//
//#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
//
//std::string GetLastErrorAsString()
//{
//	//Get the error message, if any.
//	DWORD errorMessageID = ::GetLastError();
//	if (errorMessageID == 0)
//		return std::string(); //No error message has been recorded
//
//	LPSTR messageBuffer = nullptr;
//	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
//
//	std::string message(messageBuffer, size);
//
//	//Free the buffer.
//	LocalFree(messageBuffer);
//
//	return message;
//}
//
//extern HMODULE g_hModule;
//
//CDriverInstance *g_pDriverInstance = nullptr;
//CDriverInstance g_DriverInstance;
//
//CSISharedData g_SharedData;
//CSISharedDataDebug g_SharedDataDebug;
//CSISharedMessageHandle g_SharedMessageHandleIn(_T("VRGO_SI_MESS_IN"));
//CSISharedMessageHandle g_SharedMessageHandleOut(_T("VRGO_SI_MESS_OUT"));
//
//void LoadChildDll()
//{
//	g_pDriverInstance = &g_DriverInstance;
//
//	if (!g_pDriverInstance->m_hChildDll)
//	{
//#if DEBUG_LOG_LEVEL>0
//
//		time_t rawtime;
//		struct tm *info;
//		char buffer[80];
//
//		time(&rawtime);
//		info = localtime(&rawtime);
//		strftime(buffer, 80, "%H%M%S-%d%m%y", info);
//
//		sprintf(g_pDriverInstance->m_LogFilePath, "c:\\log\\3dRSteamInjector-%s.html", buffer);
//
//		DEBUG_LOG_SETMODE(DEBUG_LOG_FILE | DEBUG_LOG_HTML | DEBUG_LOG_SHIFT_LEVEL | DEBUG_LOG_SHOW_LEVEL | DEBUG_LOG_SHOW_DTIME);
//		DEBUG_LOG_SETLEVEL(2);
//		DEBUG_LOG_INIT(g_pDriverInstance->m_LogFilePath);
//
//#endif
//
//		TCHAR sCurPath[_MAX_PATH];
//		GetCurrentDirectory(_MAX_PATH, sCurPath);
//
//		TCHAR sModulePath[_MAX_PATH];
//
//		GetModuleFileName(g_hModule, sModulePath, _MAX_PATH);
//
//		TCHAR drive[_MAX_DRIVE];
//		TCHAR dir[_MAX_DIR];
//		TCHAR fname[_MAX_FNAME];
//		TCHAR ext[_MAX_EXT];
//
//		_wsplitpath_s(sModulePath, drive, dir, fname, ext);
//
//		TCHAR  sDllPath[_MAX_PATH];
//		swprintf_s(sDllPath, _T("%ls%lsChildSteam.dll"), drive, dir);
//
//		TCHAR  sNewCurPath[_MAX_PATH];
//		swprintf_s(sNewCurPath, _T("%ls%ls"), drive, dir);
//
//
//		DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
//		DEBUG_LOG(1, "Dll Path %ls", sDllPath);
//
//		FILE *pFile;
//		_wfopen_s(&pFile, sDllPath, _T("rt"));
//		if (pFile)
//		{
//			DEBUG_LOG(1, "Dll Path Exist");
//			fclose(pFile);
//		}
//
//		SetCurrentDirectory(sNewCurPath);
//
//		g_pDriverInstance->m_hChildDll = LoadLibrary(sDllPath);
//		if (g_pDriverInstance->m_hChildDll)
//		{
//			g_pDriverInstance->m_pChildHmdDriverFactory = (ChildHmdDriverFactory)GetProcAddress(g_pDriverInstance->m_hChildDll, "HmdDriverFactory");
//			DEBUG_LOG_SETCOLOR(DEBUG_LOG_FLASHY_GREEN, DEBUG_LOG_BLACK);
//			DEBUG_LOG(1, "ChildSteam.dll(HmdDriverFactory) loader ! (%p)", g_pDriverInstance->m_pChildHmdDriverFactory);
//			DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
//		}
//		else
//		{
//			DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
//			DEBUG_LOG(1, "ChildSteam.dll(HmdDriverFactory) load Error ! %s(%0x8)", GetLastErrorAsString(), GetLastError());
//			DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
//		}
//		SetCurrentDirectory(sCurPath);
//
//		//g_pDriverInstance->m_pSdk = ns3dRudder::GetSDK();
//		//g_pDriverInstance->m_pSdk->Init();
//
//		g_pDriverInstance->m_pSharedData = &g_SharedData;
//		g_pDriverInstance->m_pSharedDataDebug = &g_SharedDataDebug;
//		g_pDriverInstance->m_pSharedMessageHandleIn = &g_SharedMessageHandleIn;
//		g_pDriverInstance->m_pSharedMessageHandleOut = &g_SharedMessageHandleOut;
//
//		DEBUG_LOG_SETCOLOR(DEBUG_LOG_FLASHY_GREEN, DEBUG_LOG_BLACK);
//		DEBUG_LOG(1, "3dRudderSDK Initalized !");
//		DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
//	}
//}
//
//HMD_DLL_EXPORT void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode)
//{
//	LoadChildDll();
//
//	CDriverInstance *pDriverInstance = g_pDriverInstance;
//
//	int ChildReturnCode;
//	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
//	DEBUG_LOG(1, "HmdDriverFactory %s", pInterfaceName);
//
//	void *pRetChild = pDriverInstance->m_pChildHmdDriverFactory(pInterfaceName, &ChildReturnCode);
//
//	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
//	DEBUG_LOG(1, "g_pChildHmdDriverFactory()=>%p", pRetChild);
//
//	if (0 == strcmp(IServerTrackedDeviceProvider_Version, pInterfaceName))
//	{
//		DEBUG_LOG(1, "%s Inizzzzzz 64!", pInterfaceName);
//
//		pDriverInstance->m_pChildServerTrackedDeviceProvider = (IServerTrackedDeviceProvider*)pRetChild;
//		pDriverInstance->m_pServerTrackedDeviceProvider = new CServerDriver(pDriverInstance);
//		return pDriverInstance->m_pServerTrackedDeviceProvider;
//	}
//
//	if (0 == strcmp(IVRWatchdogProvider_Version, pInterfaceName))
//	{
//		DEBUG_LOG(1, "%s Inizzzzzz 32!", pInterfaceName);
//
//		pDriverInstance->m_pChildWatchdogTrackedDeviceProvider = (IVRWatchdogProvider*)pRetChild;
//		pDriverInstance->m_pWatchdogTrackedDeviceProvider = new CWatchdogDriver(pDriverInstance);
//		return pDriverInstance->m_pWatchdogTrackedDeviceProvider;
//	}
//
//	if (pRetChild)
//	{
//		DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
//		DEBUG_LOG(1, "%s Ignored !", pInterfaceName);
//	}
//
//	if (pReturnCode)
//		*pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
//
//	return NULL;
//}