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

// Modifications made by Samuel Pike for VRGO Ltd.
// Copyright(c) VRGO ltd 2018.


#include "stdafx.h"
#include "VRGO_Config.h"
#include "DlgCurve.h"
#include "afxdialogex.h"


// CDlgCurve dialog

IMPLEMENT_DYNAMIC(CDlgCurve, CDialogEx)

CDlgCurve::CDlgCurve(CSISharedData::Curve *pCurve, SHORT *pCurValue, BOOL bHalfCurve, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CURVE, pParent)
	, m_sDeadZone(_T(""))
	, m_sxSat(_T(""))
	, m_sExp(_T(""))
	, m_sYMax(_T(""))
{
	m_Curve = *pCurve;
	m_pCurve = pCurve;
	m_StCurve.m_pCurValueS = pCurValue;
	m_StCurve.m_pCurValueB = NULL;
	m_StCurve.m_pfCurValue = NULL;
	m_StCurve.m_bHalfCurve = bHalfCurve;
	m_bFloat = FALSE;
}

CDlgCurve::CDlgCurve(CSISharedData::Curve *pCurve, BYTE *pCurValue, BOOL bHalfCurve, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CURVE, pParent)
	, m_sDeadZone(_T(""))
	, m_sxSat(_T(""))
	, m_sExp(_T(""))
	, m_sYMax(_T(""))
{
	m_Curve = *pCurve;
	m_pCurve = pCurve;
	m_StCurve.m_pCurValueB = pCurValue;
	m_StCurve.m_pCurValueS = NULL;
	m_StCurve.m_pfCurValue = NULL;
	m_StCurve.m_bHalfCurve = bHalfCurve;
	m_bFloat = FALSE;
}

CDlgCurve::CDlgCurve(CSISharedData::Curve *pCurve, float *pCurValue, BOOL bHalfCurve, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_CURVE, pParent)
	, m_sDeadZone(_T(""))
	, m_sxSat(_T(""))
	, m_sExp(_T(""))
	, m_sYMax(_T(""))
{
	m_Curve = *pCurve;
	m_pCurve = pCurve;
	m_StCurve.m_pCurValueB = NULL;
	m_StCurve.m_pCurValueS = NULL;
	m_StCurve.m_pfCurValue = pCurValue;
	m_StCurve.m_bHalfCurve = bHalfCurve;
	m_bFloat = TRUE;
}

CDlgCurve::~CDlgCurve()
{
}

void CDlgCurve::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SL_DEADZONE, m_SlDeadZone);
	DDX_Control(pDX, IDC_SL_XSAT, m_SlxSat);
	DDX_Control(pDX, IDC_SL_EXP, m_SlExp);
	DDX_Control(pDX, IDC_SL_YMAX, m_SlyMax);
	DDX_Text(pDX, IDC_ST_DEADZONE, m_sDeadZone);
	DDX_Text(pDX, IDC_ST_XSAT, m_sxSat);
	DDX_Text(pDX, IDC_ST_EXP, m_sExp);
	DDX_Text(pDX, IDC_ST_YMAX, m_sYMax);
	DDX_Control(pDX, IDC_ST_CURVE, m_StCurve);
}


BEGIN_MESSAGE_MAP(CDlgCurve, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgCurve::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BT_RESET, &CDlgCurve::OnBnClickedReset)
	ON_BN_CLICKED(IDCANCEL, &CDlgCurve::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_ST_DEADZONE, &CDlgCurve::OnEnChangeStDeadzone)
	ON_EN_CHANGE(IDC_ST_EXP, &CDlgCurve::OnEnChangeStExp)
	ON_EN_CHANGE(IDC_ST_XSAT, &CDlgCurve::OnEnChangeStXsat)
	ON_EN_CHANGE(IDC_ST_YMAX, &CDlgCurve::OnEnChangeStYmax)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CDlgCurve message handlers


BOOL CDlgCurve::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (m_bFloat)
	{
		m_SlDeadZone.SetRange(0, (int)(m_pCurve->m_fxSat *1000.0f));
		m_SlDeadZone.SetTicFreq(1000);
	}
	else
		if (!m_StCurve.m_bHalfCurve)
		{
			m_SlDeadZone.SetRange(0, 1000);
			m_SlDeadZone.SetTicFreq(2000);
		}
		else
		{
			m_SlDeadZone.SetRange(0, 1.0f);
			m_SlDeadZone.SetTicFreq(20);
		}
	if (m_bFloat)
		m_SlDeadZone.SetPos((int)(m_pCurve->m_fDeadZone *1000.0f));
	else
		m_SlDeadZone.SetPos((int)m_pCurve->m_fDeadZone*1000.0f);

	if (m_bFloat)
	{
		m_SlxSat.SetRange(1, 1000);
		m_SlxSat.SetTicFreq(1000);
	}
	else
		if (!m_StCurve.m_bHalfCurve)
		{
			m_SlxSat.SetRange(1, 1000);
			m_SlxSat.SetTicFreq(2000);
		}
		else
		{
			m_SlxSat.SetRange(1, 255);
			m_SlxSat.SetTicFreq(20);
		}
	if (m_bFloat)
		m_SlxSat.SetPos((int)(m_pCurve->m_fxSat *1000.0f));
	else
		m_SlxSat.SetPos((int)m_pCurve->m_fxSat*1000.0f);

	m_SlExp.SetRange(1, 5);
	m_SlExp.SetTicFreq(1);
	m_SlExp.SetPos((int)m_pCurve->m_fExp);

	if (m_bFloat)
	{
		m_SlyMax.SetRange(1, 1000);
		m_SlyMax.SetTicFreq(1000);
	}
	else
		if (!m_StCurve.m_bHalfCurve)
		{
			m_SlyMax.SetRange(1, 1000);
			m_SlyMax.SetTicFreq(2000);
		}
		else
		{
			m_SlyMax.SetRange(1, 255);
			m_SlyMax.SetTicFreq(20);
		}
	if (m_bFloat)
		m_SlyMax.SetPos((int)(m_pCurve->m_fyMax*1000.0f));
	else
		m_SlyMax.SetPos((int)m_pCurve->m_fyMax*1000.0f);

	m_StCurve.m_pCurve = m_pCurve;


	SetTimer(0, 100, NULL);

	m_sDeadZone.Format(_T("%.22f"), m_pCurve->m_fDeadZone);
	m_sxSat.Format(_T("%.22f"), m_pCurve->m_fxSat);
	m_sYMax.Format(_T("%.22f"), m_pCurve->m_fyMax);
	m_sExp.Format(_T("%.22f"), m_pCurve->m_fExp);

	m_StCurve.Invalidate();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCurve::OnTimer(UINT_PTR nIDEvent)
{
	m_StCurve.Invalidate();
	CDialogEx::OnTimer(nIDEvent);
}


void CDlgCurve::OnBnClickedOk()
{
	UpdateData(TRUE);
	m_pCurve->m_fDeadZone = (float)_wtof(m_sDeadZone);
	m_pCurve->m_fyMax = (float)_wtof(m_sYMax);
	m_pCurve->m_fxSat = (float)_wtof(m_sxSat);

	m_pCurve->m_fExp = (float)_wtof(m_sExp);
	/*m_pCurve->m_fDeadZone=(float)m_SlDeadZone.GetPos();
	m_pCurve->m_fxSat=(float)m_SlxSat.GetPos();
	m_pCurve->m_fExp=(float)m_SlExp.GetPos();
	m_pCurve->m_fyMax=(float)m_SlyMax.GetPos();*/


	CDialogEx::OnOK();
}


void CDlgCurve::OnBnClickedReset()
{
	//m_pCurve->Init();
	*m_pCurve = m_Curve;

	m_SlExp.SetPos((int)m_pCurve->m_fExp);
	if (m_bFloat)
	{
		m_SlDeadZone.SetPos((int)(m_pCurve->m_fDeadZone*1000.f));
		m_SlxSat.SetPos((int)(m_pCurve->m_fxSat*1000.f));
		m_SlyMax.SetPos((int)(m_pCurve->m_fyMax*1000.f));
	}
	else
	{
		m_SlDeadZone.SetPos((int)m_pCurve->m_fDeadZone);
		m_SlxSat.SetPos((int)m_pCurve->m_fxSat);
		m_SlyMax.SetPos((int)m_pCurve->m_fyMax);
	}

	// Now the sliders have changed, update the text fields.
	UpdateValue();

	m_StCurve.Invalidate();
	UpdateData(FALSE);

}


void CDlgCurve::OnBnClickedCancel()
{
	*m_pCurve = m_Curve;
	CDialogEx::OnCancel();
}


void CDlgCurve::OnEnChangeStDeadzone()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_pCurve->m_fDeadZone = (float)_wtof(m_sDeadZone);
	if (m_bFloat)
		m_SlDeadZone.SetPos((int)(m_pCurve->m_fDeadZone*1000.0f));
	else
		m_SlDeadZone.SetPos((int)m_pCurve->m_fDeadZone);
	m_StCurve.Invalidate();
	UpdateData(FALSE);

}


void CDlgCurve::OnEnChangeStExp()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_pCurve->m_fExp = (float)_wtof(m_sExp);
	m_SlExp.SetPos((int)m_pCurve->m_fExp);
	m_StCurve.Invalidate();
	UpdateData(FALSE);
}


void CDlgCurve::OnEnChangeStXsat()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_pCurve->m_fyMax = (float)_wtof(m_sYMax);
	if (m_bFloat)
		m_SlyMax.SetPos((int)(m_pCurve->m_fyMax*1000.0f));
	else
		m_SlyMax.SetPos((int)m_pCurve->m_fyMax);
	m_StCurve.Invalidate();
	UpdateData(FALSE);
}


void CDlgCurve::OnEnChangeStYmax()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	m_pCurve->m_fxSat = (float)_wtof(m_sxSat);
	if (m_bFloat)
		m_SlxSat.SetPos((int)(m_pCurve->m_fxSat*1000.0f));
	else
		m_SlxSat.SetPos((int)m_pCurve->m_fxSat);
	m_StCurve.Invalidate();
	UpdateData(FALSE);
}

void CDlgCurve::UpdateValue()
{
	m_sExp.Format(_T("%d"), m_SlExp.GetPos());
	m_pCurve->m_fExp = (float)m_SlExp.GetPos();
	if (m_bFloat)
	{
		m_sDeadZone.Format(_T("%.2f"), m_SlDeadZone.GetPos() / 1000.0f);
		m_sxSat.Format(_T("%.2f"), m_SlxSat.GetPos() / 1000.0f);
		m_sYMax.Format(_T("%.2f"), m_SlyMax.GetPos() / 1000.0f);

		m_pCurve->m_fDeadZone = (float)m_SlDeadZone.GetPos() / 1000.0f;
		m_pCurve->m_fxSat = (float)m_SlxSat.GetPos() / 1000.0f;
		if (m_pCurve->m_fDeadZone >= m_pCurve->m_fxSat)
		{
			m_pCurve->m_fDeadZone = m_pCurve->m_fxSat;
			m_SlDeadZone.SetPos((int)(m_pCurve->m_fDeadZone*1000.0f));
		}
		m_SlDeadZone.SetRange(0, (int)(m_pCurve->m_fxSat*1000.0f));

		m_pCurve->m_fyMax = (float)m_SlyMax.GetPos() / 1000.0f;
	}
	else
	{
		m_sDeadZone.Format(_T("%.2f"), (((float)m_SlDeadZone.GetPos())/1000.0f));
		m_sxSat.Format(_T("%.2f"), ((float)m_SlxSat.GetPos())/1000.0f);
		m_sYMax.Format(_T("%.2f"), ((float)m_SlyMax.GetPos())/1000.0f);

		m_pCurve->m_fDeadZone = (float)m_SlDeadZone.GetPos();
		m_pCurve->m_fxSat = (float)m_SlxSat.GetPos();
		if (m_pCurve->m_fDeadZone >= m_pCurve->m_fxSat)
		{
			m_pCurve->m_fDeadZone = m_pCurve->m_fxSat;
			m_SlDeadZone.SetPos((int)m_pCurve->m_fDeadZone);
		}
		m_SlDeadZone.SetRange(0, (int)m_pCurve->m_fxSat);

		m_pCurve->m_fyMax = (float)m_SlyMax.GetPos();
	}
	m_StCurve.Invalidate();
	UpdateData(FALSE);

}

void CDlgCurve::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateValue();
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
