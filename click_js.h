#pragma once

static const char clickJS [] PROGMEM = R"~(
function getWindowOrigin(element) {

  var x = 0;
  var y = 0;

  while (element) {
    x += (element.offsetLeft - element.scrollLeft + element.clientLeft);
    y += (element.offsetTop - element.scrollTop + element.clientTop);

    if (element.tagName == "BODY") {

      x -= document.documentElement.scrollLeft;
      y -= document.documentElement.scrollTop;

    }

    element = element.offsetParent;
  }

  return {
    x: x,
    y: y
  };
}

function clickListener(event) {
   origin = getWindowOrigin(canvas);

   serverSocket.send('c('+ event.button + ',' +
                           (event.clientX - origin.x) + ',' +
                           (event.clientY - origin.y) + ')');
}

function dblclickListener(event) {
   origin = getWindowOrigin(canvas);

   serverSocket.send('d('+ event.button + ',' +
                           (event.clientX - origin.x) + ',' +
                           (event.clientY - origin.y) + ')');
}

function keyupListener(event) {
   serverSocket.send('k('+ event.code + ',' +
                           event.key + ',' +
                           event.repeat + ')');
}


canvas.addEventListener('click',clickListener);
canvas.addEventListener('dblclick',dblclickListener);
window.addEventListener('keyup',keyupListener);

)~";

