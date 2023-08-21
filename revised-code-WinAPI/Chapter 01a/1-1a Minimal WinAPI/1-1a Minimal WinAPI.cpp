#include	<windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ PWSTR cmdLine, _In_ int winMode)
{
   static const WCHAR winName[ ]  = L"MinWin";

   WNDCLASS wc { };

   wc.hInstance     = hInst;
   wc.lpszClassName = winName;
   wc.lpfnWndProc   = WndProc;
   wc.style         = 0;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.lpszMenuName  = NULL;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);

   if ( FAILED(RegisterClassW(&wc)) )
   {
      MessageBoxW(NULL, L"Couldn't Register the Window Class!", L"ERROR", MB_OK | MB_ICONERROR);
      return E_FAIL;
   }

   static const WCHAR appTitle[ ] = L"WinAPI Minimal Application";

   HWND hwnd = CreateWindowW(winName, appTitle,
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             NULL, NULL, hInst, NULL);

   if ( NULL == hwnd )
   {
      MessageBoxW(NULL, L"Couldn't Create the Main Window!", L"ERROR", MB_OK | MB_ICONERROR);
      return E_FAIL;
   }

   ShowWindow(hwnd, winMode);
   UpdateWindow(hwnd);

   MSG msg;

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }

   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static const WCHAR szAboutLeft[ ]  = L"This is a minimal WinAPI program.\nYou pressed the left mouse button!";
   static const WCHAR szAboutRight[ ] = L"This is a minimal WinAPI program.\nYou pressed the right mouse button!";

   switch ( msg )
   {
   case WM_LBUTTONDOWN:
      MessageBeep(MB_ICONEXCLAMATION);
      MessageBoxW(hwnd, szAboutLeft, L"About", MB_OK | MB_ICONINFORMATION);
      return S_OK;

   case WM_RBUTTONDOWN:
      MessageBeep(MB_ICONASTERISK);
      MessageBoxW(hwnd, szAboutRight, L"About", MB_OK | MB_ICONINFORMATION);
      return S_OK;

   case WM_DESTROY:
      PostQuitMessage(0);
      return S_OK;
   }

   return DefWindowProcW(hwnd, msg, wParam, lParam);
}