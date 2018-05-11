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


#pragma once
#pragma warning(disable : 4996)
#ifndef DEBUG_LOG_LEVEL
#define DEBUG_LOG_LEVEL 0
#endif


#define DEBUG_LOG(level, fmt, ...) DEBUG_LOG ## level (level,fmt, ##__VA_ARGS__)

#if DEBUG_LOG_LEVEL >= 1
#define DEBUG_LOG1(level,fmt, ...) Debug_Log (level,fmt, ##__VA_ARGS__)
#else
#define DEBUG_LOG1(...)
#endif

#if DEBUG_LOG_LEVEL >= 2
#define DEBUG_LOG2(level,fmt, ...) Debug_Log (level,fmt, ##__VA_ARGS__)
#else
#define DEBUG_LOG2(...)
#endif

#if DEBUG_LOG_LEVEL >= 3
#define DEBUG_LOG3(level,fmt, ...) Debug_Log (level,fmt, ##__VA_ARGS__)
#else
#define DEBUG_LOG3(...)
#endif

#if DEBUG_LOG_LEVEL >= 4
#define DEBUG_LOG4(level,fmt, ...) Debug_Log (level,fmt, ##__VA_ARGS__)
#else
#define DEBUG_LOG4(...)
#endif

#if DEBUG_LOG_LEVEL >= 5
#define DEBUG_LOG5(level,fmt, ...) Debug_Log (level,fmt, ##__VA_ARGS__)
#else
#define DEBUG_LOG5(...)
#endif

#if DEBUG_LOG_LEVEL >= 6
#define DEBUG_LOG6(level,fmt, ...) Debug_Log (level,fmt, ##__VA_ARGS__)
#else
#define DEBUG_LOG6(...)
#endif

#define DEBUG_LOG_OUTDEBUG		0x01
#define DEBUG_LOG_FILE			0x02
#define DEBUG_LOG_HTML			0x04
#define DEBUG_LOG_STDOUT		0x08
#define DEBUG_LOG_SHOW_TIME		0x10
#define DEBUG_LOG_SHOW_LEVEL	0x20
#define DEBUG_LOG_SHIFT_LEVEL	0x40
#define DEBUG_LOG_SHOW_DTIME	0x80

#define DEBUG_LOG_BLACK			0
#define DEBUG_LOG_DARK_BLUE		1
#define DEBUG_LOG_GREEN			2
#define DEBUG_LOG_GRAY_BLUE		3
#define DEBUG_LOG_BROWN			4
#define DEBUG_LOG_PURPLE		5
#define DEBUG_LOG_KHAKI			6
#define DEBUG_LOG_LIGHT_GRAY	7
#define DEBUG_LOG_GRAY			8
#define DEBUG_LOG_BLUE			9
#define DEBUG_LOG_FLASHY_GREEN	10
#define DEBUG_LOG_TURQUOISE		11
#define DEBUG_LOG_RED			12
#define DEBUG_LOG_PINK			13
#define DEBUG_LOG_YELLOW		14
#define DEBUG_LOG_WHITE			15

#if DEBUG_LOG_LEVEL>0

#define DEBUG_LOG_INIT(Path)		InitDebugLog(Path)
#define DEBUG_LOG_SETCOLOR(t,f)		SetDebugLogColor(t,f)
#define DEBUG_LOG_SETMODE(nMode)	SetDebugLogMode(nMode)
#define DEBUG_LOG_SETLEVEL(nLevel)	SetDebugLevel(nLevel)

void InitDebugLog(char *sPath);
void CloseDebugLog();
void SetDebugLogColor(int t, int f);
void SetDebugLogMode(int nMode);
void SetDebugLevel(int nLevel);
void Debug_Log(int level, const char *fmt, ...);
void Debug_Log(int level, const char *fmt, va_list args);
#else
#define DEBUG_LOG_INIT(path)
#define DEBUG_LOG_SETCOLOR(t,f)
#define DEBUG_LOG_SETMODE(nMode)
#define DEBUG_LOG_SETLEVEL(nLevel)
#endif

