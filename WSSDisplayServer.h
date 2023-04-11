#pragma once

#include <WebServer.h>
#include <WebSocketsServer.h>
#include "DisplayHandler.h"

//
// A stock implementation of a WebServer
// and WebSocketsServer to implement a
// Web Display (emulated TFT in js).   It uses
// the default index_html.h and display_js.h
// which create a 480x320 canvas that communicates
// using port 81. Both can be changed
// by providing your own index.html file.
//
class WSSDisplayServer : public WebServer {

  public:
     WSSDisplayServer(DisplayHandler &handler,
                      int webServerPort = 80,
                      int webSocketsServerPort = 81);

     ~WSSDisplayServer();

     void handleClient();

     void begin();

     void onNotFound(THandlerFunction fn);

  private:

     THandlerFunction         notFoundFunction;
     WebSocketsServer         wsServer;
     int                      connectionCount;
     DisplayHandler          &displayHandler;

     static WSSDisplayServer *instance; 

     static void sWssEventHandler(uint8_t num,
                                 WStype_t type,
                                 uint8_t * payload,
                                 size_t length);

     void wssEventHandler(uint8_t num,
                          WStype_t type,
                          uint8_t * payload,
                          size_t length);

     static bool handleDisplayURLS(String path);
     
     static void handleNotFound();
};



