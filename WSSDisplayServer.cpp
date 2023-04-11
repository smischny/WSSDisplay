#include <WSSDisplayServer.h>
#include "display_js.h"
#include "index_html.h"

WSSDisplayServer * WSSDisplayServer::instance;

WSSDisplayServer::WSSDisplayServer(DisplayHandler &handler,
                                   int webServerPort,
                                   int webSocketsServerPort)
    : WebServer(webServerPort),
      wsServer(webSocketsServerPort),
      displayHandler(handler) {

  instance = this;
};

WSSDisplayServer::~WSSDisplayServer() { instance = NULL; }

void WSSDisplayServer::handleClient() {
  WebServer::handleClient();
  wsServer.loop();
};

void WSSDisplayServer::begin() {

  WebServer::onNotFound(handleNotFound);

  WebServer::begin();

  wsServer.begin();
  wsServer.onEvent(sWssEventHandler);
}

void WSSDisplayServer::onNotFound(THandlerFunction fn) {
  //
  // Since we have installed our own not
  // found funciton handler, save
  // off this function and call it from ours
  //
  this->notFoundFunction = fn;
}

void WSSDisplayServer::sWssEventHandler(uint8_t num, WStype_t type,
                                        uint8_t *payload, size_t length) {

  if (instance) {
    instance->wssEventHandler(num, type, payload, length);
  }
}

void WSSDisplayServer::wssEventHandler(uint8_t num, WStype_t type,
                                       uint8_t *payload, size_t length) {
  switch (type) {
  case WStype_DISCONNECTED:
    connectionCount--;
    displayHandler.disconnected(wsServer, num);
    break;
  case WStype_CONNECTED:
    connectionCount++;
    displayHandler.connected(wsServer, num);
    break;
  case WStype_TEXT:
    displayHandler.text(wsServer, num, payload, length);
    break;
  }
};

bool WSSDisplayServer::handleDisplayURLS(String path) {

  if (path.equals("/index.html") || path.equals("/")) {
    String text(FPSTR(indexHtml));
    instance->send(200, "text/html", text);
    return true;
  }
  if (path.equals("/display.js")) {
    String text(FPSTR(displayJS));
    instance->send(200, "text/javascript", text);
    return true;
  }
  return (false);
};

void WSSDisplayServer::handleNotFound() {
  if (instance) {
      if (!handleDisplayURLS(instance->uri())) {
        if (instance->notFoundFunction) {
          instance->notFoundFunction();
        } else {
          instance->send(404, "text/plain", "FileNotFound");
        }
      }
   }
};
