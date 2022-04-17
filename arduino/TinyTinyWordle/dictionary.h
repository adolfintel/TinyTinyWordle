uint32_t wordsInDictionary=0;
uint8_t wordLength=0;

void decompress(uint8_t* in, uint32_t startFrom, uint8_t len, char* ret){
    #define DECOMPRESS_MACRO_NEXTBIT \
      bitN++; \
      if(bitN==8){ \
        byteN++; \
        b=pgm_read_byte_near(in+byteN); \
        bitN=0; \
        mask=1; \
    }
    startFrom*=5;
    uint32_t byteN=startFrom/8;
    uint8_t bitN=startFrom%8;
    uint8_t b=pgm_read_byte_near(in+byteN);
    uint8_t mask=1<<bitN;
    for(uint8_t i=0;i<len;i++){
        uint8_t c=0;
        if(b&mask){
            c|=0x01;
        }
        mask<<=1;
        DECOMPRESS_MACRO_NEXTBIT
        if(b&mask){
            c|=0x02;
        }
        mask<<=1;
        DECOMPRESS_MACRO_NEXTBIT
        if(b&mask){
            c|=0x04;
        }
        mask<<=1;
        DECOMPRESS_MACRO_NEXTBIT
        if(b&mask){
            c|=0x08;
        }
        mask<<=1;
        DECOMPRESS_MACRO_NEXTBIT
        if(b&mask){
            c|=0x10;
        }
        mask<<=1;
        DECOMPRESS_MACRO_NEXTBIT
        ret[i]='A'+c;
    }
    ret[len]='\0';
}

void dictionaryInit(){
  wordsInDictionary=pgm_read_byte_near(dictionary+0)|(pgm_read_byte_near(dictionary+1)<<8);
  wordLength=pgm_read_byte_near(dictionary+2);
}

void getRandomWord(char* w){
  uint32_t r=random(0,wordsInDictionary);
  decompress(dictionary+3,r*wordLength,wordLength,w);
}

bool isWordInDictionary(char* w){
  char tmp[wordLength+1];
  uint32_t l=0, r=wordsInDictionary-1;
  while(l<=r){
    uint32_t m=l+(r-l)/2;
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
