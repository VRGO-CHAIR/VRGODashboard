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
#include "Properties.h"
#include "DebugLog.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVRProperties
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CVRProperties::CVRProperties(IVRProperties *pProperties)
{
	m_pProperties = pProperties;
}

CVRProperties::~CVRProperties()
{}

/** Reads a set of properties atomically. See the PropertyReadBatch_t struct for more information. */
ETrackedPropertyError CVRProperties::ReadPropertyBatch(PropertyContainerHandle_t ulContainerHandle, PropertyRead_t *pBatch, uint32_t unBatchEntryCount)
{
	ETrackedPropertyError nRet = TrackedProp_Success;
	if (m_pProperties)
		nRet = m_pProperties->ReadPropertyBatch(ulContainerHandle, pBatch, unBatchEntryCount);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_YELLOW, DEBUG_LOG_BLACK);

	for (uint32_t i = 0; i < unBatchEntryCount; i++)
	{
		switch (pBatch[i].unTag)
		{
		case k_unInvalidPropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]InvalidProperty(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unFloatPropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]FloatProperty(%016llx,%d)=%f <%d>=>%d", i, ulContainerHandle, pBatch[i].prop, *(float*)pBatch[i].pvBuffer, pBatch[i].eError, nRet);
			break;
		case k_unInt32PropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]Int32Property(%016llx,%d)=%d <%d>=>%d", i, ulContainerHandle, pBatch[i].prop, *(uint32_t*)pBatch[i].pvBuffer, pBatch[i].eError, nRet);
			break;
		case k_unUint64PropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]int64Property(%016llx,%d)=%ld <%d>=>%d", i, ulContainerHandle, pBatch[i].prop, *(uint64_t*)pBatch[i].pvBuffer, pBatch[i].eError, nRet);
			break;
		case k_unBoolPropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]BoolProperty(%016llx,%d)=%d <%d>=>%d", i, ulContainerHandle, pBatch[i].prop, *(bool*)pBatch[i].pvBuffer, pBatch[i].eError, nRet);
			break;
		case k_unStringPropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]StringProperty(%016llx,%d)=%s <%d>=>%d", i, ulContainerHandle, pBatch[i].prop, (char*)pBatch[i].pvBuffer, pBatch[i].eError, nRet);
			break;
		case k_unHmdMatrix34PropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]HmdMatrix34Property(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unHmdMatrix44PropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]HmdMatrix44Property(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unHmdVector3PropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]HmdVector3Property(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unHmdVector4PropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]HmdVector4Property(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unHiddenAreaPropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]HiddenAreaProperty(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unPathHandleInfoTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]PathHandleInfoTag(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unActionPropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]ActionPropertyTag(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unInputValuePropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]InputValuePropertyTag(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unWildcardPropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]WildcardPropertyTag(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unHapticVibrationPropertyTag:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]HapticVibrationPropertyTag(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unOpenVRInternalReserved_Start:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]OpenVRInternalReserved_Start(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unOpenVRInternalReserved_End:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]OpenVRInternalReserved_End(%016llx,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		default:
			DEBUG_LOG(5, "CVRProperties::ReadPropertyBatch[%d]default(%016llx,%d,%d)<%d>=>%d", i, ulContainerHandle, pBatch[i].unTag, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		}
	}

	return nRet;
}

/** Writes a set of properties atomically. See the PropertyWriteBatch_t struct for more information. */
ETrackedPropertyError CVRProperties::WritePropertyBatch(PropertyContainerHandle_t ulContainerHandle, PropertyWrite_t *pBatch, uint32_t unBatchEntryCount)
{
	ETrackedPropertyError nRet = TrackedProp_Success;
	if (m_pProperties)
		nRet = m_pProperties->WritePropertyBatch(ulContainerHandle, pBatch, unBatchEntryCount);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_YELLOW, DEBUG_LOG_BLACK);

	for (uint32_t i = 0; i < unBatchEntryCount; i++)
	{
		switch (pBatch[i].unTag)
		{
		case k_unInvalidPropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)InvalidProperty(%016llx,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unFloatPropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)FloatProperty(%016llx,%d)=%f <%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, *(float*)pBatch[i].pvBuffer, pBatch[i].eError, nRet);
			break;
		case k_unInt32PropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)Int32Property(%016llx,%d)=%d <%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, *(uint32_t*)pBatch[i].pvBuffer, pBatch[i].eError, nRet);
			break;
		case k_unUint64PropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)int64Property(%016llx,%d)=%ld <%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, *(uint64_t*)pBatch[i].pvBuffer, pBatch[i].eError, nRet);
			break;
		case k_unBoolPropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)BoolProperty(%016llx,%d)=%d <%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, *(bool*)pBatch[i].pvBuffer, pBatch[i].eError, nRet);
			break;
		case k_unStringPropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)StringProperty(%016llx,%d)=%s <%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, (char*)pBatch[i].pvBuffer, pBatch[i].eError, nRet);
			break;
		case k_unHmdMatrix34PropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)HmdMatrix34Property(%016llx,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unHmdMatrix44PropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)HmdMatrix44Property(%016llx,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unHmdVector3PropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)HmdVector3Property(%016llx,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unHmdVector4PropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)HmdVector4Property(%016llx,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unHiddenAreaPropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)HiddenAreaProperty(%016llx,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unPathHandleInfoTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)PathHandleInfoTag(%016llx,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unActionPropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)ActionPropertyTag(%016llx,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unInputValuePropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)InputValuePropertyTag(%016llx,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unWildcardPropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)WildcardPropertyTag(%016llx,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unHapticVibrationPropertyTag:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)HapticVibrationPropertyTag(%d,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unOpenVRInternalReserved_Start:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)OpenVRInternalReserved_Start(%d,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		case k_unOpenVRInternalReserved_End:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)OpenVRInternalReserved_End(%d,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		default:
			DEBUG_LOG(5, "CVRProperties::WritePropertyBatch[%d](%d,%d)default(%d,%d,%d)<%d>=>%d", i, pBatch[i].writeType, pBatch[i].eSetError, ulContainerHandle, pBatch[i].unTag, pBatch[i].prop, pBatch[i].eError, nRet);
			break;
		}
	}

	return nRet;
}

/** returns a string that corresponds with the specified property error. The string will be the name
* of the error enum value for all valid error codes */
const char *CVRProperties::GetPropErrorNameFromEnum(ETrackedPropertyError error)
{
	const char *pRet = "";
	if (m_pProperties)
		pRet = m_pProperties->GetPropErrorNameFromEnum(error);

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_YELLOW, DEBUG_LOG_RED);
	DEBUG_LOG(1, "CVRProperties::GetPropErrorNameFromEnum(%d)=>%s", error, pRet);
	return pRet;
}

/** Returns a container handle given a tracked device index */
PropertyContainerHandle_t CVRProperties::TrackedDeviceToPropertyContainer(TrackedDeviceIndex_t nDevice)
{
	PropertyContainerHandle_t nRet = 0;
	if (m_pProperties)
		nRet = m_pProperties->TrackedDeviceToPropertyContainer(nDevice);

	if (nRet)
		m_DeviceIdFromPropertyContainerHandle.insert(std::pair <PropertyContainerHandle_t, TrackedDeviceIndex_t>(nRet, nDevice));

	DEBUG_LOG_SETCOLOR(DEBUG_LOG_WHITE, DEBUG_LOG_BLACK);
	DEBUG_LOG(5, "CVRProperties::TrackedDeviceToPropertyContainer(%d)=>%016llx", nDevice, nRet);
	return nRet;
}

TrackedDeviceIndex_t CVRProperties::PropertyContainerToTrackedDevice(PropertyContainerHandle_t ulContainer)
{
	DeviceIdFromPropertyContainerHandle::iterator it = m_DeviceIdFromPropertyContainerHandle.find(ulContainer);
	if (it == m_DeviceIdFromPropertyContainerHandle.end())
		return 0;
	else
		return (it->second);
}