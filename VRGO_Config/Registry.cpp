#include "stdafx.h"
//
//  MODULE:   Registry.cpp
//
//	AUTHOR: Carlos Antollini 
//
//  mailto: cantollini@hotmail.com
//
//	Date: 08/04/2001
//
//	Version 1.01
// 

#include "registry.h"

BOOL CRegistry::OpenKey(enum Keys hKey, LPCTSTR szKey)
{	
	if(RegOpenKeyEx((HKEY)hKey,szKey, 0, KEY_ALL_ACCESS, &m_hKey) == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		m_hKey = NULL;
		return FALSE;
	}
}

BOOL CRegistry::CreateKey(enum Keys hKey, LPCTSTR szKey)
{	
	if(RegCreateKeyEx((HKEY)hKey,szKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey, NULL) == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		m_hKey = NULL;
		return FALSE;
	}
}

BOOL CRegistry::SetValue(LPCTSTR lpValueName, LPCTSTR lpData)
{
	ASSERT(m_hKey != NULL);

	DWORD dwType = REG_SZ;
#ifdef _UNICODE
	int nLen = (int)_tcslen(lpData)*2+2;
#else
	int nLen = _tcslen(lpData);
#endif // 

	if(::RegSetValueEx(m_hKey, lpValueName, 0, dwType, (LPBYTE)(LPCTSTR)lpData,nLen) == ERROR_SUCCESS)
	{
		::RegFlushKey(m_hKey);
		return TRUE;
	}
	return FALSE;
}

BOOL CRegistry::SetValue(LPCTSTR lpValueName, DWORD dwValue)
{
	ASSERT(m_hKey != NULL);
	
	DWORD dwType = REG_DWORD;
	
	if(::RegSetValueEx(m_hKey, lpValueName, 0, dwType, (LPBYTE)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS)
	{
		::RegFlushKey(m_hKey);
		return TRUE;
	}
	return FALSE;
}


BOOL CRegistry::GetValue(LPCTSTR lpValueName, CString& strValue)
{
	BOOL bRet = FALSE;
	LPSTR lpstrValue;
	DWORD dwType = REG_SZ;
	DWORD lpcbData;
	
	bRet = FALSE;	
	lpstrValue = (LPSTR)malloc(sizeof(char) * 256);
	memset(lpstrValue, 0, sizeof(char) * 256);
	lpcbData = 1024;
		
	if(::RegQueryValueEx(m_hKey,
		lpValueName,
		NULL,
		&dwType, 
		(BYTE*)(LPCTSTR)lpstrValue,
		&lpcbData) == ERROR_SUCCESS)
	{
		bRet = TRUE;
		strValue = (LPCTSTR)lpstrValue;
	}
	else
	{
		strValue.Empty();
	}
	free(lpstrValue);

	return bRet;
}	


BOOL CRegistry::GetValue(LPCTSTR lpValueName, DWORD& dwValue)
{
	BOOL bRet = FALSE;
	DWORD dwType = REG_DWORD;
	DWORD lpcbData = sizeof(DWORD);
	dwValue = 0;	
	
		
	if(RegQueryValueEx(m_hKey,
		lpValueName,
		NULL,
		&dwType, 
		(BYTE*)&dwValue,
		&lpcbData) == ERROR_SUCCESS)
		bRet = TRUE;

	return bRet;
}

BOOL CRegistry::DeleteKey(enum Keys hKey, LPCTSTR szKey)
{	
	return ::RegDeleteKey((HKEY)hKey,szKey) == ERROR_SUCCESS;
}

BOOL CRegistry::DeleteValue(LPCTSTR lpValueName)
{	
	if(::RegDeleteValue(m_hKey,lpValueName) == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

void CRegistry::CloseKey()
{
	::RegCloseKey(m_hKey);
	m_hKey = NULL;
}

BOOL CRegistry::SaveKey(LPCTSTR lpszFileName)
{
	ASSERT(m_hKey != NULL);
	LSTATUS lStatus=::RegSaveKey(m_hKey, lpszFileName, NULL);
	return  lStatus == ERROR_SUCCESS;
}

BOOL CRegistry::RestoreKey(LPCTSTR lpszFileName, DWORD dwFlags)
{
	ASSERT(m_hKey != NULL);
	return ::RegRestoreKey(m_hKey, lpszFileName, dwFlags) == ERROR_SUCCESS;
}

BOOL CRegistry::LoadKey(enum Keys hKey, LPCTSTR lpszSubKey, LPCTSTR lpszFileName)
{
	return ::RegLoadKey((HKEY)hKey, lpszSubKey, lpszFileName) == ERROR_SUCCESS;
}