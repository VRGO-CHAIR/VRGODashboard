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
#include "DriverInput.h"
#include "IVRGOVRServerDriverHost.h"
#include "Properties.h"


CVRDriverInput::CVRDriverInput(IVRDriverInput *pDriverInput, CDriverInstance *pDriverInstance)
{
	m_pDriverInput = pDriverInput;
	m_pDriverInstance = pDriverInstance;
	m_DeviceHandles[0] = 0;
	m_DeviceHandles[1] = 0;
}


CVRDriverInput::~CVRDriverInput()
{
}

#define NB_ITEM(a) sizeof(a)/sizeof(*a)
char *pVIVEDefaultKey[] = { "/input/trigger/click","/input/trigger/value","/input/trackpad/click","/input/trackpad/touch","/input/trackpad/x","/input/trackpad/y" };

/** Creates a boolean input component for the device */
EVRInputError CVRDriverInput::CreateBooleanComponent(PropertyContainerHandle_t ulContainer, const char *pchName, VRInputComponentHandle_t *pHandle)
{

	EVRInputError nRet = VRInputError_InvalidDevice;

	if (m_pDriverInput)
		nRet = m_pDriverInput->CreateBooleanComponent(ulContainer, pchName, pHandle);

	CVRInputInfo InputList;
	InputList.m_DeviceHandle = ulContainer;
	InputList.m_sName = pchName;
	InputList.m_nKeyHandle = *pHandle;

	m_FullKeyList.push_back(InputList);

	IVRGOVRServerDriverHost *pDriverHost = m_pDriverInstance->VRVRGOServerDriverHost();
	CTrackedDeviceServerDriver* pDeviceServerDriver = nullptr;
	if (pDriverHost)
	{
		TrackedDeviceIndex_t nDevice = PropertyContainerToTrackedDevice(ulContainer);
		pDeviceServerDriver = pDriverHost->GetDeviceIndex(nDevice);
	}
	if (pDeviceServerDriver->m_DeviceClass != TrackedDeviceClass_HMD)
	{
		int nIndex = -1;
		if (m_DeviceHandles[0] != ulContainer)
		{
			if (m_DeviceHandles[0] == 0)
				m_DeviceHandles[0] = ulContainer;
			else if (m_DeviceHandles[1] == 0)
				m_DeviceHandles[1] = ulContainer;
			if (m_DeviceHandles[1] == ulContainer)
				nIndex = 1;
		}

		if (m_DeviceHandles[0] == ulContainer)
			nIndex = 0;

		if (nIndex != -1)
		{
			for (int i = 0; i < NB_ITEM(pVIVEDefaultKey); i++)
			{
				if (strcmp(pchName, pVIVEDefaultKey[i]) == 0)
				{
					m_RealUsageKeyList[i + nIndex * ruNbItem] = InputList;
				}
			}
		}
	}


	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CVRDriverInput::CreateBooleanComponent(%016llx,%s,%p(%016llx))=>%d", ulContainer, pchName, pHandle, *pHandle, nRet);

	return nRet;
}

EVRInputError CVRDriverInput::UpdateBooleanComponent(VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset)
{
	return UpdateBooleanComponentInternal(ulComponent, bNewValue, fTimeOffset, true);
}

EVRInputError CVRDriverInput::UpdateBooleanComponentInternal(VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset, bool bInputCall)
{

	/** Updates a boolean component */
	EVRInputError nRet = VRInputError_InvalidDevice;

	if (m_pDriverInput)
		nRet = m_pDriverInput->UpdateBooleanComponent(ulComponent, bNewValue, fTimeOffset);
	if (bInputCall)
	{
		IVRGOVRServerDriverHost *pDriverHost = m_pDriverInstance->VRVRGOServerDriverHost();
		if (pDriverHost)
		{
			RealUsageKey nKey = ruNone;
			CVRInputInfo *pInputInfo = GetInputInfo(ulComponent, &nKey);
			if (pInputInfo)
			{
				TrackedDeviceIndex_t nDevice = PropertyContainerToTrackedDevice(pInputInfo->m_DeviceHandle);
				switch (nKey)
				{
				case ruTrackpadClick:
					if (bNewValue)
						pDriverHost->SendTrackedDeviceButtonPressed(nDevice, EVRButtonId::k_EButton_SteamVR_Touchpad, 0, true);
					else
						pDriverHost->SendTrackedDeviceButtonUnpressed(nDevice, EVRButtonId::k_EButton_SteamVR_Touchpad, 0, true);
					break;
				case ruTrackpadTouch:
					if (bNewValue)
					{
						pDriverHost->SendTrackedDeviceButtonTouched(nDevice, EVRButtonId::k_EButton_SteamVR_Touchpad, 0, true);
						DEBUG_LOG(2, "TrackedDeviceButtonTouched");
					}
					else
					{
						pDriverHost->SendTrackedDeviceButtonUntouched(nDevice, EVRButtonId::k_EButton_SteamVR_Touchpad, 0, true);
						DEBUG_LOG(2, "TrackedDeviceButtonUntouched");
					}
					break;
				}
			}
		}
	}

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CVRDriverInput::UpdateBooleanComponent(%016llx,%d,%f)=>%d", ulComponent, bNewValue, fTimeOffset, nRet);

	return nRet;
}

/** Creates a scalar input component for the device */
EVRInputError CVRDriverInput::CreateScalarComponent(PropertyContainerHandle_t ulContainer, const char *pchName, VRInputComponentHandle_t *pHandle, EVRScalarType eType, EVRScalarUnits eUnits)
{
	EVRInputError nRet = VRInputError_InvalidDevice;

	if (m_pDriverInput)
		nRet = m_pDriverInput->CreateScalarComponent(ulContainer, pchName, pHandle, eType, eUnits);

	CVRInputInfo InputList;
	InputList.m_DeviceHandle = ulContainer;
	InputList.m_sName = pchName;
	InputList.m_nKeyHandle = *pHandle;

	m_FullKeyList.push_back(InputList);

	IVRGOVRServerDriverHost *pDriverHost = m_pDriverInstance->VRVRGOServerDriverHost();
	CTrackedDeviceServerDriver* pDeviceServerDriver = nullptr;
	if (pDriverHost)
	{
		TrackedDeviceIndex_t nDevice = PropertyContainerToTrackedDevice(ulContainer);
		pDeviceServerDriver = pDriverHost->GetDeviceIndex(nDevice);
	}
	if (pDeviceServerDriver->m_DeviceClass != TrackedDeviceClass_HMD)
	{
		int nIndex = -1;
		if (m_DeviceHandles[0] != ulContainer)
		{
			if (m_DeviceHandles[0] == 0)
				m_DeviceHandles[0] = ulContainer;
			else if (m_DeviceHandles[1] == 0)
				m_DeviceHandles[1] = ulContainer;
			if (m_DeviceHandles[1] == ulContainer)
				nIndex = 1;
		}

		if (m_DeviceHandles[0] == ulContainer)
			nIndex = 0;

		if (nIndex != -1)
		{
			for (int i = 0; i < NB_ITEM(pVIVEDefaultKey); i++)
			{
				if (strcmp(pchName, pVIVEDefaultKey[i]) == 0)
				{
					m_RealUsageKeyList[i + nIndex * ruNbItem] = InputList;
				}
			}
		}
	}

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(2, "CVRDriverInput::CreateScalarComponent(%016llx,%s,%p(%016llx),%d,%d)=>%d", ulContainer, pchName, pHandle, *pHandle, eType, eUnits, nRet);

	return nRet;
}

EVRInputError CVRDriverInput::UpdateScalarComponent(VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset)
{
	return UpdateScalarComponentInternal(ulComponent, fNewValue, fTimeOffset, true);
}

/** Updates a boolean component */
EVRInputError CVRDriverInput::UpdateScalarComponentInternal(VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset, bool bInputCall)
{
	EVRInputError nRet = VRInputError_InvalidDevice;

	if (m_pDriverInput)
		nRet = m_pDriverInput->UpdateScalarComponent(ulComponent, fNewValue, fTimeOffset);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRDriverInput::UpdateScalarComponent(%016llx,%f,%f)=>%d", ulComponent, fNewValue, fTimeOffset, nRet);

	return nRet;
}

/** Creates a haptic component for the device */
EVRInputError CVRDriverInput::CreateHapticComponent(PropertyContainerHandle_t ulContainer, const char *pchName, VRInputComponentHandle_t *pHandle)
{
	EVRInputError nRet = VRInputError_InvalidDevice;

	if (m_pDriverInput)
		nRet = m_pDriverInput->CreateHapticComponent(ulContainer, pchName, pHandle);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRDriverInput::CreateHapticComponent(%016llx,%s,%p(%016llx)=>%d", ulContainer, pchName, pHandle, *pHandle);

	return nRet;

}

CVRInputInfo *CVRDriverInput::GetInputInfo(VRInputComponentHandle_t ulComponent, RealUsageKey *pKey)
{
	for (int i = 0; i < ruNbItem * 2; i++)
	{
		if (m_RealUsageKeyList[i].m_nKeyHandle == ulComponent)
		{
			if (pKey)
				*pKey = (RealUsageKey)(i%ruNbItem);
			return &m_RealUsageKeyList[i];
		}
	}
	return nullptr;
}

CVRInputInfo *CVRDriverInput::GetInputInfo(PropertyContainerHandle_t ulContainer, RealUsageKey key)
{
	if (m_RealUsageKeyList[key].m_DeviceHandle == ulContainer)
		return &m_RealUsageKeyList[key];
	else
		if (m_RealUsageKeyList[key + ruNbItem].m_DeviceHandle == ulContainer)
			return &m_RealUsageKeyList[key + ruNbItem];
		else
			return nullptr;
}

CVRInputInfo *CVRDriverInput::GetInputInfo(uint32_t unObjectId, RealUsageKey key)
{
	PropertyContainerHandle_t ulContainer = m_pDriverInstance->VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
	return GetInputInfo(ulContainer, key);
}

TrackedDeviceIndex_t CVRDriverInput::PropertyContainerToTrackedDevice(PropertyContainerHandle_t ulContainer)
{
	CVRProperties* pProperty = (CVRProperties*)m_pDriverInstance->VRPropertiesRaw();

	return  pProperty->PropertyContainerToTrackedDevice(ulContainer);
}
