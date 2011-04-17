// Precompiled Headers
#ifndef _STDAFX_H_
#define _STDAFX_H_

// Windows
#include <windows.h>
#define LEAN_AND_MEAN

// Visual Leak Detector
#define VLD_AGGREGATE_DUPLICATES	//	don't show me the same leak 50 times!!
#define VLD_MAX_DATA_DUMP 0
#include <vld.h>		//	for Visual Leak Detection

// Debug and Console I/O
#include <crtdbg.h>
#include <io.h>
#include <iostream>
#include <fcntl.h>

// STL containers
#include <list>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <string>

// Direct3D
#include <D3dx9math.h>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment (lib, "dxguid.lib")

// DirectInput
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <xinput.h>
#pragma comment(lib, "xinput.lib")

// Math
#include <cmath>
#include <ctype.h>

// Memory Allocation
#include <memory>
#include <malloc.h>
#include <assert.h>

// File I/O
#include <fstream>

// Global Managers
#include "Managers/Global Managers/Event Manager/CEventManager.h"
#include "Managers/Global Managers/Event Manager/CIDGen.h"
#include "Managers/Global Managers/Memory Manager/CMemoryManager.h"

// Lua
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#endif