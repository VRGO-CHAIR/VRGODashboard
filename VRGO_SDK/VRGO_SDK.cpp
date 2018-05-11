// VRGO_SDK.cpp : Defines the entry point for the application.
//

// Copyright VRGO ltd 2018.

#include "stdafx.h"
#include <process.h>
#include <atlstr.h>
#include <time.h>
#include "RawInput.h"
#include "../Common/SharedData.h"
#include "VRGO_SDK.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

CSISharedMessageHandle g_SharedMessageHandleIn(_T("VRGO_SI_MESS_IN"));
CSISharedMessageHandle g_SharedMessageHandleOut(_T("VRGO_SI_MESS_OUT"));

CVRGOSDK vrgoSDK; // The one and only.

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VRGO_SDK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VRGO_SDK));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

BOOL g_bThreadGoOut = FALSE;


void OnMemoryMapManager(void *_pDataTh)
{
	CSharedMemory SharedMemory;
	CSharedMemory SharedMemoryDebug;

	if (SharedMemoryDebug.Open(_T("VRGO_SI_DATA_DEBUG"), sizeof(CSISharedDataDebug)))
	{
	}

	if (!SharedMemory.Open(_T("VRGO_SI_DATA"), sizeof(CSISharedData)))
	{
	}

	while (!g_bThreadGoOut)
	{
		SharedMemory.Read((uint8_t*)&vrgoSDK.m_SharedData, sizeof(CSISharedData));
		SharedMemoryDebug.Write((uint8_t*)&vrgoSDK.m_pSharedDataDebug, sizeof(CSISharedDataDebug));
		
		CSI_Message *pMessage;
		while (pMessage = vrgoSDK.m_pSharedMessageHandleIn->PopMessage())
		{
			switch (pMessage->m_nType)
			{
			//case CSiMessageeType::Reset:
			//{
			//	CSI_MessageReset *pMessageReset = (CSI_MessageReset*)pMessage;
			//	//if (pMessageReset->m_bReset)
			//		//pMovement->Reset();
			//}
			//break;
			//case CSiMessageeType::Calibration:
			//{
			//	CSI_MessageCalibration *pMessageCalib = (CSI_MessageCalibration*)pMessage;
			//	/*if (pMessageCalib->m_bCalib)
			//	{
			//		pMovement->m_bInCalibration = true;
			//		pMovement->Reset(true);
			//	}*/
			//	/*else
			//	{
			//		memcpy(&pMovement->m_mMatrix34Hmd, &pMessageCalib->m_matrix, sizeof(CSISharedData::Matrix34));
			//		pMovement->m_bAskCalibration = true;
			//		pMovement->m_bInCalibration = false;
			//	}*/
			//}
			default:
				break;
			}

			delete pMessage;
		}
		Sleep(1);
	}
	SharedMemoryDebug.Close();
	SharedMemory.Close();
	_endthread();
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VRGO_LOGO_ICON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_VRGO_SDK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_VRGO_LOGO_ICON));
	
    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   _beginthread(OnMemoryMapManager, 0, nullptr);


   //ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

inline float clamp(float x, float a, float b)
{
	return x < a ? a : (x > b ? b : x);
}

clock_t clockOfLastPush;
double clockDuration;
void PushInputDataToSharedData()
{
	if (clockOfLastPush)
	clockDuration = (clock() - clockOfLastPush) / (double)CLOCKS_PER_SEC;

	clockOfLastPush = clock();
	CSISharedData* pSharedData = &vrgoSDK.m_SharedData;

	CSharedMemory SharedMemoryDebug;
	if (SharedMemoryDebug.Open(_T("VRGO_SI_DATA_DEBUG"), sizeof(CSISharedDataDebug)))
	{
		//DEBUG_LOG_SETCOLOR(DEBUG_LOG_RED, DEBUG_LOG_BLACK);
		//DEBUG_LOG(1, "Fail to open memory map debug");
		//DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	}

	SharedMemoryDebug.Read((uint8_t*)&vrgoSDK.m_pSharedDataDebug, sizeof(CSISharedDataDebug));

	 
	CSISharedDataDebug *pSharedDataDebug = &vrgoSDK.m_pSharedDataDebug;

	CString string;
	string.Format(_T("VRGO DATA: X: %ld Y: %ld Z: %ld RZ: %ld\n Time since last push: %d"), RawInput::lAxisX, RawInput::lAxisY, RawInput::lAxisZ, RawInput::lAxisRz, clockDuration);
	OutputDebugString(string);
	pSharedDataDebug->m_TranslationXYZAngleW.SetVector(RawInput::axisInfo.lAxisX, RawInput::axisInfo.lAxisY, RawInput::axisInfo.lAxisRz, RawInput::axisInfo.lAxisZ);
	pSharedDataDebug->m_RawXYZAngle.SetVector(RawInput::rawAxisInfo.lAxisX, RawInput::rawAxisInfo.lAxisY, RawInput::rawAxisInfo.lAxisRz, RawInput::rawAxisInfo.lAxisZ);
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		RAWINPUTDEVICE rid;

		rid.usUsagePage = 1;
		rid.usUsage = 4;	// Joystick
		rid.dwFlags = RIDEV_EXINPUTSINK; // This flag ensures that we receive input whilst in background.
		rid.hwndTarget = hWnd;

		if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE)))
			return -1;

		vrgoSDK.m_pSharedMessageHandleIn = &g_SharedMessageHandleIn;
		vrgoSDK.m_pSharedMessageHandleOut = &g_SharedMessageHandleOut;
	}

	

	case WM_INPUT:
	{
		//return 0;
		//
		// Get the pointer to the raw device data, process it and update the window
		//

		PRAWINPUT pRawInput;
		UINT      bufferSize;
		HANDLE    hHeap;
		
		clock_t start;
		double duration;

		start = clock();

		// Call raw input data to get the required buffer size.
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));

		// Allocated the buffer size on the heap.
		hHeap = GetProcessHeap();
		pRawInput = (PRAWINPUT)HeapAlloc(hHeap, 0, bufferSize);
		if (!pRawInput)
			return 0;

		// Now we've allocated, get the actual input data passing in the allocated buffer.
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, pRawInput, &bufferSize, sizeof(RAWINPUTHEADER));
		
		// Parse the raw input into something manageable for the VRGO drivers.
		RawInput::ParseRawInput(pRawInput);

		// Manipulate the new values using the curve from the config dashboard utility.
		RawInput::axisInfo.lAxisX = vrgoSDK.m_SharedData.m_Curve[0].CalcCurveValue(RawInput::axisInfo.lAxisX);
		RawInput::axisInfo.lAxisY = vrgoSDK.m_SharedData.m_Curve[1].CalcCurveValue(RawInput::axisInfo.lAxisY);


		duration = (clock() - start) / (double)CLOCKS_PER_SEC;

		// Push the latest input data into the shared memory for the driver and config window to access.
		PushInputDataToSharedData();

		HeapFree(hHeap, 0, pRawInput);

		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
	}
	return 0;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

