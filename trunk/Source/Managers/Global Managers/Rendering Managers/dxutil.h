//	Holds utility macros for SGD.
#ifndef _DXUTIL_H_
#define _DXUTIL_H_

// The include files for Direct3D9
#include <d3d9.h>
#include <d3dx9.h>

// The library files for Direct3D9
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment (lib, "dxguid.lib")

#include "VertexStructs.h"

//	Macro to safely Release COM Objects.
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)			if (p) { p->Release(); p = NULL; }
#endif

//	Macro to safely delete pointers.
#ifndef SAFE_DELETE
	#define SAFE_DELETE(p)			if (p) { delete p; p = NULL; }
#endif

//	Macro to safely delete array pointers.
#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(p)	if (p) { delete [] p; p = NULL; }
#endif

//	Macro for reporting Wrapper errors
#ifndef DXERROR
	#define DXERROR(a)	{ OutputDebugString(a); OutputDebugString("\n"); return false; }
#endif

#ifndef RAND_FLOAT
	#define RAND_FLOAT(min,max) (((rand()/(float)RAND_MAX)*((max)-(min)))+(min))
#endif

#ifndef SAFE_CLOSE
	#define	SAFE_CLOSE(stream) { stream.close(); stream.clear(); }
#endif

#endif // _DXUTIL_H_