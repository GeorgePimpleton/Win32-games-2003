// "Talk to me like I'm a 3 year old!" Programming Lessons - Windows Games

// Light Cycles.cpp - Light Cycles Source

#include "Light Cycles.hpp"

HRESULT GameInitialize(HINSTANCE hInstance)
{
   g_pGame = std::make_unique<GameEngine>(hInstance, L"Light Cycles", L"Light Cycles",
                                          IDI_ICON, IDI_ICON_SM, 500, 400);

   if (g_pGame == NULL)
   {
      return E_FAIL;
   }

   g_pGame->SetFrameRate(30);

   g_pGame->InitJoystick();

   return S_OK;
}

void GameStart(HWND hWindow)
{
   // create and load the background and light cycle bitmaps
   HINSTANCE hInstance { GetModuleHandleW(NULL) };

   g_pBackground  = std::make_unique<Bitmap>(IDB_BACKGROUND, hInstance);
   g_pCycle[0][0] = std::make_unique<Bitmap>(IDB_CYCLEBLUE_0, hInstance);
   g_pCycle[0][1] = std::make_unique<Bitmap>(IDB_CYCLEBLUE_90, hInstance);
   g_pCycle[0][2] = std::make_unique<Bitmap>(IDB_CYCLEBLUE_180, hInstance);
   g_pCycle[0][3] = std::make_unique<Bitmap>(IDB_CYCLEBLUE_270, hInstance);
   g_pCycle[1][0] = std::make_unique<Bitmap>(IDB_CYCLEORANGE_0, hInstance);
   g_pCycle[1][1] = std::make_unique<Bitmap>(IDB_CYCLEORANGE_90, hInstance);
   g_pCycle[1][2] = std::make_unique<Bitmap>(IDB_CYCLEORANGE_180, hInstance);
   g_pCycle[1][3] = std::make_unique<Bitmap>(IDB_CYCLEORANGE_270, hInstance);

   // start a new game
   NewGame();
}

void GameEnd()
{
}

void GameActivate(HWND hWindow)
{
   // capture the joystick
   g_pGame->CaptureJoystick();
}

void GameDeactivate(HWND hWindow)
{
   g_pGame->ReleaseJoystick();
}

void GamePaint(HDC hDC)
{
   g_pBackground->Draw(hDC, 0, 0);

   // draw the light cycle trails
   for (int i { }; i < 2; i++)
   {
      // create a blue/orange pen depending on which trail is being drawn
      HPEN hPen { CreatePen(PS_SOLID, 5, (i == 0) ? RGB(0, 0, 255) : RGB(255, 146, 73)) };
      SelectObject(hDC, hPen);

      // start at the first point in the trail
      MoveToEx(hDC, g_ptCycleTrail[i][0].x, g_ptCycleTrail[i][0].y, NULL);

      // draw a line to each of the remaining points
      for (int j { 1 }; j < g_iTrailLen[i]; j++)
      {
         LineTo(hDC, g_ptCycleTrail[i][j].x, g_ptCycleTrail[i][j].y);
      }

      // clean up the pen
      DeleteObject(hPen);
   }

   // determine the directions of the light cycles
   int iDirection[2] = { 0, 0 };

   for (int i { }; i < 2; i++)
   {
      if (g_ptCycleSpeed[i].y < 0)
      {
         iDirection[i] = 0;
      }
      else if (g_ptCycleSpeed[i].x > 0)
      {
         iDirection[i] = 1;
      }
      else if (g_ptCycleSpeed[i].y > 0)
      {
         iDirection[i] = 2;
      }
      else if (g_ptCycleSpeed[i].x < 0)
      {
         iDirection[i] = 3;
      }
   }

   // draw the light cycles
   g_pCycle[0][iDirection[0]]->Draw(hDC, g_ptCyclePos[0].x, g_ptCyclePos[0].y, TRUE);
   g_pCycle[1][iDirection[1]]->Draw(hDC, g_ptCyclePos[1].x, g_ptCyclePos[1].y, TRUE);
}

void GameCycle()
{
   if (!g_bGameOver)
   {
      // move the light cycles
      UpdateCycles();

      // force a repaint to redraw the light cycles
      InvalidateRect(g_pGame->GetWindow(), NULL, FALSE);
   }
}

void GameMenu(WPARAM wParam)
{
   switch (LOWORD(wParam))
   {
   case IDM_GAME_NEW:
      // starts a new game
      if (g_bGameOver)
      {
         NewGame();
      }
      return;

   case IDM_GAME_EXIT:
      GameEnd();
      PostQuitMessage(0);
      return;

   case IDM_HELP_ABOUT:
      DialogBoxW(g_pGame->GetInstance(), MAKEINTRESOURCEW(IDD_ABOUT), g_pGame->GetWindow(), (DLGPROC) DlgProc);
      return;
   }
}

void HandleKeys()
{
   if (!g_bGameOver)
   {
      // steer the blue light cycle in response to arrow key presses
      if (GetAsyncKeyState(VK_UP) < 0)
      {
         SteerCycle(0, 0);
      }
      else if (GetAsyncKeyState(VK_RIGHT) < 0)
      {
         SteerCycle(0, 1);
      }
      else if (GetAsyncKeyState(VK_DOWN) < 0)
      {
         SteerCycle(0, 2);
      }
      else if (GetAsyncKeyState(VK_LEFT) < 0)
      {
         SteerCycle(0, 3);
      }
   }
   // removed this so pressing enter after the message box displays
   // doesn't start a new game ::sad::
   //else if (GetAsyncKeyState(VK_RETURN) < 0)
   //{
   //   NewGame();
   //}
}

void MouseButtonDown(LONG x, LONG y, BOOL bLeft)
{ }

void MouseButtonUp(LONG x, LONG y, BOOL bLeft)
{ }

void MouseMove(LONG x, LONG y)
{ }

void HandleJoystick(JOYSTATE jsJoystickState)
{
   if (!g_bGameOver)
   {
      // steer the orange light cycle in response to joystick moves
      if (jsJoystickState & JOY_UP)
      {
         SteerCycle(1, 0);
      }
      else if (jsJoystickState & JOY_RIGHT)
      {
         SteerCycle(1, 1);
      }
      else if (jsJoystickState & JOY_DOWN)
      {
         SteerCycle(1, 2);
      }
      else if (jsJoystickState & JOY_LEFT)
      {
         SteerCycle(1, 3);
      }
   }
   else if (jsJoystickState & JOY_FIRE1)
   {
      NewGame();
   }
}

// specific game functions
void NewGame()
{
   // set the initial blue light cycle position and speed
   g_ptCyclePos[0].x   = 250 - (g_pCycle[0][0]->GetWidth() / 2);
   g_ptCyclePos[0].y   = 400 - g_pCycle[0][0]->GetHeight();
   g_ptCycleSpeed[0].x = 0;
   g_ptCycleSpeed[0].y = -g_iSPEED;

   // set the initial orange light cycle position and speed
   g_ptCyclePos[1].x   = 250 - (g_pCycle[1][0]->GetWidth() / 2);
   g_ptCyclePos[1].y   = 0;
   g_ptCycleSpeed[1].x = 0;
   g_ptCycleSpeed[1].y = g_iSPEED;

   // set the light cycle trail lengths and initial points
   g_iTrailLen[0]         = g_iTrailLen[1] = 2;
   g_ptCycleTrail[0][0].x = g_ptCycleTrail[0][1].x = 250;
   g_ptCycleTrail[0][0].y = g_ptCycleTrail[0][1].y = 400;
   g_ptCycleTrail[1][0].x = g_ptCycleTrail[1][1].x = 250;
   g_ptCycleTrail[1][0].y = g_ptCycleTrail[1][1].y = 0;

   // start the game
   g_bGameOver = FALSE;
}

void UpdateCycles()
{
   for (int i { }; i < 2; i++)
   {
      // update the light cycle position based on its speed
      g_ptCyclePos[i].x = g_ptCyclePos[i].x + g_ptCycleSpeed[i].x;
      g_ptCyclePos[i].y = g_ptCyclePos[i].y + g_ptCycleSpeed[i].y;

      // update the light cycle trail based on its new position
      g_ptCycleTrail[i][g_iTrailLen[i] - 1].x = g_ptCyclePos[i].x + (g_pCycle[i][0]->GetWidth() / 2);
      g_ptCycleTrail[i][g_iTrailLen[i] - 1].y = g_ptCyclePos[i].y + (g_pCycle[i][0]->GetHeight() / 2);

      // see if the light cycle ran into the edge of the screen
      if (g_ptCyclePos[i].x < 0 || g_ptCyclePos[i].x >(500 - g_pCycle[i][0]->GetWidth()) ||
          g_ptCyclePos[i].y < 0 || g_ptCyclePos[i].y >(400 - g_pCycle[i][0]->GetHeight()))
      {
         // the game is over
         EndGame(1 - i);
         return;
      }

      // see if the light cycle collided with its own trail
      RECT rcTmpTrail;

      if (g_iTrailLen[i] > 2) // Must have steered at least once
      {
         for (int j { }; j < g_iTrailLen[i] - 2; j++)
         {
            rcTmpTrail.left   = min(g_ptCycleTrail[i][j].x, g_ptCycleTrail[i][j + 1].x) - 1;
            rcTmpTrail.right  = max(g_ptCycleTrail[i][j].x, g_ptCycleTrail[i][j + 1].x) + 1;
            rcTmpTrail.top    = min(g_ptCycleTrail[i][j].y, g_ptCycleTrail[i][j + 1].y) - 1;
            rcTmpTrail.bottom = max(g_ptCycleTrail[i][j].y, g_ptCycleTrail[i][j + 1].y) + 1;

            if (PtInRect(&rcTmpTrail, g_ptCycleTrail[i][g_iTrailLen[i] - 1]) != 0)
            {
               // the game is over
               EndGame(1 - i);
               return;
            }
         }
      }

      // see if the light cycle collided with the other cycle's trail
      for (int j { }; j <= g_iTrailLen[1 - i] - 2; j++)
      {
         rcTmpTrail.left   = min(g_ptCycleTrail[1 - i][j].x, g_ptCycleTrail[1 - i][j + 1].x) - 3;
         rcTmpTrail.right  = max(g_ptCycleTrail[1 - i][j].x, g_ptCycleTrail[1 - i][j + 1].x) + 3;
         rcTmpTrail.top    = min(g_ptCycleTrail[1 - i][j].y, g_ptCycleTrail[1 - i][j + 1].y) - 3;
         rcTmpTrail.bottom = max(g_ptCycleTrail[1 - i][j].y, g_ptCycleTrail[1 - i][j + 1].y) + 3;

         if (PtInRect(&rcTmpTrail, g_ptCycleTrail[i][g_iTrailLen[i] - 1]) != 0)
         {
            // the game is over
            EndGame(1 - i);
            return;
         }
      }
   }
}

void SteerCycle(int iCycle, int iDirection)
{
   // remember the old light cycle speed
   POINT ptOldSpeed;

   ptOldSpeed.x = g_ptCycleSpeed[iCycle].x;
   ptOldSpeed.y = g_ptCycleSpeed[iCycle].y;

   // change the speed of the light cycle to steer it
   switch (iDirection)
   {
   case 0: // up (0 degrees)
      if (g_ptCycleSpeed[iCycle].y == 0)
      {
         g_ptCycleSpeed[iCycle].x = 0;
         g_ptCycleSpeed[iCycle].y = -g_iSPEED;
      }
      break;

   case 1: // right (90 degrees)
      if (g_ptCycleSpeed[iCycle].x == 0)
      {
         g_ptCycleSpeed[iCycle].x = g_iSPEED;
         g_ptCycleSpeed[iCycle].y = 0;
      }
      break;

   case 2: // down (180 degrees)
      if (g_ptCycleSpeed[iCycle].y == 0)
      {
         g_ptCycleSpeed[iCycle].x = 0;
         g_ptCycleSpeed[iCycle].y = g_iSPEED;
      }
      break;

   case 3: // left (270 degrees)
      if (g_ptCycleSpeed[iCycle].x == 0)
      {
         g_ptCycleSpeed[iCycle].x = -g_iSPEED;
         g_ptCycleSpeed[iCycle].y = 0;
      }
      break;
   }

   // if the speed changed, move to a new point in the light cycle trail
   if ((g_ptCycleSpeed[iCycle].x != ptOldSpeed.x) || (g_ptCycleSpeed[iCycle].y != ptOldSpeed.y))
   {
      // increment the number of trail points
      g_iTrailLen[iCycle]++;

      // set the initial position of the new trail point
      g_ptCycleTrail[iCycle][g_iTrailLen[iCycle] - 1].x = g_ptCyclePos[iCycle].x + (g_pCycle[iCycle][0]->GetWidth() / 2);
      g_ptCycleTrail[iCycle][g_iTrailLen[iCycle] - 1].y = g_ptCyclePos[iCycle].y + (g_pCycle[iCycle][0]->GetHeight() / 2);
   }
}

void EndGame(int iCycle)
{
   // set the game over flag
   g_bGameOver = TRUE;

   // display a message about the winner
   if (iCycle == 0)
   {
      MessageBoxW(g_pGame->GetWindow(), L"Blue wins!", L"Light Cycles", MB_OK);
   }
   else
   {
      MessageBox(g_pGame->GetWindow(), L"Orange wins!", L"Light Cycles", MB_OK);
   }
}