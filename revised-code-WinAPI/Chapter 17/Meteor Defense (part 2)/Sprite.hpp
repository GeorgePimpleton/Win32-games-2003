#pragma once

#include <windows.h>
#include "random_toolkit_v131.hpp"
#include "Bitmap.hpp"

using SPRITEACTION = WORD;
const SPRITEACTION SA_NONE = 0x0000L;
const SPRITEACTION SA_KILL = 0x0001L;

using BOUNDSACTION = WORD;
const BOUNDSACTION BA_STOP   = 0;
const BOUNDSACTION BA_WRAP   = 1;
const BOUNDSACTION BA_BOUNCE = 2;
const BOUNDSACTION BA_DIE    = 3;

class Sprite
{
protected:
   Bitmap*      m_bitmap;
   int          m_numFrames;
   int          m_curFrame;
   int          m_frameDelay;
   int          m_frameTrigger;
   RECT         m_position;
   RECT         m_collision;
   POINT        m_velocity;
   int          m_zOrder;
   RECT         m_bounds;
   BOUNDSACTION m_boundsAction;
   BOOL         m_hidden;
   BOOL         m_dying;
   BOOL         m_oneCycle;

   void         UpdateFrame( );
   virtual void CalcCollisionRect( );

public:
      Sprite( Bitmap* );
      Sprite( Bitmap*, RECT&, BOUNDSACTION = BA_STOP );
      Sprite( Bitmap*, POINT, POINT, int,
              RECT&, BOUNDSACTION = BA_STOP );
   virtual ~Sprite( );

   virtual SPRITEACTION Update( );

   void Draw( HDC ) const;
   BOOL IsPointInside( int, int );
   BOOL TestCollision( Sprite* );
   void Kill( ) { m_dying = TRUE; };

   Bitmap* GetBitmap( )                                 { return m_bitmap; };
   void    SetNumFrames( int, BOOL                      = FALSE );
   void    SetFrameDelay( int frameDelay )              { m_frameDelay = frameDelay; };
   RECT&   GetPosition( )                               { return m_position; };
   void    SetPosition( int, int );
   void    SetPosition( POINT );
   void    SetPosition( RECT& );
   void    OffsetPosition( int, int );
   RECT&   GetCollision( )                              { return m_collision; };
   POINT   GetVelocity( ) const                         { return m_velocity; };
   void    SetVelocity( int, int );
   void    SetVelocity( POINT );
   BOOL    GetZOrder( ) const                           { return m_zOrder; };
   void    SetZOrder( int zOrder )                      { m_zOrder = zOrder; };
   void    SetBounds( RECT& bounds )                    { CopyRect( &m_bounds, &bounds ); };
   void    SetBoundsAction( BOUNDSACTION boundsAction ) { m_boundsAction = boundsAction; };
   BOOL    IsHidden( ) const                            { return m_hidden; };
   void    SetHidden( BOOL hidden )                     { m_hidden = hidden; };
   int     GetWidth( ) const                            { return m_bitmap-> GetWidth( ); };
   int     GetHeight( ) const                           { return ( m_bitmap-> GetHeight( ) / m_numFrames ); };
};

inline void Sprite::UpdateFrame( )
{
   if ( ( m_frameDelay >= 0 ) && ( --m_frameTrigger <= 0 ) )
   {
      m_frameTrigger = m_frameDelay;

      if ( ++m_curFrame >= m_numFrames )
      {
         if ( m_oneCycle )
         {
            m_dying = TRUE;
         }
         else
         {
            m_curFrame = 0;
         }
      }
   }
}

inline void Sprite::CalcCollisionRect( )
{
   int xShrink = ( m_position.left - m_position.right ) / 12;
   int yShrink = ( m_position.top - m_position.bottom ) / 12;

   CopyRect( &m_collision, &m_position );
   InflateRect( &m_collision, xShrink, yShrink );
}

inline BOOL Sprite::TestCollision( Sprite* testSprite )
{
   RECT& test = testSprite->GetCollision( );

   return m_collision.left <= test.right && test.left <= m_collision.right &&
      m_collision.top <= test.bottom && test.top <= m_collision.bottom;
}

inline BOOL Sprite::IsPointInside( int x, int y )
{
   POINT point = { };

   point.x = x;
   point.y = y;

   return PtInRect( &m_position, point );
}

inline void Sprite::SetNumFrames( int numFrames, BOOL oneCycle )
{
   m_numFrames = numFrames;
   m_oneCycle  = oneCycle;

   RECT rect = GetPosition( );

   rect.bottom = rect.top + ( ( rect.bottom - rect.top ) / numFrames );

   SetPosition( rect );
}

inline void Sprite::SetPosition( int x, int y )
{
   OffsetRect( &m_position, x - m_position.left, y - m_position.top );
   CalcCollisionRect( );
}

inline void Sprite::SetPosition( POINT position )
{
   OffsetRect( &m_position, position.x - m_position.left, position.y - m_position.top );
   CalcCollisionRect( );
}

inline void Sprite::SetPosition( RECT& position )
{
   CopyRect( &m_position, &position );
   CalcCollisionRect( );
}

inline void Sprite::OffsetPosition( int x, int y )
{
   OffsetRect( &m_position, x, y );
   CalcCollisionRect( );
}

inline void Sprite::SetVelocity( int x, int y )
{
   m_velocity.x = x;
   m_velocity.y = y;
}

inline void Sprite::SetVelocity( POINT velocity )
{
   m_velocity.x = velocity.x;
   m_velocity.y = velocity.y;
}
