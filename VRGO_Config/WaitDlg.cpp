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

#include "stdafx.h"
#include "VRGO_Config.h"
#include "WaitDlg.h"
#include "afxdialogex.h"


// CWaitDlg dialog

IMPLEMENT_DYNAMIC(CWaitDlg, CDialogEx)

CWaitDlg::CWaitDlg(BOOL *pbExit, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_WAIT_DIALOG, pParent)
{
	m_pbExit = pbExit;
}

CWaitDlg::~CWaitDlg()
{
}

void CWaitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}


BEGIN_MESSAGE_MAP(CWaitDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CWaitDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CWaitDlg::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWaitDlg message handlers


void CWaitDlg::OnBnClickedOk()
{
}


void CWaitDlg::OnBnClickedCancel()
{
}


BOOL CWaitDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Progress.SetRange(0, 100);
	m_nProgress = 0;
	m_Progress.SetPos(m_nProgress);

	SetTimer(0, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaitDlg::Quit()
{
	CDialogEx::OnOK();
}

void CWaitDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (*m_pbExit)
		Quit();

	m_Progress.SetPos(m_nProgress++);
	if (m_nProgress >= 100)
		m_nProgress = 0;

	CDialogEx::OnTimer(nIDEvent);
}
