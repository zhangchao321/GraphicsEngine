#ifndef _UTILS_H_
#define _UTILS_H_

#include "Third/dxerr.h"

#define ReleaseCOM(x) { if (x) { x->Release(); x = nullptr; } }
#define SafeDelete(x) { delete x; x = nullptr; }

#if defined( DEBUG) | defined(_DEBUG)
#define HR(x) { HRESULT hr = (x); if(FAILED(hr)) DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true); }
#define Check(x) { bool result = (x); if(!result) return false; }
#else
#define HR(x) (x)
#define Check(x) (x)
#endif

#endif