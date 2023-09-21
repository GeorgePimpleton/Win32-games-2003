#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(_In_ HINSTANCE inst,    _In_opt_ HINSTANCE prevInst,
                    _In_ PWSTR     cmdLine, _In_     int       winMode)
{
   static const WCHAR appName[ ] { L"MinWinApp" };

   WNDCLASSW wc { };

   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = inst;
   wc.hIcon         = (HICON)   LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
   wc.hCursor       = (HCURSOR) LoadImageW(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
   wc.hbrBackground = (HBRUSH)  (COLOR_WINDOW + 1);
   wc.lpszMenuName  = NULL;
   wc.lpszClassName = appName;

   // using the FAILED() macro for generic return testing failure
   if ( FAILED(RegisterClassW(&wc)) )
   {
      MessageBoxW(NULL, L"Can't Register the Window Class!", appName, MB_OK | MB_ICONERROR);
      return E_FAIL;
   }

   // Win32 is now known as the Windows API (WinAPI) since x64 Windows
   static const WCHAR appTitle[ ] { L"Windows API Skeletal Application" };

   HWND wnd { CreateWindowW(appName, appTitle,
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL, NULL, inst, NULL) };

   if ( NULL == wnd )
   {
      MessageBoxW(NULL, L"Unable to Create the Main Window!", appName, MB_OK | MB_ICONERROR);
      return E_FAIL;
   }

   ShowWindow(wnd, winMode);
   UpdateWindow(wnd);

   MSG msg;

   while ( GetMessageW(&msg, NULL, 0, 0) )
   {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
   }

   // the cast is done for x64 Windows, otherwise Visual Studio whinges about possible loss of data
   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch ( msg )
   {
   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      HDC         dc = BeginPaint(wnd, &ps);

      RECT rect;
      GetClientRect(wnd, &rect);

      DrawTextW(dc, L"Hello, Windows!", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

      EndPaint(wnd, &ps);

      return S_OK;
   }

   case WM_DESTROY:
      PostQuitMessage(0);
      return S_OK;
   }

   return DefWindowProcW(wnd, msg, wParam, lParam);
}