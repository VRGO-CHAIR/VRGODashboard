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
#include <stdio.h>
#include "DebugLog.h"
#include "x86timer.h"
#include <time.h>

#if DEBUG_LOG_LEVEL>0



char *g_pDebugText = "C:\\Debug_Log.txt";
int g_nDebugLogLevel = DEBUG_LOG_LEVEL;
int g_nDebugLogMode = DEBUG_LOG_FILE | DEBUG_LOG_SHOW_LEVEL | DEBUG_LOG_SHIFT_LEVEL | DEBUG_LOG_SHOW_DTIME;
int g_nDebugLogColorT = DEBUG_LOG_WHITE;
int g_nDebugLogColorF = DEBUG_LOG_BLACK;
int g_nDebugLogOldColorT = -1;
int g_nDebugLogOldColorF = -1;

char *g_sDebugLogHTMLColor[] = { "Black","DarkBlue","Green","CadetBlue","SaddleBrown","Purple","Khaki","LightGray","Gray","Blue","GreenYellow","Turquoise","Red","LightPink","Yellow","White" };

x86timer g_DebugLogTime;

void InitDebugLog(char *sPath)
{
	g_pDebugText = sPath;
	g_DebugLogTime.start();

	if (g_nDebugLogMode&DEBUG_LOG_FILE)
	{
		FILE *pFile;
		fopen_s(&pFile, g_pDebugText, "wt");
		if (pFile)
		{
			time_t rawtime;
			struct tm *info = new struct tm();
			char buffer[80];
			time(&rawtime);
			localtime_s(info, &rawtime);
			strftime(buffer, 80, "%H:%M:%S - %d/%m/%y", info);

			if (g_nDebugLogMode&DEBUG_LOG_HTML)
			{
				fprintf(pFile, "<!doctype html>\n<html lang = en>\n<head>\n<style>\nspan{\n\tfont-family: monospace;\n}\nbody{\ncolor:White; background-color:black; font-family: monospace;\n}\npre{\ncolor:White; background-color:black; font-family: monospace; margin: 0em 0px 0em 0px;\n}\n</style>\n<meta charset = utf - 8>\n <title>3dRSdkLog %s</title>\n</head>\n<body>\n<table>\n", g_pDebugText);
				fprintf(pFile, "<span style=\"color:White;\">\n<br><b>== %s ==</b><br><br></span>\n", buffer);
			}
			else
			{
				fprintf(pFile, "------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
				fprintf(pFile, "== %s ==\n\n", buffer);
			}

			fclose(pFile);
		}
	}
}

void CloseDebugLog()
{
	if (g_nDebugLogMode&DEBUG_LOG_FILE)
	{
		FILE *pFile;
		fopen_s(&pFile, g_pDebugText, "at");
		if (pFile)
		{
			if (g_nDebugLogMode&DEBUG_LOG_HTML)
			{
				fprintf(pFile, "</table></body>\n</html>\n");
			}
			else
			{
				fprintf(pFile, "------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			}
			fclose(pFile);
		}
	}
}

void SetDebugLogMode(int nMode)
{
	g_nDebugLogMode = nMode;
}

void SetDebugLevel(int nLevel)
{
	g_nDebugLogLevel = nLevel;
}

void SetDebugLogColor(int t, int f)
{
	bool bFirstTime = false;
	if (g_nDebugLogOldColorT == -1 && g_nDebugLogOldColorF == -1)
		bFirstTime = true;

	g_nDebugLogOldColorT = g_nDebugLogColorT;
	g_nDebugLogOldColorF = g_nDebugLogColorF;
	g_nDebugLogColorT = t;
	g_nDebugLogColorF = f;
	if (g_nDebugLogOldColorT != g_nDebugLogColorT || g_nDebugLogOldColorF != g_nDebugLogColorF)
	{
		/*	if (g_nDebugLogMode&DEBUG_LOG_FILE)
		{
		FILE *pFile = fopen(g_pDebugText, "at");
		if (pFile)
		{
		if (g_nDebugLogMode&DEBUG_LOG_HTML)
		{
		if (!bFirstTime)
		fprintf(pFile, "</span>\n");
		fprintf(pFile, "<span style=\"color:%s; background-color:%s;\">\n", g_sDebugLogHTMLColor[g_nDebugLogColorT], g_sDebugLogHTMLColor[g_nDebugLogColorF]);
		g_bDebugLogSpanOpen = true;
		}
		fclose(pFile);
		}
		}*/

		if (g_nDebugLogMode&DEBUG_LOG_STDOUT)
		{
			HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(H, f * 16 + t);
		}
		g_nDebugLogOldColorT = g_nDebugLogColorT;
		g_nDebugLogOldColorF = g_nDebugLogColorF;
	}
}
void Debug_Log(int level, const char *fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);
	Debug_Log(level, fmt, argptr);
	va_end(argptr);
}

void Debug_Log(int level, const char *fmt, va_list args)
{
	if (level <= g_nDebugLogLevel)
	{
		char sTextLevel[10];
		char sTextTime[32];
		char sTextBuffer[1024];
		sTextLevel[0] = 0;
		sTextTime[0] = 0;
		memset(sTextBuffer, ' ', 15);

		if (g_nDebugLogMode&DEBUG_LOG_SHOW_LEVEL)
		{
			sprintf_s(sTextLevel, "[%d] ", level);
		}

		if (g_nDebugLogMode&DEBUG_LOG_SHOW_TIME || g_nDebugLogMode&DEBUG_LOG_SHOW_DTIME)
		{
			sprintf_s(sTextTime, "(% 15.8f ms) ", g_DebugLogTime.stop() / 1000000.0);
		}

		if (g_nDebugLogMode&DEBUG_LOG_SHOW_DTIME)
			g_DebugLogTime.start();

		int nShiftLevel = 0;
		if (g_nDebugLogMode&DEBUG_LOG_SHIFT_LEVEL)
			nShiftLevel = level;

		vsprintf(sTextBuffer + nShiftLevel, fmt, args);

		if (g_nDebugLogMode&DEBUG_LOG_OUTDEBUG)
		{
			char sTextBufferFinal[1024];
			sprintf_s(sTextBufferFinal, "%s%s%s", sTextLevel, sTextTime, sTextBuffer);
			::OutputDebugStringA(sTextBufferFinal);
		}

		if (g_nDebugLogMode&DEBUG_LOG_STDOUT)
		{
			char sTextBufferFinal[1024];
			sprintf_s(sTextBufferFinal, "%s%s%s", sTextLevel, sTextTime, sTextBuffer);
			printf("%s\n", sTextBufferFinal);
		}

		if (g_nDebugLogMode&DEBUG_LOG_FILE)
		{
			FILE *pFile;
			fopen_s(&pFile, g_pDebugText, "at");
			if (pFile)
			{
				if (g_nDebugLogMode&DEBUG_LOG_HTML)
				{
					/*
					if (g_nDebugLogOldColorT != g_nDebugLogColorT || g_nDebugLogOldColorF != g_nDebugLogColorF)
					{

					fprintf(pFile, "<span style=\"color:%s; background-color:%s;\">\n", g_sDebugLogHTMLColor[g_nDebugLogColorT], g_sDebugLogHTMLColor[g_nDebugLogColorF]);
					}
					*/
					fprintf_s(pFile, "<tr>");
					if (sTextLevel[0] != 0)
						fprintf_s(pFile, "<td>%s</td>", sTextLevel);
					if (sTextTime[0] != 0)
						fprintf_s(pFile, "<td><pre>%s</pre></td>", sTextTime);

					fprintf_s(pFile, "<td><pre style=\"color:%s; background-color:%s;\">%s</pre></td></tr>\n", g_sDebugLogHTMLColor[g_nDebugLogColorT], g_sDebugLogHTMLColor[g_nDebugLogColorF], sTextBuffer);
					/*if (g_nDebugLogOldColorT != g_nDebugLogColorT || g_nDebugLogOldColorF != g_nDebugLogColorF)
					{
					g_nDebugLogOldColorT = g_nDebugLogColorT;
					g_nDebugLogOldColorF = g_nDebugLogColorF;
					fprintf(pFile, "</span>\n");
					}*/
					//fprintf(pFile, "<br>\n");
				}
				else
				{
					char sTextBufferFinal[1024];
					sprintf_s(sTextBufferFinal, "%s%s%s", sTextLevel, sTextTime, sTextBuffer);
					fprintf_s(pFile, "%s\n", sTextBufferFinal);
				}
				fclose(pFile);
			}
		}
	}
}


#endif