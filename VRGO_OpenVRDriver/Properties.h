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
#include <map>

using namespace vr;

typedef std::map <PropertyContainerHandle_t, TrackedDeviceIndex_t> DeviceIdFromPropertyContainerHandle;


class CVRProperties : public IVRProperties
{
public:
	IVRProperties *m_pProperties;

	CVRProperties(IVRProperties *pProperties);
	~CVRProperties();

	/** Reads a set of properties atomically. See the PropertyReadBatch_t struct for more information. */
	virtual ETrackedPropertyError ReadPropertyBatch(PropertyContainerHandle_t ulContainerHandle, PropertyRead_t *pBatch, uint32_t unBatchEntryCount);

	/** Writes a set of properties atomically. See the PropertyWriteBatch_t struct for more information. */
	virtual ETrackedPropertyError WritePropertyBatch(PropertyContainerHandle_t ulContainerHandle, PropertyWrite_t *pBatch, uint32_t unBatchEntryCount);

	/** returns a string that corresponds with the specified property error. The string will be the name
	* of the error enum value for all valid error codes */
	virtual const char *GetPropErrorNameFromEnum(ETrackedPropertyError error);

	/** Returns a container handle given a tracked device index */
	virtual PropertyContainerHandle_t TrackedDeviceToPropertyContainer(TrackedDeviceIndex_t nDevice);
	
	DeviceIdFromPropertyContainerHandle m_DeviceIdFromPropertyContainerHandle;

	TrackedDeviceIndex_t PropertyContainerToTrackedDevice(PropertyContainerHandle_t ulContainer);

};