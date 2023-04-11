#include <WSSDisplay.h>

static const char *COMMAND = "command";
static const char *X = "x";
static const char *Y = "y";
static const char *X0 = "x0";
static const char *Y0 = "y0";
static const char *X1 = "x1";
static const char *Y1 = "y1";
static const char *X2 = "x2";
static const char *Y2 = "y2";
static const char *HEIGHT_STR = "height";
static const char *WIDTH_STR = "width";
static const char *ROTATION = "rotation";
static const char *RADIUS = "r";
static const char *CORNERNAME = "cornername";
static const char *MODE = "mode";
static const char *COLOR = "color";
static const char *BG = "bg";
static const char *SIZE = "s";
static const char *CHAR = "c";
static const char *BOOLEAN = "b";
static const char *STRING = "string";
static const char *DELTA = "delta";
static const char *DX = "dx";
static const char *DY = "dy";
static const char *BITMAP = "bitmap";
static const char *MASK = "mask";
static const char *SCRIPT = "script";

static char buffer [100];
static char bufferIdx = 0;

WSSDisplay::WSSDisplay(int16_t w, int16_t h,WebSocketsServer *wss,uint8_t client) : 
    wss(wss) {

    refreshDisplayFunc  = NULL;

    displayWidth = w;
    displayHeight = h;    
    this->client = client;
    x = 0;
    y = 0;
    wrap = true;
    rotation = 0;
    font = NULL;
    fontSize = 1;
    charHeight = 8;
}

void WSSDisplay::setWebSocketsServer(WebSocketsServer *wss) {
   this->wss = wss;
}

void WSSDisplay::sendMessage() {

    //
    // Do we have any un-written character data?
    // If so, send that first.
    //
    if (bufferIdx > 0) {
         flush();
    }
    if (wss) {
      if (client != WSSD_BROADCAST) {
          wss->sendTXT(client,message.getString());
      } else {
          wss->broadcastTXT(message.getString());
      }
   }
}

void WSSDisplay::sendJavaScript(uint8_t clientNum,const char *script) {

    //
    // Do we have any un-written character data?
    // If so, send that first.
    //
    if (bufferIdx > 0) {
         flush();
    }

    if (wss) {

      message.clear()
           .add(COMMAND,(int)RD_JS_SCRIPT)
           .add(SCRIPT,script)
           .end();

      if (clientNum != WSSD_BROADCAST) {
          wss->sendTXT(clientNum,message.getString());
      } else {
          wss->broadcastTXT(message.getString());
      }
   }
    
}

void WSSDisplay::reset() {

    x = 0;
    y = 0;
    if ((rotation % 2) != 0) {
        uint16_t temp = displayHeight;
        displayHeight = displayWidth;
        displayWidth = temp;
    }
    rotation = 0;

    message.clear()
           .add(COMMAND,(int)RD_RESET)
           .end();
    sendMessage();

}

void WSSDisplay::begin() {
    message.clear()
           .add(COMMAND,(int)RD_BEGIN)
           .end();
    sendMessage();
}


void WSSDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
    
    message.clear()
           .add(COMMAND,(int)RD_DRAW_PIXEL)
           .add(X,x)
           .add(Y,y)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::startWrite(void) {
}

void WSSDisplay::writePixel(int16_t x, int16_t y, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_WRITE_PIXEL)
           .add(X,x)
           .add(Y,y)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_WRITE_FILL_RECT)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_WRITE_FAST_VLINE)
           .add(X,x)
           .add(Y,y)
           .add(HEIGHT_STR,h)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {

   message.clear()
           .add(COMMAND,(int)RD_WRITE_FAST_HLINE)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_WRITE_LINE)
           .add(X0,x0)
           .add(Y0,y0)
           .add(X1,x1)
           .add(Y1,y1)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::endWrite(void) {

    message.clear()
           .add(COMMAND,(int)RD_END_WRITE)
           .end();

    sendMessage();
}

void WSSDisplay::setRotation(uint8_t r) {

    message.clear()
           .add(COMMAND,(int)RD_SET_ROTATION)
           .add(ROTATION,r)
           .end();

    // swap the coordinates
    // if switching from landscape to portrait, or vice versa
    if ((rotation % 2) != (r % 2)) {
        uint16_t temp = displayHeight;
        displayHeight = displayWidth;
        displayWidth = temp;
    }
    rotation = r;

    sendMessage();
}

void WSSDisplay::invertDisplay(boolean i) {

    message.clear()
           .add(COMMAND,(int)RD_INVERT_DISPLAY)
           .add(MODE,i)
           .end();

    sendMessage();
}

void WSSDisplay::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_DRAW_FAST_VLINE)
           .add(X,x)
           .add(Y,y)
           .add(HEIGHT_STR,h)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_DRAW_FAST_HLINE)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_FILL_RECT)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::fillScreen(uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_FILL_SCREEN)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    message.clear()
           .add(COMMAND,(int)RD_DRAW_LINE)
           .add(X0,x0)
           .add(Y0,y0)
           .add(X1,x1)
           .add(Y1,y1)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    message.clear()
           .add(COMMAND,(int)RD_DRAW_RECT)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_DRAW_CIRCLE)
           .add(X,x)
           .add(Y,y)
           .add(RADIUS,r)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::drawCircleHelper(int16_t x, int16_t y, int16_t r, uint8_t cornername, uint16_t color) {
    message.clear()
           .add(COMMAND,(int)RD_DRAW_CIRCLE_HELPER)
           .add(X,x)
           .add(Y,y)
           .add(RADIUS,r)
           .add(CORNERNAME,cornername)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_FILL_CIRCLE)
           .add(X,x)
           .add(Y,y)
           .add(RADIUS,r)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::fillCircleHelper(int16_t x, int16_t y, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_FILL_CIRCLE_HELPER)
           .add(X,x)
           .add(Y,y)
           .add(RADIUS,r)
           .add(CORNERNAME,cornername)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_DRAW_TRIANGLE)
           .add(X0,x0)
           .add(Y0,y0)
           .add(X1,x1)
           .add(Y1,y1)
           .add(X2,x2)
           .add(Y2,y2)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

   message.clear()
           .add(COMMAND,(int)RD_FILL_TRIANGLE)
           .add(X0,x0)
           .add(Y0,y0)
           .add(X1,x1)
           .add(Y1,y1)
           .add(X2,x2)
           .add(Y2,y2)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_DRAW_ROUND_RECT)
           .add(X,x0)
           .add(Y,y0)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .add(RADIUS,radius)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

void WSSDisplay::fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color) {

    message.clear()
           .add(COMMAND,(int)RD_FILL_ROUND_RECT)
           .add(X,x0)
           .add(Y,y0)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .add(RADIUS,radius)
           .addColor(COLOR,color)
           .end();

    sendMessage();
}

//
//  1 bit/pixel bitmap
//  Rows are byte alligned
//
void WSSDisplay::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) {

    int16_t size = (w+7)/8*h; 

    message.clear()
           .add(COMMAND,(int)RD_DRAW_BITMAP1)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addColor(COLOR,color)
           .addBinnaryArray(BITMAP,size,bitmap)
           .end();

    sendMessage();
}

void WSSDisplay::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg) {

    int16_t size = (w+7)/8*h;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_BITMAP2)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addColor(COLOR,color)
           .addColor(BG,bg)
           .addBinnaryArray(BITMAP,size,bitmap)
           .end();

    sendMessage();
}

void WSSDisplay::drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

    int16_t size = (w+7)/8*h;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_BITMAP1)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addColor(COLOR,color)
           .addBinnaryArray(BITMAP,size,bitmap)
           .end();

    sendMessage();
}

void WSSDisplay::drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {

    int16_t size = (w+7)/8*h;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_BITMAP2)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addColor(COLOR,color)
           .addColor(BG,bg)
           .addBinnaryArray(BITMAP,size,bitmap)
           .end();

    sendMessage();
}

void WSSDisplay::drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) {

    int16_t size = (w+7)/8*h;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_X_BITMAP)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addColor(COLOR,color)
           .addBinnaryArray(BITMAP,size,bitmap)
           .end();

    sendMessage();
}

void WSSDisplay::drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h) {
    uint16_t size = w*h;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_GRAYSCALE_BITMAP1)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addBinnaryArray(BITMAP,size,bitmap)
           .end();

    sendMessage();

}

void WSSDisplay::drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h) {
    uint16_t size = w*h;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_GRAYSCALE_BITMAP1)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addBinnaryArray(BITMAP,size,bitmap)
           .end();

    sendMessage();
}

void WSSDisplay::drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], const uint8_t mask[], int16_t w, int16_t h) {

    uint16_t size = w*h;
    uint16_t maskSize = (w+7)/8*h;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_GRAYSCALE_BITMAP2)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addBinnaryArray(BITMAP,size,bitmap)
           .addBinnaryArray(MASK,maskSize,mask)
           .end();

    sendMessage();
}

void WSSDisplay::drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask, int16_t w, int16_t h) {

    uint16_t size = w*h;
    uint16_t maskSize = (w+7)/8*h;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_GRAYSCALE_BITMAP2)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addBinnaryArray(BITMAP,size,bitmap)
           .addBinnaryArray(MASK,maskSize,mask)
           .end();

    sendMessage();
}

void WSSDisplay::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) {
    uint16_t size = w*h*2;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_RGB_BITMAP1)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addBinnaryArray(BITMAP,size,(const uint8_t *)bitmap)
           .end();

    sendMessage();
}

void WSSDisplay::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) {
    uint16_t size = w*h*2;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_RGB_BITMAP1)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addBinnaryArray(BITMAP,size,(const uint8_t *)bitmap)
           .end();

    sendMessage();
}

void WSSDisplay::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], const uint8_t mask[], int16_t w, int16_t h) {

    uint16_t size = w*h*2;
    uint16_t maskSize = (w+7)/8*h;

    message.clear()
           .add(COMMAND,(int)RD_DRAW_RGB_BITMAP2)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,w)
           .add(HEIGHT_STR,h)
           .addBinnaryArray(BITMAP,size,(const uint8_t *)bitmap)
           .addBinnaryArray(MASK,maskSize,mask)
           .end();

    sendMessage();
}

void WSSDisplay::drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask, int16_t w, int16_t h) {

}

void WSSDisplay::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
    message.clear()
           .add(COMMAND,(int)RD_DRAW_CHAR)
           .add(X,x)
           .add(Y,y)
           .add(CHAR,c)
           .addColor(COLOR,color)
           .addColor(BG,bg)
           .add(SIZE,size)
           .end();

    sendMessage();
}

void WSSDisplay::setCursor(int16_t x, int16_t y) {

    message.clear()
           .add(COMMAND,(int)RD_SET_CURSOR)
           .add(X,x)
           .add(Y,y)
           .end();


    this->x = x;
    this->y = y;

    sendMessage();
}

void WSSDisplay::setTextColor(uint16_t c) {

    message.clear()
           .add(COMMAND,(int)RD_SET_TEXT_COLOR1)
           .addColor(COLOR,c)
           .end();

    sendMessage();
}

void WSSDisplay::setTextColor(uint16_t c, uint16_t bg) {

    message.clear()
           .add(COMMAND,(int)RD_SET_TEXT_COLOR2)
           .addColor(COLOR,c)
           .addColor(BG,bg)
           .end();

    sendMessage();
}

void WSSDisplay::setTextSize(uint8_t s) {

    message.clear()
           .add(COMMAND,(int)RD_SET_TEXT_SIZE)
           .add(SIZE,s)
           .end();

    fontSize = s;

    sendMessage();
}

void WSSDisplay::setTextWrap(boolean w) {

    message.clear()
           .add(COMMAND,(int)RD_SET_TEXT_WRAP)
           .add(BOOLEAN,w)
           .end();

    wrap = w;
    sendMessage();
}

void WSSDisplay::cp437(boolean x) {

    message.clear()
           .add(COMMAND,(int)RD_CP437)
           .add(BOOLEAN,x)
           .end();

    sendMessage();
}

void WSSDisplay::vertScroll(uint16_t y, int16_t height, int16_t offset) {

    message.clear()
           .add(COMMAND,(int)RD_VSCROLL)
           .add(Y,y)
           .add(HEIGHT_STR,height)
           .add(DELTA,offset)
           .end();

    sendMessage();
}

void WSSDisplay::horzScroll(uint16_t x, uint16_t width, uint16_t offset) {

    message.clear()
           .add(COMMAND,(int)RD_HSCROLL)
           .add(X,x)
           .add(WIDTH_STR,width)
           .add(DELTA,offset)
           .end();

    sendMessage();
}

void WSSDisplay::scroll(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t dx, uint16_t dy) {
    message.clear()
           .add(COMMAND,(int)RD_SCROLL)
           .add(X,x)
           .add(Y,y)
           .add(WIDTH_STR,width)
           .add(HEIGHT_STR,height)
           .add(DX,dx)
           .add(DY,dy)
           .end();

    sendMessage();
}

void WSSDisplay::setFont(const GFXfont *f) {
    //int16_t size = 0;

    //message.command = RD_SET_FONT;

    //if (f != NULL) {
    //    this->font = f;
    //    charHeight = f->yAdvance ;
    //} else {
    //    charHeight = 8; 
    //}
    //sendMessage(f);
}

uint16_t WSSDisplay::charWidth(const char c) {

    if (!font) {
       return (6 * fontSize);
    } else {
       uint16_t size = 0;

       if ((c >= font->first) && (c <= font->last)) {
           const GFXglyph &glyph = font->glyph[c-font->first];
           size = glyph.width * fontSize;
       } 

       return (size); 
    }
}

void WSSDisplay::flush() {

    if (bufferIdx > 0) {

        flushMessage.clear()
               .add(COMMAND,(int)RD_DATA_MSG)
               .add(STRING,buffer)
               .end();

        if (wss) { 
            if (client != WSSD_BROADCAST) {
                wss->sendTXT(client,flushMessage.getString());
            } else {
                wss->broadcastTXT(flushMessage.getString());
            }
        }

        bufferIdx = 0;
        memset(buffer,0,sizeof(buffer));

    }
}

size_t WSSDisplay::write(uint8_t c) {

    if (bufferIdx <= (sizeof(buffer)-1)) {
       if (c != '\n') {
           buffer[bufferIdx++] = c;
       }
    }

    if (bufferIdx == (sizeof(buffer)-1) ) {
        flush();
    }

    switch (c) {
       case '\n':
           x = 0;
           y = y + (charHeight * fontSize);
           flush();
           setCursor(x,y);
           break;
       case '\r':
           // do nothing for CR
           break;
       default:
           uint16_t width = charWidth(c);
           x +=  width;
           if ((wrap) && (x >= displayWidth)) {
               x = width;
               y += (charHeight * fontSize);
           }
           break;
    }
    return (1);
}

int16_t WSSDisplay::height(void) const {
     return (displayHeight);
}

int16_t WSSDisplay::width(void) const {
    return (displayWidth);
}

uint8_t WSSDisplay::getRotation(void) const {
    return (rotation);
}

int16_t WSSDisplay::getCursorX(void) const {
    return (x);
}

int16_t WSSDisplay::getCursorY(void) const {
    return (y);
}


void WSSDisplay::getTextBounds(const char *string, int16_t x, int16_t y, 
                               int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {

   *x1 = x;  
   *y1 = y;
   *w = strlen(string)*4*fontSize;
   *h = 8 * fontSize;
}

//
// DisplayHandler functions
//
void WSSDisplay::connected(WebSocketsServer &server,
                           uint8_t number) {
   wss = &server;
   if (refreshDisplayFunc) {
       refreshDisplayFunc(number);
   }
}

void WSSDisplay::disconnected(WebSocketsServer &server,
                              uint8_t numebr) {
 if (!server.connectedClients()) {
    wss = NULL;
 }  
}

void WSSDisplay::text(WebSocketsServer &server,
                      uint8_t number,
                      uint8_t * payload,
                      size_t length) {
   Serial.println((char *)payload);
}


