#include "Blizzard.hpp"

BOOL GameInitialize( HINSTANCE inst )
{
   g_game = std::make_unique<GameEngine>( inst, L"Blizzard", L"Blizzard f",
                                          IDI_ICON, IDI_ICON_SM );

   if ( g_game == NULL )
   {
      return FALSE;
   }

   g_game->SetFrameRate( 15 );

   return TRUE;
}

void GameStart( HWND wnd )
{
   srand( ( unsigned ) GetTickCount64( ) );
}

void GameNew( )
{
   // do nothing since there is no actual game
}

void GameEnd( )
{ }

void GameActivate( HWND wnd )
{
   HDC  dc = GetDC( wnd );
   RECT rect;

   GetClientRect( wnd, &rect );

   DrawTextW( dc, L"Here comes the blizzard!", -1, &rect,
              DT_SINGLELINE | DT_CENTER | DT_VCENTER );

   ReleaseDC( wnd, dc );
}

void GameDeactivate( HWND wnd )
{
   HDC  dc = GetDC( wnd );
   RECT rect;

   GetClientRect( wnd, &rect );

   DrawTextW( dc, L"The blizzard has passed.", -1, &rect,
              DT_SINGLELINE | DT_CENTER | DT_VCENTER );

   ReleaseDC( wnd, dc );
}

void GamePaint( HDC dc )
{ }

void GameCycle( )
{
   HWND wnd = g_game->GetWindow( );
   HDC  dc  = GetDC( wnd );

   DrawIcon( dc, rand( ) % g_game->GetWidth( ), rand( ) % g_game->GetHeight( ),
             ( HICON ) GetClassLongPtrW( wnd, GCLP_HICON ) );

   ReleaseDC( wnd, dc );
}

void GameMenu( WPARAM wParam )
{
   switch ( LOWORD( wParam ) )
   {
   case IDM_GAME_NEW:
      GameNew( );
      return;

   case IDM_GAME_EXIT:
      DestroyWindow( g_game->GetWindow( ) );
      return;

   case IDM_HELP_ABOUT:
      DialogBoxParamW( g_game->GetInstance( ), MAKEINTRESOURCEW( IDD_ABOUT ),
                       g_game->GetWindow( ), ( DLGPROC ) DlgProc, 0L );
      return;
   }
}
