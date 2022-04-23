//Change language and dictionary by commenting/uncommenting the following lines:
#include "locale-en.h"
//#include "locale-it.h"

//Uncomment these to disable non essential things
//#define DISABLE_SPLASH  //uncomment to disable splash screen AND tutorial
//#define DISABLE_TUTORIAL //uncomment to remove the tutorial after the splash screen
//#define DISABLE_COMBOS //uncomment to disable the combo system
//#define DISABLE_ANIMATIONS //uncomment to disable almost all animations
//#define SERIAL_DEBUG //uncomment to receive the solution via serial

#include "dictionary.h"
#include "userInteraction.h"
#include "game.h"

void setup() {
#ifdef SERIAL_DEBUG
  Serial.begin(9600);
#endif
  ioInit();
#ifdef DISABLE_SPLASH
  uint32_t seed=0;
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
