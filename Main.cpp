#include "Game.h"

CGame* g_pGame = NULL;

void CleanUp()
{
	SafeDelete(g_pGame);
}

//The windows message handler
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        break;
        case WM_KEYUP: 
            switch (wParam)
            { 
                case VK_ESCAPE:
                    //User has pressed the escape key, so quit
                    DestroyWindow(hWnd);
                    return 0;
                break;
            } 
        break;

    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

//Application entry point
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    //Register the window class
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WinProc, 0L, 0L, 
                     GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                     "DX Project 10", NULL};
	
	//Set the mouse pointer to an arrow
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);	
	
	RegisterClassEx(&wc);

    //Create the application's window
    HWND hWnd = CreateWindow("DX Project 10", " DFS and A*", 
                              WS_OVERLAPPEDWINDOW, 0, 0, 1100, 700,
                              NULL, NULL, wc.hInstance, NULL);

	//Show our window
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);
	
	g_pGame = new CGame();
    g_pGame->StartLogging();

	//Initialize Direct3D
	if(g_pGame->Initialise(hWnd, 800, 600))
    { 
        //Start game running: Enter the game loop
		g_pGame->GameLoop();
    }
    
    CleanUp();

    UnregisterClass("DX Project 10", wc.hInstance);
    
    return 0;
}


