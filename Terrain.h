// Terrain.h: interface for the CTerrain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRAIN_H__605B63BD_B46A_44BF_A2F2_573132F97949__INCLUDED_)
#define AFX_TERRAIN_H__605B63BD_B46A_44BF_A2F2_573132F97949__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx9.h>
#include "Base.h"

//Define a FVF for our terrain
#define TERRAIN_D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

class CTerrain : public CBase  
{
private:
	//Define a custom vertex for our terrain
	struct TERRAIN_CUSTOMVERTEX
	{
		FLOAT x, y, z;		//Position of vertex in 3D space
		FLOAT nx, ny, nz;	//Lighting Normal
		FLOAT tu, tv;		//Texture coordinates
	};

public:
	bool SetMaterial(D3DCOLORVALUE rgbaDiffuse, D3DCOLORVALUE rgbaAmbient, D3DCOLORVALUE rgbaSpecular, D3DCOLORVALUE rgbaEmissive, float rPower);
	bool SetTexture(const char* szTextureFilePath);
	DWORD Render();
	CTerrain(LPDIRECT3DDEVICE9 pD3DDevice, WORD wRows = 10, WORD wCols = 10, float rTileSize = 1.0, WORD wMaxHeight = 10);
	virtual ~CTerrain();

private:
	bool CreateIndexBuffer();
	bool UpdateVertices();
	bool CreateVertexBuffer();
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DTEXTURE9 m_pTexture;
	D3DMATERIAL9 m_matMaterial;
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;	

	WORD m_wRows;
	WORD m_wCols;
	WORD m_wMaxHeight;
	float m_rTileSize;

	DWORD m_dwNumOfVertices;	
	DWORD m_dwNumOfIndices;
	DWORD m_dwNumOfPolygons;

};

#endif // !defined(AFX_TERRAIN_H__605B63BD_B46A_44BF_A2F2_573132F97949__INCLUDED_)
