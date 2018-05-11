#pragma once

//#define _3DRUDDER_SDK_STATIC
//#include "3dRudderSDK.h"
#include "..\Common\SharedData.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "OpenGlCtrl.h"


// C3dRudderUnleashedVRConfig dialog
class CVRGOConfig : public CDialogEx
{
	// Construction
public:
	CVRGOConfig(CWnd* pParent = NULL);	// standard constructor

														// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VRGO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CSISharedData m_SharedData;
	CSharedMemory m_SharedMemory;
	CSharedMemory m_SharedMemoryDebug;
	//ns3dRudder::Axis m_Axis;
	//ns3dRudder::CSdk *m_pSdk;
	CString m_sSteamPath;
	// Implementation
protected:
	HICON m_hIcon;
	void SendMatrixHmd();
	void RefreshController();
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtCurveX();
	afx_msg void OnBnClickedBtCurveY();
	afx_msg void OnBnClickedBtCurveZ();
	afx_msg void OnBnClickedBtCurveRz();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtLoad();
	afx_msg void OnBnClickedBtSave();
	afx_msg void OnBnClickedCkActivateCtrls();
	afx_msg void OnBnClickedCkActivateVRLocomotion();
	afx_msg void OnBnClickedCKActivateLeftControllerLocomotion();
	afx_msg void OnBnClickedCKActivateRightControllerLocomotion();
	afx_msg void OnBnClickedBtActivate();
	afx_msg void OnBnClickedBtReset();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtCalibration();
	afx_msg void OnBnClickedBtDebug();
	afx_msg void OnEnChangeEdSpeedAx();
	afx_msg void OnEnChangeEdSpeedAy();
	afx_msg void OnEnChangeEdSpeedAz();
	afx_msg void OnEnChangeEdSpeedRz();
	afx_msg void OnEnChangeEdHeightOffset();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCkSmoothMode();
	afx_msg void OnEnChangeEdSmoothAx();
	afx_msg void OnEnChangeEdSmoothAy();
	afx_msg void OnEnChangeEdSmoothAz();
	afx_msg void OnEnChangeEdSmoothRz();

	void UpdateSliderValue();
	void ManageActivateUI();
	void CheckActivate();
	void InitOpenVR();
	int Load(wchar_t *pPath);

	bool IsSDKRunning();

	CComboBox m_CbCtrl1;
	CComboBox m_CbCtrl2;
	BOOL m_bNonSymPitch;
	BOOL m_bActivateCtrls;
	BOOL m_bActivateVRLocomotion;
	CString m_sSpeedAxisLR;
	CString m_sSpeedAxisFB;
	CString m_sSpeedAxisZ;
	CString m_sSpeedRotateZ;
	CEdit m_EdSpeedLR;
	CEdit m_EdSpeedFB;
	CEdit m_EdSpeedAz;
	CEdit m_EdSpeedRz;
	float m_fDPIx;
	float m_fDPIy;
	CStatic m_StVal1;
	CStatic m_StRawInput;
	CString m_sHeightOffset;
	CEdit m_EdHeightOffset;
	CButton m_CkUpDown;
	CButton m_BtReset;
	CComboBox m_CbDriver;
	BOOL m_bActivated;
	CButton m_BtActivate;
	CButton m_BtCalibration;
	CStatic m_StSDKRunning;
	CStatic m_StDriverSteam32;
	CStatic m_StDriverSteam64;
	CStatic m_StDriverVRGO32;
	CStatic m_StDriverVRGO64;
	CStatic m_StVal3;
	CStatic m_StVal4;
	CStatic m_StDeltaTime;
	CStatic m_StVal6;
	CStatic m_StRLActive;
	CStatic m_StRLIndex;
	BOOL m_bCalibrateInUse;
	CButton m_BtCalibrateInUse;
	CButton m_BtActivateVRLocomotion;
	CButton m_BtActivateTrackpadLeft;
	CButton m_BtActivateTrackpadRight;
	CButton m_BtActivateControllers;
	CComboBox m_CbLeftH;
	CComboBox m_CbLeftV;
	CComboBox m_CbLeftT;
	CComboBox m_CbRightH;
	CComboBox m_CbRightV;
	CComboBox m_CbRightT;
	CStatic m_StSpeedAx;
	CStatic m_StSpeedAy;
	CStatic m_StSpeedAz;
	CStatic m_StSpeedRz;
	CStatic m_StLCtrl;
	CStatic m_StLeftH;
	CStatic m_StLeftV;
	CStatic m_StLeftT;
	CStatic m_StRCtrl;
	CStatic m_StRightH;
	CStatic m_StRightV;
	CStatic m_StRightT;
	CButton m_BtDebug;
	CStatic m_StDebug;
	COpenGlCtrl m_St3D;
	CSliderCtrl m_SlSpeedAx;
	CSliderCtrl m_SlSpeedAy;
	CSliderCtrl m_SlSpeedAz;
	CSliderCtrl m_SlSpeedRz;
	CSliderCtrl m_SlHOffset;
	CSliderCtrl m_SlSmoothnessAx;
	CSliderCtrl m_SlSmoothnessAy;
	CSliderCtrl m_SlSmoothnessAz;
	CSliderCtrl m_SlSmoothnessRz;
	CString m_sSmoothnessAx;
	CString m_sSmoothnessAy;
	CString m_sSmoothnessAz;
	CString m_sSmoothnessRz;
	CEdit m_EdSmoothAx;
	CEdit m_EdSmoothAy;
	CEdit m_EdSmoothAz;
	CEdit m_EdSmoothRz;
	CStatic m_StSmotthAx;
	CStatic m_StSmotthAy;
	CStatic m_StSmotthAz;
	CStatic m_StSmotthRz;
	CButton m_BtSmoothMode;

	int m_MaxSpeedAx;
	int m_MaxSpeedAy;
	int m_MaxSpeedAz;
	int m_MaxSpeedRz;
	int m_nMaxHOffset;
	BOOL m_bSmoothMode;
	BOOL m_bDebug;

	BOOL m_bExitWait;
	CString m_sConfigLoaded;

	CButton m_BtOverrideLeft;
	BOOL m_bOverrideLeft;
	CButton m_BtOverrideRight;
	BOOL m_bOverrideRight;
	CButton m_BtTrackPadLeftOnlyTouch;
	BOOL m_bTrackPadLeftOnlyTouch;
	CButton m_BtTrackPadRightOnlyTouch;
	BOOL m_bTrackPadRightOnlyTouch;
};