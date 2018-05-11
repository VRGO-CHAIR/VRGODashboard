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


#include "openvr_driver.h"
#include "DebugLog.h"

#include "DriverInstance.h"

#include <vector>
#include <string>

using namespace vr;


class CVRInputInfo
{
public:
	CVRInputInfo()
	{
		m_DeviceHandle = 0;
		m_nKeyHandle = 0;
	}
	std::string m_sName;
	PropertyContainerHandle_t m_DeviceHandle;
	VRInputComponentHandle_t m_nKeyHandle;
};


typedef std::vector <CVRInputInfo> vCVRInputList;





class CVRDriverInput : public IVRDriverInput
{
	IVRDriverInput *m_pDriverInput;
	CDriverInstance *m_pDriverInstance;

public:
	CVRDriverInput(IVRDriverInput *pDriverInput, CDriverInstance *pDriverInstance);
	virtual ~CVRDriverInput();

	/** Creates a boolean input component for the device */
	virtual EVRInputError CreateBooleanComponent(PropertyContainerHandle_t ulContainer, const char *pchName, VRInputComponentHandle_t *pHandle);

	/** Updates a boolean component */
	virtual EVRInputError UpdateBooleanComponent(VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset);

	/** Creates a scalar input component for the device */
	virtual EVRInputError CreateScalarComponent(PropertyContainerHandle_t ulContainer, const char *pchName, VRInputComponentHandle_t *pHandle, EVRScalarType eType, EVRScalarUnits eUnits);

	/** Updates a boolean component */
	virtual EVRInputError UpdateScalarComponent(VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset);

	/** Creates a haptic component for the device */
	virtual EVRInputError CreateHapticComponent(PropertyContainerHandle_t ulContainer, const char *pchName, VRInputComponentHandle_t *pHandle);


	EVRInputError UpdateBooleanComponentInternal(VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset, bool bInputCall = false);
	EVRInputError UpdateScalarComponentInternal(VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset, bool bInputCall = false);


	vCVRInputList m_FullKeyList;

	enum RealUsageKey
	{
		ruTriggerClick,
		ruTriggerValue,
		ruTrackpadClick,
		ruTrackpadTouch,
		ruTrackpadX,
		ruTrackpadY,
		ruNbItem,
		ruNone
	};

	//vCVRInputList m_RealUsageKeyList;
	CVRInputInfo m_RealUsageKeyList[ruNbItem * 2];
	PropertyContainerHandle_t m_DeviceHandles[2];


	CVRInputInfo *GetInputInfo(VRInputComponentHandle_t ulComponent, RealUsageKey *pIndex);
	CVRInputInfo *GetInputInfo(PropertyContainerHandle_t ulContainer, RealUsageKey key);
	CVRInputInfo *GetInputInfo(uint32_t unObjectId, RealUsageKey key);

	TrackedDeviceIndex_t PropertyContainerToTrackedDevice(PropertyContainerHandle_t ulContainer);
};
