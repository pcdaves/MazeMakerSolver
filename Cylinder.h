// Cylinder.h: interface for the CCylinder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CYLINDER_H__F7B8B61C_6337_483D_A227_A28ECF766D93__INCLUDED_)
#define AFX_CYLINDER_H__F7B8B61C_6337_483D_A227_A28ECF766D93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx9.h>
#include "Base.h"

//Define a FVF for our cylinder
#define CYLINDER_D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

class CCylinder : public CBase  
{
private:
	//Define a custom vertex for our cylinder
	struct CYLINDER_CUSTOMVERTEX
	{
		FLOAT x, y, z;		//Position of vertex in 3D space
		FLOAT nx, ny, nz;	//Lighting Normal
		FLOAT tu, tv;		//Texture coordinates
	};

public:
	bool SetMaterial(D3DCOLORVALUE rgbaDiffuse, D3DCOLORVALUE rgbaAmbient, D3DCOLORVALUE rgbaSpecular, D3DCOLORVALUE rgbaEmissive, float rPower);
	bool SetSideTexture(const char* szTextureFilePath);
	bool SetEndTexture(const char* szTextureFilePath);
	DWORD Render();
	CCylinder(LPDIRECT3DDEVICE9 pD3DDevice, float rHeight = 1.0, float rRadius = 1.0, int nSegments = 20);
	virtual ~CCylinder();

private:
	bool UpdateVertices();
	bool CreateVertexBuffer();
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DTEXTURE9 m_pSideTexture;
	LPDIRECT3DTEXTURE9 m_pEndTexture;
	D3DMATERIAL9 m_matMaterial;

	float m_rHeight;
	float m_rRadius;
	int m_nSegments; 

	DWORD m_dwNumOfVertices;	
	DWORD m_dwNumOfSidePolygons;
	DWORD m_dwNumOfEndPolygons;

};

#endif // !defined(AFX_CYLINDER_H__F7B8B61C_6337_483D_A227_A28ECF766D93__INCLUDED_)
