// Cuboid.h: interface for the CCuboid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUBOID_H__AE25AF5F_AE56_48F5_99DC_47CAAA14F245__INCLUDED_)
#define AFX_CUBOID_H__AE25AF5F_AE56_48F5_99DC_47CAAA14F245__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx9.h>
#include "Base.h"

//Define a FVF for our cubiods
#define CUBOID_D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

class CCuboid : public CBase
{
private:
	//Define a custom vertex for our cubiods
	struct CUBOID_CUSTOMVERTEX
	{
		FLOAT x, y, z;		//Position of vertex in 3D space
		FLOAT nx, ny, nz;	//Lighting Normal
		FLOAT tu, tv;		//Texture coordinates
	};

public:
	bool SetMaterial(D3DCOLORVALUE rgbaDiffuse, D3DCOLORVALUE rgbaAmbient, D3DCOLORVALUE rgbaSpecular, D3DCOLORVALUE rgbaEmissive, float rPower);
	bool SetTexture(const char* szTextureFilePath);
	DWORD Render();
	CCuboid(LPDIRECT3DDEVICE9 pD3DDevice, float rWidth = 1.0, float rHeight = 1.0, float rDepth = 1.0);
	virtual ~CCuboid();

private:
	bool CreateIndexBuffer();
	bool UpdateVertices();
	bool CreateVertexBuffer();
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DTEXTURE9 m_pTexture;
	D3DMATERIAL9 m_matMaterial;
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;	

	float m_rWidth;
	float m_rHeight;
	float m_rDepth;

	DWORD m_dwNumOfVertices;	
	DWORD m_dwNumOfIndices;
	DWORD m_dwNumOfPolygons;
};

#endif // !defined(AFX_CUBOID_H__AE25AF5F_AE56_48F5_99DC_47CAAA14F245__INCLUDED_)
