//This is a barebones stripped-down version of the Adafruit GFX library to save progmem in TinyTinyWordle

#include "glcdfont.c"
#include <avr/pgmspace.h>

MiniAdafruit_GFX::MiniAdafruit_GFX(int16_t w, int16_t h) : WIDTH(w), HEIGHT(h) {
  cursor_y = cursor_x = 0;
  textsize = 1;
  textcolor = textbgcolor = 0xFFFF;
}

void MiniAdafruit_GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  for (int16_t i = x; i < x + w; i++) {
    drawFastVLine(i, y, h, color);
  }
}

void MiniAdafruit_GFX::drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
                            uint16_t color) {
  
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
  
}

void MiniAdafruit_GFX::drawChar(int16_t x, int16_t y, unsigned char c,
                            uint16_t color, uint16_t bg, uint8_t size) {
  drawChar(x, y, c, color, bg, size, size);
}

void MiniAdafruit_GFX::drawChar(int16_t x, int16_t y, unsigned char c,
                            uint16_t color, uint16_t bg, uint8_t size_x,
                            uint8_t size_y) {

  if ((x >= WIDTH) || (y >= HEIGHT) || ((x + 6 * size_x - 1) < 0) || ((y + 8 * size_y - 1) < 0)) return;
#ifdef REDUCED_CHARSET
    if(c>='A'&&c<='Z'){
      c-='A'-14;
    }else if(c>='a'&&c<='z'){
      c-='a'-40;
    }else if(c>='0'&&c<='9'){
      c-='0'-4;
    }else if(c=='_'){
      c=0;
    }else if(c=='!'){
      c=1;
    }else if(c=='/'){
      c=2;
    }else if(c=='>'){
      c=3;
    }else return;
#endif
  for (int8_t i = 0; i < 5; i++) {
    uint8_t line = pgm_read_byte(&font[c*5+i]);
    for (int8_t j = 0; j < 8; j++, line >>= 1) {
        fillRect(x + i * size_x, y + j * size_y, size_x, size_y, line&1?color:bg);
    }
  }
}

size_t MiniAdafruit_GFX::write(uint8_t c) {
  if (c == '\n') {
    cursor_x = 0;
    cursor_y += textsize * 8;
  } else {
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize,
              textsize);
    cursor_x += textsize * 6;
  }
  return 1;
}

void MiniAdafruit_GFX::setTextSize(uint8_t s) { textsize=s<1?1:s; }

