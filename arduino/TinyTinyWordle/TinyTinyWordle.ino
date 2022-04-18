//Change language and dictionary by commenting/uncommenting the following lines:
#include "locale-en.h"
//#include "locale-it.h"

#include "dictionary.h"
#include "userInteraction.h"
#include "game.h"

//#define DEBUG_BUILD

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
