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

#pragma warning(disable : 4996)
#include <windows.h>
#include <stdint.h>
//#include <stdio.h>
#include <tchar.h>
#include <AclAPI.h>

class CSharedMemory
{
public: 
	CSharedMemory()
	{
		m_hMapFile = NULL;
		m_hMutex = NULL;

	}
	~CSharedMemory()
	{
		Close();
	}
	bool Open(const TCHAR *sName, uint32_t nSize = 0, bool bSetBlobalRight = false)
	{
		if (m_hMapFile)
			Close();
		m_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, sName);
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());
		if (m_hMapFile == NULL && nSize > 0)
		{
			m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, nSize, sName);
			if (m_hMapFile&&bSetBlobalRight)
				SetSecurityInfo(m_hMapFile, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL);
			if (m_hMapFile&&nSize > 0)
			{
				uint8_t *pData = new uint8_t[nSize];
				memset(pData, 0, nSize);
				while (!Write(pData, nSize)) Sleep(100);
				delete[] pData;
			}
		}
		if (m_hMapFile)
		{
			TCHAR*pMutexName = new TCHAR[_tcslen(sName) + 1 + 10];
			_stprintf(pMutexName, _T("%s_mutex"), sName);
			m_hMutex = ::CreateMutex(NULL, FALSE, pMutexName);
			delete[]pMutexName;
		}

		return m_hMapFile != NULL;
	}
	bool Close()
	{
		bool bRet = false;
		if (m_hMapFile)
		{
			CloseHandle(m_hMapFile);
			m_hMapFile = NULL;
			::ReleaseMutex(m_hMutex);
			CloseHandle(m_hMutex);
			m_hMutex = NULL;
			bRet = true;
		}
		return bRet;
	}
	bool Write(uint8_t *pData, uint32_t nSize)
	{
		bool bRet = false;
		if (m_hMapFile)
		{
			LPCTSTR pBuf = (LPTSTR)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, nSize);
			if (pBuf != NULL)
			{
				WaitForSingleObject(m_hMutex, INFINITE);
				memcpy((void*)pBuf, pData, nSize);
				::ReleaseMutex(m_hMutex);
				UnmapViewOfFile(pBuf);
				bRet = true;
			}
		}
		return bRet;
	}
	bool Write(int nIndex, uint8_t *pData, uint32_t nSize)
	{
		bool bRet = false;
		if (m_hMapFile)
		{
			LPCTSTR pBuf = (LPTSTR)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, nSize);
			if (pBuf != NULL)
			{
				WaitForSingleObject(m_hMutex, INFINITE);
				memcpy((void*)(pBuf + nIndex), pData, nSize);
				::ReleaseMutex(m_hMutex);
				UnmapViewOfFile(pBuf);
				bRet = true;
			}
		}
		return bRet;
	}
	bool Read(uint8_t *pData, int nSize)
	{
		bool bRet = false;
		if (m_hMapFile)
		{
			LPCTSTR pBuf = (LPTSTR)MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, nSize);
			if (pBuf != NULL)
			{
				WaitForSingleObject(m_hMutex, INFINITE);
				memcpy(pData, pBuf, nSize);
				::ReleaseMutex(m_hMutex);
				UnmapViewOfFile(pBuf);
				bRet = true;
			}
		}
		return bRet;
	}

	void *Lock(int nSize)
	{
		void *pBuf = NULL;
		if (m_hMapFile)
		{
			pBuf = MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, nSize);
			if (pBuf != NULL)
			{
				WaitForSingleObject(m_hMutex, INFINITE);
			}
		}
		return pBuf;
	}

	bool Unlock(void *pBuf)
	{
		bool bRet = false;
		if (m_hMapFile)
		{
			if (pBuf != NULL)
			{
				::ReleaseMutex(m_hMutex);
				UnmapViewOfFile(pBuf);
				bRet = true;
			}
		}
		return bRet;
	}

private :
	HANDLE m_hMapFile;
	HANDLE	m_hMutex;
};



class CSI_Message
{
public:
	int m_nLen;
	int m_nType;
};

#define SHARDED_MESSAGE_DATA_BUFFER_SIZE  50*1024

class CSISharedMessage
{
public:
	uint8_t m_DataBuffer[SHARDED_MESSAGE_DATA_BUFFER_SIZE];
	int m_nCurDataSize;
	CSISharedMessage()
	{
		m_nCurDataSize = 0;
	}
};




class CSISharedMessageHandle
{
public:
	CSharedMemory m_SM;
	CSISharedMessageHandle(TCHAR *pName)
	{
		m_SM.Open(pName, sizeof(CSISharedMessage));
	}

	~CSISharedMessageHandle()
	{
		m_SM.Close();
	}

	bool PushMessage(CSI_Message *pMsg)
	{
		bool bRet = false;
		if (pMsg)
		{
			uint8_t *pData = (uint8_t*)m_SM.Lock(sizeof(CSISharedMessage));
			if (pData)
			{
				CSISharedMessage *pSMsg = (CSISharedMessage *)pData;
				if (pSMsg->m_nCurDataSize + pMsg->m_nLen < SHARDED_MESSAGE_DATA_BUFFER_SIZE)
				{
					memcpy(pSMsg->m_DataBuffer + pSMsg->m_nCurDataSize, pMsg, pMsg->m_nLen);
					pSMsg->m_nCurDataSize += pMsg->m_nLen;
					bRet = true;
				}
				m_SM.Unlock(pData);
			}
		}
		return bRet;
	}

	CSI_Message *PopMessage()
	{
		CSI_Message *pRet = NULL;
		uint8_t *pData = (uint8_t*)m_SM.Lock(sizeof(CSISharedMessage));
		if (pData)
		{
			CSISharedMessage *pSMsg = (CSISharedMessage *)pData;
			if (pSMsg->m_nCurDataSize > 0)
			{
				CSI_Message *pLocalMessage = (CSI_Message*)pSMsg->m_DataBuffer;
				uint8_t *pRetB = new uint8_t[pLocalMessage->m_nLen];
				memcpy(pRetB, pSMsg->m_DataBuffer, pLocalMessage->m_nLen);
				pRet = (CSI_Message *)pRetB;
				pSMsg->m_nCurDataSize -= pLocalMessage->m_nLen;
				if (pSMsg->m_nCurDataSize > 0)
				{
					memmove(pSMsg->m_DataBuffer, pSMsg->m_DataBuffer + pLocalMessage->m_nLen, pSMsg->m_nCurDataSize);
				}
				else
				{
					memset(pSMsg->m_DataBuffer, 0, pLocalMessage->m_nLen);
					pSMsg->m_nCurDataSize = 0;
				}
			}
			m_SM.Unlock(pData);
		}
		return pRet;
	}
};

