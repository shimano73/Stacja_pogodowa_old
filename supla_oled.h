#ifndef SUPLA_OLED_H
#define SUPLA_OLED_H

//https://www.online-utility.org/image/convert/to/XBM
#define supla_logo_width 48
#define supla_logo_height 48
const uint8_t supla_logo_bits[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0x00,
  0x00, 0xFC, 0x3F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x7F, 0x00, 0x00, 0x00,
  0x00, 0x1F, 0xFC, 0x00, 0x00, 0x00, 0x80, 0x07, 0xF0, 0x01, 0x00, 0x00,
  0xC0, 0x03, 0xE0, 0x01, 0x00, 0x00, 0xC0, 0x01, 0xC0, 0x03, 0x00, 0x00,
  0xE0, 0x01, 0x80, 0x03, 0x00, 0x00, 0xE0, 0x00, 0x80, 0x03, 0x00, 0x00,
  0xE0, 0x00, 0x80, 0x03, 0x00, 0x00, 0xE0, 0x01, 0xC0, 0x01, 0x00, 0x00,
  0xC0, 0x03, 0xC0, 0x03, 0x00, 0x00, 0x80, 0x07, 0xE0, 0x07, 0x00, 0x00,
  0x80, 0x0F, 0xF0, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0xF8, 0x3E, 0x00, 0x00,
  0x00, 0xFE, 0x7F, 0xF8, 0x00, 0x00, 0x00, 0xFC, 0x3F, 0xE0, 0x03, 0x00,
  0x00, 0xF0, 0x0F, 0x80, 0x87, 0x07, 0x00, 0x00, 0x0E, 0x00, 0xEE, 0x0F,
  0x00, 0x00, 0x1E, 0x00, 0x7C, 0x1C, 0x00, 0x00, 0x1C, 0x00, 0x38, 0x38,
  0x00, 0x00, 0x1C, 0x00, 0x1C, 0x70, 0x00, 0x00, 0x1C, 0x00, 0x1C, 0x70,
  0x00, 0x00, 0x18, 0x00, 0x38, 0x38, 0x00, 0x00, 0x38, 0x00, 0x70, 0x1C,
  0x00, 0x00, 0x38, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0x30, 0x00, 0x9C, 0x07,
  0x00, 0x00, 0x30, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x70, 0x00, 0x06, 0x00,
  0x00, 0x00, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00, 0x60, 0x80, 0x01, 0x00,
  0x00, 0x00, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x60, 0x00, 0x00,
  0x00, 0x00, 0xC0, 0x37, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x1F, 0x00, 0x00,
  0x00, 0x00, 0xE0, 0x38, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x00, 0x00,
  0x00, 0x00, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00, 0x30, 0x60, 0x00, 0x00,
  0x00, 0x00, 0x70, 0x70, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x38, 0x00, 0x00,
  0x00, 0x00, 0xC0, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define temp_width 32
#define temp_height 32
const uint8_t  temp_bits[] = {
  0x00, 0x3C, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0xC3, 0x00, 0x00,
  0x00, 0x81, 0xF8, 0x03, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00,
  0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0xF8, 0x03, 0x00, 0x99, 0x00, 0x00,
  0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0xF8, 0x03,
  0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00,
  0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00,
  0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00, 0x00, 0x99, 0x00, 0x00,
  0x80, 0x99, 0x01, 0x00, 0xC0, 0x18, 0x03, 0x00, 0x60, 0x3C, 0x06, 0x00,
  0x20, 0x7E, 0x04, 0x00, 0x20, 0x7E, 0x04, 0x00, 0x20, 0x7E, 0x04, 0x00,
  0x60, 0x3C, 0x06, 0x00, 0xC0, 0x18, 0x03, 0x00, 0x80, 0x81, 0x01, 0x00,
  0x00, 0xC3, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00
};

#define humidity_width 32
#define humidity_height 32
const uint8_t humidity_bits[] = {
  0x00, 0x80, 0x01, 0x00, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00,
  0x00, 0x30, 0x0C, 0x00, 0x00, 0x38, 0x1C, 0x00, 0x00, 0x18, 0x18, 0x00,
  0x00, 0x0C, 0x30, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x06, 0x40, 0x00,
  0x00, 0x03, 0xC0, 0x00, 0x80, 0x01, 0x80, 0x00, 0x80, 0x01, 0x80, 0x01,
  0xC0, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x03, 0x60, 0x30, 0x08, 0x06,
  0x30, 0x48, 0x0C, 0x0C, 0x30, 0x48, 0x06, 0x0C, 0x30, 0x30, 0x03, 0x0C,
  0x30, 0x80, 0x01, 0x0C, 0x30, 0xC0, 0x00, 0x0C, 0x30, 0x60, 0x06, 0x0C,
  0x30, 0x30, 0x09, 0x0C, 0x30, 0x10, 0x09, 0x0C, 0x30, 0x00, 0x06, 0x0C,
  0x20, 0x00, 0x00, 0x04, 0x60, 0x00, 0x00, 0x06, 0xC0, 0x00, 0x00, 0x03,
  0x80, 0x01, 0x80, 0x01, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x0E, 0x70, 0x00,
  0x00, 0xFC, 0x3F, 0x00, 0x00, 0xF0, 0x0F, 0x00
};

#define pressure_width 32
#define pressure_height 32
const uint8_t pressure_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x0F, 0x00,
  0x00, 0xFC, 0x3F, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00,
  0x80, 0x81, 0x81, 0x01, 0xC0, 0x80, 0x01, 0x03, 0x60, 0x80, 0x01, 0x06,
  0x30, 0x00, 0xC0, 0x0C, 0x30, 0x03, 0xE0, 0x18, 0x18, 0x07, 0x40, 0x10,
  0x0C, 0x82, 0x01, 0x30, 0x0C, 0x80, 0x01, 0x30, 0x0C, 0x80, 0x01, 0x30,
  0x0C, 0x80, 0x01, 0x30, 0x0C, 0x80, 0x01, 0x30, 0x0C, 0x81, 0x81, 0x30,
  0x8C, 0x83, 0xC1, 0x31, 0x8C, 0x81, 0x81, 0x31, 0x08, 0xC0, 0x03, 0x10,
  0x08, 0xE0, 0x07, 0x10, 0x00, 0xE0, 0x07, 0x00, 0x00, 0xC0, 0x03, 0x00,
  0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t activeSymbol[] PROGMEM = {
  B00000000,
  B00000000,
  B00011000,
  B00100100,
  B01000010,
  B01000010,
  B00100100,
  B00011000
};

const uint8_t inactiveSymbol[] PROGMEM = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00011000,
  B00011000,
  B00000000,
  B00000000
};

void supla_oled_start(void);
void supla_oled_timer(void);

#endif //SUPLA_OLED_H