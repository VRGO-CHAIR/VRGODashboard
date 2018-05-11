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
#include "IVRGOVRServerDriverHost.h"
#include "..\Common\MathTools.h"
#include "..\Common\SharedData.h"
#include "ServerDriver.h"
#include "DriverContext.h"
//include driver input


IVRGOVRServerDriverHost::IVRGOVRServerDriverHost(CDriverInstance *pDriverInstance)
{
	m_bTrackpadTouched[0] = false;
	m_bTrackpadTouched[1] = false;
	m_bCalibrated = false;
	m_pDriverInstance = pDriverInstance;
}


IVRGOVRServerDriverHost::~IVRGOVRServerDriverHost()
{
	DriverServerMap::iterator it = m_MapDeviceServerDriver.begin();
	if (it != m_MapDeviceServerDriver.end())
	{
		while (it != m_MapDeviceServerDriver.end())
		{
			CTrackedDeviceServerDriver* pTrackedDevice = (it->second);
			delete pTrackedDevice;
			it++;
		}
		m_MapDeviceServerDriver.clear();
	}
}

CTrackedDeviceServerDriver* IVRGOVRServerDriverHost::GetDeviceIndex(int nWhichDevice)
{
	CTrackedDeviceServerDriver* pTrackedDevice = nullptr;
	DriverServerMap::iterator it = m_MapDeviceServerDriver.begin();
	if (it != m_MapDeviceServerDriver.end())
	{
		while (it != m_MapDeviceServerDriver.end())
		{
			if ((it->second)->m_nID == nWhichDevice)
			{
				pTrackedDevice = (it->second);
				break;
			}
			it++;
		}
	}
	return pTrackedDevice;
}

void IVRGOVRServerDriverHost::Calibration(CMovement *pMovement, const DriverPose_t& pos)
{
	//// Get orientation of HMD
	//Mat_33 HmdRotationMat;
	//for (int i = 0; i < 3; ++i)
	//	for (int j = 0; j < 3; ++j)
	//		HmdRotationMat[i][j] = pMovement->m_mMatrix34Hmd.m[i][j];

	//HmdQuaternion_t HmdRotationQuat;
	//MatToQuat(HmdRotationMat, HmdRotationQuat);
	//if (_tcscmp(m_pDriverInstance->m_sModuleName, L"driver_lighthouse") == 0)
	//{
	//	// rotation x -180
	//	HmdRotationQuat.y *= -1.0;
	//	HmdRotationQuat.z *= -1.0;
	//}
	//HmdQuaternion_t HmdTransposedQuat = pos.qRotation;;
	//Inverse(HmdTransposedQuat);
	//// Applique la rotation au repere du driver dans le repere du casque
	//HmdQuaternion_t driverRotationQuat = HmdTransposedQuat * HmdRotationQuat;
	//Inverse(driverRotationQuat);
	///* MIXED REALITY CHANGE*/
	////if (_tcscmp(m_pDriverInstance->m_sModuleName, L"driver_Holographic") == 0)
	////{
	////	HmdQuaternion_t inverseAxis; // x - 180
	////	inverseAxis.x = 1;
	////	inverseAxis.w = inverseAxis.y = inverseAxis.z = 0;
	////	driverRotationQuat = driverRotationQuat * inverseAxis;
	////}
	///**********************/
	//// Transform en vecteur X Y Z
	//QuatToVectors(driverRotationQuat, pMovement->m_v3AxisX, pMovement->m_v3AxisY, pMovement->m_v3AxisZ);

	//HmdVector3d_t offset = pMovement->m_v3AxisZ;
	//// init position
	//for (int i = 0; i < 3; ++i)
	//	pMovement->m_v3PositionHmd.v[i] = pos.vecPosition[i] + (offset.v[i] * m_pDriverInstance->m_pSharedData->m_fHeadOffset);

	//// Save initial rotation
	//pMovement->m_qRotationInitial = driverRotationQuat;
	//pMovement->m_bAskCalibration = false;
	//// Restore delta movement
	//pMovement->Restore();
	//m_bCalibrated = true;
}

void IVRGOVRServerDriverHost::ComputeMovement(float fDeltaTime)
{
	//ns3dRudder::Axis OurAxis;
	//ns3dRudder::Status status;

	//ns3dRudder::CSdk *pSdk = m_pDriverInstance->m_pSdk;
	//CMovement *pMovement = &m_pDriverInstance->m_Movement;
	CSISharedData *pSharedData = m_pDriverInstance->m_pSharedData;

	//if (pSdk)
	//{
	//	// Create curves
	//	ns3dRudder::CurveArray CurveArray;
	//	ns3dRudder::Curve Curve[4];

	//	// Get shared curves
	//	Curve[ns3dRudder::CurveXAxis].m_fDeadZone = pSharedData->m_Curve[ns3dRudder::CurveXAxis].m_fDeadZone;
	//	Curve[ns3dRudder::CurveXAxis].m_fxSat = pSharedData->m_Curve[ns3dRudder::CurveXAxis].m_fxSat;
	//	Curve[ns3dRudder::CurveXAxis].m_fyMax = pSharedData->m_Curve[ns3dRudder::CurveXAxis].m_fyMax;
	//	Curve[ns3dRudder::CurveXAxis].m_fExp = pSharedData->m_Curve[ns3dRudder::CurveXAxis].m_fExp;
	//	Curve[ns3dRudder::CurveYAxis].m_fDeadZone = pSharedData->m_Curve[ns3dRudder::CurveYAxis].m_fDeadZone;
	//	Curve[ns3dRudder::CurveYAxis].m_fxSat = pSharedData->m_Curve[ns3dRudder::CurveYAxis].m_fxSat;
	//	Curve[ns3dRudder::CurveYAxis].m_fyMax = pSharedData->m_Curve[ns3dRudder::CurveYAxis].m_fyMax;
	//	Curve[ns3dRudder::CurveYAxis].m_fExp = pSharedData->m_Curve[ns3dRudder::CurveYAxis].m_fExp;
	//	Curve[ns3dRudder::CurveZAxis].m_fDeadZone = pSharedData->m_Curve[ns3dRudder::CurveZAxis].m_fDeadZone;
	//	Curve[ns3dRudder::CurveZAxis].m_fxSat = pSharedData->m_Curve[ns3dRudder::CurveZAxis].m_fxSat;
	//	Curve[ns3dRudder::CurveZAxis].m_fyMax = pSharedData->m_Curve[ns3dRudder::CurveZAxis].m_fyMax;
	//	Curve[ns3dRudder::CurveZAxis].m_fExp = pSharedData->m_Curve[ns3dRudder::CurveZAxis].m_fExp;
	//	Curve[ns3dRudder::CurveZRotation].m_fDeadZone = pSharedData->m_Curve[ns3dRudder::CurveZRotation].m_fDeadZone;
	//	Curve[ns3dRudder::CurveZRotation].m_fxSat = pSharedData->m_Curve[ns3dRudder::CurveZRotation].m_fxSat;
	//	Curve[ns3dRudder::CurveZRotation].m_fyMax = pSharedData->m_Curve[ns3dRudder::CurveZRotation].m_fyMax;
	//	Curve[ns3dRudder::CurveZRotation].m_fExp = pSharedData->m_Curve[ns3dRudder::CurveZRotation].m_fExp;

	//	// Set curves
	//	CurveArray.SetCurve(ns3dRudder::CurveXAxis, &Curve[ns3dRudder::CurveXAxis]);
	//	CurveArray.SetCurve(ns3dRudder::CurveYAxis, &Curve[ns3dRudder::CurveYAxis]);
	//	CurveArray.SetCurve(ns3dRudder::CurveZAxis, &Curve[ns3dRudder::CurveZAxis]);
	//	CurveArray.SetCurve(ns3dRudder::CurveZRotation, &Curve[ns3dRudder::CurveZRotation]);

	//	// Get Axis (Symmetrical Pitch || Curves)
	//	if (pSharedData->m_bNonSymPitch)
	//		pSdk->GetAxis(0, ns3dRudder::ValueWithCurveNonSymmetricalPitch, &OurAxis, &CurveArray);
	//	else
	//		pSdk->GetAxis(0, ns3dRudder::ValueWithCurve, &OurAxis, &CurveArray);

	//	// Get Status
	//	//status = pSdk->GetStatus(0);						
	//}

	// Speed Axis
	float fSpeedAx = pSharedData->m_fSpeed[0];
	float fSpeedAy = pSharedData->m_fSpeed[1];
	float fSpeedAz = pSharedData->m_fSpeed[2];
	float fSpeedRz = pSharedData->m_fSpeed[3];

	// update translation
	//HmdVector3d_t trans;
	if (pSharedData->m_bSmoothMode)
	{
		//trans.v[0] = fDeltaTime * pMovement->m_SmoothRoll.ComputeSpeed(OurAxis.m_aX, fSpeedAx, pSharedData->m_fSmoothValue[0], fDeltaTime); // Roll (X)			
		//trans.v[1] = fDeltaTime * pMovement->m_SmoothUpDown.ComputeSpeed(-OurAxis.m_aZ, fSpeedAz, pSharedData->m_fSmoothValue[2], fDeltaTime); // Updown (Y)			
		//trans.v[2] = fDeltaTime * pMovement->m_SmoothPitch.ComputeSpeed(OurAxis.m_aY, fSpeedAy, pSharedData->m_fSmoothValue[1], fDeltaTime); // Pitch (Z)																															 
		//pMovement->m_fAngle += fDeltaTime * pMovement->m_SmoothYaw.ComputeSpeed(OurAxis.m_rZ, fSpeedRz, pSharedData->m_fSmoothValue[3], fDeltaTime); // Yaw (rZ)
	}
	else
	{
		//trans.v[0] = OurAxis.m_aX * fSpeedAx * fDeltaTime; // Roll (X)			
		//trans.v[1] = -OurAxis.m_aZ * fSpeedAz * fDeltaTime; // Updown (Y)			
		//trans.v[2] = OurAxis.m_aY * fSpeedAy * fDeltaTime; // Pitch (Z)													  
		//pMovement->m_fAngle += OurAxis.m_rZ * fSpeedRz * fDeltaTime; // Yaw (rZ)
	}

	//pMovement->m_fAngle = nfmod(pMovement->m_fAngle, M_PI * 2.0);
	// Transform the movement's vector with initial hmd rotation and 3dRudder rotation
	//Transform(pMovement->m_qRotationInitial * pMovement->m_qRotation, trans);
	//for (int i = 0; i < 3; ++i)
	//	pMovement->m_v3Translation.v[i] += trans.v[i];

	// Compute Rotation
	//pMovement->m_qRotation = FromAngleAxis(pMovement->m_fAngle, pMovement->m_v3AxisY);
}

void IVRGOVRServerDriverHost::HandleTrackedDevicePoseUpdated(uint32_t unWhichDevice, DriverPose_t & pos, uint32_t unPoseStructSize)
{
	//DriverPose_t newPose = pos;
	//CTrackedDeviceServerDriver* pTrackedDevice = GetDeviceIndex(unWhichDevice);
	//CMovement *pMovement = &m_pDriverInstance->m_Movement;

	//if (!pTrackedDevice)
	//	return;
	//// Delta Time
	//float fDeltaTime = float(pTrackedDevice->m_TimeFrame.stop() / 1000000.0) / 1000.0f;
	//pTrackedDevice->m_TimeFrame.start();

	//CSISharedDataDebug *pSharedDataDebug = m_pDriverInstance->m_pSharedDataDebug;
	//if (pTrackedDevice->m_DeviceClass == ETrackedDeviceClass::TrackedDeviceClass_HMD)
	//	pSharedDataDebug->m_fDeltaTime = fDeltaTime;

	//if (m_pDriverInstance->m_bActivated)
	//{
	//	if (pMovement->m_bInCalibration && pos.poseIsValid && pTrackedDevice->m_DeviceClass == ETrackedDeviceClass::TrackedDeviceClass_HMD)
	//	{
	//		pMovement->m_bInCalibration = false;
	//		// send message to ask matrix
	//		CSI_MessageCalibration msgCalib;
	//		m_pDriverInstance->m_pSharedMessageHandleOut->PushMessage(&msgCalib);
	//	}
	//	else
	//	{
	//		// Init position and orientation of HMD (look forward)
	//		if (pMovement->m_bAskCalibration && pos.poseIsValid && pTrackedDevice->m_DeviceClass == ETrackedDeviceClass::TrackedDeviceClass_HMD)
	//		{
	//			// Calibration
	//			Calibration(pMovement, pos);
	//			CSI_MessageAckCalibration msgCalibrated;
	//			m_pDriverInstance->m_pSharedMessageHandleOut->PushMessage(&msgCalibrated);

	//			/******DEBUG******/
	//			CSISharedDataDebug *pSharedDataDebug = m_pDriverInstance->m_pSharedDataDebug;
	//			pSharedDataDebug->m_PositionInitial.m_fx = pMovement->m_v3PositionHmd.v[0];
	//			pSharedDataDebug->m_PositionInitial.m_fy = pMovement->m_v3PositionHmd.v[1];
	//			pSharedDataDebug->m_PositionInitial.m_fz = pMovement->m_v3PositionHmd.v[2];

	//			pSharedDataDebug->m_RotationInitial.m_fx = pMovement->m_qRotationInitial.x;
	//			pSharedDataDebug->m_RotationInitial.m_fy = pMovement->m_qRotationInitial.y;
	//			pSharedDataDebug->m_RotationInitial.m_fz = pMovement->m_qRotationInitial.z;
	//			pSharedDataDebug->m_RotationInitial.m_fw = pMovement->m_qRotationInitial.w;
	//			/******END DEBUG******/
	//		}

	//		if (m_bCalibrated)
	//		{
	//			// Active VR Locomotion
	//			if (m_pDriverInstance->m_pSharedData->m_bActivateVRLocomotion)
	//			{
	//				// Get 3dRudder input
	//				if (!isnan(fDeltaTime) && !isinf(fDeltaTime))
	//					ComputeMovement(fDeltaTime);

	//				// Apply Rotation
	//				pos.qRotation = pos.qRotation * pMovement->m_qRotation;
	//				// Compute the difference between the hmd and the controller
	//				HmdVector3d_t controllerFromHead;
	//				for (int i = 0; i < 3; ++i)
	//					controllerFromHead.v[i] = pos.vecPosition[i] - pMovement->m_v3PositionHmd.v[i];
	//				// Rotate by angle
	//				Transform(pMovement->m_qRotation, controllerFromHead);
	//				// Apply Movement
	//				for (int i = 0; i < 3; ++i)
	//					pos.vecPosition[i] = pMovement->m_v3PositionHmd.v[i] + pMovement->m_v3Translation.v[i] + controllerFromHead.v[i];

	//				// Save current pose to use on ServerDriver::RunFrame
	//				pTrackedDevice->m_currentPose = newPose;
	//			}

	//			// Height offset				
	//			pMovement->m_v3OffsetY = MultVector(pMovement->m_v3AxisY, -m_pDriverInstance->m_pSharedData->m_fHeightOffset);
	//			for (int i = 0; i < 3; ++i)
	//				pos.vecPosition[i] += pMovement->m_v3OffsetY.v[i];
	//		}
	//	}
	//}
}
