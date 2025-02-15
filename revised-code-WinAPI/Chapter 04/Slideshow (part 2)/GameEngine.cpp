#include "GameEngine.h"

GameEngine* GameEngine::m_pGameEngine = NULL;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
{
   MSG        msg;
   static int iTickTrigger = 0;
   int        iTickCount;

   if ( GameInitialize( hInstance ) )
   {
      if ( !GameEngine::GetEngine( )->Initialize( iCmdShow ) )
         return FALSE;

      while ( TRUE )
      {
         if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
         {
            if ( msg.message == WM_QUIT )
               break;
            TranslateMessage( &msg );
            DispatchMessage( &msg );
         }
         else
         {
            if ( !GameEngine::GetEngine( )->GetSleep( ) )
            {
               iTickCount = GetTickCount( );
               if ( iTickCount > iTickTrigger )
               {
                  iTickTrigger = iTickCount + GameEngine::GetEngine( )->GetFrameDelay( );
                  GameCycle( );
               }
            }
         }
      }
      return ( int ) msg.wParam;
   }

   // End the game
   GameEnd( );

   return TRUE;
}

LRESULT CALLBACK WndProc( HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam )
{
   return GameEngine::GetEngine( )->HandleEvent( hWindow, msg, wParam, lParam );
}

GameEngine::GameEngine( HINSTANCE hInstance, PCTSTR szWindowClass,
                        PCTSTR szTitle, WORD wIcon, WORD wSmallIcon, int iWidth, int iHeight )
{
   m_pGameEngine = this;
   m_hInstance   = hInstance;
   m_hWindow     = NULL;
   if ( lstrlen( szWindowClass ) > 0 )
      lstrcpy( m_szWindowClass, szWindowClass );
   if ( lstrlen( szTitle ) > 0 )
      lstrcpy( m_szTitle, szTitle );
   m_wIcon       = wIcon;
   m_wSmallIcon  = wSmallIcon;
   m_iWidth      = iWidth;
   m_iHeight     = iHeight;
   m_iFrameDelay = 50;
   m_bSleep      = TRUE;
}

GameEngine::~GameEngine( )
{ }

BOOL GameEngine::Initialize( int iCmdShow )
{
   WNDCLASSEX wndclass;

   wndclass.cbSize        = sizeof( wndclass );
   wndclass.style         = CS_HREDRAW | CS_VREDRAW;
   wndclass.lpfnWndProc   = WndProc;
   wndclass.cbClsExtra    = 0;
   wndclass.cbWndExtra    = 0;
   wndclass.hInstance     = m_hInstance;
   wndclass.hIcon         = LoadIcon( m_hInstance, MAKEINTRESOURCE( GetIcon( ) ) );
   wndclass.hIconSm       = LoadIcon( m_hInstance, MAKEINTRESOURCE( GetSmallIcon( ) ) );
   wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
   wndclass.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );
   wndclass.lpszMenuName  = NULL;
   wndclass.lpszClassName = m_szWindowClass;

   if ( !RegisterClassEx( &wndclass ) )
      return FALSE;

   int iWindowWidth  = m_iWidth + GetSystemMetrics( SM_CXFIXEDFRAME )  * 2,
       iWindowHeight = m_iHeight + GetSystemMetrics( SM_CYFIXEDFRAME ) * 2 + GetSystemMetrics( SM_CYCAPTION );
   if ( wndclass.lpszMenuName != NULL )
      iWindowHeight += GetSystemMetrics( SM_CYMENU );
   int iXWindowPos = ( GetSystemMetrics( SM_CXSCREEN ) - iWindowWidth )  / 2,
       iYWindowPos = ( GetSystemMetrics( SM_CYSCREEN ) - iWindowHeight ) / 2;

   m_hWindow = CreateWindow( m_szWindowClass, m_szTitle, WS_POPUPWINDOW |
                             WS_CAPTION | WS_MINIMIZEBOX, iXWindowPos, iYWindowPos, iWindowWidth,
                             iWindowHeight, NULL, NULL, m_hInstance, NULL );
   if ( !m_hWindow )
      return FALSE;

   ShowWindow( m_hWindow, iCmdShow );
   UpdateWindow( m_hWindow );

   return TRUE;
}

LRESULT GameEngine::HandleEvent( HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch ( msg )
   {
   case WM_CREATE:
      SetWindow( hWindow );
      GameStart( hWindow );
      return 0;

   case WM_SETFOCUS:
      GameActivate( hWindow );
      SetSleep( FALSE );
      return 0;

   case WM_KILLFOCUS:
      GameDeactivate( hWindow );
      SetSleep( TRUE );
      return 0;

   case WM_PAINT:
      HDC         hDC;
      PAINTSTRUCT ps;
      hDC = BeginPaint( hWindow, &ps );

      GamePaint( hDC );

      EndPaint( hWindow, &ps );
      return 0;

   case WM_DESTROY:
      GameEnd( );
      PostQuitMessage( 0 );
      return 0;
   }
   return DefWindowProc( hWindow, msg, wParam, lParam );
}

void GameEngine::ErrorQuit( LPTSTR szErrorMsg )
{
   MessageBox( GetWindow( ), szErrorMsg, TEXT( "Critical Error" ), MB_OK | MB_ICONERROR );
   PostQuitMessage( 0 );
}
