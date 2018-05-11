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



#include "afxcmn.h"
#include "CurveCtrl.h"
#include "afxwin.h"


// CDlgCurve dialog

class CDlgCurve : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCurve)

public:
	CDlgCurve(CSISharedData::Curve *pCurve, SHORT *pCurValue, BOOL bHalfCurve = FALSE, CWnd* pParent = NULL);   // standard constructor
	CDlgCurve(CSISharedData::Curve *pCurve, BYTE *pCurValue, BOOL bHalfCurve = FALSE, CWnd* pParent = NULL);   // standard constructor
	CDlgCurve(CSISharedData::Curve *pCurve, float *pCurValue, BOOL bHalfCurve = FALSE, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCurve();
	CSISharedData::Curve m_Curve;
	CSISharedData::Curve *m_pCurve;
	BOOL m_bFloat;
	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CURVE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CSliderCtrl m_SlDeadZone;
	CSliderCtrl m_SlxSat;
	CSliderCtrl m_SlExp;
	CSliderCtrl m_SlyMax;
	CString m_sDeadZone;
	CString m_sxSat;
	CString m_sExp;
	CString m_sYMax;
	CCurveCtrl m_StCurve;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeStDeadzone();
	afx_msg void OnEnChangeStExp();
	afx_msg void OnEnChangeStXsat();
	afx_msg void OnEnChangeStYmax();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void UpdateValue();
};
