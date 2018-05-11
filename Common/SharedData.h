#pragma once

#include <stdio.h>
#include <string>
#include "SharedMemory.h"

class CSISharedData
{
public:

	enum AxisValue
	{
		avDefault,
		avLeftRight,
		avBackwardForward,
		avUpDown,
		avRotationLeftRight
	};

	enum HalfAxisValue
	{
		havDefault,
		havLeft,
		havRight,
		havBackward,
		havForward,
		havUp,
		havDown,
		havRotationLeft,
		havRotationRight
	};

	// right-handed system
	// +y is up
	// +x is to the right
	// -z is going away from you
	// Distance unit is  meters
	struct Matrix34
	{
		float m[3][4];
	};

	class Curve
	{
	public:
		Curve()
		{
			m_fDeadZone = 0; m_fxSat = 1; m_fyMax = 1; m_fExp = 1;
		}
		Curve(float fDeadZone, float fxSat, float fyMax, float fExp)
		{
			m_fDeadZone = fDeadZone; m_fxSat = fxSat; m_fyMax = fyMax; m_fExp = fExp;
		}

		float CalcCurveValue(float x)
		{
			float y;
			//memorisation du signe et calcul valeur absolue
			int signe = 1;

			if (x < 0.0)
			{
				signe = -1;
				x = -x;
			}

			//dead zone
			if (x <= m_fDeadZone)
				return 0; //dead zone
			else
				x = x - m_fDeadZone;

			//normalisation 
			x = x / (m_fxSat - m_fDeadZone);

			//Sortie du polynome
			y = m_fyMax * powf(x, m_fExp);

			//Saturation haute
			if (y > m_fyMax)
				y = m_fyMax;

			//Affectation du bon signe
			if (signe < 0)
				y = -y;

			return y;
		}

		float GetDeadZone()  const { return m_fDeadZone; }
		float GeXSat()		 const { return m_fxSat; }
		float GetYMax()		 const { return m_fyMax; }
		float GetExp()		 const { return m_fExp; }

		void SetDeadZone(float fV) { m_fDeadZone = fV; }
		void SeXSat(float fV) { m_fxSat = fV; }
		void SetYMax(float fV) { m_fyMax = fV; }
		void SetExp(float fV) { m_fExp = fV; }

		float m_fDeadZone;
		float m_fxSat;
		float m_fyMax;
		float m_fExp;
	};

	

	CSISharedData()
	{
		m_nPadIndex[0] = 0;
		m_nPadIndex[1] = 0;

		m_Curve[0] = Curve(2.0f / 18.0f, 1.0f, 1.0f, 2.0f);
		m_Curve[1] = Curve(2.0f / 18.0f, 1.0f, 1.0f, 2.0f);
		//m_Curve[ns3dRudder::CurveZAxis] = *Curve.GetCurve(ns3dRudder::CurveZAxis);
		//m_Curve[ns3dRudder::CurveZRotation] = *Curve.GetCurve(ns3dRudder::CurveZRotation);
		m_bNonSymPitch = true;
		m_fHeightOffset = 0;
		m_bCalibrateOnInUse = true;

		m_bActivateCtrls = false;
		for (int i = 0; i < 2; i++)
		{
			m_bOverridePad[i] = false;
			m_bAxisOnlyTouched[i] = false;
		}
		for (int i = 0; i < 4; i++)
			m_nAxis[i] = avDefault;
		for (int i = 0; i < 2; i++)
			m_nHalfAxis[i] = havDefault;

		m_bActivateVRLocomotion = false;
		m_bActivateTrackpadLeft = false;
		m_bActivateTrackpadRight = false;
		m_bSmoothMode = false;
		for (int i = 0; i < 4; i++)
		{
			m_fSpeed[i] = 1.0f;
			m_fSmoothValue[i] = 0.15f;
		}
	}

	Curve m_Curve[2];
	bool m_bNonSymPitch;
	bool m_bCalibrateOnInUse;
	float m_fHeightOffset;

	bool m_bActivateCtrls;
	int m_nPadIndex[2];
	bool m_bOverridePad[2];
	bool m_bAxisOnlyTouched[2];

	AxisValue m_nAxis[4];
	HalfAxisValue m_nHalfAxis[2];

	bool m_bActivateVRLocomotion;
	bool m_bActivateTrackpadLeft;
	bool m_bActivateTrackpadRight;
	float m_fSpeed[4];
	bool m_bSmoothMode;
	float m_fSmoothValue[4];

	int currentSaveVersion = 1;
	int lastLoadStatus = -1;

	int Load(wchar_t *pPathName)
	{
		int bRet = 0;
		FILE *pFile;
		_wfopen_s(&pFile, pPathName, L"rt");
		if (pFile)
		{
			int nVal1;
			int nVal2;
			int nVal3;
			int nVal4;
			int nVal5;
			int nVal6;
			//int nVal7;

			float fVal1;
			float fVal2;
			float fVal3;
			float fVal4;

			// First do a check against the version we can load. For instance, version 1.0 of the application would only be able to load files of 1.0 and below.
			// Version 1.1 should be able to load configs from 1.0. Just use a generic int for now that increases each time we add more variables to the save/load functions.
			// This would also catch cases where we are trying to load a 1.1 config in 1.0 as the version number would be too low.
			int nNbVal = fscanf_s(pFile, "%d\n", &nVal1);
			if (nNbVal > currentSaveVersion)
			{
				bRet = 2;
				lastLoadStatus = nNbVal;
			}

			if (nVal1 <= currentSaveVersion)
			{
				int i = 0;
				/*nNbVal = fscanf_s(pFile, "%d,%d,%d,%d,%d,%d,%d\n", &nVal1, &nVal2, &nVal3, &nVal4, &nVal5, &nVal6, &nVal7);
				m_bActivateCtrls = !!nVal1;
				m_nAxis[0] = (AxisValue)nVal2;
				m_nAxis[1] = (AxisValue)nVal3;
				m_nAxis[2] = (AxisValue)nVal5;
				m_nAxis[3] = (AxisValue)nVal6;
				m_nHalfAxis[0] = (HalfAxisValue)nVal4;
				m_nHalfAxis[1] = (HalfAxisValue)nVal7;*/

				nNbVal = fscanf_s(pFile, "%d, %d, %d, %d. %d, %d\n", &nVal1, &nVal2, &nVal3, &nVal4, &nVal5, &nVal6);
				m_bActivateCtrls = !!nVal1;
				m_bActivateTrackpadLeft = !!nVal2;
				m_bActivateTrackpadRight = !!nVal3;
				m_bAxisOnlyTouched[0] = !!nVal4;
				m_bAxisOnlyTouched[1] = !!nVal5;
				m_bActivateVRLocomotion = !!nVal6;


			/*	nNbVal = fscanf_s(pFile, "%d,%d,%f,%f,%f,%f\n", &nVal1, &nVal2, &fVal1, &fVal2, &fVal3, &fVal4);
				m_bActivateVRLocomotion = !!nVal1;
				m_fSpeed[0] = fVal1;
				m_fSpeed[1] = fVal2;
				m_fSpeed[2] = fVal3;
				m_fSpeed[3] = fVal4;
				m_bSmoothMode = !!nVal2;*/

				/*for (i = 0; i < 4; i++)
				{
					nNbVal = fscanf_s(pFile, "%f\n", &fVal1);
					m_fSmoothValue[i] = fVal1;
				}*/

				//nNbVal = fscanf_s(pFile, "%d,%d,%f\n", &nVal1, &nVal2, &fVal1);
				//m_bNonSymPitch = !!nVal1;
				//m_bCalibrateOnInUse = !!nVal2;
				//m_fHeightOffset = fVal1;


				for (i = 0; i < 2; i++)
				{
					nNbVal = fscanf_s(pFile, "%f,%f,%f,%f\n", &fVal1, &fVal2, &fVal3, &fVal4);
					m_Curve[i].m_fDeadZone = fVal1;
					m_Curve[i].m_fxSat = fVal2;
					m_Curve[i].m_fExp = fVal3;
					m_Curve[i].m_fyMax = fVal4;
				}
				bRet = 1; // 1 is success
				lastLoadStatus = 1;
			}
			fclose(pFile);
		}

		// 0: default Error.
		// 1: Success
		// 2: Config with newer save version (update software).
		return bRet;
	}

	bool Save(wchar_t *pPathName)
	{
		bool bRet = false;
		FILE *pFile;
		_wfopen_s(&pFile, pPathName, L"wt");
		if (pFile)
		{
			int i;
			// NOTE: Everytime we include more variables to this save function, we must increment the number below. This must then be checked against during the load function.
			// This helps with versioning and keeping save configs between versions of the software.
			fprintf(pFile, "%d\n", currentSaveVersion);
			//fprintf(pFile, "%d, %d, %d, %d, %d, %d, %d\n", m_bActivateCtrls, m_nAxis[0], m_nAxis[1], m_nHalfAxis[0], m_nAxis[2], m_nAxis[3], m_nHalfAxis[1]);
			fprintf(pFile, "%d, %d, %d, %d. %d, %d\n", m_bActivateCtrls, m_bActivateTrackpadLeft, m_bActivateTrackpadRight, m_bAxisOnlyTouched[0], m_bAxisOnlyTouched[1], m_bActivateVRLocomotion);

			//fprintf(pFile, "%d, %d, %f, %f, %f, %f\n", m_bActivateVRLocomotion, m_bSmoothMode, m_fSpeed[0], m_fSpeed[1], m_fSpeed[2], m_fSpeed[3]);
			//for (i = 0; i < 4; i++)
			//	fprintf(pFile, "%f\n", m_fSmoothValue[i]);

			//fprintf(pFile, "%d, %d, %f\n", m_bNonSymPitch, m_bCalibrateOnInUse, m_fHeightOffset);
			for (i = 0; i < 2; i++)
			{
				fprintf(pFile, "%f, %f, %f, %f\n", m_Curve[i].m_fDeadZone, m_Curve[i].m_fxSat, m_Curve[i].m_fExp, m_Curve[i].m_fyMax);
			}

			fclose(pFile);
			bRet = true;
		}
		return bRet;
	}
};

class CSISharedDataDebug
{
public:

	//CSISharedDataDebug()
	//{
	//	m_RotationInitial = CVector::CVector();
	//	m_PositionInitial = CVector::CVector();
	//	m_TranslationXYZAngleW = CVector::CVector();
	//	m_SmoothSpeed = CVector::CVector();
	//}

	class CVector
	{
	public:
		CVector()
		{
			m_fx = 0.0f;
			m_fy = 0.0f;
			m_fz = 0.0f;
			m_fw = 0.0f;
		}

		void SetVector(double fx, double fy, double fz, double fw) 
		{
			m_fx = fx;
			m_fy = fy;
			m_fz = fz;
			m_fw = fw;
		}

		double m_fx;
		double m_fy;
		double m_fz;
		double m_fw;
	};

	CVector m_RotationInitial;
	CVector m_PositionInitial;
	CVector m_TranslationXYZAngleW;
	CVector m_RawXYZAngle;
	CVector m_SmoothSpeed;
	double m_fDeltaTime;

	std::string m_lastStatus;
};



typedef enum :uint32_t
{
	Reset,
	AskCalibration,
	Calibration,
	AckCalibration,
	DriverInfo,
	ActivateDriver,
	IsActivatedDriver,
	WrongVersionOpenVR,
	SendCtrlInfo,
	QuitBridge

}CwsSiMessageeType;



class CSI_MessageReset : public CSI_Message
{
public:
	CSI_MessageReset()
	{
		m_nLen = sizeof(CSI_MessageReset);
		m_nType = CwsSiMessageeType::Reset;
	}
	bool m_bReset;
};

class CSI_MessageAskCalibration : public CSI_Message
{
public:
	CSI_MessageAskCalibration()
	{
		m_nLen = sizeof(CSI_MessageAskCalibration);
		m_nType = CwsSiMessageeType::AskCalibration;
	}
};

class CSI_MessageAckCalibration : public CSI_Message
{
public:
	CSI_MessageAckCalibration()
	{
		m_nLen = sizeof(CSI_MessageAckCalibration);
		m_nType = CwsSiMessageeType::AckCalibration;
	}
};


class CSI_MessageCalibration : public CSI_Message
{
public:
	CSI_MessageCalibration()
	{
		m_nLen = sizeof(CSI_MessageCalibration);
		m_nType = CwsSiMessageeType::Calibration;
		m_bCalib = false;
	}
	CSISharedData::Matrix34 m_matrix;
	bool m_bCalib;
};

//////////////////////////////////////////////////////////////////////////
// Not sure about these

class CSI_MessageActivateDriver : public CSI_Message
{
public:
	CSI_MessageActivateDriver()
	{
		m_nLen = sizeof(CSI_MessageActivateDriver);
		m_nType = CwsSiMessageeType::ActivateDriver;
		m_bActive = false;
	}
	bool m_bActive;
};

class CSI_MessageWrongVersionOpenVR : public CSI_Message
{
public:
	CSI_MessageWrongVersionOpenVR()
	{
		m_nLen = sizeof(CSI_MessageActivateDriver);
		m_nType = CwsSiMessageeType::WrongVersionOpenVR;
		m_hModule = NULL;
	}
	TCHAR m_sModuleName[_MAX_PATH];
	HMODULE m_hModule;
};