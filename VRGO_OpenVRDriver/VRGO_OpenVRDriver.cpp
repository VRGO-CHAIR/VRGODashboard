// VRGO_OpenVRDriver.cpp : Defines the exported functions for the DLL application.
//

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

// Modifications made by Samuel Pike for VRGO Ltd.
// Copyright(c) VRGO ltd 2018.

#include "stdafx.h"
#include <tchar.h>
#include "VRGO_OpenVRDriver.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <process.h>


#include "ServerDriver.h"
#include "WatchdogDriver.h"

#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )

std::string GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

CDriverInstance g_DriverInstance;

class CMemmoryMapThreadData
{
public:
	CMemmoryMapThreadData()
	{
		m_bThreadGoOut = FALSE;
		m_pSharedMessageHandleIn = new CSISharedMessageHandle(_T("VRGO_SI_MESS_IN"));
		m_pSharedMessageHandleOut = new CSISharedMessageHandle(_T("VRGO_SI_MESS_OUT"));
	}

	~CMemmoryMapThreadData()
	{
		m_bThreadGoOut = TRUE;
		delete m_pSharedMessageHandleIn;
		delete m_pSharedMessageHandleOut;
	}
	HMODULE m_hModule;
	BOOL m_bThreadGoOut;
	CSISharedData m_SharedData;
	CSISharedDataDebug m_SharedDataDebug;
	CSISharedMessageHandle *m_pSharedMessageHandleIn;
	CSISharedMessageHandle *m_pSharedMessageHandleOut;
};


CMemmoryMapThreadData g_MemoryMapData;


void LoadChildDll(CDriverInstance *pDriverInstance)
{
	if (!pDriverInstance->m_hChildDll)
	{
#if DEBUG_LOG_LEVEL>0

		time_t rawtime;
		struct tm *info = new struct tm();
		char buffer[80];

		time(&rawtime);
		localtime_s(info, &rawtime);
		strftime(buffer, 80, "%H%M%S-%d%m%y", info);

		sprintf_s(pDriverInstance->m_LogFilePath, "c:\\log\\VRGOSteamInjector-%s.html", buffer);

		DEBUG_LOG_SETMODE(DEBUG_LOG_FILE | DEBUG_LOG_HTML | DEBUG_LOG_SHIFT_LEVEL | DEBUG_LOG_SHOW_LEVEL | DEBUG_LOG_SHOW_DTIME);
		DEBUG_LOG_SETLEVEL(0);
		DEBUG_LOG_INIT(pDriverInstance->m_LogFilePath);

#endif

		TCHAR sCurPath[_MAX_PATH];
		GetCurrentDirectory(_MAX_PATH, sCurPath);

		TCHAR sModulePath[_MAX_PATH];

		GetModuleFileName(pDriverInstance->m_hParentModule, sModulePath, _MAX_PATH);

		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];

		_wsplitpath_s(sModulePath, drive, dir, fname, ext);

		TCHAR  sDllPath[_MAX_PATH];
		swprintf_s(sDllPath, _T("%ls%lsChildSteam.dll"), drive, dir);

		TCHAR  sNewCurPath[_MAX_PATH];
		swprintf_s(sNewCurPath, _T("%ls%ls"), drive, dir);


		DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
		DEBUG_LOG(1, "Dll Path %ls", sDllPath);

		FILE *pFile;
		_wfopen_s(&pFile, sDllPath, _T("rt"));
		if (pFile)
		{
			DEBUG_LOG(1, "Dll Path Exist");
			fclose(pFile);
		}

		SetCurrentDirectory(sNewCurPath);

		pDriverInstance->m_hChildDll = LoadLibrary(sDllPath);
		if (pDriverInstance->m_hChildDll)
		{
			pDriverInstance->m_pChildHmdDriverFactory = (ChildHmdDriverFactory)GetProcAddress(pDriverInstance->m_hChildDll, "HmdDriverFactory");
			DEBUG_LOG_SETCOLOR(DEBUG_LOG_FLASHY_GREEN, DEBUG_LOG_BLACK);
			DEBUG_LOG(1, "ChildSteam.dll(HmdDriverFactory) loader ! (%p)", pDriverInstance->m_pChildHmdDriverFactory);
			DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
		}
		else
		{
			DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
			DEBUG_LOG(1, "ChildSteam.dll(HmdDriverFactory) load Error ! %s(%0x8)", GetLastErrorAsString(), GetLastError());
			DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
		}
		SetCurrentDirectory(sCurPath);

		//g_pDriverInstance->m_pSdk = ns3dRudder::GetSDK();
		//g_pDriverInstance->m_pSdk->Init();

		pDriverInstance->m_pSharedData = &g_MemoryMapData.m_SharedData;
		pDriverInstance->m_pSharedDataDebug = &g_MemoryMapData.m_SharedDataDebug;
		pDriverInstance->m_pSharedMessageHandleIn = g_MemoryMapData.m_pSharedMessageHandleIn;
		pDriverInstance->m_pSharedMessageHandleOut = g_MemoryMapData.m_pSharedMessageHandleOut;

		DEBUG_LOG_SETCOLOR(DEBUG_LOG_FLASHY_GREEN, DEBUG_LOG_BLACK);
		DEBUG_LOG(1, "VRGOSDK Initalized !");
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	}
}

HMD_DLL_EXPORT void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode)
{
	CDriverInstance *pDriverInstance = &g_DriverInstance;

	LoadChildDll(pDriverInstance);

	int ChildReturnCode = vr::VRInitError_None;
	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "HmdDriverFactory %s", pInterfaceName);

	void *pRetChild = pDriverInstance->m_pChildHmdDriverFactory(pInterfaceName, &ChildReturnCode);

	
	if (pRetChild)
	{
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
		DEBUG_LOG(1, "g_pChildHmdDriverFactory()=>%p", pRetChild);

		if (pReturnCode)
			*pReturnCode = ChildReturnCode;

		if (0 == strcmp(IServerTrackedDeviceProvider_Version, pInterfaceName))
		{
			DEBUG_LOG(1, "%s Inizzzzzz 64!", pInterfaceName);

			pDriverInstance->m_pChildServerTrackedDeviceProvider = (IServerTrackedDeviceProvider*)pRetChild;
			pDriverInstance->m_pServerTrackedDeviceProvider = new CServerDriver(pDriverInstance);
			return pDriverInstance->m_pServerTrackedDeviceProvider;
		}
		else
		{
			// Wrong version ! 
			CSI_MessageWrongVersionOpenVR mi;
			mi.m_hModule = pDriverInstance->m_hChildDll;
			_tcscpy(mi.m_sModuleName, pDriverInstance->m_sModuleName);
			pDriverInstance->m_bOpenVrError = true;
			pDriverInstance->m_pSharedMessageHandleOut->PushMessage(&mi);
		}

		if (0 == strcmp(IVRWatchdogProvider_Version, pInterfaceName))
		{
			DEBUG_LOG(1, "%s Inizzzzzz 32!", pInterfaceName);

			pDriverInstance->m_pChildWatchdogTrackedDeviceProvider = (IVRWatchdogProvider*)pRetChild;
			pDriverInstance->m_pWatchdogTrackedDeviceProvider = new CWatchdogDriver(pDriverInstance);
			return pDriverInstance->m_pWatchdogTrackedDeviceProvider;
		}

		if (pRetChild)
		{
			DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
			DEBUG_LOG(1, "%s Ignored !", pInterfaceName);
		}

		if (pReturnCode)
			*pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
	}
	else
	{
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
		DEBUG_LOG(1, "HmdDriverFactory(%s) Error !", pInterfaceName);
	}

	return NULL;
}

void OnMemoryMapManager(void *_pDataTh)
{
	CMemmoryMapThreadData *pThData = (CMemmoryMapThreadData*)_pDataTh;

	CSharedMemory SharedMemory;
	CSharedMemory SharedMemoryDebug;
	CSharedMemory SharedDriverInfo;

	if (SharedMemoryDebug.Open(_T("VRGO_SI_DATA_DEBUG"), sizeof(CSISharedDataDebug)))
	{
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
		DEBUG_LOG(1, "Fail to open memory map debug");
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	}

	if (!SharedMemory.Open(_T("VRGO_SI_DATA"), sizeof(CSISharedData)))
	{
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
		DEBUG_LOG(1, "Fail to open memory map");
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	}

	HMODULE hSelectedModule = NULL;

	while (!pThData->m_bThreadGoOut)
	{
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
		DEBUG_LOG(1, "Reading Memory Map");
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
		if (!SharedMemory.Read((uint8_t*)&pThData->m_SharedData, sizeof(CSISharedData)))
		{
			DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
			DEBUG_LOG(1, "FAILED TO READ MEMORY MAP (%d)", GetLastError());
			DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
		}
		SharedMemoryDebug.Read((uint8_t*)&pThData->m_SharedDataDebug, sizeof(CSISharedDataDebug));

		//CDriverInstance *pDriverInstance = &g_DriverInstance;


		//CSI_Message *pMessage;
		//while (pMessage = pThData->m_pSharedMessageHandleIn->PopMessage())
		//{
		//	if (pDriverInstance)
		//	{
		//		/*CMovement *pMovement = &pDriverInstance->m_Movement;
		//		switch (pMessage->m_nType)
		//		{
		//		case CwsSiMessageeType::Reset:
		//		{
		//			CSI_MessageReset *pMessageReset = (CSI_MessageReset*)pMessage;
		//			if (pMessageReset->m_bReset)
		//				pMovement->Reset();
		//			DEBUG_LOG_SETCOLOR(DEBUG_LOG_GREEN, DEBUG_LOG_BLACK);
		//			DEBUG_LOG(5, "Th CwsSiMessageeType::Reset %d", pMessageReset->m_bReset);
		//			DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
		//		}
		//		break;
		//		case CwsSiMessageeType::Calibration:
		//		{
		//			CSI_MessageCalibration *pMessageCalib = (CSI_MessageCalibration*)pMessage;
		//			DEBUG_LOG_SETCOLOR(DEBUG_LOG_GREEN, DEBUG_LOG_BLACK);
		//			DEBUG_LOG(5, "Th CwsSiMessageeType::Calibration %d", pMessageCalib->m_bCalib);
		//			DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);

		//			if (pMessageCalib->m_bCalib)
		//			{
		//				pMovement->m_bInCalibration = true;
		//				pMovement->Reset(true);
		//			}
		//			else
		//			{
		//				memcpy(&pMovement->m_mMatrix34Hmd, &pMessageCalib->m_matrix, sizeof(CSISharedData::Matrix34));
		//				pMovement->m_bAskCalibration = true;
		//				pMovement->m_bInCalibration = false;
		//			}
		//		}
		//		default:
		//			break;
		//		}*/
		//	}
		//	delete pMessage;
		//}
		Sleep(200);
	}
	DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "ENDING ON MEMORY MAP MANAGER THREAD");
	_endthread();
}

void CreateMemoryMapManagerThread()
{
	_beginthread(OnMemoryMapManager, 0, &g_MemoryMapData);
}

void ExitMemoryMapManagerThread()
{
	g_MemoryMapData.m_bThreadGoOut = TRUE;
}

char g_LogFilePath[_MAX_PATH];

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_DriverInstance.m_hParentModule = hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}


	return TRUE;
}

