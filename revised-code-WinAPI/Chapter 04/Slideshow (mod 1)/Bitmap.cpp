#include "Bitmap.hpp"

Bitmap::Bitmap( ) :
   m_bitmap(nullptr), m_width(0), m_height(0)
{
}

Bitmap::Bitmap(PCWSTR fileName) :
   m_bitmap(nullptr), m_width(0), m_height(0)
{
   Create(fileName);
}

Bitmap::Bitmap(UINT resID, HINSTANCE hInst) :
   m_bitmap(nullptr), m_width(0), m_height(0)
{
   Create(resID, hInst);
}

Bitmap::Bitmap(HWND hwnd, LONG width, LONG height, COLORREF color)
   : m_bitmap(nullptr), m_width(0), m_height(0)
{
   Create(hwnd, width, height, color);
}

Bitmap::~Bitmap( )
{
   Free( );
}

BOOL Bitmap::Create(PCWSTR fileName)
{
   Free();

   m_bitmap = (HBITMAP) LoadImageW(nullptr, fileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

   if (m_bitmap == nullptr)
   {
      Free();

      return FALSE;
   }

   BITMAP bitmap;

   GetObjectW(m_bitmap, sizeof(BITMAP), &bitmap);

   m_width  = bitmap.bmWidth;
   m_height = bitmap.bmHeight;

   return TRUE;
}

BOOL Bitmap::Create(UINT resID, HINSTANCE hInst)
{
   Free( );

   m_bitmap = (HBITMAP) LoadImageW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(resID), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

   if (m_bitmap == nullptr)
   {
      Free();

      return FALSE;
   }

   BITMAP bitmap;

   GetObjectW(m_bitmap, sizeof(BITMAP), &bitmap);

   m_width  = bitmap.bmWidth;
   m_height = bitmap.bmHeight;

   return TRUE;
}

BOOL Bitmap::Create(HWND hwnd, LONG width, LONG height, COLORREF color)
{
   HDC hDC = GetDC(hwnd);

   m_bitmap = CreateCompatibleBitmap(hDC, width, height);

   if (m_bitmap == nullptr)
   {
      return FALSE;
   }

   m_width  = width;
   m_height = height;

   HDC     memDC     = CreateCompatibleDC(hDC);
   HBRUSH  brush     = CreateSolidBrush(color);
   HBITMAP oldBitmap = (HBITMAP) SelectObject(memDC, m_bitmap);
   RECT    bitmap    = { 0, 0, m_width, m_height };

   FillRect(memDC, &bitmap, brush);

   SelectObject(memDC, oldBitmap);
   DeleteDC(memDC);
   DeleteObject(brush);

   return TRUE;
}

void Bitmap::Draw(HDC hDC, int x, int y) const
{
   if (m_bitmap != nullptr)
   {
      HDC memDC = CreateCompatibleDC(hDC);

      HBITMAP oldBitmap = (HBITMAP) SelectObject(memDC, m_bitmap);

      BitBlt(hDC, x, y, GetWidth(), GetHeight(), memDC, 0, 0, SRCCOPY);

      SelectObject(memDC, oldBitmap);
      DeleteDC(memDC);
   }
}

void Bitmap::Free( )
{
   if (m_bitmap != nullptr)
   {
       DeleteObject(m_bitmap);
       m_bitmap = nullptr;
   }
}