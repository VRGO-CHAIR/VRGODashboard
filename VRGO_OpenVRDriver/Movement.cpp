
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
#include "Movement.h"
#include "../Common/MathTools.h"

void CMovement::Reset(bool restoreMovement)
{
	// Save
	if (restoreMovement)
	{
		m_SavedMovement.fAngle = m_fAngle;
		m_SavedMovement.qRotation = m_qRotation;
		m_SavedMovement.v3Translation = m_v3Translation;
	}
	// Clean
	else
	{
		m_SavedMovement.fAngle = 0;
		Identity(m_SavedMovement.qRotation);
		for (int i = 0; i < 3; ++i)
			m_SavedMovement.v3Translation.v[i] = 0;
	}

	m_fAngle = 0;
	Identity(m_qRotation);
	for (int i = 0; i < 3; ++i)
		m_v3Translation.v[i] = 0;

	// Reset smooth velocity
	m_SmoothPitch.speedCurrent = 0;
	m_SmoothRoll.speedCurrent = 0;
	m_SmoothYaw.speedCurrent = 0;
	m_SmoothUpDown.speedCurrent = 0;
}

void CMovement::Restore()
{
	m_fAngle = m_SavedMovement.fAngle;
	m_qRotation = m_SavedMovement.qRotation;
	m_v3Translation = m_SavedMovement.v3Translation;
}