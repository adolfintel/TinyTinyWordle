#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

uint8_t* compress(char* in, int* compressedSize){
    int len=strlen(in);
    *compressedSize=(int)ceil(len*5.0/8.0);
    uint8_t* ret=malloc(*compressedSize);
    for(int i=0;i<*compressedSize;i++){
        ret[i]=0;
    }
    int byteN=0, bitN=0;
    for(int i=0;i<len;i++){
        if(in[i]<'A'||in[i]>'Z'){
            free(ret);
            return NULL;
        }
        uint8_t c=in[i]-'A';
        for(int j=0;j<5;j++){
            if(c&(1<<j)){
                ret[byteN]|=1<<bitN;
            }
            bitN++;
            if(bitN==8){
                byteN++;
                bitN=0;
            }
        }
    }
    return ret;
}

int validate(char* w){
    for(int i=0;i<strlen(w);i++){
        if(w[i]>='a'&&w[i]<='z') w[i]-='a'-'A';
        if(w[i]<'A'||w[i]>'Z') return 0;
        return 1;
    }
}

int main(int argc, char* argv[]){
    if(argc!=2){
        printf("Usage: cookDictionary dictionary.txt\n\nThe dictionary must be a text file with one word per line (can use either \\n or \\r\\n), words must be the same length and contain only uppercase characters between A and Z\n\n");
        return 1;
    }
    FILE *in=fopen(argv[1],"r");
    if(in==NULL){
        printf("Failed to open %s\n",argv[1]);
        return 2;
    }
    char** strings=malloc(1*sizeof(char*));
    char temp[256];
    int len=-1, words=0;
    while(fscanf(in,"%s",temp)!=EOF){
        int l=strlen(temp);
        if(len==-1){
            len=l;
        }else{
            if(l!=len){
                printf("All words must have the same length: \"%s\"\n",temp);
                return 3;
            } 
        }
        if(!validate(temp)){
            printf("Characters not allowed found in word: \"%s\"\n",temp);
            return 4;
        }
        strings=realloc(strings,(words+1)*sizeof(char*));
        strings[words]=malloc((len+1)*sizeof(char));
        strcpy(strings[words],temp);
        words++;
    }
    if(words==0){
        printf("Input file is empty\n");
        return 5;
    }
    fclose(in);
    printf("Sorting %d words\n",words);
    for(int i=0;i<words-1;i++){
        for(int j=i+1;j<words;j++){
            if(strcmp(strings[i],strings[j])>0){
                char* tmp=strings[i];
                strings[i]=strings[j];
                strings[j]=tmp;
            }
        }
    }
    printf("Generating raw stream\n");
    char* data=calloc(words*len+1,sizeof(char));
    int dataPtr=0;
    for(int i=0;i<words;i++){
        memcpy(data+dataPtr,strings[i],len);
        dataPtr+=len;
        free(strings[i]);
    }
    free(strings);
    printf("Compressing %d words of %d characters\n",words,len);
    int compressedSize;
    uint8_t* compressed=compress(data,&compressedSize);
    printf("Compressed to %d bytes\n",compressedSize);
    free(data);
    printf("Writing output files\n");
    char* outFileName;
    outFileName=malloc((strlen(argv[1])+32)*sizeof(char));
    sprintf(outFileName,"%s.cooked.bin",argv[1]);
    FILE *out=fopen(outFileName,"wb");
    if(out==NULL){
        printf("Failed to open %s\n",outFileName);
        return 6;
    }
    sprintf(outFileName,"%s.arduino.h",argv[1]);
    FILE *h=fopen(outFileName,"w");
    if(h==NULL){
        printf("Failed to open %s\n",outFileName);
        return 7;
    }
    fprintf(h,"const PROGMEM uint8_t dictionary[]={");
    uint16_t wordCount16=(uint16_t)words;
    uint8_t wordLength=(uint8_t)len;
    fwrite(&wordCount16,sizeof(uint16_t),1,out);
    fprintf(h,"0x%02x,0x%02x,",wordCount16&0xFF,wordCount16>>8);
    fwrite(&wordLength,sizeof(uint8_t),1,out);
    fprintf(h,"0x%02x,",wordLength);
    fwrite(compressed,sizeof(uint8_t),compressedSize,out);
    for(int i=0;i<compressedSize;i++){
        fprintf(h,i<compressedSize-1?"0x%02x,":"0x%02x",compressed[i]);
    }
    fprintf(h,"};\n");
    fflush(out);
    fclose(out);
    fflush(h);
    fclose(h);
    printf("Done\n");
    return 0;
}
