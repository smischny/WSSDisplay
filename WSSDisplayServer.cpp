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

  on("/",HTTP_GET,onIndexHtml);
  on("/index.html",HTTP_GET,onIndexHtml);
  on("/display.js",HTTP_GET,onDisplayJS);
  on("/favicon.ico",HTTP_GET,onNotFound);
  WebServer::begin();

  wsServer.begin();
  wsServer.onEvent(sWssEventHandler);
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
}

void WSSDisplayServer::onIndexHtml() {
    String text(FPSTR(indexHtml));
    instance->send(200, "text/html", text);
}

void WSSDisplayServer::onDisplayJS() {
    String text(FPSTR(displayJS));
    instance->send(200, "text/javascript", text);
}

void WSSDisplayServer::onNotFound() {
    instance->send(404, "text/plain", "FileNotFound");
}

