#include "winmsg.hpp"
#include "winfuncs.hpp"

static const WCHAR winName[ ]  = L"IdleTime";
static const WCHAR appTitle[ ] = L"Idle Time Processing";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
      HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLButtonDown);
      HANDLE_MSG(hwnd, WM_RBUTTONDOWN, OnRButtonDown);
      HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
   }

   return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HRESULT InitApplication(HINSTANCE hInst)
{
   WNDCLASS wc { };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = hInst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = winName;

   if ( FAILED(RegisterClassW(&wc)) )
   {
      MessageBoxW(NULL, L"Can't Register the Window Class!", winName, MB_OK | MB_ICONERROR);
      return E_FAIL;
   }
   else return S_OK;
}

HRESULT InitInstance(HINSTANCE hInst, int winMode)
{
   HWND hwnd = CreateWindowW(winName, appTitle,
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, NULL, hInst, NULL);

   if ( NULL == hwnd )
   {
      MessageBoxW(NULL, L"Can't Create the Main Window!", winName, MB_OK | MB_ICONERROR);
      return E_FAIL;
   }

   ShowWindow(hwnd, winMode);
   UpdateWindow(hwnd);

   return S_OK;
}

int MessageLoop( )
{
   MSG msg;

   ZeroMemory(&msg, sizeof(MSG));

   while ( msg.message != WM_QUIT )
   {
      if ( PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) )
      {
         TranslateMessage(&msg);
         DispatchMessageW(&msg);
      }
      else
      {
         // idle-time processing
         // best if a function call is used
      }
   }

   return (int) msg.wParam;
}