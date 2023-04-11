# WSSDisplay
This project emulates a TFT in a Web browser.  A WebServer and WebSocketsServer are created to serve up a HTML page that contains a Canvas object.   The host comunicatates over WebSockets to the JavaScript running in the browser to draw on the canvas.

## Dependancies:
 - [JSONString](https://github.com/smischny/JSONString.git)
 - [RemoteDisplay](https://github.com/smischny/RemoteDisplay.git)
 - [WebSockets](https://github.com/Links2004/arduinoWebSockets.git)

## Security
No security is provided by default.   You could configure the WebServer and WebSocketsServer to require authentication/encrption.

## TODO:
Fonts

## Tested on:
ESP32

## Author:
Toby Smischny
