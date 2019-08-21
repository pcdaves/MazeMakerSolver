// Cone.h: interface for the CCone class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONE_H__8D338063_9E7F_4455_8A3B_394075E29035__INCLUDED_)
#define AFX_CONE_H__8D338063_9E7F_4455_8A3B_394075E29035__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx9.h>
#include "Base.h"

//Define a FVF for our cone
#define CONE_D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

class CCone : public CBase  
{

private:
	//Define a custom vertex for our cone
	struct CONE_CUSTOMVERTEX
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
	CCone(LPDIRECT3DDEVICE9 pD3DDevice, float rHeight = 1.0, float rRadius = 1.0, int nSegments = 20);
	virtual ~CCone();

private:
	bool UpdateVertices();
	bool CreateIndexBuffer();
	bool CreateVertexBuffer();
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;
	LPDIRECT3DTEXTURE9 m_pSideTexture;
	LPDIRECT3DTEXTURE9 m_pEndTexture;
	D3DMATERIAL9 m_matMaterial;

	float m_rHeight;
	float m_rRadius;
	int m_nSegments;

	DWORD m_dwNumOfIndices;
	DWORD m_dwNumOfVertices;	
	DWORD m_dwNumOfSidePolygons;
	DWORD m_dwNumOfEndPolygons;

};

#endif // !defined(AFX_CONE_H__8D338063_9E7F_4455_8A3B_394075E29035__INCLUDED_)
