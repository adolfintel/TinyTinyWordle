//Change language and dictionary by commenting/uncommenting the following lines:
//#include "locale-en.h"
#include "locale-it.h"

//Uncomment this to disable all non essential things (splash screen, tutorial, transitions, combos)
//#define MINIMAL_BUILD

#include "dictionary.h"
#include "userInteraction.h"
#include "game.h"

void setup() {
  ioInit();
#ifdef MINIMAL_BUILD
  uint8_t seed=0;
  for(uint8_t i=0;i<32;i++){
    seed=(seed<<1)^analogRead(0);
  }
  randomSeed(seed);
#else
  splashScreen();
  randomSeed(micros());
#endif
  dictionaryInit();
}

void loop(){
  gameLoop();
}
