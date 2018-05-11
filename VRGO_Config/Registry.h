//
//  MODULE:   Registry.h
//
//	AUTHOR: Carlos Antollini 
//
//  mailto: cantollini@hotmail.com
//
//	Date: 08/04/2001
//
//	Version 1.01
// 

#include "stdafx.h"
#include <stdint.h>


class CRegistry
{
public:
	enum cregRestoreEnum
	{
		regVolatile = REG_WHOLE_HIVE_VOLATILE,
		regRefresh = REG_REFRESH_HIVE,
		regNoLazy = REG_NO_LAZY_FLUSH
	};
#ifdef _WIN64
	enum Keys
	{
		classesRoot = (int64_t)HKEY_CLASSES_ROOT,
		currentUser = (int64_t)HKEY_CURRENT_USER,
		localMachine = (int64_t)HKEY_LOCAL_MACHINE,
		currentConfig = (int64_t)HKEY_CURRENT_CONFIG,
		users = (int64_t)HKEY_USERS,
		performanceData = (int64_t)HKEY_PERFORMANCE_DATA,	//Windows NT/2000
		dynData = (int64_t)HKEY_DYN_DATA						//Windows 95/98
	};
#else
	enum Keys
	{
		classesRoot = (int)HKEY_CLASSES_ROOT,
		currentUser = (int)HKEY_CURRENT_USER,
		localMachine = (int)HKEY_LOCAL_MACHINE,
		currentConfig = (int)HKEY_CURRENT_CONFIG,
		users = (int)HKEY_USERS,
		performanceData = (int)HKEY_PERFORMANCE_DATA,	//Windows NT/2000
		dynData = (int)HKEY_DYN_DATA						//Windows 95/98
	};
#endif 

	CRegistry()
		{
			m_hKey = NULL;
		};

	~CRegistry()
		{
			CloseKey();
			m_hKey = NULL;
		};

	BOOL OpenKey(enum Keys hKey, LPCTSTR szKey);
	BOOL CreateKey(enum Keys hKey, LPCTSTR szKey);
	BOOL DeleteKey(enum Keys hKey, LPCTSTR szKey);
	BOOL DeleteValue(LPCTSTR lpValueName);
	BOOL GetValue(LPCTSTR lpValueName, CString& strValue);
	BOOL GetValue(LPCTSTR lpValueName, DWORD& dwValue);
	BOOL SetValue(LPCTSTR lpValueName, LPCTSTR lpData);
	BOOL SetValue(LPCTSTR lpValueName, DWORD dwValue);
	BOOL SaveKey(LPCTSTR lpszFileName);
	BOOL RestoreKey(LPCTSTR lpszFileName, DWORD dwFlag);
	BOOL LoadKey(enum Keys hKey, LPCTSTR lpszSubKey, LPCTSTR lpszFileName);
	void CloseKey();	

protected:
	HKEY m_hKey;
};