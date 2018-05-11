#pragma once
#include "../Common/SharedData.h"
#include "resource.h"
//#include "afxwin.h"
//#include "afxcmn.h"
//#include "OpenGlCtrl.h"


class CVRGOSDK //:public CDialogEx
{
public:
	//CVRGOSDK(CWnd* pParent = NULL);

	//CStatic m_xValue;
	//CStatic m_yValue;
	//CStatic m_zValue;
	CSISharedData m_SharedData;
	CSISharedData m_SharedDataDebug;
	CSharedMemory m_SharedMemory;
	CSharedMemory m_SharedMemoryDebug;

	CSISharedDataDebug m_pSharedDataDebug;

	CSISharedMessageHandle *m_pSharedMessageHandleIn;
	CSISharedMessageHandle *m_pSharedMessageHandleOut;
protected:
	//virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	




};