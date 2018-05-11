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
#include "VRGO_Config.h"
#include "VRGO_ConfigDlg.h"
#include "afxdialogex.h"
#include "DlgCurve.h"
#include <string>
#include "../Common/openvr.h"
#include "ProcessManagement.h"
#include "Registry.h"
#include "..\Common\MathTools.h"
#include "WaitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CString GetVersionProduct()
{
	TCHAR moduleName[MAX_PATH + 1];
	(void)GetModuleFileName(AfxGetInstanceHandle(), moduleName, MAX_PATH);
	DWORD dummyZero;
	DWORD versionSize = GetFileVersionInfoSize(moduleName, &dummyZero);
	if (versionSize == 0)
	{
		return CString(_T(""));
	}
	void* pVersion = malloc(versionSize);
	if (pVersion == NULL)
	{
		return CString(_T(""));
	}
	if (!GetFileVersionInfo(moduleName, NULL, versionSize, pVersion))
	{
		free(pVersion);
		return CString(_T(""));
	}

	UINT length;
	VS_FIXEDFILEINFO* pFixInfo;
	if (!VerQueryValue(pVersion, TEXT("\\"), (LPVOID*)&pFixInfo, &length))
	{
		free(pVersion);
		return CString(_T(""));
	}

	CString strProductVersion;
	strProductVersion.Format(_T("%u.%u.%u.%u"),
		HIWORD(pFixInfo->dwProductVersionMS),
		LOWORD(pFixInfo->dwProductVersionMS),
		HIWORD(pFixInfo->dwProductVersionLS),
		LOWORD(pFixInfo->dwProductVersionLS));

	free(pVersion);

	return strProductVersion;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CString m_sVersion;
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
, m_sVersion(_T(""))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ST_VERSION, m_sVersion);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_sVersion.Format(_T("VRGO Dashboard  v%s"), GetVersionProduct());
	UpdateData(FALSE);

	return TRUE;
}


// C3dRudderUnleashedVRConfig dialog

CSISharedMessageHandle g_SharedMessageHandleIn(_T("VRGO_SI_MESS_IN"));
CSISharedMessageHandle g_SharedMessageHandleOut(_T("VRGO_SI_MESS_OUT"));


CVRGOConfig::CVRGOConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VRGO_DIALOG, pParent)
	, m_bNonSymPitch(TRUE)
	, m_bActivateCtrls(FALSE)
	, m_bActivateVRLocomotion(FALSE)
	, m_sSpeedAxisLR(_T("1.0"))
	, m_sSpeedAxisFB(_T("1.0"))
	, m_sSpeedAxisZ(_T("1.0"))
	, m_sSpeedRotateZ(_T("1.0"))
	, m_sHeightOffset(_T("0"))
	, m_bCalibrateInUse(FALSE)
	, m_bSmoothMode(FALSE)
	, m_sSmoothnessAx(_T(""))
	, m_sSmoothnessAy(_T(""))
	, m_sSmoothnessAz(_T(""))
	, m_sSmoothnessRz(_T(""))
	, m_sConfigLoaded(_T("No Config Loaded"))
	, m_bOverrideLeft(FALSE)
	, m_bOverrideRight(FALSE)
	, m_bTrackPadLeftOnlyTouch(FALSE)
	, m_bTrackPadRightOnlyTouch(FALSE)
{
	m_bActivated = FALSE;
	m_bDebug = TRUE;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_VRGO_LOGO);// IDR_MAINFRAME);

}

void CVRGOConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Check(pDX, IDC_CK_NON_SYM_PITCH, m_bNonSymPitch);
	DDX_Check(pDX, IDC_CK_ACTIVATE_CTRLS, m_bActivateCtrls);
	DDX_Check(pDX, IDC_CK_ACTIVATE_VRLOCOMOTION, m_bActivateVRLocomotion);
	//DDX_Text(pDX, IDC_ED_SPEED_LR, m_sSpeedAxisLR);
	//DDX_Text(pDX, IDC_ED_SPEED_FB, m_sSpeedAxisFB);
	//DDX_Text(pDX, IDC_ED_SPEED_AZ, m_sSpeedAxisZ);
	//DDX_Text(pDX, IDC_ED_SPEED_RZ, m_sSpeedRotateZ);
	DDX_Control(pDX, IDC_ED_SPEED_LR, m_EdSpeedLR);
	DDX_Control(pDX, IDC_ED_SPEED_FB, m_EdSpeedFB);
	//DDX_Control(pDX, IDC_ED_SPEED_AZ, m_EdSpeedAz);
	//DDX_Control(pDX, IDC_ED_SPEED_RZ, m_EdSpeedRz);
	//DDX_Text(pDX, IDC_ED_HEIGHT_OFFSET, m_sHeightOffset);
	//DDX_Control(pDX, IDC_ED_HEIGHT_OFFSET, m_EdHeightOffset);
	DDX_Control(pDX, IDC_BT_RESET, m_BtReset);
	DDX_Control(pDX, IDC_CB_DRIVER, m_CbDriver);
	DDX_Control(pDX, IDC_BT_ACTIVATE, m_BtActivate);
	DDX_Control(pDX, IDC_BT_CALIBRATION, m_BtCalibration);
	DDX_Control(pDX, IDC_ST_SDKRUNNING, m_StSDKRunning);
	DDX_Control(pDX, IDC_ST_DRIVERPATH_STEAM_32, m_StDriverSteam32);
	DDX_Control(pDX, IDC_ST_DRIVERPATH_STEAM_64, m_StDriverSteam64);
	DDX_Control(pDX, IDC_ST_DRIVERPATH_VRGO32, m_StDriverVRGO32);
	DDX_Control(pDX, IDC_ST_DRIVERPATH_VRGO64, m_StDriverVRGO64);
	DDX_Control(pDX, IDC_ST_VAL1, m_StVal1);
	DDX_Control(pDX, IDC_ST_RAWINPUT, m_StRawInput);
	DDX_Control(pDX, IDC_ST_VAL3, m_StVal3);
	//DDX_Control(pDX, IDC_ST_VAL4, m_StVal4);
	DDX_Control(pDX, IDC_ST_DELTATIME, m_StDeltaTime);
	DDX_Control(pDX, IDC_ST_RLACTIVE, m_StRLActive);
	DDX_Control(pDX, IDC_ST_RLINDEX, m_StRLIndex);
	DDX_Check(pDX, IDC_CK_CALIBRATE_INUSE, m_bCalibrateInUse);
	DDX_Control(pDX, IDC_CK_CALIBRATE_INUSE, m_BtCalibrateInUse);
	DDX_Control(pDX, IDC_CK_ACTIVATE_VRLOCOMOTION, m_BtActivateVRLocomotion);
	DDX_Control(pDX, IDC_CK_ACTIVATE_CTRLS, m_BtActivateControllers);
	//DDX_Control(pDX, IDC_CB_L_H, m_CbLeftH);
	//DDX_Control(pDX, IDC_CB_L_V, m_CbLeftV);
	//DDX_Control(pDX, IDC_CB_L_T, m_CbLeftT);
	//DDX_Control(pDX, IDC_CB_R_H, m_CbRightH);
	//DDX_Control(pDX, IDC_CB_R_V, m_CbRightV);
	//DDX_Control(pDX, IDC_CB_R_T, m_CbRightT);
	DDX_Control(pDX, IDC_ST_SPEED_AX, m_StSpeedAx);
	DDX_Control(pDX, IDC_ST_SPEED_AY, m_StSpeedAy);
	//DDX_Control(pDX, IDC_ST_SPEED_AZ, m_StSpeedAz);
	//DDX_Control(pDX, IDC_ST_SPEED_RZ, m_StSpeedRz);
	/*DDX_Control(pDX, IDC_ST_L_CTRL, m_StLCtrl);
	DDX_Control(pDX, IDC_ST_L_H, m_StLeftH);
	DDX_Control(pDX, IDC_ST_L_V, m_StLeftV);
	DDX_Control(pDX, IDC_ST_L_T, m_StLeftT);
	DDX_Control(pDX, IDC_ST_R_CTRL, m_StRCtrl);
	DDX_Control(pDX, IDC_ST_R_H, m_StRightH);
	DDX_Control(pDX, IDC_ST_R_V, m_StRightV);
	DDX_Control(pDX, IDC_ST_R_T, m_StRightT);*/
	DDX_Control(pDX, IDC_BT_DEBUG, m_BtDebug);
	DDX_Control(pDX, IDC_ST_DEBUG, m_StDebug);
	DDX_Control(pDX, IDC_ST_3D, m_St3D);
	DDX_Control(pDX, IDC_SL_SPEED_AX, m_SlSpeedAx);
	DDX_Control(pDX, IDC_SL_SPEED_AY, m_SlSpeedAy);
	//DDX_Control(pDX, IDC_SL_SPEED_AZ, m_SlSpeedAz);
	//DDX_Control(pDX, IDC_SL_SPEED_RZ, m_SlSpeedRz);
	//DDX_Control(pDX, IDC_SL_HOFFSET, m_SlHOffset);
	//DDX_Check(pDX, IDC_CK_SMOOTH_MODE, m_bSmoothMode);
	//DDX_Control(pDX, IDC_CK_SMOOTH_MODE, m_BtSmoothMode);
	//DDX_Control(pDX, IDC_SL_SMOOTH_AX, m_SlSmoothnessAx);
	//DDX_Control(pDX, IDC_SL_SMOOTH_AY, m_SlSmoothnessAy);
	//DDX_Control(pDX, IDC_SL_SMOOTH_AZ, m_SlSmoothnessAz);
	//DDX_Control(pDX, IDC_SL_SMOOTH_RZ, m_SlSmoothnessRz);
	//DDX_Text(pDX, IDC_ED_SMOOTH_AX, m_sSmoothnessAx);
	//DDX_Text(pDX, IDC_ED_SMOOTH_AY, m_sSmoothnessAy);
	//DDX_Text(pDX, IDC_ED_SMOOTH_AZ, m_sSmoothnessAz);
	//DDX_Text(pDX, IDC_ED_SMOOTH_RZ, m_sSmoothnessRz);
	//DDX_Control(pDX, IDC_ED_SMOOTH_AX, m_EdSmoothAx);
	//DDX_Control(pDX, IDC_ED_SMOOTH_AY, m_EdSmoothAy);
	//DDX_Control(pDX, IDC_ED_SMOOTH_AZ, m_EdSmoothAz);
	//DDX_Control(pDX, IDC_ED_SMOOTH_RZ, m_EdSmoothRz);
	//DDX_Control(pDX, IDC_ST_SMOOTH_AX, m_StSmotthAx);
	//DDX_Control(pDX, IDC_ST_SMOOTH_AY, m_StSmotthAy);
	//DDX_Control(pDX, IDC_ST_SMOOTH_AZ, m_StSmotthAz);
	//DDX_Control(pDX, IDC_ST_SMOOTH_RZ, m_StSmotthRz);
	DDX_Control(pDX, IDC_CK_INPUTTRACKPAD_L, m_BtOverrideLeft);
	DDX_Check(pDX, IDC_CK_INPUTTRACKPAD_L, m_bOverrideLeft);
	DDX_Control(pDX, IDC_CK_INPUTTRACKPAD_R, m_BtOverrideRight);
	DDX_Check(pDX, IDC_CK_INPUTTRACKPAD_R, m_bOverrideRight);
	DDX_Control(pDX, IDC_CK_TOUCHONLY_L, m_BtTrackPadLeftOnlyTouch);
	DDX_Check(pDX, IDC_CK_TOUCHONLY_L, m_bTrackPadLeftOnlyTouch);
	DDX_Control(pDX, IDC_CK_TOUCHONLY_R, m_BtTrackPadRightOnlyTouch);
	DDX_Check(pDX, IDC_CK_TOUCHONLY_R, m_bTrackPadRightOnlyTouch);
	DDX_Text(pDX, IDC_ST_CONFIG_LOADED, m_sConfigLoaded);
}

BEGIN_MESSAGE_MAP(CVRGOConfig, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CVRGOConfig::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CVRGOConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BT_CURVE_X, &CVRGOConfig::OnBnClickedBtCurveX)
	ON_BN_CLICKED(IDC_BT_CURVE_Y, &CVRGOConfig::OnBnClickedBtCurveY)
	/*ON_BN_CLICKED(IDC_BT_CURVE_Z, &CVRGOConfig::OnBnClickedBtCurveZ)
	ON_BN_CLICKED(IDC_BT_CURVE_RZ, &CVRGOConfig::OnBnClickedBtCurveRz)*/
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BT_LOAD, &CVRGOConfig::OnBnClickedBtLoad)
	ON_BN_CLICKED(IDC_BT_SAVE, &CVRGOConfig::OnBnClickedBtSave)
	ON_BN_CLICKED(IDC_CK_ACTIVATE_CTRLS, &CVRGOConfig::OnBnClickedCkActivateCtrls)
	ON_BN_CLICKED(IDC_CK_ACTIVATE_VRLOCOMOTION, &CVRGOConfig::OnBnClickedCkActivateVRLocomotion)
	ON_BN_CLICKED(IDC_CK_INPUTTRACKPAD_L, &CVRGOConfig::OnBnClickedCKActivateLeftControllerLocomotion)
	ON_BN_CLICKED(IDC_CK_INPUTTRACKPAD_R, &CVRGOConfig::OnBnClickedCKActivateRightControllerLocomotion)
	ON_BN_CLICKED(IDC_BT_ACTIVATE, &CVRGOConfig::OnBnClickedBtActivate)
	ON_BN_CLICKED(IDC_BT_RESET, &CVRGOConfig::OnBnClickedBtReset)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BT_CALIBRATION, &CVRGOConfig::OnBnClickedBtCalibration)
	ON_BN_CLICKED(IDC_BT_DEBUG, &CVRGOConfig::OnBnClickedBtDebug)
	ON_EN_CHANGE(IDC_ED_SPEED_LR, &CVRGOConfig::OnEnChangeEdSpeedAx)
	ON_EN_CHANGE(IDC_ED_SPEED_FB, &CVRGOConfig::OnEnChangeEdSpeedAy)
	ON_EN_CHANGE(IDC_ED_SPEED_AZ, &CVRGOConfig::OnEnChangeEdSpeedAz)
	ON_EN_CHANGE(IDC_ED_SPEED_RZ, &CVRGOConfig::OnEnChangeEdSpeedRz)
	ON_EN_CHANGE(IDC_ED_HEIGHT_OFFSET, &CVRGOConfig::OnEnChangeEdHeightOffset)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CK_SMOOTH_MODE, &CVRGOConfig::OnBnClickedCkSmoothMode)
	ON_EN_CHANGE(IDC_ED_SMOOTH_AX, &CVRGOConfig::OnEnChangeEdSmoothAx)
	ON_EN_CHANGE(IDC_ED_SMOOTH_AY, &CVRGOConfig::OnEnChangeEdSmoothAy)
	ON_EN_CHANGE(IDC_ED_SMOOTH_AZ, &CVRGOConfig::OnEnChangeEdSmoothAz)
	ON_EN_CHANGE(IDC_ED_SMOOTH_RZ, &CVRGOConfig::OnEnChangeEdSmoothRz)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()




// CVRGOConfig message handlers

BOOL CVRGOConfig::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRegistry reg;

	// Find the steam path in the registry
	if (reg.OpenKey(CRegistry::currentUser, _T("SOFTWARE\\Valve\\Steam")))
	{
		if (reg.GetValue(_T("SteamPath"), m_sSteamPath))
		{
			m_sSteamPath.Replace(_T("/"), _T("\\"));
			CString sPath = m_sSteamPath + _T("\\steamapps\\common\\SteamVR\\drivers\\*.*");

			CFileFind finder;
			BOOL bWorking = finder.FindFile(sPath);
			while (bWorking)
			{
				bWorking = finder.FindNextFile();
				if (finder.IsDirectory() && finder.GetFileName() != _T(".") && finder.GetFileName() != _T(".."))
				{
					CFileFind finder2;
					CString sPath2 = finder.GetFilePath() + _T("\\*.*");
					BOOL bWorking2 = finder2.FindFile(sPath2);
					while (bWorking2)
					{
						bWorking2 = finder2.FindNextFile();
						if (finder2.IsDirectory() && finder2.GetFileName() == _T("bin"))
						{
							m_CbDriver.AddString(finder.GetFileName());
						}
					}
				}
			}
		}
		else
		{
			AfxMessageBox(_T("Can't find steam path"));
		}
	}
	else
	{
		AfxMessageBox(_T("Steam is not installed"));
	}


	m_StVal1.SetWindowText(_T(""));
	m_StRawInput.SetWindowText(_T(""));
	m_StVal3.SetWindowText(_T(""));
	m_StDeltaTime.SetWindowText(_T(""));
	m_StRLActive.SetWindowText(_T("Override Trackpad: Left(N/A) Right(N/A)"));

	m_StRLIndex.SetWindowText(_T("Identified Trackpad Indexes: Left(N/A) Right(N/A)"));

	m_StSDKRunning.SetWindowText(_T(""));

	CString sTitleText;
	sTitleText.Format(_T("VRGO Dashboard v%s"), GetVersionProduct());
	SetWindowText(sTitleText);


	ID2D1Factory* m_pDirect2dFactory;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	FLOAT dpiX, dpiY;
	m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

	m_fDPIx = (float)round(dpiX / 100.0f);
	m_fDPIy = (float)round(dpiY / 100.0f);
	OnBnClickedBtDebug();

	m_CbDriver.SetCurSel(m_CbDriver.FindString(0, _T("lighthouse")));

	m_MaxSpeedAx  = 1;
	m_MaxSpeedAy  = 1;
	m_MaxSpeedAz  = 1;
	m_MaxSpeedRz  = 1;
	m_nMaxHOffset = 10;

	m_SharedData.m_fSpeed[0] = (float)_wtof(m_sSpeedAxisLR);
	m_SharedData.m_fSpeed[1] = (float)_wtof(m_sSpeedAxisFB);
	m_SharedData.m_fSpeed[2] = (float)_wtof(m_sSpeedAxisZ);
	m_SharedData.m_fSpeed[3] = (float)_wtof(m_sSpeedRotateZ);
	
	OnEnChangeEdSpeedAx();
	OnEnChangeEdSpeedAy();
	
	// not used
	OnEnChangeEdSpeedAz();
	OnEnChangeEdSpeedRz();
	OnEnChangeEdHeightOffset();
	
	// used
	UpdateSliderValue();

	UpdateData(FALSE);


	m_SharedMemory.Open(_T("VRGO_SI_DATA"), sizeof(CSISharedData));
	m_SharedMemoryDebug.Open(_T("VRGO_SI_DATA_DEBUG"), sizeof(CSISharedDataDebug));

	SetTimer(0, 20, NULL);
	CheckActivate();
	ManageActivateUI();

	FILE *pFile = _wfopen(_T("DefaultConfig.txt"), _T("rt"));
	if (pFile)
	{
		fclose(pFile);

		Load(_T("DefaultConfig.txt"));
	}


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVRGOConfig::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVRGOConfig::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVRGOConfig::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVRGOConfig::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CVRGOConfig::OnBnClickedOk()
{

}


void CVRGOConfig::OnBnClickedBtCurveX()
{
	CSISharedDataDebug SharedDataDebug;
	m_SharedMemoryDebug.Read((uint8_t*)&SharedDataDebug, sizeof(CSISharedDataDebug));
	CDlgCurve dlg(&m_SharedData.m_Curve[0], (float*)&SharedDataDebug.m_TranslationXYZAngleW.m_fx);// &m_Axis.m_aX);
	dlg.DoModal();

}


void CVRGOConfig::OnBnClickedBtCurveY()
{
	CSISharedDataDebug SharedDataDebug;
	m_SharedMemoryDebug.Read((uint8_t*)&SharedDataDebug, sizeof(CSISharedDataDebug));
	CDlgCurve dlg(&m_SharedData.m_Curve[1], (float*)&SharedDataDebug.m_TranslationXYZAngleW.m_fx);// &m_Axis.m_aY);
	dlg.DoModal();

}


void CVRGOConfig::OnBnClickedBtCurveZ()
{
	//CDlgCurve dlg(&m_SharedData.m_Curve[ns3dRudder::CurveZAxis], &m_Axis.m_aZ);
	//dlg.DoModal();

}


void CVRGOConfig::OnBnClickedBtCurveRz()
{
	//CDlgCurve dlg(&m_SharedData.m_Curve[ns3dRudder::CurveZRotation], &m_Axis.m_rZ);
	//dlg.DoModal();

}


void CVRGOConfig::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData(TRUE);

	CSISharedDataDebug SharedDataDebug;
	m_SharedMemoryDebug.Read((uint8_t*)&SharedDataDebug, sizeof(CSISharedDataDebug));

	CString sText;
	// Update the VRGO Tilt angle.
	sText.Format(_T("Moderated Axis Tilt Value : (%f,%f,%f,%f)"), SharedDataDebug.m_TranslationXYZAngleW.m_fx, SharedDataDebug.m_TranslationXYZAngleW.m_fy, SharedDataDebug.m_TranslationXYZAngleW.m_fz, SharedDataDebug.m_TranslationXYZAngleW.m_fw);
	m_StVal3.SetWindowText(sText);
	sText.Format(_T("Raw Axis Tilt Value: (%f,%f,%f,%f)"), SharedDataDebug.m_RawXYZAngle.m_fx, SharedDataDebug.m_RawXYZAngle.m_fy, SharedDataDebug.m_RawXYZAngle.m_fz, SharedDataDebug.m_RawXYZAngle.m_fw);
	m_StRawInput.SetWindowText(sText);
	
	// Debug info of whether the VRGO SDK process is running.
	bool x = IsSDKRunning();
	if (x)	sText.Format(_T("VRGO SDK Running: True"));
	else sText.Format(_T("VRGO SDK Running: False"));
	m_StSDKRunning.SetWindowTextW(sText);

	// Update the delta time debug value.
	if (SharedDataDebug.m_fDeltaTime > 0)
	{
		sText.Format(_T("Delta Time Runframe: (%f)  = FPS : (%f)"), SharedDataDebug.m_fDeltaTime, 1.0 / SharedDataDebug.m_fDeltaTime);
		m_StDeltaTime.SetWindowText(sText);
	}

	// Update 3D model rotation
	m_St3D.UpdateRotation(-(float)SharedDataDebug.m_TranslationXYZAngleW.m_fy, (float)SharedDataDebug.m_TranslationXYZAngleW.m_fz, -(float)SharedDataDebug.m_TranslationXYZAngleW.m_fx);

	m_SharedData.m_nAxis[0] = CSISharedData::AxisValue::avLeftRight;
	m_SharedData.m_nAxis[1] = CSISharedData::AxisValue::avLeftRight;
	m_SharedData.m_nAxis[2] = CSISharedData::AxisValue::avLeftRight;
	m_SharedData.m_nAxis[3] = CSISharedData::AxisValue::avLeftRight;
	m_SharedData.m_nHalfAxis[0] = CSISharedData::HalfAxisValue::havDefault;
	m_SharedData.m_nHalfAxis[1] = CSISharedData::HalfAxisValue::havDefault;

	vr::IVRSystem *pOpenVR = vr::VRSystem();
	if (pOpenVR != nullptr)
	{
		/******DEBUG******/
		// for somebody asking for the default figure out the time from now to photons.
		float fSecondsSinceLastVsync;
		pOpenVR->GetTimeSinceLastVsync(&fSecondsSinceLastVsync, NULL);
		float fDisplayFrequency = vr::VRSystem()->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
		float fFrameDuration = fDisplayFrequency > 0 ? 1.f / fDisplayFrequency : 0;
		float fVsyncToPhotons = vr::VRSystem()->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SecondsFromVsyncToPhotons_Float);

		float fPredictedSecondsFromNow = fFrameDuration - fSecondsSinceLastVsync + fVsyncToPhotons;
		vr::TrackedDevicePose_t hmdPose;
		vr::VRCompositor()->GetLastPoseForTrackedDeviceIndex(vr::k_unTrackedDeviceIndex_Hmd, nullptr, &hmdPose);
		
#pragma region HmdDebugging
		// We never used this so removing it. If it's ever reimplemented, we need to add some new GUI objects.
		// Look at 3D rudder for inspiration.

		//Mat_33 HmdRotationMat;
		//for (int i = 0; i < 3; ++i)
		//	for (int j = 0; j < 3; ++j)
		//		HmdRotationMat[i][j] = hmdPose.mDeviceToAbsoluteTracking.m[i][j];
		//vr::HmdQuaternion_t HmdRotationQuat;
		//MatToQuat(HmdRotationMat, HmdRotationQuat);
		//bool displayHMDInfo = false;
		//if (displayHMDInfo)
		//{
		//	/*sText.Format(_T("qR : (%f,%f,%f,%f)"), HmdRotationQuat.x, HmdRotationQuat.y, HmdRotationQuat.z, HmdRotationQuat.w);
		//	m_StVal6.SetWindowText(sText);
		//	sText.Format(_T("aX : (%f,%f,%f)"), HmdRotationMat[0][0], HmdRotationMat[1][0], HmdRotationMat[2][0]);
		//	m_StRLActive.SetWindowText(sText);
		//	sText.Format(_T("aY : (%f,%f,%f)"), HmdRotationMat[0][1], HmdRotationMat[1][1], HmdRotationMat[2][1]);
		//	m_StValY.SetWindowText(sText);
		//	sText.Format(_T("aZ : (%f,%f,%f)"), HmdRotationMat[0][2], HmdRotationMat[1][2], HmdRotationMat[2][2]);
		//	m_StRLIndex.SetWindowText(sText);*/
		//}
#pragma endregion HmdDebugging

		// Controller Role		
		vr::VREvent_t pEvent;
		// TODO : test without for
		for (int i = 0; i < 64; i++)
		{
			if (!pOpenVR->PollNextEvent(&pEvent, sizeof(vr::VREvent_t)))
				break;

			// Get event for the role
			if (pEvent.eventType == vr::VREvent_TrackedDeviceRoleChanged || pEvent.eventType == vr::VREvent_TrackedDeviceActivated || pEvent.eventType == vr::VREvent_TrackedDeviceDeactivated)
				RefreshController();
		}
		
			sText.Format(_T("Override Trackpad (0 = false, 1 = true): Left(%d) Right(%d)"), (int)m_SharedData.m_bOverridePad[0], (int)m_SharedData.m_bOverridePad[1]);
			m_StRLActive.SetWindowText(sText);

			sText.Format(_T("Identified Trackpad Indexes: Left(%d) Right(%d)"), m_SharedData.m_nPadIndex[0], m_SharedData.m_nPadIndex[1]);
			m_StRLIndex.SetWindowText(sText);
		
	}
	else
	{

	}

	m_SharedData.m_bActivateCtrls = !!m_bActivateCtrls;
	m_SharedData.m_fHeightOffset = (float)_wtof(m_sHeightOffset);
	m_SharedData.m_bOverridePad[0] = !!m_bOverrideLeft;
	m_SharedData.m_bOverridePad[1] = !!m_bOverrideRight;

	m_SharedData.m_bAxisOnlyTouched[0] = !!m_bTrackPadLeftOnlyTouch;
	m_SharedData.m_bAxisOnlyTouched[1] = !!m_bTrackPadRightOnlyTouch;

	m_SharedData.m_bActivateVRLocomotion = !!m_bActivateVRLocomotion;

	m_SharedData.m_fSpeed[0] = (float)_wtof(m_sSpeedAxisLR);
	m_SharedData.m_fSpeed[1] = (float)_wtof(m_sSpeedAxisFB);
	m_SharedData.m_fSpeed[2] = 1.0f;//(float)_wtof(m_sSpeedAxisZ);
	m_SharedData.m_fSpeed[3] = 1.0f;//(float)_wtof(m_sSpeedRotateZ);

	m_SharedData.m_fSmoothValue[0] = 1.0f;// (float)_wtof(m_sSmoothnessAx);
	m_SharedData.m_fSmoothValue[1] = 1.0f;//(float)_wtof(m_sSmoothnessAy);
	m_SharedData.m_fSmoothValue[2] = 1.0f;//(float)_wtof(m_sSmoothnessAz);
	m_SharedData.m_fSmoothValue[3] = 1.0f;//(float)_wtof(m_sSmoothnessRz);


	////m_SharedData.m_bNonSymPitch = !!m_bNonSymPitch;
	m_SharedData.m_bCalibrateOnInUse = !!m_bCalibrateInUse;
	m_SharedData.m_bSmoothMode = !!m_bSmoothMode;

	m_SharedMemory.Write((uint8_t*)&m_SharedData, sizeof(CSISharedData));


	CSI_Message *pMessage;

	while (pMessage = g_SharedMessageHandleOut.PopMessage())
	{
		switch (pMessage->m_nType)
		{
		case CwsSiMessageeType::Calibration:
		{
			SendMatrixHmd();
		}
		break;
		default:
			break;
		}

		delete pMessage;
	}


	CDialogEx::OnTimer(nIDEvent);
}


std::string ConvUnicodeString(CString sString)
{
	char *sStringChar = new char[sString.GetLength() + 1];
	size_t CharactersConverted = 0;
	wcstombs_s(&CharactersConverted, sStringChar, sString.GetLength() + 1, sString, _TRUNCATE);

	std::string sNewString(sStringChar);

	delete[] sStringChar;
	return sNewString;
}

void CVRGOConfig::SendMatrixHmd()
{
	if (vr::VRSystem() != nullptr)
	{
		// Send pose and orientation
		vr::TrackedDevicePose_t hmdPose;
		vr::VRCompositor()->GetLastPoseForTrackedDeviceIndex(vr::k_unTrackedDeviceIndex_Hmd, nullptr, &hmdPose);
		if (hmdPose.bPoseIsValid)
		{
			CSI_MessageCalibration msgCalib;
			memcpy(&msgCalib.m_matrix, &hmdPose.mDeviceToAbsoluteTracking.m, sizeof(CSISharedData::Matrix34));
			g_SharedMessageHandleIn.PushMessage(&msgCalib);
		}
	}
}

void CVRGOConfig::RefreshController()
{
	vr::IVRSystem *pOpenVR = vr::VRSystem();
	if (pOpenVR != nullptr)
	{
		// LEFT
		vr::TrackedDeviceIndex_t leftIndex = pOpenVR->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
		if (leftIndex != vr::k_unTrackedDeviceIndexInvalid && m_SharedData.m_nPadIndex[0] != leftIndex)
			m_SharedData.m_nPadIndex[0] = leftIndex;
		// RIGHT
		vr::TrackedDeviceIndex_t rightIndex = pOpenVR->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
		if (rightIndex != vr::k_unTrackedDeviceIndexInvalid && m_SharedData.m_nPadIndex[1] != rightIndex)
			m_SharedData.m_nPadIndex[1] = rightIndex;
	}
}


int CVRGOConfig::Load(wchar_t *pPath)
{
	int bRet = 0;

	bRet = m_SharedData.Load(pPath);
	if (bRet == 1) // Successfully Loaded
	{
		m_bActivateCtrls = m_SharedData.m_bActivateCtrls;

		m_bActivateVRLocomotion = m_SharedData.m_bActivateVRLocomotion;
		m_bOverrideLeft = m_SharedData.m_bActivateTrackpadLeft;
		m_bOverrideRight = m_SharedData.m_bActivateTrackpadRight;
		m_bTrackPadLeftOnlyTouch = m_SharedData.m_bAxisOnlyTouched[0];
		m_bTrackPadRightOnlyTouch = m_SharedData.m_bAxisOnlyTouched[1];

		TCHAR fname[_MAX_FNAME];
		_tsplitpath(pPath, NULL, NULL, fname, NULL);

		m_sConfigLoaded.Format(_T("Config : %s"), fname);

		UpdateData(FALSE);
		ManageActivateUI();

		OnEnChangeEdSpeedAx();
		OnEnChangeEdSpeedAy();
		OnEnChangeEdSpeedAz();
		OnEnChangeEdSpeedRz();
		OnEnChangeEdHeightOffset();
		UpdateSliderValue();
	}

	// 0: default value (error?)
	// 1: Success
	// 2: Config Save version is newer than software (update software).

	return bRet;
}

void CVRGOConfig::OnBnClickedBtLoad()
{
	TCHAR szFilters[] = _T("txt Files (*.txt)|*.txt|All Files (*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("txt"), _T("*.txt"), OFN_FILEMUSTEXIST, szFilters);
	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();

		int loadStatus = Load(pathName.GetBuffer());
		switch (loadStatus)
		{
		case 0:
			// Default, indicates Error.
			AfxMessageBox(_T("Config Load Error"), MB_OK | MB_ICONERROR);
			break;
		case 1:
			// Success
			AfxMessageBox(_T("Config Loaded Successfully"), MB_OK | MB_ICONINFORMATION);
			break;
		case 2:
			// Versioning Error
			AfxMessageBox(_T("Config Load Error: Config saved with newer version of VRGO Dashboard. Please update to the latest version."), MB_OK | MB_ICONERROR);
		}			
	}
}


void CVRGOConfig::OnBnClickedBtSave()
{
	UpdateData(TRUE);
	TCHAR szFilters[] = _T("txt Files (*.txt)|*.txt|All Files (*.*)|*.*||");
	CFileDialog fileDlg(FALSE, _T("txt"), _T("*.txt"), OFN_FILEMUSTEXIST, szFilters);
	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();
		if (m_SharedData.Save(pathName.GetBuffer()))
			AfxMessageBox(_T("Config Save Successful"), MB_OK | MB_ICONINFORMATION);
		else
			AfxMessageBox(_T("Config Save Error"), MB_OK | MB_ICONERROR);

	}
}


void CVRGOConfig::ManageActivateUI()
{
	m_BtActivateVRLocomotion.EnableWindow(m_bActivated);
	m_BtActivateControllers.EnableWindow(m_bActivated);
	m_BtCalibration.EnableWindow(m_bActivated);
	OnBnClickedCkActivateCtrls();
	OnBnClickedCkActivateVRLocomotion();
}

void CVRGOConfig::OnBnClickedCkActivateCtrls()
{
	UpdateData(TRUE);

	m_BtOverrideLeft.EnableWindow(m_bActivateCtrls&&m_bActivated);
	m_BtOverrideRight.EnableWindow(m_bActivateCtrls&&m_bActivated);

	m_BtTrackPadLeftOnlyTouch.EnableWindow(m_bActivateCtrls&&m_bActivated);
	m_BtTrackPadRightOnlyTouch.EnableWindow(m_bActivateCtrls&&m_bActivated);

	RefreshController();

	m_SharedData.m_bActivateCtrls = !!m_bActivateCtrls;
	m_SharedData.m_bOverridePad[0] = !!m_bOverrideLeft;
	m_SharedData.m_bOverridePad[1] = !!m_bOverrideRight;
	m_SharedData.m_bAxisOnlyTouched[0] = !!m_bTrackPadLeftOnlyTouch;
	m_SharedData.m_bAxisOnlyTouched[1] = !!m_bTrackPadRightOnlyTouch;

	m_SharedMemory.Write((uint8_t*)&m_SharedData, sizeof(CSISharedData));
}


void CVRGOConfig::OnBnClickedBtReset()
{
	CSI_MessageReset msg;
	msg.m_bReset = true;
	g_SharedMessageHandleIn.PushMessage(&msg);
}


void CVRGOConfig::OnBnClickedCkActivateVRLocomotion()
{
	UpdateData(TRUE);
	m_StSpeedAx.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);
	m_StSpeedAy.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);
	m_BtReset.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);
	
	m_SlSpeedAx.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);
	m_SlSpeedAy.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);

	
	
	m_StSpeedAx.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);
	m_StSpeedAy.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);

	m_EdSpeedLR.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);
	m_EdSpeedFB.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);
	m_BtReset.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);

	m_SlSpeedAx.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);
	m_SlSpeedAy.EnableWindow(m_bActivateVRLocomotion&&m_bActivated);

	RefreshController();

	OnBnClickedCkSmoothMode();
	m_SharedData.m_bActivateVRLocomotion = !!m_bActivateVRLocomotion;
	m_SharedMemory.Write((uint8_t*)&m_SharedData, sizeof(CSISharedData));
}

void CVRGOConfig::OnBnClickedCKActivateLeftControllerLocomotion()
{
	UpdateData(TRUE);
	m_SharedData.m_bActivateTrackpadLeft = !!m_bOverrideLeft;
	m_SharedData.m_bOverridePad[0] = !!m_bOverrideLeft;
	m_SharedMemory.Write((uint8_t*)&m_SharedData, sizeof(CSISharedData));

	OnBnClickedCkActivateVRLocomotion();
}

void CVRGOConfig::OnBnClickedCKActivateRightControllerLocomotion()
{
	UpdateData(TRUE);
	m_SharedData.m_bActivateTrackpadRight = !!m_bOverrideRight;
	m_SharedData.m_bOverridePad[1] = !!m_bOverrideRight;
	m_SharedMemory.Write((uint8_t*)&m_SharedData, sizeof(CSISharedData));

	OnBnClickedCkActivateVRLocomotion();
}


CString GetErrorString()
{
	TCHAR pMsg[1024];

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&pMsg, 1024, NULL);

	return CString(pMsg);
}

CString GetWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);
	CString sOut(wc);
	delete[]wc;
	return sOut;
}

void CVRGOConfig::InitOpenVR()
{
	vr::EVRInitError eVRInitError;
	vr::VR_Init(&eVRInitError, vr::VRApplication_Background);
	if (!vr::VRSystem() || eVRInitError != vr::VRInitError_None)
	{
		CString sErrorMessage;
		const char *pError = VR_GetVRInitErrorAsEnglishDescription(eVRInitError);
		CString sError = GetWC(pError);
		sErrorMessage.Format(_T("OpenVr Init Error : %s"), sError.GetBuffer());

		AfxMessageBox(sErrorMessage);
	}
}

void CVRGOConfig::CheckActivate()
{
	int nDriverSelected = m_CbDriver.GetCurSel();
	if (nDriverSelected != -1)
	{
		CString sDriverName;
		m_CbDriver.GetLBText(nDriverSelected, sDriverName);
		CString sPath = m_sSteamPath + _T("\\steamapps\\common\\SteamVR\\drivers\\") + sDriverName + _T("\\bin");
		CString sPathWin32 = sPath + _T("\\win32\\");
		CString sPathWin64 = sPath + _T("\\win64\\");
		CString sDriverChildPath64 = sPathWin64 + _T("ChildSteam.dll");

		FILE *pFile = _wfopen(sDriverChildPath64.GetBuffer(), L"rb");
		if (pFile)
		{
			m_bActivated = true;
			m_BtActivate.SetWindowText(_T("Deactivate"));

			InitOpenVR();

			UpdateData(FALSE);
			ManageActivateUI();
			fclose(pFile);
		}
	}
}

void OnWaitThread(void *_pDataTh)
{
	CVRGOConfig*pThis = (CVRGOConfig*)_pDataTh;

	CWaitDlg waitdlg(&pThis->m_bExitWait);
	waitdlg.DoModal();

	_endthread();
}

void CVRGOConfig::OnBnClickedBtActivate()
{
	// If inactive, this function closes any open instances of Steam and subprocesses. It then relaunches Steam with the injected VRGO lighthouse driver, and launches the VRGO Device Driver.
	// If active, this function closes any open instances of Steam and subprocesses. It then relaunches Steam with it's original Steam VR driver, and terminates the VRGO Device Driver.

	// Get the appropriate driver. By default for initial Beta release this is always lighthouse by default.
	int nDriverSelected = m_CbDriver.GetCurSel();
	if (nDriverSelected != -1)
	{
		m_bActivated ^= 1;

		if (m_bActivated)
		{
			m_BtActivate.SetWindowText(_T("Deactivate"));
		}
		else
		{
			vr::VR_Shutdown();
			m_BtActivate.SetWindowText(_T("Activate"));
		}


		m_bExitWait = FALSE;
		_beginthread(OnWaitThread, 0, this);


		m_SharedMemory.Write((uint8_t*)&m_SharedData, sizeof(CSISharedData));

		CString sDriverName;
		m_CbDriver.GetLBText(nDriverSelected, sDriverName);
		CString sPath = m_sSteamPath + _T("\\steamapps\\common\\SteamVR\\drivers\\") + sDriverName + _T("\\bin");

		DWORD nPID = FindProcessByName(_T("Steam.exe"));

		if (nPID != 0)
		{
			KillProcessTree(nPID);
			while (FindProcessByName(_T("Steam.exe")) != 0)
			{
				Sleep(100);
			}
		}

		nPID = FindProcessByName(_T("vrmonitor.exe"));
		if (nPID != 0)
			KillProcessTree(nPID);
		nPID = FindProcessByName(_T("vrserver.exe"));
		if (nPID != 0)
			KillProcessTree(nPID);
		nPID = FindProcessByName(_T("vrcompositor.exe"));
		if (nPID != 0)
			KillProcessTree(nPID);

		nPID = FindProcessByName(_T("VRGO_DeviceDriverExec.exe"));
		if (nPID != 0)
			KillProcessTree(nPID);

		CString sPathWin32 = sPath + _T("\\win32\\");
		CString sPathWin64 = sPath + _T("\\win64\\");
		CString sDriverPath32_1 = sPathWin32 + _T("driver_") + sDriverName + _T(".dll");
		CString sDriverPath32_2 = sPathWin32 + _T("driver_") + sDriverName + _T("_copy.dll");
		CString sDriverPath64_1 = sPathWin64 + _T("driver_") + sDriverName + _T(".dll");
		CString sDriverPath64_2 = sPathWin64 + _T("driver_") + sDriverName + _T("_copy.dll");
		CString sDriverChildPath32 = sPathWin32 + _T("ChildSteam.dll");
		CString sDriverChildPath64 = sPathWin64 + _T("ChildSteam.dll");


		TCHAR buff[MAX_PATH];
		memset(buff, 0, MAX_PATH);
		::GetModuleFileName(NULL, buff, MAX_PATH);
		CString strFolder = buff;
		strFolder = strFolder.Left(strFolder.ReverseFind(_T('\\')) + 1);


		CString sDriverInjection32 = strFolder + _T("VRGO_OpenVRDriver32.dll");
		CString sDriverInjection64 = strFolder + _T("VRGO_OpenVRDriver64.dll");


		if (m_bActivated)
		{
			CString sError;
			CopyFile(sDriverPath32_1, sDriverPath32_2, TRUE); // original then copy destination 32
			sError = GetErrorString();
			m_StDriverSteam32.SetWindowText(sDriverPath32_1);
			CopyFile(sDriverPath64_1, sDriverPath64_2, TRUE); // original then copy destination 64
			sError = GetErrorString();
			m_StDriverSteam64.SetWindowText(sDriverPath64_1);
			CopyFile(sDriverPath32_1, sDriverChildPath32, TRUE); // original then copy path destination 32
			sError = GetErrorString();
			CopyFile(sDriverPath64_1, sDriverChildPath64, TRUE); // original then copy path destination 64
			sError = GetErrorString();

			CopyFile(sDriverInjection32, sDriverPath32_1, FALSE); // VRGO 32 then new path mimicking 32
			sError = GetErrorString();
			m_StDriverVRGO32.SetWindowText(CString("VRGO 32 Driver - ") + sDriverInjection32);
			CopyFile(sDriverInjection64, sDriverPath64_1, FALSE); // VRGO 64 then new path mimicking 64
			sError = GetErrorString();
			m_StDriverVRGO64.SetWindowText(CString("VRGO 64 Driver - ") + sDriverInjection64);
		}
		else
		{

			CopyFile(sDriverPath32_2, sDriverPath32_1, FALSE);
			CopyFile(sDriverPath64_2, sDriverPath64_1, FALSE);
			DeleteFile(sDriverChildPath32);
			DeleteFile(sDriverChildPath64);
		}


		CString sExePath = m_sSteamPath + _T("\\Steam.exe -applaunch 250820");
		RunProcess(sExePath.GetBuffer());

		if (m_bActivated)
		{
			CString vrgoSDKPath = strFolder + _T("VRGO_DeviceDriverExec.exe");

			RunProcess(vrgoSDKPath.GetBuffer());


			//
			while (FindProcessByName(_T("vrserver.exe")) == 0)
			{
				Sleep(100);
			}
			InitOpenVR();
		}

		ManageActivateUI();
		m_bExitWait = TRUE;
	}
}

bool CVRGOConfig::IsSDKRunning()
{
	DWORD nPID = FindProcessByName(_T("VRGO_DeviceDriverExec.exe"));
	if (nPID != 0)
	{
		return true;
	}
	else return false;
}


void CVRGOConfig::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (m_bActivated)
		OnBnClickedBtActivate();
}


void CVRGOConfig::OnBnClickedBtCalibration()
{
	CSI_MessageCalibration msgCalib;
	msgCalib.m_bCalib = true;
	g_SharedMessageHandleIn.PushMessage(&msgCalib);
}

void CVRGOConfig::OnBnClickedBtDebug()
{
	CRect rcButton;
	CRect rcGroupService;
	m_BtDebug.GetWindowRect(&rcButton);
	m_StDebug.GetWindowRect(&rcGroupService);
	CRect rcWin;
	GetWindowRect(&rcWin);
	ScreenToClient(&rcButton);
	ScreenToClient(&rcGroupService);
	m_bDebug ^= 1;
	if (m_bDebug)
	{
		SetWindowPos(NULL, 0, 0, rcWin.Width(), int(rcGroupService.bottom + 50.0f* m_fDPIx), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
	else
	{
		SetWindowPos(NULL, 0, 0, rcWin.Width(), int(rcButton.bottom + 40.0f* m_fDPIx), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
}


void CVRGOConfig::OnEnChangeEdSpeedAx()
{
	UpdateData(TRUE);
	m_SharedData.m_fSpeed[0] = (float)_wtof(m_sSpeedAxisLR);
	if (fabs(m_SharedData.m_fSpeed[0]) > m_MaxSpeedAx)
	{
		m_MaxSpeedAx = (int)fabs(m_SharedData.m_fSpeed[0]);
		m_SlSpeedAx.SetRange(-m_MaxSpeedAx * 100, m_MaxSpeedAx * 100);
		m_SlSpeedAx.SetTicFreq(m_MaxSpeedAx * 100 / 10);
	}
	m_SlSpeedAx.SetPos(int(m_SharedData.m_fSpeed[0] * 100));
}


void CVRGOConfig::OnEnChangeEdSpeedAy()
{
	UpdateData(TRUE);
	m_SharedData.m_fSpeed[1] = (float)_wtof(m_sSpeedAxisFB);
	if (fabs(m_SharedData.m_fSpeed[1]) > m_MaxSpeedAy)
	{
		m_MaxSpeedAy = (int)fabs(m_SharedData.m_fSpeed[1]);
		m_SlSpeedAy.SetRange(-m_MaxSpeedAy * 100, m_MaxSpeedAy * 100);
		m_SlSpeedAy.SetTicFreq(m_MaxSpeedAy * 100 / 10);
	}
	m_SlSpeedAy.SetPos(int(m_SharedData.m_fSpeed[1] * 100));
}


void CVRGOConfig::OnEnChangeEdSpeedAz()
{
	//UpdateData(TRUE);
	//m_SharedData.m_fSpeed[2] = (float)_wtof(m_sSpeedAxisZ);
	//if (fabs(m_SharedData.m_fSpeed[2]) > m_MaxSpeedAz)
	//{
	//	m_MaxSpeedAz = (int)fabs(m_SharedData.m_fSpeed[2]);
	//	m_SlSpeedAz.SetRange(-m_MaxSpeedAz * 100, m_MaxSpeedAz * 100);
	//	m_SlSpeedAz.SetTicFreq(m_MaxSpeedAz * 100 / 10);
	//}
	//m_SlSpeedAz.SetPos(int(m_SharedData.m_fSpeed[2] * 100));
}


void CVRGOConfig::OnEnChangeEdSpeedRz()
{
	//UpdateData(TRUE);
	//m_SharedData.m_fSpeed[3] = (float)_wtof(m_sSpeedRotateZ);
	//if (fabs(m_SharedData.m_fSpeed[3]) > m_MaxSpeedRz)
	//{
	//	m_MaxSpeedRz = (int)fabs(m_SharedData.m_fSpeed[3]);
	//	m_SlSpeedRz.SetRange(-m_MaxSpeedRz * 100, m_MaxSpeedRz * 100);
	//	m_SlSpeedRz.SetTicFreq(m_MaxSpeedRz * 100 / 10);
	//}
	//m_SlSpeedRz.SetPos(int(m_SharedData.m_fSpeed[3] * 100));
}


void CVRGOConfig::OnEnChangeEdHeightOffset()
{
	//UpdateData(TRUE);
	//m_SharedData.m_fHeightOffset = (float)_wtof(m_sHeightOffset);
	//if (fabs(m_SharedData.m_fHeightOffset) > m_nMaxHOffset)
	//{
	//	m_nMaxHOffset = (int)fabs(m_SharedData.m_fHeightOffset);
	//	m_SlHOffset.SetRange(-m_nMaxHOffset * 100, m_nMaxHOffset * 100);
	//	m_SlHOffset.SetTicFreq(m_nMaxHOffset * 100 / 10);
	//}
	//m_SlHOffset.SetPos(int(m_SharedData.m_fHeightOffset * 100));
}


void CVRGOConfig::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateSliderValue();

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVRGOConfig::UpdateSliderValue()
{
	m_sSpeedAxisLR.Format(_T("%.2f"), m_SlSpeedAx.GetPos() / 100.0f);
	m_sSpeedAxisFB.Format(_T("%.2f"), m_SlSpeedAy.GetPos() / 100.0f);


	UpdateData(FALSE);
}

void CVRGOConfig::OnBnClickedCkSmoothMode()
{
	UpdateData(TRUE);
}


void CVRGOConfig::OnEnChangeEdSmoothAx()
{
	UpdateData(TRUE);
	m_SharedData.m_fSmoothValue[0] = (float)_wtof(m_sSmoothnessAx);
	m_SlSmoothnessAx.SetPos(int(m_SharedData.m_fSmoothValue[0] * 1000));
}

void CVRGOConfig::OnEnChangeEdSmoothAy()
{
	UpdateData(TRUE);
	m_SharedData.m_fSmoothValue[1] = (float)_wtof(m_sSmoothnessAy);
	m_SlSmoothnessAy.SetPos(int(m_SharedData.m_fSmoothValue[1] * 1000));
}

void CVRGOConfig::OnEnChangeEdSmoothAz()
{
	UpdateData(TRUE);
	m_SharedData.m_fSmoothValue[2] = (float)_wtof(m_sSmoothnessAz);
	m_SlSmoothnessAz.SetPos(int(m_SharedData.m_fSmoothValue[2] * 1000));
}

void CVRGOConfig::OnEnChangeEdSmoothRz()
{
	UpdateData(TRUE);
	m_SharedData.m_fSmoothValue[3] = (float)_wtof(m_sSmoothnessRz);
	m_SlSmoothnessRz.SetPos(int(m_SharedData.m_fSmoothValue[3] * 1000));
}

