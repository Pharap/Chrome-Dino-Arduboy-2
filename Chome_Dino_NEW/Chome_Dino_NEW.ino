
#include <Tinyfont.h>
#include <Arduboy2.h>
#include <ArduboyTones.h>

#include "cactus_1.h"
#include "cloud_1.h"
#include "dino_leg_0.h"
#include "dino_leg_1.h"
#include "dino_leg_2.h"
#include "dino_top.h"
#include "dino_tumble.h"
#include "dino_up.h"
#include "dino_full.h"

Arduboy2 arduboy;
Tinyfont tinyfont = Tinyfont(Arduboy2::sBuffer, Arduboy2::width(), Arduboy2::height());
ArduboyTones sound(arduboy.audio.enabled);

enum class GameState
{
  TitleScreen,
  CountDown,
  Gameplay,
  GameOver,
};

GameState gameState = GameState::TitleScreen;

// frame counter
unsigned int frame = 0;

// distance ran
int d;
int delta;
int cloud_1_y;
int d_jump;
int d_jump_t;
bool d_run;
int d_tumble_t;
int ox;
int countdownx = 64;
int countdowny = 30;
int wait = 750;

void reset()
{
  d = 0;
  delta = 0;
  cloud_1_y = 2;
  d_jump = 0;
  d_jump_t = 0;
  d_tumble_t = 0;
  d_run = false;
  ox = 130;
}

void setup()
{
  arduboy.begin();
  reset();
}

void loop()
{
  if(!arduboy.nextFrame())
    return;

  arduboy.pollButtons();

  arduboy.clear();

  switch (gameState)
  {
    case GameState::TitleScreen:
      updateTitleScreen();
      drawTitleScreen();
      break;

    case GameState::CountDown:
      break;

    case GameState::Gameplay:
      updateGameplay();
      drawGameplay();
      break;

    case GameState::GameOver:
      updateGameOver();
      drawGameOver();
      break;
  }

  arduboy.display();
}

void updateTitleScreen()
{
  if(arduboy.justPressed(A_BUTTON))
  {
    gameState = GameState::Gameplay;
    d_run = true;
  }
}

void drawTitleScreen()
{
  arduboy.setCursor(32, 20);
  arduboy.println("Chrome Dino");

  tinyfont.setCursor(49, 30);
  tinyfont.print("v0.1.0");

  arduboy.setCursor(18, 40);
  arduboy.println("Press A To Start");
}

void updateGameOver()
{
  if(arduboy.justPressed(A_BUTTON))
  {
    reset();
    gameState = GameState::TitleScreen;
  }
}

void drawGameOver()
{
  arduboy.setCursor(35, 20);
  arduboy.println("Game Over!");
  arduboy.setCursor(40, 30);
  arduboy.print("Score:");
  arduboy.print(d);
}

void updateGameplay()
{
  if (d_tumble_t == 1)
  {
    gameState = GameState::GameOver;
  }

  ++frame;
  if (frame>16000) frame = 0;

  // increase distance whilst running
  if (d_run && (++delta > 8)) {
    delta = 0; ++d; 
  }

  // obstacles
  if (d_run) {
    ox -= (frame%2)*(d/100) + 2;
    if (ox < -15) ox += 140 + random(80,160);
  }

  // jump!
  if (!d_jump_t && arduboy.pressed(A_BUTTON)) {
    d_jump_t = 1;
    d_jump=5;

    sound.tone(440, 40);

  } else if (d_jump_t) {
    if (d_jump_t == 3) sound.tone(880, 80);

    ++d_jump_t;

    if (d_jump_t<6) {
      d_jump +=6;
    } else if (d_jump_t<9) {
      d_jump +=2;
    } else if (d_jump_t<13) {
      d_jump +=1;
    } else if (d_jump_t == 16 || d_jump_t == 18) {
      d_jump +=1;
    } else if (d_jump_t == 20 || d_jump_t == 22) {
      d_jump -=1;
    } else if (d_jump_t>38) {
      d_jump = 0;
      d_jump_t = 0;
    } else if (d_jump_t>32) {
      d_jump -=6;
    } else if (d_jump_t>29) {
      d_jump -=2;
    } else if (d_jump_t>25) {
      d_jump -=1;
    }
  }

  // hit detect
  if (!d_tumble_t && ox > -15 && ox <12 && d_jump_t < 14) {
    d_tumble_t = 1;    
    sound.tone(25, 100);
  }

  if (d_tumble_t != 0) {
    if (d_tumble_t == 1) {
    } else if (d_tumble_t == 6) {
      sound.tone(174, 200);
    }

    if (d_jump > -4) {
      d_jump -= 1;
      ox -= 1;
    } else {
      d_run = false;
    }
  }
}

void drawGameplay()
{
  drawHUD();
  drawClouds();
  drawTerrain();
  drawObstacles();
  drawDino();
}

void drawHUD()
{
  arduboy.setCursor(50, 6);
  arduboy.print(d);
}

void drawClouds()
{
  // parallax clouds
  arduboy.drawBitmap(128 - (d % 128), cloud_1_y, cloud_1, 20, 10, WHITE);

  if ((d % 128) == 0)
    cloud_1_y = random(0,10);
}

void drawTerrain()
{
  if (d_jump > 4)
  {
    arduboy.drawLine(0, 60, 127, 60, WHITE);
  }
  else
  {
    arduboy.drawLine(0, 60, 3, 60, WHITE);
    arduboy.drawLine(12, 60, 127, 60, WHITE); 
  }
}

void drawObstacles()
{
  arduboy.drawBitmap(ox, 40, cactus_1, 12, 24, WHITE);
}

void drawDino()
{
  // dino
  int dy = (40 - d_jump);

  // tumbles!
  if (d_tumble_t != 0)
  {
    arduboy.drawBitmap(0, dy+3, dino_tumble, 30, 18, WHITE);
  }
  // runs!
  else
  {
    arduboy.drawBitmap(0, dy, dino_top, 20, 18, WHITE);
  
    // Run, Dino, Run!
    if (d_run && !d_jump)
    {
      if ((frame % 8) / 4)
      {
        arduboy.drawBitmap(0, dy + 16, dino_leg_1, 20, 8, WHITE);
      }
      else
      {
        arduboy.drawBitmap(0, dy + 16, dino_leg_2, 20, 8, WHITE);
      }
    }
    else
    {
      arduboy.drawBitmap(0, dy+16, dino_leg_0, 20, 8, WHITE);
    }
  }
}