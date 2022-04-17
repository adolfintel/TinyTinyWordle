//#define DEBUG_BUILD

#include "locale-en.h"

#include "dictionary.h"
#include "userInteraction.h"
#include "game.h"

void setup() {
  ioInit();
  splashScreen();
  randomSeed(micros());
  dictionaryInit();
  #ifdef DEBUG_BUILD
  Serial.begin(9600);
  #endif
}

void loop(){
  gameLoop();
}
