// dllmain.cpp : Defines the entry point for the DLL application.
//#include "stdafx.h"
//#include "RawInput.h"
//
//
//
//HMODULE g_hModule;
//
//HHOOK hook;
//HHOOK hKeyboardHook;
//
//// Forward declaration
//LRESULT CALLBACK VRGORawInputProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//	switch (nCode)
//	{
//		case WM_INPUT:
//			//
//			// Get the pointer to the raw device data, process it and update the window
//			//
//
//			PRAWINPUT pRawInput;
//			UINT      bufferSize;
//			HANDLE    hHeap;
//
//			RawInput::bSuccessfulHook = true;
//
//			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
//
//			hHeap = GetProcessHeap();
//			pRawInput = (PRAWINPUT)HeapAlloc(hHeap, 0, bufferSize);
//			if (!pRawInput)
//				return 0;
//
//			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, pRawInput, &bufferSize, sizeof(RAWINPUTHEADER));
//			RawInput::ParseRawInput(pRawInput);
//
//			HeapFree(hHeap, 0, pRawInput);
//			break;
//	}
//	return CallNextHookEx(hook, nCode, wParam, lParam);
//}
//
//void UpdateKeyState(BYTE* keyState, int keycode)
//{
//	keyState[keycode] = GetKeyState(keycode);
//}
//
//LRESULT CALLBACK VRGORawInputKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
//{
//	//WPARAM is WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP
//	//LPARAM is the key information
//
//	MessageBoxA(NULL, "Message", "KeyPressed!", MB_ICONINFORMATION);
//
//	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
//	//KBDLLHOOKSTRUCT cKey = *((KBDLLHOOKSTRUCT*)lParam);
//	//
//	//wchar_t buffer[5];
//
//	//// get the keyboard state
//	//BYTE keyboard_state[256];
//	//GetKeyboardState(keyboard_state);
//	//UpdateKeyState(keyboard_state, VK_SHIFT);
//	//UpdateKeyState(keyboard_state, 0x57); // W key
//	//UpdateKeyState(keyboard_state, 0x41); // A key
//	//UpdateKeyState(keyboard_state, 0x53); // S key
//	//UpdateKeyState(keyboard_state, 0x44); // D key
//	//UpdateKeyState(keyboard_state, VK_SHIFT);
//	//
//	//if (keyboard_state[0x57] || keyboard_state[0x44] || keyboard_state[0x41] || keyboard_state[0x53])
//	//{
//	//	RawInput::lAxisX = 5000.0f;
//	//}
//
//	//// Get the keyboard layout
//	//HKL keyboard_layout = GetKeyboardLayout(0);
//	//
//	//// Get the name
//	//char lpszName[0x100] = { 0 };
//
//	//DWORD dwMsg = 1;
//	//dwMsg += cKey.scanCode << 16;
//	//dwMsg += cKey.flags << 24;
//
//	//int i = GetKeyNameText(dwMsg, (LPTSTR)lpszName, 255);
//
//	//// Try to convert the key info.
//	//int result = ToUnicodeEx(cKey.vkCode, cKey.scanCode, keyboard_state, buffer, 4, 0,keyboard_layout);
//	//buffer[4] = L'\0';
//
//	//// Can print they key code here with cKey.vkCode + " " + convert from utf16((ushort*)buffer)
//
//}


//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//					 )
//{
//	g_hModule = hModule;
//	switch (ul_reason_for_call)
//	{
//	case DLL_PROCESS_ATTACH:
//	//hook = SetWindowsHookEx(WH_GETMESSAGE, VRGORawInputProc, NULL, 0);
//		//LPWSTR lpFileName = NULL;
//		//GetModuleFileName(g_hModule, lpFileName, 256);
//		//MessageBoxA(NULL, "DLL ATTACHED", ":)", MB_ICONINFORMATION); }
//		//hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, VRGORawInputKeyboardProc, NULL, 0); }
//		//break;
//		//RawInput::bSuccessfulHook = true;
//	case DLL_THREAD_ATTACH:
//	case DLL_THREAD_DETACH:
//	case DLL_PROCESS_DETACH:
//		break;
//	}
//	return TRUE;
//}

