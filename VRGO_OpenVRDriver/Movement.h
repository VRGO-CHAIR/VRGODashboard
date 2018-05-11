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
#include "../Common/SharedData.h"
#include "../Common/MathTools.h"

using namespace vr;

class CMovement
{
public:
	HmdVector3d_t m_v3PositionHmd;
	HmdVector3d_t m_v3AxisX;
	HmdVector3d_t m_v3AxisY;
	HmdVector3d_t m_v3AxisZ;
	HmdVector3d_t m_v3Translation;
	HmdVector3d_t m_v3OffsetY;

	HmdQuaternion_t m_qRotation;
	HmdQuaternion_t m_qRotationInitial;

	CSISharedData::Matrix34 m_mMatrix34Hmd;

	struct SmoothMovement
	{
		double speedCurrent;
		// Inertia formula
		double ComputeSpeed(double input, double speed, double smoothness, double deltatime)
		{
			double speedTarget = input * speed;
			double acceleration = (speedTarget - speedCurrent) / smoothness;
			speedCurrent = speedCurrent + (acceleration * deltatime);
			return speedCurrent;
		}
	};
	SmoothMovement m_SmoothPitch;
	SmoothMovement m_SmoothRoll;
	SmoothMovement m_SmoothYaw;
	SmoothMovement m_SmoothUpDown;

	bool m_bInCalibration;
	bool m_bAskCalibration;
	double m_fAngle;

	struct CopyResetData
	{
		double fAngle;
		HmdQuaternion_t qRotation;
		HmdVector3d_t v3Translation;
	};
	CopyResetData m_SavedMovement;

	CMovement()
	{
		Identity(m_qRotation);
		Identity(m_qRotationInitial);
		Identity(m_SavedMovement.qRotation);
		m_bInCalibration = false;
		m_bAskCalibration = false;
		m_fAngle = 0;
		m_SavedMovement.fAngle = 0;
		for (int i = 0; i < 3; ++i)
		{
			m_v3PositionHmd.v[i] = 0;
			m_v3AxisX.v[i] = 0;
			m_v3AxisY.v[i] = 0;
			m_v3AxisZ.v[i] = 0;
			m_v3Translation.v[i] = 0;
			m_v3OffsetY.v[i] = 0;
			m_SavedMovement.v3Translation.v[i];
		}
	}
	~CMovement()
	{
	}

	void Reset(bool restoreMovement = false);
	void Restore();


};

