// Game.h: interface for the CGame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAME_H__687221F3_ACFF_4D66_97DB_88AB70E0CA59__INCLUDED_)
#define AFX_GAME_H__687221F3_ACFF_4D66_97DB_88AB70E0CA59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx9.h>
#include "Base.h"

#include "Cuboid.h"
#include "Terrain.h"
#include "Sphere.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Mesh.h"

#include "Maze.h"

class CGame : public CBase
{
public:
	bool Initialise(HWND hWnd, UINT nWidth, UINT nHeight);
	void GameLoop();
	LPDIRECT3DDEVICE9 GetDevice();
	CGame();
	virtual ~CGame();
	

private:
	bool InitialiseLights();
	D3DFORMAT CheckDisplayMode(UINT nWidth, UINT nHeight, UINT nDepth);
	bool InitialiseGame();
	bool InitialiseD3D(HWND hWnd, UINT nWidth, UINT nHeight);
	void Render();
	LPDIRECT3D9 m_pD3D;
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	void SetupCamera();

	HWND hWnd;
	
	DWORD m_dwFrames;
	DWORD m_dwStartTime;
	DWORD m_dwEndTime;
	DWORD m_dwTotalPolygons;

	CMesh* m_pMesh1;
	CMesh* m_pMesh2;
	CMesh* m_pMesh3;

	CMaze* m_pMaze1;
};

#endif // !defined(AFX_GAME_H__687221F3_ACFF_4D66_97DB_88AB70E0CA59__INCLUDED_)
