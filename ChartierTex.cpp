// include the basic windows header files and the Direct3D header file
#include "windows.h"
#include "mmsystem.h"
#include <windowsx.h>
#include <d3d9.h>
#include <stdio.h>
//#include <d3dx9.h>
#include "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\include\\d3dx9.h"
// include the Direct3D Library files
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\lib\\x86\\d3dx9.lib")
#pragma comment(lib, "winmm.lib")

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH 1900
#define SCREEN_HEIGHT 1200
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define timerId 120

// global declarations
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;
LPDIRECT3DTEXTURE9 texture = NULL; // our first texture
LPDIRECT3DTEXTURE9 mBox = NULL; // our first texture
LPDIRECT3DTEXTURE9 mBox2 = NULL; // our first texture
LPDIRECT3DTEXTURE9 fBox = NULL; // our first texture
LPDIRECT3DTEXTURE9 pole = NULL; // our first texture
LPDIRECT3DTEXTURE9 poleTop = NULL; // our first texture
LPDIRECT3DTEXTURE9 thwomp = NULL; // our first texture
LPDIRECT3DTEXTURE9 charTex = NULL; // our first texture
LPDIRECT3DTEXTURE9 winTex = NULL; // our first texture
LPDIRECT3DTEXTURE9 fireBallTex = NULL; // our first texture
LPD3DXMESH pTempSunMesh = NULL;
float cameraX, cameraY, cameraZ;
float tX, tY, tZ; //Thwomps location
float charx, chary, charz;
float jumt = 20.0f;//used for jumping
bool isJumping = false;
float jumpBaseline = 5.0f;//where to jump from.
float block1X, block1Y, block1Z;
int numBlock1Hits = 0;
bool block1Hit = false;
bool notWon = true;
bool showWin = false;


int heightScreen, widthScreen;

TCHAR winStr[] = TEXT("Winner");

//fireball variables
float leftEnd = -5.0f;
float rightEnd = 20.0f;
bool goingLeft = false;
bool goingRight = true;
float winboxX, winboxY, winboxZ;
float fireballSpeedFactor;
float fireballSX = 0.25f;
float fireballSY = 0.25f;

float fireballX, fireballY, fireballZ;




// function prototypes
void initD3D(HWND hWnd);
void render_frame(void);
void cleanD3D(void);
void init_graphics(void);
void init_light(void); // sets up the light and the material
void jump(void);

struct CUSTOMVERTEX { FLOAT X, Y, Z; D3DVECTOR NORMAL; FLOAT U, V; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	widthScreen = GetSystemMetrics(SM_CXSCREEN);
	heightScreen = GetSystemMetrics(SM_CYSCREEN);
	PlaySound(TEXT("theme.wav"), NULL, SND_ASYNC | SND_NOSTOP | SND_LOOP);
	cameraX = 0.0f;
	cameraY = 8.0f;
	cameraZ = -40.0f;
	charx = 20.0f;
	chary = 1.0f;
	charz = 0.0f;
	tX = 14.0f;
	tY = 4.0f;
	tZ = 0.0f;
	block1X = 9.0f;
	block1Y = 6.0f;
	block1Z = 0.0f;
	//-19.90f, 12.0f, 0.0f
	winboxX = -20.0f;
	winboxY = 1.0f;
	winboxZ = 0.0f;

	fireballX = -5.0f;
	fireballY = 5.0f;
	fireballZ = 0.0f;

fireballSpeedFactor = (winboxY / 11.0f) + 1.0f;;//used to increase fireball speed;

HWND hWnd;
WNDCLASSEX wc;




ZeroMemory(&wc, sizeof(WNDCLASSEX));

wc.cbSize = sizeof(WNDCLASSEX);
wc.style = CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc = WindowProc;
wc.hInstance = hInstance;
wc.hCursor = LoadCursor(NULL, IDC_ARROW);
wc.lpszClassName = L"WindowClass";

RegisterClassEx(&wc);

hWnd = CreateWindowEx(NULL, L"WindowClass", L"Simple Game",
	WS_OVERLAPPEDWINDOW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
	NULL, NULL, hInstance, NULL);

SetTimer(hWnd, timerId, 30, NULL);//timer

ShowWindow(hWnd, nCmdShow);

// set up and initialize Direct3D
initD3D(hWnd);

// enter the main loop:

MSG msg;
while (TRUE)
{
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
		break;

	render_frame();

	if (KEY_DOWN(VK_ESCAPE))
		PostMessage(hWnd, WM_DESTROY, 0, 0);
}

// clean up DirectX and COM
cleanD3D();

return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
	case WM_PAINT:
	{
	}
	break;
	case WM_TIMER:
	{
		fireballSpeedFactor = ((14 * winboxY) / 11.0f) + 1.0f;;//used to increase fireball speed;
		if (winboxY > 11.0f)
		{
			notWon = false;
		}
		if ((winboxY > 1.0f) && (notWon))
		{
			winboxY -= 0.003f;
		}
		if (fireballX >= rightEnd)
		{
			goingRight = false;
			goingLeft = true;
		}
		if (fireballX <= leftEnd)
		{
			goingRight = true;
			goingLeft = false;
		}

		if ((goingRight) && (fireballX < rightEnd))
		{
			fireballX += 0.1f * fireballSpeedFactor;
		}
		if ((goingLeft) && (fireballX > leftEnd))
		{
			fireballX -= 0.1f * fireballSpeedFactor;
		}

		//check for collisions

		float charleftX = charx - 0.5f;
		float charrightX = charx + 0.5f;
		float charUpperY = chary + 0.5f;
		float charLowerY = chary - 0.5f;

		if ((fireballX < charrightX) && (fireballX > charleftX))//it got hit
		{
			if ((fireballY < charUpperY) && (fireballY > charLowerY))//got hit
			{
				winboxY /= 2.0f;
				if (winboxY < 1.0f)
				{
					winboxY = 1.0f;
				}
			}
		}
		if (!notWon)
		{
			fireballX = 0.0f;
			fireballY = 0.0f;
			if ((fireballSX < 50.0f) && (fireballSY < 50.0))
			{
				fireballSX++;
				fireballSY++;
			}
			
		}

		
	}
	
		break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	case WM_KEYDOWN:
		switch (wParam)
		{
			case VK_SPACE:
				isJumping = true;
				break;
			case VK_RIGHT:
				if (charx < 20.0f)
				{
					charx += 1.0f;
				}
				
				break;
			case VK_LEFT:
				if (charx > -5.0f)
				{
					charx += -1.0f;
				}
				
				break;
		}
		break;
	/*case WM_KEYUP:
		switch (wParam)
		{
		case VK_SPACE: 
			isJumping = true;
			break;
		case VK_RIGHT:
			charx += 1.0f;
			break;
		case VK_LEFT:
			charx += -1.0f;
			break;
		}
		break;*/
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	init_graphics(); // call the function to initialize the cube
	init_light(); // call the function to initialize the light and material

	d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE); // turn on the 3D lighting
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE); // turn on the z-buffer
	d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50)); // ambient light

	d3ddev->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8); // anisotropic level
	d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC); // minification
	d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); // magnification
	d3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR); // mipmap
	
}


// this is the function used to render a single frame
void render_frame(void)
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();

	// select which vertex format we are using
	d3ddev->SetFVF(CUSTOMFVF);

	// set the view transform
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(cameraX, cameraY, cameraZ), // the camera position
		&D3DXVECTOR3(0.0f, 0.0f, -5.0f), // the look-at position
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f)); // the up direction
	d3ddev->SetTransform(D3DTS_VIEW, &matView);

	// set the projection transform
	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT,
		1.0f, // the near view-plane
		100.0f); // the far view-plane
	d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);

	// set the world transform
	static float index = 0.0f; index += 0.03f;
/*	if ((cameraZ >20) && (cameraX < 20))
	{
		cameraZ -= 0.25f;
		cameraX += 0.25f;
	}
	else if (cameraZ > -60)
	{
		cameraZ -= 0.25f;
	}
	else
	{
		cameraX = 0.0f;
		cameraZ = 50.0f;
	}*/

	D3DXMATRIX matRotateY;
	D3DXMATRIX rTran;
	D3DXMATRIX rScale;
	D3DXMatrixRotationY(&matRotateY, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &(matRotateY));

	// select the vertex and index buffers to use
	d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
	d3ddev->SetIndices(i_buffer);

	// set the texture
	d3ddev->SetTexture(0, texture);


	// draw the cube

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq8
	D3DXMatrixTranslation(&rTran, 0.0f, .0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq9

	D3DXMatrixTranslation(&rTran, 1.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq9

	D3DXMatrixTranslation(&rTran, 2.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq10

	D3DXMatrixTranslation(&rTran, 3.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq11

	D3DXMatrixTranslation(&rTran, 4.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq12

	D3DXMatrixTranslation(&rTran,5.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq13
	//put in a mystery box

	D3DXMatrixTranslation(&rTran, block1X, block1Y, block1Z);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);
	d3ddev->SetTexture(0, mBox);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq14
	d3ddev->SetTexture(0, texture);

	//end mystery box
	
	D3DXMatrixTranslation(&rTran, 6.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq14
	
	D3DXMatrixTranslation(&rTran, 7.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq15

	D3DXMatrixTranslation(&rTran, 8.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq16

	D3DXMatrixTranslation(&rTran, 9.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq17

	D3DXMatrixTranslation(&rTran, 10.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq18

	D3DXMatrixTranslation(&rTran, 11.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq19

	D3DXMatrixTranslation(&rTran, 12.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq20

	D3DXMatrixTranslation(&rTran, 13.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, 14.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq22

	D3DXMatrixTranslation(&rTran, 15.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq23

	D3DXMatrixTranslation(&rTran, 16.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq24

	D3DXMatrixTranslation(&rTran, -1.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -2.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -3.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);



	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21


	//end mystery box
	D3DXMatrixTranslation(&rTran, -4.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -5.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -6.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	///start here for drawing the final acent
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -7.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -8.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -9.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -10.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -11.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -12.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -13.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -14.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -15.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -16.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -17.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -18.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21


	D3DXMatrixTranslation(&rTran, -19.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21


	D3DXMatrixTranslation(&rTran, -20.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21


	D3DXMatrixTranslation(&rTran, -21.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, 17.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, 18.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, 19.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, 20.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, 21.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, 22.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, 23.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, 24.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	

	//start the stairs 
	d3ddev->SetTexture(0, fBox);
	

	D3DXMatrixTranslation(&rTran, -6.0f, 1.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -7.0f, 1.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -8.0f, 1.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -9.0f, 1.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -10.0f, 1.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -11.0f, 1.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -12.0f, 1.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -13.0f, 1.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -14.0f, 1.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21
	//begin row 2



	D3DXMatrixTranslation(&rTran, -7.0f, 2.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -8.0f, 2.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -9.0f, 2.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -10.0f, 2.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -11.0f, 2.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -12.0f, 2.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -13.0f, 2.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -14.0f, 2.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21
	//begin row 3, start at -8.

	D3DXMatrixTranslation(&rTran, -8.0f, 3.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -9.0f, 3.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -10.0f, 3.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -11.0f, 3.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -12.0f, 3.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -13.0f, 3.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -14.0f, 3.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21
	//begin row 4, star at -9

	D3DXMatrixTranslation(&rTran, -9.0f, 4.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -10.0f, 4.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -11.0f, 4.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -12.0f, 4.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -13.0f, 4.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -14.0f, 4.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21
	//begin row 5, start at -10

	D3DXMatrixTranslation(&rTran, -10.0f, 5.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -11.0f, 5.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -12.0f, 5.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -13.0f, 5.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -14.0f, 5.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21
	//begin row 6, start at -11

	D3DXMatrixTranslation(&rTran, -11.0f, 6.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

	D3DXMatrixTranslation(&rTran, -12.0f, 6.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &rTran);

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

D3DXMatrixTranslation(&rTran, -13.0f, 6.0f, 0.0f);
d3ddev->SetTransform(D3DTS_WORLD, &rTran);

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

D3DXMatrixTranslation(&rTran, -14.0f, 6.0f, 0.0f);
d3ddev->SetTransform(D3DTS_WORLD, &rTran);

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

//begin row 7, start at -12

D3DXMatrixTranslation(&rTran, -12.0f, 7.0f, 0.0f);
d3ddev->SetTransform(D3DTS_WORLD, &rTran);

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

D3DXMatrixTranslation(&rTran, -13.0f, 7.0f, 0.0f);
d3ddev->SetTransform(D3DTS_WORLD, &rTran);

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

D3DXMatrixTranslation(&rTran, -14.0f, 7.0f, 0.0f);
d3ddev->SetTransform(D3DTS_WORLD, &rTran);

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

//begin row 8, start at -13

D3DXMatrixTranslation(&rTran, -13.0f, 8.0f, 0.0f);
d3ddev->SetTransform(D3DTS_WORLD, &rTran);

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

D3DXMatrixTranslation(&rTran, -14.0f, 8.0f, 0.0f);
d3ddev->SetTransform(D3DTS_WORLD, &rTran);

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

//work on the flag pole, scale it and change texture
d3ddev->SetTexture(0, pole);

D3DXMatrixScaling(&rScale, 0.10f, 11.0f, 0.10f);
D3DXMatrixTranslation(&rTran, -20.0f, 1.0f, 0.0f);
d3ddev->SetTransform(D3DTS_WORLD, &(rScale * rTran));

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21
//try to put a top to the pole
d3ddev->SetTexture(0, poleTop);
D3DXMatrixScaling(&rScale, 0.35f, 0.35f, 0.10f);
D3DXMatrixTranslation(&rTran, -19.90f, 12.0f, 0.0f);
d3ddev->SetTransform(D3DTS_WORLD, &(rScale * rTran));

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21

//work on thwomp

/*d3ddev->SetTexture(0, thwomp);
D3DXMatrixScaling(&rScale, 1.15f, 1.15f, 1.15f);
D3DXMatrixTranslation(&rTran, tX, tY, tZ);
d3ddev->SetTransform(D3DTS_WORLD, &(rScale * rTran));
if ((tY >= 1))//go down
{
	tY -= 0.05f;
}
else
{
	tY = 4.0f;
}

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21
*/
//draw the character


d3ddev->SetTexture(0, charTex);
D3DXMatrixScaling(&rScale, 1.0f, 1.0f, 1.5f);
D3DXMatrixTranslation(&rTran, charx, chary, charz);
d3ddev->SetTransform(D3DTS_WORLD, &(rScale * rTran));
if (isJumping)
{
	jump();
}
d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw sq21


//draw the winning box, make it decrease in height steadily

d3ddev->SetTexture(0, winTex);
D3DXMatrixScaling(&rScale, 1.0f, 1.0f, 1.0f);
D3DXMatrixTranslation(&rTran, winboxX, winboxY, winboxZ);
d3ddev->SetTransform(D3DTS_WORLD, &(rScale * rTran));

d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);//draw the win box.

d3ddev->SetTexture(0, texture);

//work on the fireball
d3ddev->SetTexture(0, thwomp);
D3DXCreateSphere(d3ddev, 1.0f, 20, 20, &pTempSunMesh, NULL);

D3DXMatrixScaling(&rScale, fireballSX, fireballSY, 0.25f);
D3DXMatrixTranslation(&rTran, fireballX, fireballY, fireballZ);
d3ddev->SetTransform(D3DTS_WORLD, &(rScale * rTran));

pTempSunMesh->DrawSubset(0);//draw fireball

d3ddev->EndScene();

d3ddev->Present(NULL, NULL, NULL, NULL);
}
void jump(void)
{
	//printf("%f\n", charx);
	if (charx == block1X)
	{
		if (chary == jumpBaseline)
		{
			winboxY += 1.0f;//will put the box up more on each hit
			//D3DXCreateTextureFromFile(d3ddev, L"t.jpg", &mBox);
		}
		 
		
		
	}
	if (jumt < 21.0f)
	{
		
		chary = (-0.01f * (jumt* jumt) + jumpBaseline);
		jumt += 1.0f;
	}
	else
	{
		jumt = -21.0f;
		isJumping = false;
	}

	if (!notWon)//won the game,
	{
		chary = 1.0f;

	}
}

// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	v_buffer->Release();
	i_buffer->Release();
	texture->Release(); // close and release the texture
	d3ddev->Release();
	d3d->Release();
}


// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
	D3DXCreateTextureFromFile(d3ddev, // the Direct3D device
		L"block.png", // the filename of the texture
		&texture); // the address of the texture storage
	D3DXCreateTextureFromFile(d3ddev, L"m.jpg", &mBox);
	D3DXCreateTextureFromFile(d3ddev, L"m.jpg", &mBox2);
	D3DXCreateTextureFromFile(d3ddev, L"sb.png", &fBox);
	D3DXCreateTextureFromFile(d3ddev, L"pole.jpg", &pole);
	D3DXCreateTextureFromFile(d3ddev, L"poleTop.png", &poleTop);
	D3DXCreateTextureFromFile(d3ddev, L"red.png", &thwomp);
	D3DXCreateTextureFromFile(d3ddev, L"t.jpg", &charTex);
	D3DXCreateTextureFromFile(d3ddev, L"win.jpg", &winTex);

		// create the vertices using the CUSTOMVERTEX struct
	CUSTOMVERTEX vertices[] =
	{
	{ -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, }, // side 1
	{ 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, },
	{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, },
	{ 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, },

	{ -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, }, // side 2
	{ -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, },
	{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, },
	{ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, },

	{ -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, }, // side 3
	{ -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, },
	{ 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, },
	{ 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, },

	{ -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, }, // side 4
	{ 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, },
	{ -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, },
	{ 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, },

	{ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, }, // side 5
	{ 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, },
	{ 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, },
	{ 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, },

	{ -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, }, // side 6
	{ -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, },
	{ -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, },
	{ -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, },
	};

	// create a vertex buffer interface called v_buffer
	d3ddev->CreateVertexBuffer(24 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);

	VOID* pVoid; // a void pointer

	// lock v_buffer and load the vertices into it
	v_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	v_buffer->Unlock();

	// create the indices using an int array
	short indices[] =
	{
		//draw the Square 8
	0, 1, 2, // side 1
	2, 1, 3,
	4, 5, 6, // side 2
	6, 5, 7,
	8, 9, 10, // side 3
	10, 9, 11,
	12, 13, 14, // side 4
	14, 13, 15,
	16, 17, 18, // side 5
	18, 17, 19,
	20, 21, 22, // side 6
	22, 21, 23,
	};

	// create an index buffer interface called i_buffer
	d3ddev->CreateIndexBuffer(36 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&i_buffer,
		NULL);

	// lock i_buffer and load the indices into it
	i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indices, sizeof(indices));
	i_buffer->Unlock();
}


// this is the function that sets up the lights and materials
void init_light(void)
{
	D3DLIGHT9 light; // create the light struct
	D3DMATERIAL9 material; // create the material struct

	ZeroMemory(&light, sizeof(light)); // clear out the light struct for use
	light.Type = D3DLIGHT_DIRECTIONAL; // make the light type 'directional light'
	light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f); // set the light's color
	light.Direction = D3DXVECTOR3(-1.0f, -0.3f, 1.0f);//sets the direction of the light

	d3ddev->SetLight(0, &light); // send the light struct properties to light #0
	d3ddev->LightEnable(0, TRUE); // turn on light #0

	ZeroMemory(&material, sizeof(D3DMATERIAL9)); // clear out the struct for use
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); // set diffuse color to white
	material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); // set ambient color to white

	d3ddev->SetMaterial(&material); // set the globably-used material to &material
}
