//Change language and dictionary by commenting/uncommenting the following lines:
#include "locale-en.h"
//#include "locale-it.h"

#include "dictionary.h"
#include "userInteraction.h"
#include "game.h"

void setup() {
  ioInit();
  splashScreen();
  randomSeed(micros());
  dictionaryInit();
}

void loop(){
  gameLoop();
}
