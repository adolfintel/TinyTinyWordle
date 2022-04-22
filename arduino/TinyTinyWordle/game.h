#define ATTEMPTS 6

bool check(char* in, char* solution, uint8_t len, uint8_t* colorout){
    for(uint8_t i=0;i<len;i++){
        if(in[i]==solution[i]) colorout[i]=2; else colorout[i]=0;
    }
    for(uint8_t i=0;i<len;i++){
        if(colorout[i]==0){
            for(uint8_t j=0;j<len;j++){
                if(i==j) continue;
                if(in[i]==solution[j]){
                    int8_t ctr=0;
                    for(uint8_t k=0;k<len;k++){
                        if(solution[k]==solution[j]) ctr++;
                        if(in[k]==solution[j]&&colorout[k]!=0) ctr--;
                    }
                    if(ctr>0) colorout[i]=1;
                }
            }
        }
    }
    for(uint8_t i=0;i<len;i++){
        if(colorout[i]!=2) return false;
    }
    return true;
}

bool game(){
  clearDisplay();
  char sol[wordLength+1], userWord[wordLength+1];
  uint8_t colors[wordLength];
  getRandomWord(sol);
  for(uint8_t attempt=0;attempt<ATTEMPTS;attempt++){
    input(userWord,wordLength,attempt+1,ATTEMPTS);
    if(isWordInDictionary(userWord)){
      uint8_t solved=check(userWord,sol,wordLength,colors);
      output(userWord,colors,attempt,ATTEMPTS,wordLength);
      if(solved){
        win(sol,wordLength);
        return true;
      }
    }else{
      notInDictionary();
      attempt--;
    }
  }
  lose(sol,wordLength);
  return false;
}

#ifdef DISABLE_COMBOS
void gameLoop(){
  game();
}
#else
uint8_t comboCounter=0;
void gameLoop() {
  if(game()) comboCounter++; else comboCounter=0;
  if(comboCounter>=3){
    combo(comboCounter);
  }
}
#endif
