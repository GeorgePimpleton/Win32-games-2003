#include "GameEngine.hpp"

GameEngine* GameEngine::m_gameEngine = nullptr;

int WINAPI wWinMain( _In_ HINSTANCE inst,    _In_opt_ HINSTANCE prevInst,
                     _In_ PWSTR     cmdLine, _In_     int       cmdShow )
{
   if ( GameInitialize( inst ) == S_OK )
   {
      if ( GameEngine::GetEngine( )->Initialize( cmdShow ) != S_OK )
      {
         return E_FAIL;
      }

      HACCEL accel = LoadAcceleratorsW( inst, MAKEINTRESOURCEW( IDR_ACCELERATORS ) );

      if ( nullptr == accel )
      {
         MessageBoxW( nullptr, L"Unable to Load the Accelerators!", GameEngine::GetEngine( )->GetTitle( ), MB_OK | MB_ICONERROR );
         return E_FAIL;
      }

      MSG msg;

      while ( TRUE )
      {
         if ( PeekMessageW( &msg, nullptr, 0, 0, PM_REMOVE ) != 0 )
         {
            if ( WM_QUIT == msg.message )
            {
               break;
            }

            if ( 0 == TranslateAcceleratorW( GameEngine::GetEngine( )->GetWindow( ), accel, &msg ) )
            {
               TranslateMessage( &msg );
               DispatchMessageW( &msg );
            }
         }
         else
         {
            if ( GameEngine::GetEngine( )->GetSleep( ) == FALSE )
            {
               static ULONGLONG tickTrigger = 0;
               ULONGLONG        tickCount   = GetTickCount64( );

               if ( tickCount > tickTrigger )
               {
                  tickTrigger = tickCount + GameEngine::GetEngine( )->GetFrameDelay( );

                  HandleKeys( );
                  GameCycle( );
               }
            }
         }
      }

      return ( int ) msg.wParam;
   }

   GameEnd( );
   return S_OK;
}

LRESULT CALLBACK WndProc( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   return GameEngine::GetEngine( )->HandleEvent( wnd, msg, wParam, lParam );
}

BOOL CALLBACK DlgProc( HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_COMMAND:
      switch ( LOWORD( wParam ) )
      {
      case IDOK:
         EndDialog( dlg, 0 );
         return TRUE;
      }
   }

   return FALSE;
}

GameEngine::GameEngine( HINSTANCE inst, PCWSTR wndClass, PCWSTR title,
                        WORD icon, WORD smallIcon, UINT width, UINT height )
{
   m_gameEngine = this;
   m_inst       = inst;
   m_wnd        = nullptr;
   m_icon       = icon;
   m_smallIcon  = smallIcon;
   m_width      = width;
   m_height     = height;
   m_frameDelay = 50;
   m_asleep     = TRUE;
   m_wndClass   = wndClass;
   m_title      = title;
}

GameEngine::~GameEngine( )
{ }

HRESULT GameEngine::Initialize( int cmdShow )
{
   WNDCLASSEXW wc { };

   wc.cbSize        = sizeof( WNDCLASSEXW );
   wc.style         = CS_HREDRAW | CS_VREDRAW;
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = m_inst;
   wc.hIcon         = ( HICON ) LoadImageW( m_inst, MAKEINTRESOURCEW( IDI_ICON ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR );
   wc.hIconSm       = ( HICON ) LoadImageW( m_inst, MAKEINTRESOURCEW( IDI_ICON_SM ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR ); ;
   wc.hCursor       = ( HCURSOR ) LoadImageW( nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
   wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wc.lpszMenuName  = MAKEINTRESOURCEW( IDR_MENU );
   wc.lpszClassName = m_wndClass;

   if ( FAILED( RegisterClassExW( &wc ) ) )
   {
      MessageBoxW( nullptr, L"Unable to initialize Main Window!", L"ERROR", MB_ICONERROR | MB_OK );
      return E_FAIL;
   }

   UINT windowWidth  = m_width + GetSystemMetrics( SM_CXFIXEDFRAME ) * 2 + 10;
   UINT windowHeight = m_height + GetSystemMetrics( SM_CYFIXEDFRAME ) * 2
      + GetSystemMetrics( SM_CYCAPTION ) + 10;

   if ( wc.lpszMenuName != nullptr )
   {
      windowHeight += GetSystemMetrics( SM_CYMENU );
   }

   UINT windowPosX { ( GetSystemMetrics( SM_CXSCREEN ) - windowWidth ) / 2 };
   UINT windowPosY { ( GetSystemMetrics( SM_CYSCREEN ) - windowHeight ) / 2 };

   m_wnd = CreateWindowW( m_wndClass, m_title,
                          WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX,
                          windowPosX, windowPosY,
                          windowWidth, windowHeight,
                          nullptr, nullptr, m_inst, nullptr );

   if ( nullptr == m_wnd )
   {
      MessageBoxW( nullptr, L"Unable to create Main Window!", L"ERROR", MB_ICONERROR | MB_OK );
      return E_FAIL;
   }

   ShowWindow( m_wnd, cmdShow );
   UpdateWindow( m_wnd );

   return S_OK;
}

LRESULT GameEngine::HandleEvent( HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_CREATE:
      SetWindow( wnd );
      GameStart( wnd );
      return S_OK;

   case WM_ACTIVATE:
      if ( wParam != WA_INACTIVE )
      {
         GameActivate( wnd );
         SetSleep( FALSE );
      }
      else
      {
         GameDeactivate( wnd );
         SetSleep( TRUE );
      }
      return S_OK;

   case WM_COMMAND:
      GameMenu( wParam );
      return S_OK;

   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      HDC         dc = BeginPaint( wnd, &ps );

      GamePaint( dc );

      EndPaint( wnd, &ps );
      return S_OK;
   }

   case WM_LBUTTONDOWN:
      MouseButtonDown( LOWORD( lParam ), HIWORD( lParam ), TRUE );
      return S_OK;

   case WM_LBUTTONUP:
      MouseButtonUp( LOWORD( lParam ), HIWORD( lParam ), TRUE );
      return S_OK;

   case WM_RBUTTONDOWN:
      MouseButtonDown( LOWORD( lParam ), HIWORD( lParam ), FALSE );
      return S_OK;

   case WM_RBUTTONUP:
      MouseButtonUp( LOWORD( lParam ), HIWORD( lParam ), FALSE );
      return S_OK;

   case WM_MOUSEMOVE:
      MouseMove( LOWORD( lParam ), HIWORD( lParam ) );
      return S_OK;

   case WM_DESTROY:
      GameEnd( );
      PostQuitMessage( 0 );
      return S_OK;
   }

   return DefWindowProcW( wnd, msg, wParam, lParam );
}
