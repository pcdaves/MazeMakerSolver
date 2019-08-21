// Cuboid.cpp: implementation of the CCuboid class.
//
//////////////////////////////////////////////////////////////////////

#include "Cuboid.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCuboid::CCuboid(LPDIRECT3DDEVICE9 pD3DDevice, float rWidth, float rHeight, float rDepth)
{
	m_pD3DDevice = pD3DDevice;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_pTexture = NULL;

	//Setup counts for this object
	m_dwNumOfVertices = 24;	
	m_dwNumOfIndices  = 36;
	m_dwNumOfPolygons = 12;

	//Set a default size and position
	m_rWidth = rWidth;
	m_rHeight = rHeight;
	m_rDepth = rDepth;

	//Set material default values (R, G, B, A)
	D3DCOLORVALUE rgbaDiffuse  = {1.0, 1.0, 1.0, 0.0,};
	D3DCOLORVALUE rgbaAmbient  = {1.0, 1.0, 1.0, 0.0,};
	D3DCOLORVALUE rgbaSpecular = {0.0, 0.0, 0.0, 0.0,};
	D3DCOLORVALUE rgbaEmissive = {0.0, 0.0, 0.0, 0.0,};
	
	SetMaterial(rgbaDiffuse, rgbaAmbient, rgbaSpecular, rgbaEmissive, 0);

	//Initialize Vertex Buffer
    if(CreateVertexBuffer())
	{
		if(CreateIndexBuffer())
		{
			if(UpdateVertices())
			{
				LogInfo("<li>Cuboid created OK");
				return;
			}
		}
	}

	LogError("<li>Cuboid failed to create");
}

CCuboid::~CCuboid()
{
	SafeRelease(m_pTexture);
	SafeRelease(m_pIndexBuffer);
	SafeRelease(m_pVertexBuffer);

	LogInfo("<li>Cuboid destroyed OK");
}

DWORD CCuboid::Render()
{
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(CUBOID_CUSTOMVERTEX));
	m_pD3DDevice->SetFVF(CUBOID_D3DFVF_CUSTOMVERTEX);

	if(m_pTexture != NULL)
	{
		//A texture has been set. We want our texture to be shaded based
		//on the current light levels, so used D3DTOP_MODULATE.
		m_pD3DDevice->SetTexture(0, m_pTexture);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}
	else
	{
		//No texture has been set
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}

	//Select the material to use
	m_pD3DDevice->SetMaterial(&m_matMaterial);
	
	//Select index buffer
	m_pD3DDevice->SetIndices(m_pIndexBuffer);

	//Render polygons from index buffer
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwNumOfVertices, 0, m_dwNumOfPolygons);
	
	m_pD3DDevice->SetTexture(0, 0);
	m_pD3DDevice->SetStreamSource(0, 0, 0, 0);
	m_pD3DDevice->SetIndices(0);
	
	//Return the number of polygons rendered
	return m_dwNumOfPolygons;
}

bool CCuboid::CreateVertexBuffer()
{
    //Create the vertex buffer from our device.
    if(FAILED(m_pD3DDevice->CreateVertexBuffer(m_dwNumOfVertices * sizeof(CUBOID_CUSTOMVERTEX),
                                               0, CUBOID_D3DFVF_CUSTOMVERTEX,
                                               D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL)))
    {
        LogError("<li>CCuboid: Unable to create vertex buffer.");
		return false;
    }

    return true;
}

bool CCuboid::CreateIndexBuffer()
{
	VOID* pBufferIndices;
	
	//Create the index buffer from our device
	if(FAILED(m_pD3DDevice->CreateIndexBuffer(m_dwNumOfIndices * sizeof(WORD), 
											  0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
											  &m_pIndexBuffer, NULL)))
	{
	    LogError("<li>CCuboid: Unable to create index buffer.");
		return false;
	}
	
	//Set values for the index buffer
	WORD pIndices[] = { 0, 1, 2, 3, 2, 1,	//Top
					    4, 5, 6, 7, 6, 5,	//Face 1
					    8, 9,10,11,10, 9,	//Face 2
					   12,13,14,15,14,13,	//Face 3
					   16,17,18,19,18,17,	//Face 4
					   20,21,22,23,22,21};	//Bottom
	   
	//Get a pointer to the index buffer indices and lock the index buffer    
	if(FAILED(m_pIndexBuffer->Lock(0, m_dwNumOfIndices * sizeof(WORD), &pBufferIndices, 0)))
	{
		LogError("<li>CCuboid: Unable to lock index buffer.");
		return false;
	}

	//Copy our stored indices values into the index buffer
	memcpy(pBufferIndices, pIndices, m_dwNumOfIndices * sizeof(WORD));
	
	//Unlock the index buffer
	if(FAILED(m_pIndexBuffer->Unlock()))
	{
		LogError("<li>CCuboid: Unable to unlock index buffer.");
		return false;
	}

	return true;
}

bool CCuboid::UpdateVertices()
{
	DWORD i;
	VOID* pVertices;
	WORD* pBufferIndices;
	D3DXVECTOR3 vNormal;
	DWORD dwVertex1;
	DWORD dwVertex2;
	DWORD dwVertex3;

	WORD* pNumOfSharedPolygons = new WORD[m_dwNumOfVertices];			//Array holds how many times this vertex is shared
	D3DVECTOR* pSumVertexNormal = new D3DVECTOR[m_dwNumOfVertices];		//Array holds sum of all face normals for shared vertex

	//Clear memory
	for(i = 0; i < m_dwNumOfVertices; i++)
	{
		pNumOfSharedPolygons[i] = 0;
		pSumVertexNormal[i] = D3DXVECTOR3(0,0,0);
	}


	CUBOID_CUSTOMVERTEX cvVertices[] =
	{	
		//Top Face
		{0.0f - (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 0
		{0.0f - (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 1
		{0.0f + (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 2
		{0.0f + (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 3
		
		//Face 1
		{0.0f - (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 4
		{0.0f - (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 5
		{0.0f + (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 6
		{0.0f + (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 7
		
		//Face 2
		{0.0f + (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 8
		{0.0f + (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 9
		{0.0f + (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 10
		{0.0f + (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 11
		
		//Face 3
		{0.0f + (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 12
		{0.0f + (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 13
		{0.0f - (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 14
		{0.0f - (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 15

		//Face 4
		{0.0f - (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 16
		{0.0f - (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 17
		{0.0f - (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 18
		{0.0f - (m_rWidth / 2), 0.0f + (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 19

		//Bottom Face
		{0.0f + (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,},		//Vertex 20
		{0.0f + (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,},		//Vertex 21
		{0.0f - (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f - (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,},		//Vertex 22
		{0.0f - (m_rWidth / 2), 0.0f - (m_rHeight / 2), 0.0f + (m_rDepth / 2), 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,},		//Vertex 23
	};




	//Get a pointer to the index buffer indices and lock the index buffer    
	if(FAILED(m_pIndexBuffer->Lock(0, m_dwNumOfIndices * sizeof(WORD), (void**)&pBufferIndices, D3DLOCK_READONLY)))
	{
		LogError("<li>CCuboid: Unable to lock index buffer.");
		return false;
	}

	//For each triangle, count the number of times each vertex is used and
	//add together the normals of faces that share a vertex
	for(i = 0; i < m_dwNumOfIndices; i += 3)
	{
		dwVertex1 = pBufferIndices[i];
		dwVertex2 = pBufferIndices[i + 1];
		dwVertex3 = pBufferIndices[i + 2];


		vNormal = GetTriangeNormal(&D3DXVECTOR3(cvVertices[dwVertex1].x, cvVertices[dwVertex1].y, cvVertices[dwVertex1].z), 
								   &D3DXVECTOR3(cvVertices[dwVertex2].x, cvVertices[dwVertex2].y, cvVertices[dwVertex2].z), 
								   &D3DXVECTOR3(cvVertices[dwVertex3].x, cvVertices[dwVertex3].y, cvVertices[dwVertex3].z));

		
		pNumOfSharedPolygons[dwVertex1]++;
		pNumOfSharedPolygons[dwVertex2]++;
		pNumOfSharedPolygons[dwVertex3]++;

		pSumVertexNormal[dwVertex1].x += vNormal.x;
		pSumVertexNormal[dwVertex1].y += vNormal.y;
		pSumVertexNormal[dwVertex1].z += vNormal.z;
		
		pSumVertexNormal[dwVertex2].x += vNormal.x;
		pSumVertexNormal[dwVertex2].y += vNormal.y;
		pSumVertexNormal[dwVertex2].z += vNormal.z;

		pSumVertexNormal[dwVertex3].x += vNormal.x;
		pSumVertexNormal[dwVertex3].y += vNormal.y;
		pSumVertexNormal[dwVertex3].z += vNormal.z;
	}

	//Unlock the index buffer
	if(FAILED(m_pIndexBuffer->Unlock()))
	{
		LogError("<li>CCuboid: Unable to unlock index buffer.");
		return false;
	}

	//For each vertex, calculate and set the average normal
	for(i = 0; i < m_dwNumOfVertices; i++)
	{
		vNormal.x = pSumVertexNormal[i].x / pNumOfSharedPolygons[i];
		vNormal.y = pSumVertexNormal[i].y / pNumOfSharedPolygons[i];
		vNormal.z = pSumVertexNormal[i].z / pNumOfSharedPolygons[i];

		D3DXVec3Normalize(&vNormal, &vNormal);
		
		cvVertices[i].nx = vNormal.x;
		cvVertices[i].ny = vNormal.y;
		cvVertices[i].nz = vNormal.z;
	}


	//Get a pointer to the vertex buffer vertices and lock the vertex buffer
    if(FAILED(m_pVertexBuffer->Lock(0, sizeof(cvVertices), &pVertices, 0)))
    {
        LogError("<li>CCuboid: Unable to lock vertex buffer.");
		return false;
    }

    //Copy our stored vertices values into the vertex buffer
    memcpy(pVertices, cvVertices, sizeof(cvVertices));

    //Unlock the vertex buffer
    if(FAILED(m_pVertexBuffer->Unlock()))
	{
		LogError("<li>CCuboid: Unable to unlock vertex buffer.");
        return false;
    }

	//Clean up
	delete pNumOfSharedPolygons;
	delete pSumVertexNormal;

	pNumOfSharedPolygons = NULL;
	pSumVertexNormal = NULL;

	return true;
}

bool CCuboid::SetTexture(const char *szTextureFilePath)
{
	if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, szTextureFilePath, &m_pTexture)))
	{
		return false;
	}

	return true;
}

bool CCuboid::SetMaterial(D3DCOLORVALUE rgbaDiffuse, D3DCOLORVALUE rgbaAmbient, D3DCOLORVALUE rgbaSpecular, D3DCOLORVALUE rgbaEmissive, float rPower)
{
	//Set the RGBA for diffuse light reflected from this material. 
	m_matMaterial.Diffuse = rgbaDiffuse; 

	//Set the RGBA for ambient light reflected from this material. 
	m_matMaterial.Ambient = rgbaAmbient; 

	//Set the color and sharpness of specular highlights for the material. 
	m_matMaterial.Specular = rgbaSpecular; 
	m_matMaterial.Power = rPower;

	//Set the RGBA for light emitted from this material. 
	m_matMaterial.Emissive = rgbaEmissive;

	return true;
}


