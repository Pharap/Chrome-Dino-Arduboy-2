
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
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());
ArduboyTones sound(arduboy.audio.enabled);

//Menu stuff
byte menu = 0;

// frame counter
unsigned int frame = 0;

// general use string buffer
char text[16];

// distance ran
int d, delta;

int cloud_1_y;

int d_jump, d_jump_t;

int d_run;
int d_tumble_t;

int ox;

int countdownx = 64;
int countdowny = 30;

int wait = 750;

void setup()
{
  arduboy.begin();
  arduboy.setFrameRate(60);
  d = 0;
  delta = 0;

  cloud_1_y = 2;

  d_jump = 0;
  d_jump_t = 0;

  d_tumble_t = 0;
  d_run = 0;

  ox = 130;
  
  arduboy.clear();
  arduboy.setCursor(32, 20);
  arduboy.println("Chrome Dino");
  tinyfont.setCursor(49, 30);
  tinyfont.print("v0.1.0");
  arduboy.setCursor(18, 40);
  arduboy.println("Press A To Start");
  arduboy.display();
  
  while (!arduboy.pressed(A_BUTTON)) {}
}

void loop() {
  
  if (!d_run && arduboy.pressed(A_BUTTON)) {
    arduboy.clear();
    arduboy.setCursor(countdownx, countdowny);
    arduboy.print("3");
    arduboy.display();
    delay(wait);
    arduboy.clear();
    arduboy.setCursor(countdownx, countdowny);
    arduboy.print("2");
    arduboy.display();
    delay(wait);
    arduboy.clear();
    arduboy.setCursor(countdownx, countdowny);
    arduboy.print("1");
    arduboy.display();
    delay(wait);
    arduboy.clear();
    d_run = 1;
  }

  if (d_tumble_t == 1) {
    delay(750);
    arduboy.clear();
    arduboy.setCursor(35, 20);
    arduboy.println("Game Over!");
    arduboy.setCursor(40, 30);
    arduboy.print("Score:");
    arduboy.print(d);
    arduboy.display();
    delay(2500);
    setup();
    return;
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

  if (d_tumble_t) {
    if (d_tumble_t == 1) {
    } else if (d_tumble_t == 6) {
      sound.tone(174, 200);
    }

    d_tumble_t;
    if (d_jump > -4) {
      d_jump -= 1;
      ox -= 1;
    } else {
      d_run = 0;
    }
  }

  arduboy.clear();


  // hud
  arduboy.setCursor(50, 6);
  sprintf(text,"%d",d);
  arduboy.print(text);
  // parallax clouds
  arduboy.drawBitmap(128 -(d%128),cloud_1_y,cloud_1,20,10,WHITE);

  if (d%128 == 0) {
    cloud_1_y = random(0,10);
  }

  // terrain
  if (d_jump > 4) {
    arduboy.drawLine(0,60,127,60,WHITE);
  } else {
    arduboy.drawLine(0,60,3,60,WHITE);
    arduboy.drawLine(12,60,127,60,WHITE); 
  }

  // obstacles
  arduboy.drawBitmap(ox,40,cactus_1,12,24,WHITE);

  // dino
  int dy = 40-d_jump;
  // tumbles!
  if (d_tumble_t) {
    arduboy.drawBitmap(0,dy+3,dino_tumble,30,18,WHITE);
  // runs!
  } else {
    arduboy.drawBitmap(0,dy,dino_top,20,18,WHITE);
  
    // Run, Dino, Run!
    if (d_run && !d_jump) {
      if ((frame%8)/4) {
        arduboy.drawBitmap(0,dy+16,dino_leg_1,20,8,WHITE);
      } else {
        arduboy.drawBitmap(0,dy+16,dino_leg_2,20,8,WHITE);
      }
    } else {
      arduboy.drawBitmap(0,dy+16,dino_leg_0,20,8,WHITE);
    }
  }
  
  arduboy.display();
  delay(16);
}
