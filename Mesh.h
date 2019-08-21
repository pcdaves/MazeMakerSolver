// Mesh.h: interface for the CMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESH_H__F87899E4_6F53_4769_B9C3_2ED555BC3871__INCLUDED_)
#define AFX_MESH_H__F87899E4_6F53_4769_B9C3_2ED555BC3871__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Base.h"

#define MESH_D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

class CMesh : public CBase  
{
public:
	DWORD Render();
	CMesh(LPDIRECT3DDEVICE9 pD3DDevice, LPSTR pFilename);
	virtual ~CMesh();

private:
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	DWORD m_dwNumMaterials;
	LPD3DXMESH m_pMesh;
	D3DMATERIAL9* m_pMeshMaterials;
    LPDIRECT3DTEXTURE9* m_pMeshTextures;
};

#endif // !defined(AFX_MESH_H__F87899E4_6F53_4769_B9C3_2ED555BC3871__INCLUDED_)
