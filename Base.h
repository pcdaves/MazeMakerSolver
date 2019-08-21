// Base.h: interface for the CBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASE_H__19BD59EB_1A65_420F_B8CC_09F798CE485C__INCLUDED_)
#define AFX_BASE_H__19BD59EB_1A65_420F_B8CC_09F798CE485C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdarg.h>
#include <d3dx9.h>
#include <tlhelp32.h>
#include <windows.h>

#define SafeRelease(pInterface) if(pInterface != NULL) {pInterface->Release(); pInterface=NULL;}
#define SafeDelete(pObject) if(pObject != NULL) {delete pObject; pObject=NULL;}

class CBase  
{
private:
	static bool m_fEnableLogging;

public:
	CBase();
	virtual ~CBase();

	D3DXVECTOR3 GetTriangeNormal(D3DXVECTOR3* vVertex1, D3DXVECTOR3* vVertex2, D3DXVECTOR3* vVertex3);

	static void StartLogging();
	static void StopLogging();
	static void LogError(char *lpszText, ...);
	static void LogInfo(char *lpszText, ...);
	static void LogWarning(char *lpszText, ...);
	static DWORD GetMemoryUsage();
	static void LogMemoryUsage();
};

#endif // !defined(AFX_BASE_H__19BD59EB_1A65_420F_B8CC_09F798CE485C__INCLUDED_)
