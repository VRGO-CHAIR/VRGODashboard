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
#include "DriverContext.h"
#include "Properties.h"
#include "ServerDriverHost4.h"
#include "ServerDriverHost5.h"
#include "DebugLog.h"
#include "DriverInput.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVRDriverContext
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CVRDriverContext::CVRDriverContext(IVRDriverContext *pDriverContext, CDriverInstance *pDriverInstance)
{
	m_pDriverContext = pDriverContext;
	m_pDriverInstance = pDriverInstance;
	m_pVRProperties = nullptr;
	m_pVRServerDriverHost = nullptr;
	m_pDriverInput = nullptr;
}

CVRDriverContext::~CVRDriverContext()
{
	if (m_pVRProperties)
		delete m_pVRProperties;
	m_pVRProperties = nullptr;
	if (m_pVRServerDriverHost)
		delete m_pVRServerDriverHost;
	m_pVRServerDriverHost = nullptr;
	if (m_pDriverInput)
		delete m_pDriverInput;
	m_pDriverInput = nullptr;
}

void *CVRDriverContext::GetGenericInterface(const char *pchInterfaceVersion, EVRInitError *peError)
{
	EVRInitError nError = vr::VRInitError_Init_NotInitialized;
	void *pRet = nullptr;
	if (m_pDriverContext)
		pRet = m_pDriverContext->GetGenericInterface(pchInterfaceVersion, peError);

	bool bVersionOK = false;

	bool bHooked = false;

	if (CheckInterface(pchInterfaceVersion, IVRProperties_Version, bVersionOK))
	{
		if (bVersionOK)
		{
			if (!m_pVRProperties)
				m_pVRProperties = new CVRProperties((IVRProperties*)pRet);
			pRet = (void *)m_pVRProperties;
			bHooked = true;
		}
	}
	else if (CheckInterface(pchInterfaceVersion, "IVRServerDriverHost_005", bVersionOK))
	{
		if (bVersionOK)
		{
			if (!m_pVRServerDriverHost)
				m_pVRServerDriverHost = new CVRServerDriverHost5((IVRServerDriverHost *)pRet, m_pDriverInstance);
			CVRServerDriverHost5 *pHost = (CVRServerDriverHost5 *)m_pVRServerDriverHost;
			m_pDriverInstance->m_pVRGOVRServerDriverHost = (IVRGOVRServerDriverHost*)pHost;
			pRet = (void*)m_pVRServerDriverHost;
			bHooked = true;
		}
		else if (CheckInterface(pchInterfaceVersion, "IVRServerDriverHost_004", bVersionOK))
		{
			if (bVersionOK)
			{
				if (!m_pVRServerDriverHost)
					m_pVRServerDriverHost = new CVRServerDriverHost4((IVRServerDriverHost *)pRet, m_pDriverInstance);
				CVRServerDriverHost4 *pHost = (CVRServerDriverHost4 *)m_pVRServerDriverHost;
				m_pDriverInstance->m_pVRGOVRServerDriverHost = (IVRGOVRServerDriverHost*)pHost;
				pRet = (void*)m_pVRServerDriverHost;
				bHooked = true;
			}
		}
	}
	else if (CheckInterface(pchInterfaceVersion, IVRDriverInput_Version, bVersionOK))
	{
		if (bVersionOK)
		{
			if (!m_pDriverInput)
				m_pDriverInput = new CVRDriverInput((IVRDriverInput *)pRet, m_pDriverInstance);
			pRet = (void*)m_pDriverInput;
			bHooked = true;
		}
	}

	if (peError)
		nError = *peError;

	if (bHooked)
	{
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_PURPLE, DEBUG_LOG_BLACK);
		DEBUG_LOG(1, "CVRDriverContext::GetGenericInterface(%s,%d)=>%p Hooked !", pchInterfaceVersion, nError, pRet);
	}
	else
	{
		DEBUG_LOG_SETCOLOR(DEBUG_LOG_PINK, DEBUG_LOG_BLACK);
		DEBUG_LOG(1, "CVRDriverContext::GetGenericInterface(%s,%d)=>%p", pchInterfaceVersion, nError, pRet);
	}
	return pRet;
}

/** Returns the property container handle for this driver */
DriverHandle_t CVRDriverContext::GetDriverHandle()
{
	DriverHandle_t nRet = 0;
	if (m_pDriverContext)
		nRet = m_pDriverContext->GetDriverHandle();

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(1, "CVRDriverContext::GetDriverHandle()=>%d", nRet);
	return nRet;
}
