// Cone.cpp: implementation of the CCone class.
//
//////////////////////////////////////////////////////////////////////

#include "Cone.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCone::CCone(LPDIRECT3DDEVICE9 pD3DDevice, float rHeight, float rRadius, int nSegments)
{
	m_pD3DDevice = pD3DDevice;
	m_pVertexBuffer = NULL;
	m_pSideTexture = NULL;
	m_pEndTexture = NULL;
	
	m_rHeight = rHeight;
	m_rRadius = rRadius;
	m_nSegments = nSegments;

	//Setup counts for this object
	m_dwNumOfVertices = (m_nSegments * 3) + 2;
	m_dwNumOfIndices = m_nSegments * 3;
	m_dwNumOfSidePolygons = m_nSegments;
	m_dwNumOfEndPolygons = m_nSegments;

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
				LogInfo("<li>Cone created OK");
				return;
			}
		}
	}

	LogError("<li>Cone failed to create");
}

CCone::~CCone()
{
	SafeRelease(m_pSideTexture);
	SafeRelease(m_pEndTexture);
	SafeRelease(m_pIndexBuffer);
	SafeRelease(m_pVertexBuffer);

	LogInfo("<li>Cone destroyed OK");
}

DWORD CCone::Render()
{
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(CONE_CUSTOMVERTEX));
	m_pD3DDevice->SetFVF(CONE_D3DFVF_CUSTOMVERTEX);

	//Select the side texture
	if(m_pSideTexture != NULL)
	{
		//A texture has been set. We want our texture to be shaded based
		//on the current light levels, so used D3DTOP_MODULATE.
		m_pD3DDevice->SetTexture(0, m_pSideTexture);
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

	//Render polygons from vertex buffer - Sides
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (m_nSegments * 3), 0, m_dwNumOfSidePolygons);
	
	m_pD3DDevice->SetTexture(0, 0); 
	m_pD3DDevice->SetIndices(0);

	//Select the end texture
	if(m_pEndTexture != NULL)
	{
		//A texture has been set. We want our texture to be shaded based
		//on the current light levels, so used D3DTOP_MODULATE.
		m_pD3DDevice->SetTexture(0, m_pEndTexture);
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

	//Render polygons from vertex buffer - End
	m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, (m_nSegments * 2), m_dwNumOfEndPolygons);
	
	m_pD3DDevice->SetTexture(0, 0);
	m_pD3DDevice->SetStreamSource(0, 0, 0, 0);

	//Return the number of polygons rendered
	return m_dwNumOfSidePolygons + m_dwNumOfEndPolygons;
}

bool CCone::CreateVertexBuffer()
{
    //Create the vertex buffer from our device.
    if(FAILED(m_pD3DDevice->CreateVertexBuffer(m_dwNumOfVertices * sizeof(CONE_CUSTOMVERTEX),
                                               0, CONE_D3DFVF_CUSTOMVERTEX,
                                               D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL)))
    {
        LogError("<li>CCone: Unable to create vertex buffer.");
		return false;
    }

    return true;
}

bool CCone::CreateIndexBuffer()
{
	//Create the index buffer from our device
	if(FAILED(m_pD3DDevice->CreateIndexBuffer(m_dwNumOfIndices * sizeof(WORD), 
											  0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
											  &m_pIndexBuffer, NULL)))
	{
		LogError("<li>CCone: Unable to create index buffer.");
	    return false;
	}

	return true;
}

bool CCone::UpdateVertices()
{
	CONE_CUSTOMVERTEX* pVertex;
	WORD* pIndices;
	WORD wVertexIndex = 0;
	int nCurrentSegment;

	//Lock the vertex buffer
	if(FAILED(m_pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0)))
	{
		LogError("<li>CCone: Unable to lock vertex buffer.");
		return false;
	}

	//Lock the index buffer 
	if(FAILED(m_pIndexBuffer->Lock(0, m_dwNumOfIndices, (void**)&pIndices, 0)))
	{
		LogError("<li>CCone: Unable to lock index buffer.");
		return false;
	}

	float rDeltaSegAngle = (2.0f * D3DX_PI / m_nSegments);
	float rSegmentLength = 1.0f / (float)m_nSegments;
	float ny0 = (90.0f - (float)D3DXToDegree(atan(m_rHeight / m_rRadius))) / 90.0f;

	//For each segment, add a triangle to the sides triangle list
	for(nCurrentSegment = 0; nCurrentSegment < m_nSegments; nCurrentSegment++)
	{
		float x0 = m_rRadius * sinf(nCurrentSegment * rDeltaSegAngle);
		float z0 = m_rRadius * cosf(nCurrentSegment * rDeltaSegAngle);

		pVertex->x = 0.0f;
		pVertex->y = 0.0f + (m_rHeight / 2.0f);
		pVertex->z = 0.0f;
		pVertex->nx = x0;
		pVertex->ny = ny0;
		pVertex->nz = z0;
		pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
		pVertex->tv = 0.0f;
		pVertex++;

		pVertex->x = x0;
		pVertex->y = 0.0f - (m_rHeight / 2.0f);
		pVertex->z = z0;
		pVertex->nx = x0;
		pVertex->ny = ny0;
		pVertex->nz = z0;
		pVertex->tu = 1.0f - (rSegmentLength * (float)nCurrentSegment);
		pVertex->tv = 1.0f;
		pVertex++;

		//Set three indices (1 triangle) per segment
		*pIndices = wVertexIndex; 
		pIndices++;
		wVertexIndex++;
		
		*pIndices = wVertexIndex; 
		pIndices++;
		wVertexIndex += 2;

		if(nCurrentSegment == m_nSegments - 1)
		{
			*pIndices = 1; 
			pIndices++;
			wVertexIndex--;
		}
		else
		{
			*pIndices = wVertexIndex; 
			pIndices++;
			wVertexIndex--;	
		}
	}

	//Create the bottom triangle fan: Center vertex
	pVertex->x = 0.0f;
	pVertex->y = 0.0f - (m_rHeight / 2.0f);
	pVertex->z = 0.0f;
	pVertex->nx = 0.0f;
	pVertex->ny = -1.0f;
	pVertex->nz = 0.0f;
	pVertex->tu = 0.5f;
	pVertex->tv = 0.5f;
	pVertex++;

	//Create the bottom triangle fan: Edge vertices
	for(nCurrentSegment = m_nSegments; nCurrentSegment >= 0; nCurrentSegment--)
	{
		float x0 = m_rRadius * sinf(nCurrentSegment * rDeltaSegAngle);
		float z0 = m_rRadius * cosf(nCurrentSegment * rDeltaSegAngle);
	
		pVertex->x = x0;
		pVertex->y = 0.0f - (m_rHeight / 2.0f);
		pVertex->z = z0;
		pVertex->nx = 0.0f;
		pVertex->ny = -1.0f;
		pVertex->nz = 0.0f;

		float tu0 = (0.5f * sinf(nCurrentSegment * rDeltaSegAngle)) + 0.5f;
		float tv0 = (0.5f * cosf(nCurrentSegment * rDeltaSegAngle)) + 0.5f;

		pVertex->tu = tu0;
		pVertex->tv = tv0;
		pVertex++;
	}

	if(FAILED(m_pVertexBuffer->Unlock()))
	{
		LogError("<li>CCone: Unable to unlock vertex buffer.");
		return false;
	}

	if(FAILED(m_pIndexBuffer->Unlock()))
	{
		LogError("<li>CCone: Unable to unlock index buffer.");
		return false;
	}

	return true; 
}

bool CCone::SetSideTexture(const char *szTextureFilePath)
{
	if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, szTextureFilePath, &m_pSideTexture)))
	{
		return false;
	}

	return true;
}

bool CCone::SetEndTexture(const char *szTextureFilePath)
{
	if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, szTextureFilePath, &m_pEndTexture)))
	{
		return false;
	}

	return true;
}

bool CCone::SetMaterial(D3DCOLORVALUE rgbaDiffuse, D3DCOLORVALUE rgbaAmbient, D3DCOLORVALUE rgbaSpecular, D3DCOLORVALUE rgbaEmissive, float rPower)
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




