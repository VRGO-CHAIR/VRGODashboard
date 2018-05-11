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


#include "StdAfx.h"
#include "OpenGlCtrl.h"
#include "WGL_ARB_multisample.h"

//Macro to release COM Components

#ifdef SAFE_RELEASE
#undef SAFE_RELEASE
#endif
#define SAFE_RELEASE(x) do{\
							if((x) != NULL)\
							{\
								while((x)->Release() != 0);\
								(x) = NULL;\
							}\
						}while(0)

COpenGlCtrl::COpenGlCtrl(void)
	:CStatic()
{


}

COpenGlCtrl::~COpenGlCtrl(void)
{
}

float lpos[4] = { 100,100,0,1 };

void COpenGlCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
	ModifyStyle(0, SS_OWNERDRAW);
	oglInitialize();
}
void COpenGlCtrl::oglInitialize(void)
{
	// Initial Setup:
	//
	PIXELFORMATDESCRIPTOR pfd =											// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),									// Size Of This Pixel Format Descriptor
		1,																// Version Number
		PFD_DRAW_TO_WINDOW |											// Format Must Support Window
		PFD_SUPPORT_OPENGL |											// Format Must Support OpenGL
		PFD_DOUBLEBUFFER |											// Must Support Double Buffering
		PFD_SUPPORT_COMPOSITION,										// When running under Windows Vista or later support desktop composition.		
		PFD_TYPE_RGBA,													// Request An RGBA Format
		32,																// Select Our Color Depth
		0, 0, 0, 0, 0, 0,												// Color Bits Ignored
		1,																// Alpha Buffer
		0,																// Shift Bit Ignored
		0,																// No Accumulation Buffer
		0, 0, 0, 0,														// Accumulation Bits Ignored
		16,																// 16Bit Z-Buffer (Depth Buffer)  
		0,																// No Stencil Buffer
		0,																// No Auxiliary Buffer
		PFD_MAIN_PLANE,													// Main Drawing Layer
		0,																// Reserved
		0, 0, 0															// Layer Masks Ignored
	};

	// Pixel format.
	int nPixelFormat;
	ChooseBestAntiAliasingPixelFormat(nPixelFormat);


	// Get device context only once.
	m_hdc = GetDC()->m_hDC;

	//int nPixelFormat = ChoosePixelFormat(m_hdc, &pfd);

	SetPixelFormat(m_hdc, nPixelFormat, &pfd);

	// Create the OpenGL Rendering Context.
	m_hrc = wglCreateContext(m_hdc);
	wglMakeCurrent(m_hdc, m_hrc);


	// Basic Setup:
	//
	// Set color to use when clearing the background.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	// Turn on backface culling
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	// Turn on depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	/*
	*	Setup Material Porperties & colours for the light
	*/
	{
		float Ambient[] = { 0.4f,0.4f,0.4f,0.0f };
		float Diffuse[] = { 0.75f,0.75f,0.75f,0.0f };

		glMaterialfv(GL_FRONT, GL_AMBIENT, Ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, Diffuse);
	}
	{
		float Ambient[] = { 0.4f,0.4f,0.4f,0.0f };
		float Diffuse[] = { 0.95f,0.95f,0.95f,0.0f };

		glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
	}

	m_Obj3d = LoadOBJ("VRGO_ChairModel.OBJ");
	ConvertMeshTo(m_Obj3d, VERTEX_BUFFER_OBJECT/*VERTEX_ARRAY*/);
	SetTextures(m_Obj3d, "VRGOTex.tga", "VRGOTex.tga");

	SetLightPosition(lpos[0], lpos[1], lpos[2]);

	m_fPosX = 0.0f;		// X position of model in camera view
	m_fPosY = -20.0f;		// Y position of model in camera view
	m_fZoom = 110.0f;		// Zoom on model in camera view
	m_fRotX = 45.0f;		// Rotation on model in camera view
	m_fRotY = 0.0f;		// Rotation on model in camera view
	m_fRotZ = 0.0f;		// Rotation on model in camera view



	SetTimer(0, 60, NULL);

	DrawItem(NULL);
}


void COpenGlCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//Get control measures
	CRect rc;
	GetClientRect(&rc);

	int cx = rc.Width();
	int cy = rc.Height();
	glEnable(GL_MULTISAMPLE_ARB);							// Enable Our Multisampling

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.35294, 0.82745, 0.36863, 1.0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	// Map the OpenGL coordinates.
	glViewport(0, 0, cx, cy);

	// Projection view
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	// Set our current view perspective
	gluPerspective(35.0f, (float)cx / (float)cy, 0.001f, 200.0f);

	// Model view
	glMatrixMode(GL_MODELVIEW);

	// If the current view is perspective...
	glLoadIdentity();

	/*ns3dRudder::CSdk* pSDK = ns3dRudder::GetSDK();
	if (pSDK->IsDeviceConnected(0))
	{
		ns3dRudder::Axis sAxis;
		pSDK->GetAxis(0, ns3dRudder::Angle, &sAxis);
		m_fRotZ = -sAxis.GetXAxis();
		m_fRotX = 45.0f - sAxis.GetYAxis();
		m_fRotY = -sAxis.GetZRotation();
		m_fPosY = sAxis.GetZAxis() / 100.0f;
	}*/


	glTranslatef(0.0f, 0.0f, -m_fZoom);
	glTranslatef(m_fPosX, m_fPosY, 0.0f);
	glRotatef(m_fRotX, 1.0f, 0.0f, 0.0f);
	glRotatef(m_fRotY, 0.0f, 1.0f, 0.0f);
	glRotatef(m_fRotZ, 0.0f, 0.0f, 1.0f);


	DrawOBJ(m_Obj3d);

	// Swap buffers
	SwapBuffers(m_hdc);

}

BOOL COpenGlCtrl::OnEraseBkgnd(CDC *pDC)
{
#if 0
	if (0)

		//Get control measures
		RECT rc;
	this->GetClientRect(&rc);

	Graphics graphics(pDC->GetSafeHdc());
	/*Image image(m_pStream);
	graphics.DrawImage(&image, (INT)rc.left, (INT)rc.top, (INT)(rc.right-rc.left), (INT)(rc.bottom-rc.top));*/
	return TRUE;
}
	else
	{
		return CStatic::OnEraseBkgnd(pDC);
	}
#else
	return FALSE;
#endif
}BEGIN_MESSAGE_MAP(COpenGlCtrl, CStatic)
ON_WM_TIMER()
ON_WM_DESTROY()
END_MESSAGE_MAP()


void COpenGlCtrl::OnTimer(UINT_PTR nIDEvent)
{
	Invalidate();

	CStatic::OnTimer(nIDEvent);
}


void COpenGlCtrl::OnDestroy()
{
	CStatic::OnDestroy();

	DeleteOBJ(m_Obj3d);
}

void COpenGlCtrl::UpdateRotation(float x, float y, float z)
{
	m_fRotX = (x * 20.f) + 45.0f;
	m_fRotY = y * 360.f;
	m_fRotZ = z * 20.f;
}