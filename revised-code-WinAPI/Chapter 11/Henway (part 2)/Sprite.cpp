#include "Sprite.hpp"

Sprite::Sprite( Bitmap* bitmap )
{
   m_bitmap = bitmap;

   SetRect( &m_position, 0, 0, bitmap->GetWidth( ), bitmap->GetHeight( ) );
   CalcCollisionRect( );

   m_velocity.x = m_velocity.y = 0;
   m_zOrder     = 0;

   SetRect( &m_bounds, 0, 0, 640, 480 );

   m_boundsAction = BA_STOP;
   m_hidden       = FALSE;
}

Sprite::Sprite( Bitmap* bitmap, RECT& bounds, BOUNDSACTION boundsAction )
{
   int xPos = rtk::rand( 0, ( bounds.right - bounds.left ) );
   int yPos = rtk::rand( 0, ( bounds.bottom - bounds.top ) );

   m_bitmap = bitmap;

   SetRect( &m_position, xPos, yPos, xPos + bitmap->GetWidth( ), yPos + bitmap->GetHeight( ) );
   CalcCollisionRect( );

   m_velocity.x = m_velocity.y = 0;
   m_zOrder     = 0;

   CopyRect( &m_bounds, &bounds );

   m_boundsAction = boundsAction;
   m_hidden       = FALSE;
}

Sprite::Sprite( Bitmap* bitmap, POINT position, POINT velocity, LONG zOrder, RECT& bounds,
                BOUNDSACTION boundsAction )
{
   m_bitmap = bitmap;

   SetRect( &m_position, position.x, position.y, bitmap->GetWidth( ), bitmap->GetHeight( ) );
   CalcCollisionRect( );

   m_velocity = position;
   m_zOrder   = zOrder;

   CopyRect( &m_bounds, &bounds );

   m_boundsAction = boundsAction;
   m_hidden       = FALSE;
}

Sprite::~Sprite( )
{ }

void Sprite::CalcCollisionRect( )
{
   int xShrink = ( m_position.left - m_position.right ) / 12;
   int yShrink = ( m_position.top - m_position.bottom ) / 12;

   CopyRect( &m_collision, &m_position );
   InflateRect( &m_collision, xShrink, yShrink );
}

SPRITEACTION Sprite::Update( )
{
   BOOL  bounce      = FALSE;
   POINT newPosition = { };
   POINT spriteSize  = { };
   POINT boundsSize  = { };

   newPosition.x = m_position.left + m_velocity.x;
   newPosition.y = m_position.top + m_velocity.y;
   spriteSize.x  = m_position.right - m_position.left;
   spriteSize.y  = m_position.bottom - m_position.top;
   boundsSize.x  = m_bounds.right - m_bounds.left;
   boundsSize.y  = m_bounds.bottom - m_bounds.top;

   if ( BA_WRAP == m_boundsAction )
   {
      if ( ( newPosition.x + spriteSize.x ) < m_bounds.left )
      {
         newPosition.x = m_bounds.right;
      }
      else if ( newPosition.x > m_bounds.right )
      {
         newPosition.x = m_bounds.left - spriteSize.x;
      }

      if ( ( newPosition.y + spriteSize.y ) < m_bounds.top )
      {
         newPosition.y = m_bounds.bottom;
      }
      else if ( newPosition.y > m_bounds.bottom )
      {
         newPosition.y = m_bounds.top - spriteSize.y;
      }
   }
   else if ( BA_BOUNCE == m_boundsAction )
   {
      bounce             = FALSE;
      POINT  newVelocity = m_velocity;

      if ( newPosition.x < m_bounds.left )
      {
         bounce        = TRUE;
         newPosition.x = m_bounds.left;
         newVelocity.x = -newVelocity.x;
      }
      else if ( ( newPosition.x + spriteSize.x ) > m_bounds.right )
      {
         bounce        = TRUE;
         newPosition.x = m_bounds.right - spriteSize.x;
         newVelocity.x = -newVelocity.x;
      }

      if ( newPosition.y < m_bounds.top )
      {
         bounce        = TRUE;
         newPosition.y = m_bounds.top;
         newVelocity.y = -newVelocity.y;
      }
      else if ( ( newPosition.y + spriteSize.y ) > m_bounds.bottom )
      {
         bounce        = TRUE;
         newPosition.y = m_bounds.bottom - spriteSize.y;
         newVelocity.y = -newVelocity.y;
      }

      if ( bounce )
      {
         SetVelocity( newVelocity );
      }
   }
   else if ( BA_DIE == m_boundsAction )
   {
      if ( ( newPosition.x + spriteSize.x ) < m_bounds.left || newPosition.x > m_bounds.right ||
           ( newPosition.y + spriteSize.y ) < m_bounds.top || newPosition.y > m_bounds.bottom )
      {
         return SA_KILL;
      }
   }
   else
   {
      if ( newPosition.x  < m_bounds.left || newPosition.x >( m_bounds.right - spriteSize.x ) )
      {
         newPosition.x = max( m_bounds.left, min( newPosition.x, m_bounds.right - spriteSize.x ) );
         SetVelocity( 0, 0 );
      }

      if ( newPosition.y  < m_bounds.top || newPosition.y >( m_bounds.bottom - spriteSize.y ) )
      {
         newPosition.y = max( m_bounds.top, min( newPosition.y, m_bounds.bottom - spriteSize.y ) );
         SetVelocity( 0, 0 );
      }
   }

   SetPosition( newPosition );

   return SA_NONE;
}

void Sprite::Draw( HDC dc ) const
{
   if ( m_bitmap != NULL && !m_hidden )
   {
      m_bitmap->Draw( dc, m_position.left, m_position.top, TRUE );
   }
}
