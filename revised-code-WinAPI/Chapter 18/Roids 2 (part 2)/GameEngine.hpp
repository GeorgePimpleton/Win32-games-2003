#pragma once

#include <windows.h>
#include "resource.h"
#include <mmsystem.h>
#include <vector>
#include "Sprite.hpp"

using JOYSTATE = WORD;
const JOYSTATE JOY_NONE  = 0x0000L;
const JOYSTATE JOY_LEFT  = 0x0001L;
const JOYSTATE JOY_RIGHT = 0x0002L;
const JOYSTATE JOY_UP    = 0x0004L;
const JOYSTATE JOY_DOWN  = 0x0008L;
const JOYSTATE JOY_FIRE1 = 0x0010L;
const JOYSTATE JOY_FIRE2 = 0x0020L;

int     WINAPI   wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL    CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

HRESULT GameInitialize( HINSTANCE hInstance );
void    GameStart( HWND );
void    GameNew( );
void    GameEnd( );
void    GameActivate( HWND );
void    GameDeactivate( HWND );
void    GamePaint( HDC );
void    GameCycle( );
void    GameMenu( WPARAM );
void    HandleKeys( );
void    MouseButtonDown( int, int, BOOL );
void    MouseButtonUp( int, int, BOOL );
void    MouseMove( int, int );
void    HandleJoystick( JOYSTATE );
BOOL    SpriteCollision( Sprite*, Sprite* );
void    SpriteDying( Sprite* );

class GameEngine
{
protected:
   static GameEngine*   m_gameEngine;
   HINSTANCE            m_inst;
   HWND                 m_wnd;
   PCWSTR               m_wndClass;
   PCWSTR               m_title;
   WORD                 m_icon;
   WORD                 m_smallIcon;
   LONG                 m_width;
   LONG                 m_height;
   LONG                 m_frameDelay;
   BOOL                 m_asleep;
   UINT                 m_joyID;
   RECT                 m_joyTrip;
   std::vector<Sprite*> m_sprites;
   UINT                 m_MIDIPlayerID;

   BOOL CheckSpriteCollision( Sprite* testSprite );

public:
            GameEngine( HINSTANCE, PCWSTR, PCWSTR,
                        WORD, WORD, int = 640, int = 480 );
   virtual ~GameEngine( );

   static GameEngine* GetEngine( )         { return m_gameEngine; };

   HRESULT Initialize( int );
   LRESULT HandleEvent( HWND wnd, UINT, WPARAM, LPARAM );
   BOOL    InitJoystick( );
   void    CaptureJoystick( );
   void    ReleaseJoystick( );
   void    CheckJoystick( );
   void    AddSprite( Sprite* );
   void    DrawSprites( HDC ) const;
   void    UpdateSprites( );
   void    CleanupSprites( );
   Sprite* IsPointInSprite( int, int );
   void    PlayMIDISong( PCWSTR = L"", BOOL = TRUE );
   void    PauseMIDISong( );
   void    CloseMIDIPlayer( );

   HINSTANCE GetInstance( ) const           { return m_inst; };
   HWND      GetWindow( ) const             { return m_wnd; };
   void      SetWindow( HWND wnd )          { m_wnd = wnd; };
   PCWSTR    GetTitle( )                    { return m_title; };
   WORD      GetIcon( ) const               { return m_icon; };
   WORD      GetSmallIcon( ) const          { return m_smallIcon; };
   int       GetWidth( ) const              { return m_width; };
   int       GetHeight( ) const             { return m_height; };
   int       GetFrameDelay( ) const         { return m_frameDelay; };
   void      SetFrameRate( LONG frameRate ) { m_frameDelay = 1000 / frameRate; };
   BOOL      GetSleep( ) const              { return m_asleep; };
   void      SetSleep( BOOL asleep )        { m_asleep = asleep; };
};
