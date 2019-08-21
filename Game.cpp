// Game.cpp: implementation of the CGame class.
//
//////////////////////////////////////////////////////////////////////

#include "Game.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

POINT  g_ptLastMousePosit;
POINT  g_ptCurrentMousePosit;
bool   g_bMousing = false;
float  g_fMoveSpeed = 25.0f;
float  g_fElpasedTime;
double g_dCurTime;
double g_dLastTime;

D3DXVECTOR3	g_vEye(110.0f, 300.0f, 280.0f);    // Eye Position
D3DXVECTOR3	g_vLook(0.0f, -1.0f, -0.5f);  // Look Vector
D3DXVECTOR3	g_vUp(0.0f, 1.0f, 0.0f);      // Up Vector
D3DXVECTOR3	g_vRight(1.0f, 0.0f, 0.0f);   // Right Vector

void getRealTimeUserInput( HWND hWnd );
void updateViewMatrix( LPDIRECT3DDEVICE9 m_pD3DDevice );


CGame::CGame()
{
	m_pD3D = NULL;
	m_pD3DDevice = NULL;
	m_dwFrames = 0;
	m_dwStartTime = 0;
	m_dwEndTime = 0;
	m_dwTotalPolygons = 0;

	m_pMesh1 = NULL;
	m_pMesh2 = NULL;
	m_pMesh3 = NULL;
	m_pMaze1 = NULL;
	
}

CGame::~CGame()
{
	//Game finished, so destroy game objects
	LogInfo("<br>Finish Game:");

	SafeDelete(m_pMesh1);
	SafeDelete(m_pMesh2);
	SafeDelete(m_pMesh3);

	SafeRelease(m_pD3DDevice);
	SafeRelease(m_pD3D);

	//Game finished, so save statistics to log
	DWORD dwDuration = (m_dwEndTime - m_dwStartTime) / 1000;
	
	if((dwDuration != 0)&&(m_dwFrames != 0))
	{
		//Log stats
		LogInfo("<br>Statistics:");
		LogInfo("<li>Start Time (ms): %d", m_dwStartTime);
		LogInfo("<li>End Time (ms): %d", m_dwEndTime);
		LogInfo("<li>Duration (s): %d", dwDuration);
		LogInfo("<li>Total Frame Count: %d", m_dwFrames);
		LogInfo("<li>Average FPS: %d", (m_dwFrames / dwDuration));
		LogInfo("<li>Total Polygons: %d", m_dwTotalPolygons);
		LogInfo("<li>Average Polygons per Frame: %d", (m_dwTotalPolygons / m_dwFrames));
	}
	else
	{
		LogInfo("<br>No statistics to report");
	}

	StopLogging();
}

bool CGame::Initialise(HWND hWnd, UINT nWidth, UINT nHeight)
{
	if(InitialiseD3D(hWnd, nWidth, nHeight))
	{
		if(InitialiseLights()) 
		{
			return InitialiseGame();
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
		
	return true;
}

bool CGame::InitialiseGame()
{
	LogInfo("<br>Initialise Game:");

	//Setup games objects here
	m_pMesh1 = new CMesh(m_pD3DDevice, "Spaceship01.x");
	m_pMesh2 = new CMesh(m_pD3DDevice, "Spaceship01.x");
	m_pMesh3 = new CMesh(m_pD3DDevice, "Spaceship01.x");
	m_pMaze1 = new CMaze(m_pD3DDevice);

	return true;
}

D3DFORMAT CGame::CheckDisplayMode(UINT nWidth, UINT nHeight, UINT nDepth)
{
	UINT x;
	D3DDISPLAYMODE d3ddm;

	for(x = 0; x < m_pD3D->GetAdapterModeCount(0, D3DFMT_X8R8G8B8); x++)
	{
		m_pD3D->EnumAdapterModes(0, D3DFMT_X8R8G8B8, x, &d3ddm);
		if(d3ddm.Width == nWidth)
		{
			if(d3ddm.Height == nHeight)
			{
				if((d3ddm.Format == D3DFMT_R5G6B5) || (d3ddm.Format == D3DFMT_X1R5G5B5) || (d3ddm.Format == D3DFMT_X4R4G4B4))
				{
					if(nDepth == 16)
					{
						return d3ddm.Format;
					}
				}
				else if((d3ddm.Format == D3DFMT_R8G8B8) || (d3ddm.Format == D3DFMT_X8R8G8B8))
				{
					if(nDepth == 32)
					{
						return d3ddm.Format;
					}
				}
			}
		}
	}

	return D3DFMT_UNKNOWN;
}

bool CGame::InitialiseD3D(HWND hWnd, UINT nWidth, UINT nHeight)
{
	LogInfo("<br>Initialise Direct3D:");

    //First of all, create the main D3D object. If it is created successfully we 
    //should get a pointer to an IDirect3D8 interface.
    m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if(m_pD3D == NULL)
    {
		LogError("<li>Unable to create DirectX8 interface.");
        return false;
    }

    //Get the current display mode
    D3DDISPLAYMODE d3ddm;

	d3ddm.Format = CheckDisplayMode(nWidth, nHeight, 32);
	if(d3ddm.Format != D3DFMT_UNKNOWN)
	{
		//Width x Height x 32bit has been selected
		d3ddm.Width = nWidth;
		d3ddm.Height = nHeight;

		LogInfo("<li>%d x %d x 32bit back buffer format selected. Format = %d.", nWidth, nHeight, d3ddm.Format);
	}
	else
	{
		d3ddm.Format = CheckDisplayMode(nWidth, nHeight, 16);
		if(d3ddm.Format != D3DFMT_UNKNOWN)
		{
            //Width x Height x 16bit has been selected
			d3ddm.Width = nWidth;
			d3ddm.Height = nHeight;

			LogInfo("<li>%d x %d x 16bit back buffer format selected. Format = %d.", nWidth, nHeight, d3ddm.Format);
		}
        else
		{
			LogError("<li>Unable to select back buffer format for %d x %d.", nWidth, nHeight);
            return false;
        }
	}


    //Create a structure to hold the settings for our device
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.BackBufferWidth = d3ddm.Width;
    d3dpp.BackBufferHeight = d3ddm.Height;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	//Select the best depth buffer, select 32, 24 or 16 bit
    if(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32) == D3D_OK)
	{
        d3dpp.AutoDepthStencilFormat = D3DFMT_D32;
        d3dpp.EnableAutoDepthStencil = TRUE;

		LogInfo("<li>32bit depth buffer selected");
    }
    else if(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8) == D3D_OK)
    {
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
        d3dpp.EnableAutoDepthStencil = TRUE;

		LogInfo("<li>24bit depth buffer selected");
	}
    else if(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3ddm.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16) == D3D_OK)
    {
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        d3dpp.EnableAutoDepthStencil = TRUE;

		LogInfo("<li>16bit depth buffer selected");
	}
    else
	{
        d3dpp.EnableAutoDepthStencil = FALSE;
		LogError("<li>Unable to select depth buffer.");
	}


    //Create a Direct3D device.
    if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
                                   D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice)))
    {
		LogError("<li>Unable to create device.");
        return false;
    }
    
	//Turn on back face culling. This is becuase we want to hide the back of our polygons
	if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW)))
	{
		LogError("<li>SetRenderState: D3DRS_CULLMODE Failed");
		return false;
	}
	else
	{
		LogInfo("<li>SetRenderState: D3DRS_CULLMODE OK");
	}


	//Turn on Depth Buffering
	if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE)))
	{
		LogError("<li>SetRenderState: D3DRS_ZENABLE Failed");
		return false;
	}
	else
	{
		LogInfo("<li>SetRenderState: D3DRS_ZENABLE OK");
	}


	//Set fill state. Possible values: D3DFILL_POINT, D3DFILL_WIREFRAME, D3DFILL_SOLID
	if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID)))
	{
		LogError("<li>SetRenderState: D3DRS_FILLMODE Failed");
		return false;
	}
	else
	{
		LogInfo("<li>SetRenderState: D3DRS_FILLMODE OK");
	}

	//Set the D3DRS_NORMALIZENORMALS render state to fix the problem when scaling the objects get darker
	if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE)))
	{
		LogError("<li>SetRenderState: D3DRS_NORMALIZENORMALS Failed");
		return false;
	}
	else
	{
		LogInfo("<li>SetRenderState: D3DRS_NORMALIZENORMALS OK");
	}

    return true;
}

bool CGame::InitialiseLights()
{
	LogInfo("<br>Initialise Lights:");

	D3DLIGHT9 d3dLight;

	//Initialize the light structure.
	ZeroMemory(&d3dLight, sizeof(D3DLIGHT9));

	d3dLight.Type = D3DLIGHT_POINT;
	
	d3dLight.Position.x = 0.0f;
	d3dLight.Position.y = 10.0f;
	d3dLight.Position.z = 0.0f;

	d3dLight.Attenuation0 = 1.0f; 
	d3dLight.Attenuation1 = 0.0f; 
	d3dLight.Attenuation2 = 0.0f; 
	d3dLight.Range = 1000.0f;	

	d3dLight.Diffuse.r = 1.0f;
	d3dLight.Diffuse.g = 1.0f;
	d3dLight.Diffuse.b = 1.0f;
	
	d3dLight.Ambient.r = 0.0f;
	d3dLight.Ambient.g = 0.0f;
	d3dLight.Ambient.b = 0.0f;
	
	d3dLight.Specular.r = 0.0f;
	d3dLight.Specular.g	= 0.0f;
	d3dLight.Specular.b	= 0.0f;

	//Assign the point light to our device in poisition (index) 0
	if(FAILED(m_pD3DDevice->SetLight(0, &d3dLight)))
	{
		LogError("<li>SetLight Failed");
		return false;
	}
	else
	{
		LogInfo("<li>SetLight OK");
	}

	//Enable our point light in position (index) 0
	if(FAILED(m_pD3DDevice->LightEnable(0, TRUE)))
	{
		LogError("<li>LightEnable Failed");
		return false;
	}
	else
	{
		LogInfo("<li>LightEnable OK");
	}

	//Turn on lighting
    if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE)))
	{
		LogError("<li>SetRenderState: D3DRS_LIGHTING Failed");
		return false;
	}
	else
	{
		LogInfo("<li>SetRenderState: D3DRS_LIGHTING OK");
	}

	//Set ambient light level
	if(FAILED(m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(60, 60, 60))))	
	{
		LogError("<li>SetRenderState: D3DRS_AMBIENT Failed");
		return false;
	}
	else
	{
		LogInfo("<li>SetRenderState: D3DRS_AMBIENT OK");
	}

	return true;
}

LPDIRECT3DDEVICE9 CGame::GetDevice()
{
	return m_pD3DDevice;
}

void CGame::GameLoop()
{
    //Enter the game loop
    MSG msg; 
    BOOL fMessage;

    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	//Game started, so record time
	m_dwStartTime = timeGetTime();

    while(msg.message != WM_QUIT)
    {
        fMessage = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);

		
		if(msg.message == WM_LBUTTONDOWN)
		{
			g_bMousing = true;
		}
		else if(msg.message == WM_LBUTTONUP)
		{
			g_bMousing = false;
		}

		else if(msg.message == WM_RBUTTONDOWN)
		{
			m_pMaze1->ReMake();
		}
		



        if(fMessage)
        {
            //Process message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            g_dCurTime     = timeGetTime();
			g_fElpasedTime = (float)((g_dCurTime - g_dLastTime) * 0.001);
			g_dLastTime    = g_dCurTime;
			//No message to process, so render the current scene
            Render();
        }
    }

	//Game finished, so record time
	m_dwEndTime = timeGetTime();
}

void CGame::Render()
{
	D3DXMATRIX matMove1, matMove3;
	D3DXMATRIX matShip1, matShip2, matShip3;
	D3DXMATRIX matRotateX, matRotateY, matRotateZ, matScale1, matScale2;

	if(m_pD3DDevice == NULL)
    {
        return;
    }

    //Clear the back buffer and depth buffer
    m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    
    //Begin the scene
    m_pD3DDevice->BeginScene();
    
	//Setup camera and perspective - no longer needed
	//SetupCamera();
	getRealTimeUserInput(hWnd);
	updateViewMatrix(m_pD3DDevice);

	//Here we specify the field of view, aspect ration and near and far clipping planes.
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 1.25f, 1.0f, 2000.0f);
    m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	
	//Create the transformation matrices
	D3DXMatrixRotationX(&matRotateX, timeGetTime()/1000.0f);
	D3DXMatrixRotationY(&matRotateY, timeGetTime()/1000.0f);
	D3DXMatrixRotationZ(&matRotateZ, timeGetTime()/1000.0f);
	D3DXMatrixScaling(&matScale1, 2.0f, 2.0f, 2.0f);
	D3DXMatrixScaling(&matScale2, 0.5f, 0.5f, 0.5f);
		
	D3DXMatrixTranslation(&matMove1, -20.0f, 15.0f, 0.0f);
	D3DXMatrixTranslation(&matMove3, 20.0f, 15.0f, 0.0f);
	
	D3DXMatrixMultiply(&matShip1, &matScale2, &matRotateX);
	D3DXMatrixMultiply(&matShip2, &matScale1, &matRotateY);
	D3DXMatrixMultiply(&matShip3, &matScale2, &matRotateZ);

	D3DXMatrixMultiply(&matShip1, &matShip1, &matMove1);
	D3DXMatrixMultiply(&matShip3, &matShip3, &matMove3);


	//Render our objects
	/*m_pD3DDevice->SetTransform(D3DTS_WORLD, &matShip1);
	m_dwTotalPolygons += m_pMesh1->Render();

	m_pD3DDevice->SetTransform(D3DTS_WORLD, &matShip2);
	m_dwTotalPolygons += m_pMesh2->Render();

	m_pD3DDevice->SetTransform(D3DTS_WORLD, &matShip3);
	m_dwTotalPolygons += m_pMesh3->Render();*/

	m_pMaze1->Render(m_pD3DDevice);


    //End the scene
    m_pD3DDevice->EndScene();
    
    //Filp the back and front buffers so that whatever has been rendered on the back buffer
    //will now be visible on screen (front buffer).
    m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
	
	//Count Frames
	m_dwFrames++;
}

void CGame::SetupCamera()
{
	//Here we will setup the camera.
	//The camera has three settings: "Camera Position", "Look at Position" and "Up Direction"
	//We have set the following:
	//Camera Position:	(0, 15, -50)
	//Look at Position: (0, 0, 0)
	//Up direction:		Y-Axis.
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(50.0f, 50.0f, -50.0f),		//Camera Position
                                 &D3DXVECTOR3(0.0f, 0.0f, 0.0f),		//Look At Position
                                 &D3DXVECTOR3(0.0f, 1.0f, 0.0f));		//Up Direction
    m_pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

	//Here we specify the field of view, aspect ration and near and far clipping planes.
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 1.25f, 1.0f, 2000.0f);
    m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

//------------------------------------------------------------------------------
// Name: getRealTimeUserInput()
// Desc: 
//------------------------------------------------------------------------------
void getRealTimeUserInput( HWND hWnd )
{
	//
	// Get mouse input...
	//

	POINT mousePosit;
	GetCursorPos( &mousePosit );
	ScreenToClient( hWnd, &mousePosit );

    g_ptCurrentMousePosit.x = mousePosit.x;
    g_ptCurrentMousePosit.y = mousePosit.y;

	D3DXMATRIX matRotation;

    if( g_bMousing )
    {
		int nXDiff = (g_ptCurrentMousePosit.x - g_ptLastMousePosit.x);
        int nYDiff = (g_ptCurrentMousePosit.y - g_ptLastMousePosit.y);
        
        if( nYDiff != 0 )
		{
			D3DXMatrixRotationAxis( &matRotation, &g_vRight, D3DXToRadian((float)nYDiff / 3.0f));
			D3DXVec3TransformCoord( &g_vLook, &g_vLook, &matRotation );
			D3DXVec3TransformCoord( &g_vUp, &g_vUp, &matRotation );
		}

		if( nXDiff != 0 )
		{
			D3DXMatrixRotationAxis( &matRotation, &D3DXVECTOR3(0,1,0), D3DXToRadian((float)nXDiff / 3.0f) );
			D3DXVec3TransformCoord( &g_vLook, &g_vLook, &matRotation );
			D3DXVec3TransformCoord( &g_vUp, &g_vUp, &matRotation );
		}
    }

    g_ptLastMousePosit.x = g_ptCurrentMousePosit.x;
    g_ptLastMousePosit.y = g_ptCurrentMousePosit.y;

	//
	// Get keyboard input...
	//

	unsigned char keys[256];
	GetKeyboardState( keys );
    
    D3DXVECTOR3 tmpLook  = g_vLook;
	D3DXVECTOR3 tmpRight = g_vRight;

	// Up Arrow Key - View moves forward
	if( keys[VK_UP] & 0x80 )
		g_vEye -= tmpLook*-g_fMoveSpeed*g_fElpasedTime;

	// Down Arrow Key - View moves backward
	if( keys[VK_DOWN] & 0x80 )
		g_vEye += (tmpLook*-g_fMoveSpeed)*g_fElpasedTime;

	// Left Arrow Key - View side-steps or strafes to the left
	if( keys[VK_LEFT] & 0x80 )
		g_vEye -= (tmpRight*g_fMoveSpeed)*g_fElpasedTime;

	// Right Arrow Key - View side-steps or strafes to the right
	if( keys[VK_RIGHT] & 0x80 )
		g_vEye += (tmpRight*g_fMoveSpeed)*g_fElpasedTime;

	// Home Key - View elevates up
	if( keys[VK_HOME] & 0x80 )
		g_vEye.y += g_fMoveSpeed*g_fElpasedTime; 

	// End Key - View elevates down
	if( keys[VK_END] & 0x80 )
		g_vEye.y -= g_fMoveSpeed*g_fElpasedTime;


}

void updateViewMatrix( LPDIRECT3DDEVICE9 m_pD3DDevice )
{
	D3DXMATRIX view;
	D3DXMatrixIdentity( &view );

	D3DXVec3Normalize( &g_vLook, &g_vLook );
	D3DXVec3Cross( &g_vRight, &g_vUp, &g_vLook );
	D3DXVec3Normalize( &g_vRight, &g_vRight );
	D3DXVec3Cross( &g_vUp, &g_vLook, &g_vRight );
	D3DXVec3Normalize( &g_vUp, &g_vUp );

	view._11 = g_vRight.x;
    view._12 = g_vUp.x;
    view._13 = g_vLook.x;
	view._14 = 0.0f;

	view._21 = g_vRight.y;
    view._22 = g_vUp.y;
    view._23 = g_vLook.y;
	view._24 = 0.0f;

	view._31 = g_vRight.z;
    view._32 = g_vUp.z;
    view._33 = g_vLook.z;
	view._34 = 0.0f;

	view._41 = -D3DXVec3Dot( &g_vEye, &g_vRight );
	view._42 = -D3DXVec3Dot( &g_vEye, &g_vUp );
	view._43 = -D3DXVec3Dot( &g_vEye, &g_vLook );
	view._44 =  1.0f;

	m_pD3DDevice->SetTransform( D3DTS_VIEW, &view ); 
}