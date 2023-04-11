#ifndef _WSS_DISPLAY_H
#define _WSS_DISPLAY_H

#include <Arduino.h>
#include <WebSocketsServer.h>
#include "DisplayHandler.h"
#include <gfxfont.h>
#include <JSONString.h>
#include <CommonDisplay.h>
#include <Adafruit_GFX.h>

#define WSSD_BROADCAST  255

class WSSDisplay : public Print, public DisplayHandler {

 public:

  WSSDisplay(int16_t w = 480, int16_t h = 320,WebSocketsServer *wss=NULL,uint8_t client = WSSD_BROADCAST);

  void setWebSocketsServer(WebSocketsServer *wss);

  uint8_t  getClientNum() { return (client); };

  void     reset();
  void     begin();

  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void startWrite(void);
  void writePixel(int16_t x, int16_t y, uint16_t color);
  void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void endWrite(void);
  virtual void setRotation(uint8_t r);
  virtual void invertDisplay(boolean i);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fillScreen(uint16_t color);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

  void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
  void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
  void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
  void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
  void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
  void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
  void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg);
  void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
  void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);
  void drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);
  void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h);
  void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h);
  void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], const uint8_t mask[], int16_t w, int16_t h);
  void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], const uint8_t mask[], int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h);
  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
  void setCursor(int16_t x, int16_t y);
  void setTextColor(uint16_t c);
  void setTextColor(uint16_t c, uint16_t bg);
  void setTextSize(uint8_t s);
  void setTextWrap(boolean w);
  void cp437(boolean x=true);
  void setFont(const GFXfont *f = NULL);

  void vertScroll(uint16_t y, int16_t height, int16_t offset);
  void horzScroll(uint16_t x, uint16_t width, uint16_t offset);
  void scroll(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t dx, uint16_t dy);

  size_t write(uint8_t);
  void   flush();

  uint16_t color565(uint8_t r, uint8_t g, uint8_t b) 
    { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3); }

  int16_t height(void) const;
  int16_t width(void) const;
  uint8_t getRotation(void) const;
  int16_t getCursorX(void) const;
  int16_t getCursorY(void) const;

  void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1,
                     int16_t *y1, uint16_t *w, uint16_t *h);

  //
  // DisplayHandler functions
  //
  virtual void connected(WebSocketsServer &server,
                         uint8_t number);
  virtual void disconnected(WebSocketsServer &server,
                            uint8_t numebr);
  virtual void text(WebSocketsServer &server,
                    uint8_t number,
                    uint8_t * payload,
                    size_t length);

  //
  // Set a callback function called each time a 
  // client is connected to refresh the
  // newly attached display.
  //
  void refreshDisplayFunction(void (*func)(uint8_t)) {
       refreshDisplayFunc = func;
  }

  void sendJavaScript(uint8_t clientNum,const char *script);

   private:

       void sendMessage();

       uint16_t   charWidth(const char c);

       uint16_t   displayWidth;
       uint16_t   displayHeight;
       uint16_t   x;
       uint16_t   y;
       boolean    wrap;
       uint8_t    rotation;
       const GFXfont   *font;
       uint8_t    fontSize;
       uint8_t    charHeight;

       WebSocketsServer *wss;
       uint8_t           client;
       JSONString        message;
       JSONString        flushMessage;

       void (*refreshDisplayFunc)(uint8_t);
};

// New color definitions.  thanks to Bodmer
#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
#define TFT_PINK        0xFC9F

#endif
