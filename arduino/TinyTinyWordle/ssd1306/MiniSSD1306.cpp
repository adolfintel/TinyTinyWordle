//This is a barebones stripped-down version of the Adafruit SSD1306 library to save progmem in TinyTinyWordle

#include <avr/pgmspace.h>

#define WIRE_MAX 32

#define ssd1306_swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

MiniSSD1306::MiniSSD1306(TwoWire *twi)
    : MiniAdafruit_GFX(128,64), wire(twi), buffer(NULL)
{
}

MiniSSD1306::~MiniSSD1306(void) {
  if(buffer){
    free(buffer);
    buffer = NULL;
  }
}

void MiniSSD1306::ssd1306_command1(uint8_t c) {
  wire->beginTransmission(i2caddr);
  wire->write((uint8_t)0x00);
  wire->write(c);
  wire->endTransmission();
}

void MiniSSD1306::ssd1306_commandList(const uint8_t *c, uint8_t n) {
  wire->beginTransmission(i2caddr);
  wire->write((uint8_t)0x00);
  uint16_t bytesOut = 1;
  while (n--) {
    if (bytesOut >= WIRE_MAX) {
      wire->endTransmission();
      wire->beginTransmission(i2caddr);
      wire->write((uint8_t)0x00);
      bytesOut = 1;
    }
    wire->write(pgm_read_byte(c++));
    bytesOut++;
  }
  wire->endTransmission();
}

void MiniSSD1306::ssd1306_command(uint8_t c) {
  ssd1306_command1(c);
}

bool MiniSSD1306::begin(uint8_t vcs, uint8_t addr, uint32_t clk) {

  if ((!buffer) && !(buffer = (uint8_t *)malloc(128 * ((64 + 7) / 8))))
    return false;

  clearDisplay();

  i2caddr = addr;
    
  wire->begin();
  wire->setClock(clk);

  // Init sequence
  static const uint8_t PROGMEM init1[] = {SSD1306_DISPLAYOFF,         // 0xAE
                                          SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
                                          0x80, // the suggested ratio 0x80
                                          SSD1306_SETMULTIPLEX}; // 0xA8
  ssd1306_commandList(init1, sizeof(init1));
  ssd1306_command1(64 - 1);

  static const uint8_t PROGMEM init2[] = {SSD1306_SETDISPLAYOFFSET, // 0xD3
                                          0x0,                      // no offset
                                          SSD1306_SETSTARTLINE | 0x0, // line #0
                                          SSD1306_CHARGEPUMP};        // 0x8D
  ssd1306_commandList(init2, sizeof(init2));

  ssd1306_command1((vcs == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

  static const uint8_t PROGMEM init3[] = {SSD1306_MEMORYMODE, // 0x20
                                          0x00, // 0x0 act like ks0108
                                          SSD1306_SEGREMAP | 0x1,
                                          SSD1306_COMSCANDEC};
  ssd1306_commandList(init3, sizeof(init3));

  uint8_t comPins = 0x12;

  ssd1306_command1(SSD1306_SETCOMPINS);
  ssd1306_command1(comPins);
  ssd1306_command1(SSD1306_SETCONTRAST);
  ssd1306_command1((vcs == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF);

  ssd1306_command1(SSD1306_SETPRECHARGE); // 0xd9
  ssd1306_command1((vcs == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
  static const uint8_t PROGMEM init5[] = {
      SSD1306_SETVCOMDETECT, // 0xDB
      0x40,
      SSD1306_DISPLAYALLON_RESUME, // 0xA4
      SSD1306_NORMALDISPLAY,       // 0xA6
      SSD1306_DEACTIVATE_SCROLL,
      SSD1306_DISPLAYON}; // Main screen turn on
  ssd1306_commandList(init5, sizeof(init5));

  return true; // Success
}

void MiniSSD1306::clearDisplay(void) {
  memset(buffer, 0, 128 * ((64 + 7) / 8));
}

void MiniSSD1306::drawFastHLine(int16_t x, int16_t y, int16_t w,
                                             uint16_t color) {

  if ((y >= 0) && (y < 64)) { // Y coord in bounds?
    if (x < 0) {                  // Clip left
      w += x;
      x = 0;
    }
    if ((x + w) > 128) { // Clip right
      w = (128 - x);
    }
    if (w > 0) { // Proceed only if width is positive
      uint8_t *pBuf = &buffer[(y / 8) * 128 + x], mask = 1 << (y & 7);
      switch (color) {
      case SSD1306_WHITE:
        while (w--) {
          *pBuf++ |= mask;
        };
        break;
      case SSD1306_BLACK:
        mask = ~mask;
        while (w--) {
          *pBuf++ &= mask;
        };
        break;
      case SSD1306_INVERSE:
        while (w--) {
          *pBuf++ ^= mask;
        };
        break;
      }
    }
  }
}

void MiniSSD1306::drawFastVLine(int16_t x, int16_t __y,
                                             int16_t __h, uint16_t color) {

  if ((x >= 0) && (x < 128)) { // X coord in bounds?
    if (__y < 0) {               // Clip top
      __h += __y;
      __y = 0;
    }
    if ((__y + __h) > 64) { // Clip bottom
      __h = (64 - __y);
    }
    if (__h > 0) { // Proceed only if height is now positive
      // this display doesn't need ints for coordinates,
      // use local byte registers for faster juggling
      uint8_t y = __y, h = __h;
      uint8_t *pBuf = &buffer[(y / 8) * 128 + x];

      // do the first partial byte, if necessary - this requires some masking
      uint8_t mod = (y & 7);
      if (mod) {
        mod = 8 - mod;
        static const uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE};
        uint8_t mask = premask[mod];
        // adjust the mask if we're not going to reach the end of this byte
        if (h < mod)
          mask &= (0XFF >> (mod - h));
        
        switch (color) {
        case SSD1306_WHITE:
          *pBuf |= mask;
          break;
        case SSD1306_BLACK:
          *pBuf &= ~mask;
          break;
        case SSD1306_INVERSE:
          *pBuf ^= mask;
          break;
        }
        pBuf += 128;
      }

      if (h >= mod) { // More to go?
        h -= mod;
        // Write solid bytes while we can - effectively 8 rows at a time
        if (h >= 8) {
          if (color == SSD1306_INVERSE) {
            // separate copy of the code so we don't impact performance of
            // black/white write version with an extra comparison per loop
            do {
              *pBuf ^= 0xFF; // Invert byte
              pBuf += 128; // Advance pointer 8 rows
              h -= 8;        // Subtract 8 rows from height
            } while (h >= 8);
          } else {
            // store a local value to work with
            uint8_t val = (color != SSD1306_BLACK) ? 255 : 0;
            do {
              *pBuf = val;   // Set byte
              pBuf += 128; // Advance pointer 8 rows
              h -= 8;        // Subtract 8 rows from height
            } while (h >= 8);
          }
        }

        if (h) { // Do the final partial byte, if necessary
          mod = h & 7;
          // this time we want to mask the low bits of the byte,
          // vs the high bits we did above
          static const uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F};
          uint8_t mask = postmask[mod];
          switch (color) {
          case SSD1306_WHITE:
            *pBuf |= mask;
            break;
          case SSD1306_BLACK:
            *pBuf &= ~mask;
            break;
          case SSD1306_INVERSE:
            *pBuf ^= mask;
            break;
          }
        }
      }
    } // endif positive height
  }   // endif x in bounds
}

void MiniSSD1306::display(void) {
  static const uint8_t PROGMEM dlist1[] = {
      SSD1306_PAGEADDR,
      0,                      // Page start address
      0xFF,                   // Page end (not really, but works here)
      SSD1306_COLUMNADDR, 0}; // Column start address
  ssd1306_commandList(dlist1, sizeof(dlist1));
  ssd1306_command1(128 - 1); // Column end address

  uint16_t count = 128 * ((64 + 7) / 8);
  uint8_t *ptr = buffer;
  wire->beginTransmission(i2caddr);
  wire->write((uint8_t)0x40);
  uint16_t bytesOut = 1;
  while (count--) {
    if (bytesOut >= WIRE_MAX) {
      wire->endTransmission();
      wire->beginTransmission(i2caddr);
      wire->write((uint8_t)0x40);
      bytesOut = 1;
    }
    wire->write(*ptr++);
    bytesOut++;
  }
  wire->endTransmission();
}
