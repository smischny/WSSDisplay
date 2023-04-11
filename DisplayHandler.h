#pragma once

#include <WebSocketsServer.h>

class DisplayHandler {
   public:
      virtual void connected(WebSocketsServer &server,
                             uint8_t number) = 0;
      virtual void disconnected(WebSocketsServer &server,
                                uint8_t numebr) = 0;
      virtual void text(WebSocketsServer &server,
                        uint8_t number, 
                        uint8_t * payload,
                        size_t length) = 0;
};



