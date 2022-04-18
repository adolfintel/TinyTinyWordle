//This is a barebones stripped-down version of the Adafruit GFX library to save progmem in TinyTinyWordle

#ifndef _MiniAdafruit_GFX_H
#define _MiniAdafruit_GFX_H

#include "Arduino.h"
#include "Print.h"

class MiniAdafruit_GFX : public Print {

public:
  MiniAdafruit_GFX(int16_t w, int16_t h);

  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                        uint16_t color);
  virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h,
                        uint16_t color);

  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                uint16_t bg, uint8_t size);
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                uint16_t bg, uint8_t size_x, uint8_t size_y);
  void setTextSize(uint8_t s);

  void setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  }

  void setTextColor(uint16_t c) { textcolor = textbgcolor = c; }

  void setTextColor(uint16_t c, uint16_t bg) {
    textcolor = c;
    textbgcolor = bg;
  }

  using Print::write;
  virtual size_t write(uint8_t);

  int16_t width(void) const { return WIDTH; };
  int16_t height(void) const { return HEIGHT; }
  
  int16_t getCursorX(void) const { return cursor_x; }
  int16_t getCursorY(void) const { return cursor_y; };

protected:
  void charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx,
                  int16_t *miny, int16_t *maxx, int16_t *maxy);
  int16_t WIDTH;
  int16_t HEIGHT;
  int16_t cursor_x;
  int16_t cursor_y;
  uint16_t textcolor;
  uint16_t textbgcolor;
  uint8_t textsize;
};

#include "MiniAdafruit_GFX.cpp"

#endif // _MiniAdafruit_GFX_H
