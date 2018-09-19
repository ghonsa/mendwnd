// mendwnd.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "mendwnd.h"
#include "commctrl.h"
#include <stdio.h>
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name



double currx = -2.3 ;
double curry = -1.0;
double currmag = 0.002;

HWND hStatusWnd;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MENDWND, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MENDWND));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MENDWND));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MENDWND);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

// DoCreateStatusBar - creates a status bar and divides it into
// the specified number of parts.
// Returns the handle to the status bar.
// hwndParent - parent window for the status bar.
// nStatusID - child window identifier.
// hinst - handle to the application instance.
// nParts - number of parts into which to divide the status bar.
HWND DoCreateStatusBar(HWND hwndParent, int nStatusID, HINSTANCE
                      hinst, int nParts)
{
    HWND hwndStatus;
    RECT rcClient;
    HLOCAL hloc;
    LPINT lpParts;
    int i, nWidth;

    // Ensure that the common control DLL is loaded.
    InitCommonControls();

    // Create the status bar.
    hwndStatus = CreateWindowEx(
        0,                       // no extended styles
        STATUSCLASSNAME,         // name of status bar class
        (LPCTSTR) NULL,          // no text when first created
        SBARS_SIZEGRIP |         // includes a sizing grip
        WS_CHILD,                // creates a child window
        0, 0, 0, 0,              // ignores size and position
        hwndParent,              // handle to parent window
        (HMENU) nStatusID,       // child window identifier
        hinst,                   // handle to application instance
        NULL);                   // no window creation data

    // Get the coordinates of the parent window's client area.
    GetClientRect(hwndParent, &rcClient);

    // Allocate an array for holding the right edge coordinates.
    hloc = LocalAlloc(LHND, sizeof(int) * nParts);
    lpParts = (LPINT)LocalLock(hloc);

    // Calculate the right edge coordinate for each part, and
    // copy the coordinates to the array.
    nWidth = rcClient.right / nParts;
    for (i = 0; i < nParts; i++) { 
       lpParts[i] = nWidth;
       nWidth += nWidth;
    }

    // Tell the status bar to create the window parts.
    SendMessage(hwndStatus, SB_SETPARTS, (WPARAM) nParts, (LPARAM)
               lpParts);

    // Free the array, and return.
    LocalUnlock(hloc);
    LocalFree(hloc);
    return hwndStatus;
}  
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   // create a status window
	hStatusWnd = DoCreateStatusBar(hWnd, 1, hInst, 4);
   ShowWindow(hStatusWnd, nCmdShow);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// CmtestView drawing
void DrawImage(HWND hwnd ,HDC hdc,double initx, double inity,double mag, unsigned int iterations)
{
	RECT rct;
 	SIZE sz;
	GetClientRect(hwnd,&rct);
	sz.cx = rct.right-rct.left;
	sz.cy = rct.bottom - rct.top;
	int cres =  GetDeviceCaps(hdc,COLORRES);

	char textbuff[256];

	double fMagLevel = mag;
    double fViewRectReal = initx, fViewRectImg = inity;
	for (int y=0; y < sz.cy; y++)
    {
        // imaginary component of "c"

        double fCImg = fViewRectImg + y * fMagLevel;
        
        for (int x=0; x < sz.cx; x++)        {
            // real component of "c"

            double fCReal = fViewRectReal + x * fMagLevel;
            double fZReal = fCReal;
            double fZImg = fCImg;
            
            // apply the formula...

            bool bInside = true;
            int n;
            for (n=0; n < iterations; n++)
            {
                double fZRealSquared = fZReal * fZReal;
                double fZImgSquared = fZImg * fZImg;

                // have we escaped?

                if (fZRealSquared + fZImgSquared > 4)
                {
                    bInside = false;
                    break;
                }

                fZImg = 2 * fZReal * fZImg + fCImg;
                fZReal = fZRealSquared - fZImgSquared + fCReal;
            }
			if( n==iterations)
				n=0;
			//SetPixel(hdc,x,y,RGB(n%0xff,n*10%0xff,n*100%0xff));
			SetPixel(hdc,x,y,n*10);
			//SetPixel(hdc,x,y,RGB(n,n*10,n*100));

		}
	}
	sprintf(textbuff,"x:%f  y:%f, d:%f",currx,curry,currmag);
	SendMessage( hStatusWnd,SB_SETTEXT, 0, (LPARAM)textbuff );
	
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
    RECT rcClient;                 // client area rectangle 
    POINT ptClientUL;              // client upper left corner 
    POINT ptClientLR;              // client lower right corner 
    static POINTS ptsBegin;        // beginning point 
    static POINTS ptsEnd;          // new endpoint 
    static POINTS ptsPrevEnd;      // previous endpoint 
    static BOOL fPrevLine = FALSE; // previous line flag 
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_FILE_SAVEIMAGE:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawImage(hWnd ,hdc,currx, curry,currmag, 1024*4);
		EndPaint(hWnd, &ps);
		break;

	case WM_SIZE:
		SendMessage( hStatusWnd,WM_SIZE, wParam, lParam);
		break;

	case WM_CHAR:
		{
			char key = LOWORD(wParam);
			if(key == '+')
			{
				currmag -= currmag* 0.1;
			}
			else if(key == '-')
			{
				currmag += currmag* 0.1;
			}
			else if(key == '8')
				curry+= currmag;
			else if(key == '2')
				curry-= currmag;
			else if(key == '4')
				currx += currmag;
			else if(key == '6')
				currx -= currmag;
			else if(key == '0')
			{
				currx = -2.3 ;
				curry = -1.0;
				currmag = 0.002;
			}
			else if(key == ' ')
			{
				RECT rct;
 				GetClientRect(hWnd,&rct);				
				InvalidateRect( hWnd,&rct,false);

			}
		}
		break;

       case WM_LBUTTONDOWN: 
 
            // Capture mouse input. 
 
            SetCapture(hWnd); 
 
            // Retrieve the screen coordinates of the client area, 
            // and convert them into client coordinates. 
 
            GetClientRect(hWnd, &rcClient); 
            ptClientUL.x = rcClient.left; 
            ptClientUL.y = rcClient.top; 
 
            // Add one to the right and bottom sides, because the 
            // coordinates retrieved by GetClientRect do not 
            // include the far left and lowermost pixels. 
 
            ptClientLR.x = rcClient.right + 1; 
            ptClientLR.y = rcClient.bottom + 1; 
            ClientToScreen(hWnd, &ptClientUL); 
            ClientToScreen(hWnd, &ptClientLR); 
 
            // Copy the client coordinates of the client area 
            // to the rcClient structure. Confine the mouse cursor 
            // to the client area by passing the rcClient structure 
            // to the ClipCursor function. 
 
            SetRect(&rcClient, ptClientUL.x, ptClientUL.y, 
                ptClientLR.x, ptClientLR.y); 
            ClipCursor(&rcClient); 
 
            // Convert the cursor coordinates into a POINTS 
            // structure, which defines the beginning point of the 
            // line drawn during a WM_MOUSEMOVE message. 
 
            ptsBegin = MAKEPOINTS(lParam); 
            return 0; 

        case WM_MOUSEMOVE: 
 
            // When moving the mouse, the user must hold down 
            // the left mouse button to draw lines. 
 
            if (wParam & MK_LBUTTON) 
            { 
 
                // Retrieve a device context (DC) for the client area. 
 
                hdc = GetDC(hWnd); 
 
                // The following function ensures that pixels of 
                // the previously drawn line are set to white and 
                // those of the new line are set to black. 
 
                SetROP2(hdc, R2_NOTXORPEN); 
 
                // If a line was drawn during an earlier WM_MOUSEMOVE 
                // message, draw over it. This erases the line by 
                // setting the color of its pixels to white. 
 
                if (fPrevLine) 
                { 
                    MoveToEx(hdc, ptsBegin.x, ptsBegin.y, 
                        (LPPOINT) NULL); 
                    LineTo(hdc, ptsPrevEnd.x, ptsPrevEnd.y); 
                } 
 
                // Convert the current cursor coordinates to a 
                // POINTS structure, and then draw a new line. 
 
                ptsEnd = MAKEPOINTS(lParam); 

                //MoveToEx(hdc, ptsBegin.x, ptsBegin.y, (LPPOINT) NULL); 
                //LineTo(hdc, ptsEnd.x, ptsEnd.y); 
 
                // Set the previous line flag, save the ending 
                // point of the new line, and then release the DC. 
 
                //fPrevLine = TRUE; 
                //ptsPrevEnd = ptsEnd; 
                ReleaseDC(hWnd, hdc); 
            } 
            break; 
 
        case WM_LBUTTONUP: 
 
            // The user has finished drawing the line. Reset the 
            // previous line flag, release the mouse cursor, and 
            // release the mouse capture. 
			RECT rct;
			GetClientRect(hWnd,&rct);				

			ptsEnd = MAKEPOINTS(lParam); 
            currx = currx+(ptsBegin.x*currmag);
			curry = curry+(ptsBegin.y*currmag);
			currmag = currmag *(ptsEnd.x-ptsBegin.x)/(rct.right-rct.left);
			fPrevLine = FALSE; 
            ClipCursor(NULL); 
            ReleaseCapture(); 
			InvalidateRect( hWnd,&rct,false);

			break; 
 

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
