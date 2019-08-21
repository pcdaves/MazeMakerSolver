// Sphere.cpp: implementation of the CSphere class.
//
//////////////////////////////////////////////////////////////////////

#include "Sphere.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSphere::CSphere(LPDIRECT3DDEVICE9 pD3DDevice, int nRings, int nSegments)
{
	m_pD3DDevice = pD3DDevice;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_pTexture = NULL;
	
	m_nRings = nRings;
	m_nSegments = nSegments;

	//Setup counts for this object
	m_dwNumOfVertices = (m_nRings + 1) * (m_nSegments + 1);	
	m_dwNumOfIndices  = 2 * m_nRings * (m_nSegments + 1);
	m_dwNumOfPolygons = m_dwNumOfIndices - 2;

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
				LogInfo("<li>Sphere created OK");
				return;
			}
		}
	}

	LogError("<li>Sphere failed to create");
}

CSphere::~CSphere()
{
	SafeRelease(m_pTexture);
	SafeRelease(m_pIndexBuffer);
	SafeRelease(m_pVertexBuffer);

	LogInfo("<li>Sphere destroyed OK");
}

DWORD CSphere::Render()
{
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(SPHERE_CUSTOMVERTEX));
	m_pD3DDevice->SetFVF(SPHERE_D3DFVF_CUSTOMVERTEX);

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
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_dwNumOfVertices, 0, m_dwNumOfPolygons);
	
	m_pD3DDevice->SetTexture(0, 0);
	m_pD3DDevice->SetStreamSource(0, 0, 0, 0);
	m_pD3DDevice->SetIndices(0);

	//Return the number of polygons rendered
	return m_dwNumOfPolygons;
}

bool CSphere::CreateVertexBuffer()
{
    //Create the vertex buffer from our device.
    if(FAILED(m_pD3DDevice->CreateVertexBuffer(m_dwNumOfVertices * sizeof(SPHERE_CUSTOMVERTEX),
                                               0, SPHERE_D3DFVF_CUSTOMVERTEX,
                                               D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL)))
    {
		LogError("<li>CSphere: Unable to create vertex buffer.");
        return false;
    }

    return true;
}

bool CSphere::CreateIndexBuffer()
{
	//Create the index buffer from our device
	if(FAILED(m_pD3DDevice->CreateIndexBuffer(m_dwNumOfIndices * sizeof(WORD), 
											  0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
											  &m_pIndexBuffer, NULL)))
	{
		LogError("<li>CSphere: Unable to create index buffer.");
	    return false;
	}

	return true;
}

bool CSphere::UpdateVertices()
{
	//Code adapted from a sample by "Laurent" posted on the GameDev.net DirectX forum
	//http://www.gamedev.net/community/forums/topic.asp?topic_id=85779

	WORD* pIndices;
	SPHERE_CUSTOMVERTEX* pVertex; 
	WORD wVertexIndex = 0;
	int nCurrentRing;
	int nCurrentSegment;
	D3DXVECTOR3 vNormal;

	//Lock the vertex buffer
	if(FAILED(m_pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0)))
	{
		LogError("<li>CSphere: Unable to lock vertex buffer.");
		return false;
	}

	//Lock the index buffer 
	if(FAILED(m_pIndexBuffer->Lock(0, m_dwNumOfIndices, (void**)&pIndices, 0)))
	{
		LogError("<li>CSphere: Unable to lock index buffer.");
		return false;
	}

	//Establish constants used in sphere generation
	FLOAT rDeltaRingAngle = (D3DX_PI / m_nRings);
	FLOAT rDeltaSegAngle = (2.0f * D3DX_PI / m_nSegments);

	//Generate the group of rings for the sphere
	for(nCurrentRing = 0; nCurrentRing < m_nRings + 1; nCurrentRing++)
	{
		FLOAT r0 = sinf(nCurrentRing * rDeltaRingAngle);
		FLOAT y0 = cosf(nCurrentRing * rDeltaRingAngle);

		//Generate the group of segments for the current ring
		for(nCurrentSegment = 0; nCurrentSegment < m_nSegments + 1; nCurrentSegment++)
		{
			FLOAT x0 = r0 * sinf(nCurrentSegment * rDeltaSegAngle);
			FLOAT z0 = r0 * cosf(nCurrentSegment * rDeltaSegAngle);

			vNormal.x = x0;
			vNormal.y = y0;
			vNormal.z = z0;
	
			D3DXVec3Normalize(&vNormal, &vNormal);

			//Add one vertex to the strip which makes up the sphere
			pVertex->x = x0;
			pVertex->y = y0;
			pVertex->z = z0;
			pVertex->nx = vNormal.x;
			pVertex->ny = vNormal.y;
			pVertex->nz = vNormal.z;
			pVertex->tu = 1.0f - ((FLOAT)nCurrentSegment / (FLOAT)m_nSegments);
			pVertex->tv = (FLOAT)nCurrentRing / (FLOAT)m_nRings;

			pVertex++;
			
			//Add two indices except for the last ring 
			if(nCurrentRing != m_nRings) 
			{
				*pIndices = wVertexIndex; 
				pIndices++;
				
				*pIndices = wVertexIndex + (WORD)(m_nSegments + 1); 
				pIndices++;
				
				wVertexIndex++; 
			}
		}
	}

	if(FAILED(m_pIndexBuffer->Unlock()))
	{
		LogError("<li>CSphere: Unable to unlock index buffer.");
		return false;
	}

	if(FAILED(m_pVertexBuffer->Unlock()))
	{
		LogError("<li>CSphere: Unable to unlock vertex buffer.");
		return false;
	}

	return true; 
}

bool CSphere::SetTexture(const char *szTextureFilePath)
{
	if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, szTextureFilePath, &m_pTexture)))
	{
		return false;
	}

	return true;
}

bool CSphere::SetMaterial(D3DCOLORVALUE rgbaDiffuse, D3DCOLORVALUE rgbaAmbient, D3DCOLORVALUE rgbaSpecular, D3DCOLORVALUE rgbaEmissive, float rPower)
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




