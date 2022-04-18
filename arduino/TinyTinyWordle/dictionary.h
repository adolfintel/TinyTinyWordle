uint16_t wordsInDictionary=0;
uint8_t wordLength=0;

void decompress(uint8_t* in, uint32_t startFrom, uint8_t len, char* ret){
    startFrom*=5;
    uint16_t byteN=startFrom/8;
    uint8_t bitN=startFrom%8;
    uint8_t b=pgm_read_byte_near(in+byteN);
    for(uint8_t i=0;i<len;i++){
      uint8_t c=0;
      for(uint8_t j=0;j<5;j++){
        if(b&(1<<bitN)){
          c|=(1<<j);
        }
        bitN++;
        if(bitN==8){
          byteN++;
          b=pgm_read_byte_near(in+byteN);
          bitN=0;
        }
      }
      ret[i]='A'+c;
    }
    ret[len]='\0';
}

void dictionaryInit(){
  wordsInDictionary=pgm_read_byte_near(dictionary+0)|(pgm_read_byte_near(dictionary+1)<<8);
  wordLength=pgm_read_byte_near(dictionary+2);
}

void getRandomWord(char* w){
  uint16_t r=random(0,wordsInDictionary);
  decompress(dictionary+3,r*wordLength,wordLength,w);
}

bool isWordInDictionary(char* w){
  char tmp[wordLength+1];
  uint16_t l=0, r=wordsInDictionary-1;
  while(l<=r){
    uint16_t m=l+(r-l)/2;
    decompress(dictionary+3,m*wordLength,wordLength,tmp);
    int cmp=strcmp(w,tmp);
    if(cmp>0){
      l=m+1;
      if(l<0) break;
    }else if(cmp<0){
      r=m-1;
      if(r>=wordsInDictionary) break;
    }else{
      return true;
    }
  }
  return false;
}
