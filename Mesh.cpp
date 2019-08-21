// Mesh.cpp: implementation of the CMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "Mesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMesh::CMesh(LPDIRECT3DDEVICE9 pD3DDevice, LPSTR pFilename)
{
	LPD3DXBUFFER pMaterialsBuffer = NULL;
	LPD3DXMESH pMesh = NULL;	

	m_pD3DDevice = pD3DDevice;
	
	if(FAILED(D3DXLoadMeshFromX(pFilename, D3DXMESH_SYSTEMMEM, m_pD3DDevice, NULL, 
                                &pMaterialsBuffer, NULL,&m_dwNumMaterials, &pMesh)))
	{
		m_pMesh = NULL;
		m_pMeshMaterials = NULL;
		m_pMeshTextures = NULL;

		LogError("<li>Mesh '%s' failed to load", pFilename);
		return;
	}

    D3DXMATERIAL* matMaterials = (D3DXMATERIAL*)pMaterialsBuffer->GetBufferPointer();
    
	//Create two arrays. One to hold the materials and only to hold the textures
	m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];
    m_pMeshTextures  = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];

    for(DWORD i = 0; i < m_dwNumMaterials; i++)
    {
        // Copy the material
        m_pMeshMaterials[i] = matMaterials[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

		// Create the texture
        if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, 
                                            matMaterials[i].pTextureFilename, 
                                            &m_pMeshTextures[i])))
        {
            m_pMeshTextures[i] = NULL;
        }
    }

    //We've finished with the material buffer, so release it
    SafeRelease(pMaterialsBuffer);

	
	//Make sure that the normals are setup for our mesh
	pMesh->CloneMeshFVF(D3DXMESH_MANAGED, MESH_D3DFVF_CUSTOMVERTEX, m_pD3DDevice, &m_pMesh);
	SafeRelease(pMesh);

	D3DXComputeNormals(m_pMesh, NULL);


	LogInfo("<li>Mesh '%s' loaded OK", pFilename);
}

CMesh::~CMesh()
{
	SafeDelete(m_pMeshMaterials);

	if(m_pMeshTextures != NULL)
	{
		for(DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			if(m_pMeshTextures[i])
			{
				SafeRelease(m_pMeshTextures[i]);
			}
		}
	}

	SafeDelete(m_pMeshTextures);
	SafeRelease(m_pMesh);
    
	LogInfo("<li>Mesh destroyed OK");
}

DWORD CMesh::Render()
{
	if(m_pMesh != NULL)
	{
		for(DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			m_pD3DDevice->SetMaterial(&m_pMeshMaterials[i]);
			m_pD3DDevice->SetTexture(0, m_pMeshTextures[i]);
        
			m_pMesh->DrawSubset(i);
		}

		return m_pMesh->GetNumFaces();
	}
	else
	{
		return 0;
	}
}
