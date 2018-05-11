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

#include "StdAfx.h"
#include "CurveCtrl.h"
#include "../Common/SharedData.h"
#include <GdiPlus.h>
using namespace Gdiplus;

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

CCurveCtrl::CCurveCtrl(void)
	:CStatic()
	, m_gdiplusToken(0)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);


}

CCurveCtrl::~CCurveCtrl(void)
{
	//Tidy up
	GdiplusShutdown(m_gdiplusToken);
}


void CCurveCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
	ModifyStyle(0, SS_OWNERDRAW);
}

float ApplyCurvePolynomiale(float x, float x0, float xsat, float ysat, float exposant)
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
	if (x <= x0)
		return 0; //dead zone
	else
		x = x - x0;

	//normalisation 
	x = x / (xsat - x0);

	//Sortie du polynome
	y = ysat * powf(x, exposant);

	//Saturation haute
	if (y > ysat)
		y = ysat;

	//Affectation du bon signe
	if (signe < 0)
		y = -y;

	return y;
}

void CCurveCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//Check if pic data is loaded

	//Get control measures
	CRect rc;
	this->GetClientRect(&rc);

	Bitmap *pMemBitmap = new Bitmap(rc.Width(), rc.Height());

	Graphics* pMemGraphics = Graphics::FromImage(pMemBitmap);
	pMemGraphics->SetInterpolationMode(InterpolationModeHighQuality);
	pMemGraphics->SetSmoothingMode(SmoothingModeAntiAlias);
	pMemGraphics->SetTextRenderingHint(TextRenderingHintAntiAlias);

	SolidBrush black(Color(255, 0, 0, 0));
	Rect rcRectangle(rc.left, rc.top, rc.Width(), rc.Height());
	pMemGraphics->FillRectangle(&black, rcRectangle);


	Gdiplus::Pen WhitePen(Color(255, 255, 255, 255), 2);
	Gdiplus::Pen WhitePen2(Color(64, 255, 255, 255), 1);
	Gdiplus::Pen RedPen(Color(255, 255, 0, 0), 2);
	Gdiplus::Pen GreenPen(Color(64, 0, 255, 0), 2);


	int nCenterW = rc.Width() / 2;
	int nCenterH = rc.Height() / 2;

	if (m_bHalfCurve)
		nCenterW = 0;

	pMemGraphics->DrawLine(&WhitePen, rc.left, nCenterH, rc.Width(), nCenterH);
	pMemGraphics->DrawLine(&WhitePen, nCenterW, 0, nCenterW, rc.Height());

	float xmax = 32768.0f;
	float ymax = 32768.0f;

	if (m_pCurValueB)
	{
		xmax = 255.0f;
		ymax = 255.0f;
	}
	if (m_pfCurValue)
	{
		xmax = 1.1f;
		ymax = 1.1f;
	}

	float fTick = xmax / float(rc.Width() / 10);

	int j;

	SolidBrush Green(Color(32, 0, 255, 0));
	float fMinus = -xmax;
	float fFactor = 2.0f;
	if (!m_bHalfCurve)
	{
		for (j = 0; j < rc.Height() / 2; j += (rc.Height() / 10))
		{
			pMemGraphics->DrawLine(&WhitePen2, rc.left, j + nCenterH, rc.Width(), j + nCenterH);
			pMemGraphics->DrawLine(&WhitePen2, rc.left, nCenterH - j, rc.Width(), nCenterH - j);
		}
		for (j = 0; j < rc.Width() / 2; j += (rc.Width() / 10))
		{
			pMemGraphics->DrawLine(&WhitePen2, j + nCenterW, 0, j + nCenterW, rc.Height());
			pMemGraphics->DrawLine(&WhitePen2, nCenterW - j, 0, nCenterW - j, rc.Height());
		}
		pMemGraphics->FillRectangle(&Green, int(((xmax - m_pCurve->m_fDeadZone) / (2.0f* xmax))*(rc.Width() - 2)), 0, int(((m_pCurve->m_fDeadZone) / (xmax))*(rc.Width() - 2)), rc.Height());
		pMemGraphics->DrawRectangle(&GreenPen, int(((xmax - m_pCurve->m_fDeadZone) / (2.0f* xmax))*(rc.Width() - 2)), 0, int(((m_pCurve->m_fDeadZone) / (xmax))*(rc.Width() - 2)), rc.Height());
	}
	else
	{
		for (j = 0; j < rc.Height(); j += (rc.Height() / 10))
		{
			pMemGraphics->DrawLine(&WhitePen2, rc.left, j + nCenterH, rc.Width(), j + nCenterH);
		}
		for (j = 0; j < rc.Width(); j += (rc.Width() / 10))
		{
			pMemGraphics->DrawLine(&WhitePen2, j, 0, j, rc.Height());
		}
		pMemGraphics->FillRectangle(&Green, 0, 0, int(((m_pCurve->m_fDeadZone) / (xmax))*(rc.Width() - 2)), rc.Height());
		pMemGraphics->DrawRectangle(&GreenPen, 0, 0, int(((m_pCurve->m_fDeadZone) / (xmax))*(rc.Width() - 2)), rc.Height());
		fMinus = 0;
		fFactor = 1.0f;
	}
	CSISharedData::Curve localCurve = CSISharedData::Curve(m_pCurve->m_fDeadZone, m_pCurve->m_fxSat, m_pCurve->m_fyMax, m_pCurve->m_fExp);

	for (float x = fMinus; x < xmax; x += fTick)
	{
		//float y1 = -ApplyCurvePolynomiale(x, float(m_pCurve->m_fDeadZone), float(m_pCurve->m_fxSat), float(m_pCurve->m_fyMax), float(m_pCurve->m_fExp));



		float y1 = -localCurve.CalcCurveValue(x);
		//float y2 = -ApplyCurvePolynomiale(x + fTick, float(m_pCurve->m_fDeadZone), float(m_pCurve->m_fxSat), float(m_pCurve->m_fyMax), float(m_pCurve->m_fExp));
		float y2 = -localCurve.CalcCurveValue(x + fTick);


		pMemGraphics->DrawLine(&RedPen, int(((x + (xmax*(fFactor - 1.0f))) / (fFactor* xmax))*(rc.Width() - 2))
			, int(((y1 + (ymax*(fFactor - 1.0f))) / (fFactor * ymax))*(rc.Height() - 2))
			, int((((x + fTick) + (xmax*(fFactor - 1.0f))) / (fFactor*xmax))*(rc.Width() - 2))
			, int(((y2 + (ymax*(fFactor - 1.0f))) / (fFactor * ymax))*(rc.Height() - 2))
		);
	}


	float SphereSize = 10.0f*(rc.Width() / 256.0f);
	SolidBrush red(Color(128, 255, 0, 0));
	REAL x = 0;

	if (m_pCurValueS)
		x = (REAL)*m_pCurValueS;

	if (m_pCurValueB)
		x = (REAL)*m_pCurValueB;

	if (m_pfCurValue)
		x = (REAL)*m_pfCurValue*m_pCurve->m_fxSat;


	//REAL y = -ApplyCurvePolynomiale(x, float(m_pCurve->m_fDeadZone), float(m_pCurve->m_fxSat), float(m_pCurve->m_fyMax), float(m_pCurve->m_fExp));
	REAL y = -localCurve.CalcCurveValue(x);

	pMemGraphics->FillEllipse(&red, float(((x + (xmax*(fFactor - 1.0f))) / (fFactor* xmax))*(rc.Width() - 2.0f) - (SphereSize / 2.0f)), float(((y + (ymax*(fFactor - 1.0f))) / (fFactor * ymax))*(rc.Height() - 2.0f) - (SphereSize / 2.0f)), SphereSize, SphereSize);
	pMemGraphics->DrawEllipse(&WhitePen, float(((x + (xmax*(fFactor - 1.0f))) / (fFactor* xmax))*(rc.Width() - 2.0f) - (SphereSize / 2.0f)), float(((y + (ymax*(fFactor - 1.0f))) / (fFactor * ymax))*(rc.Height() - 2.0f) - (SphereSize / 2.0f)), SphereSize, SphereSize);

	Graphics graphics(lpDrawItemStruct->hDC);

	Status status;
	if ((status = graphics.DrawImage(pMemBitmap, 0, 0)) != Ok)
	{
		//some error
		printf("error");
	}

	delete pMemGraphics;
	delete pMemBitmap;

}

BOOL CCurveCtrl::OnEraseBkgnd(CDC *pDC)
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
}