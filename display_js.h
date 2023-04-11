#ifndef DISPLAY_JS__H
#define DISPLAY_JS__H

static const char displayJS [] PROGMEM = R"~(const RDCommand = {
    RD_DRAW_PIXEL:        0,
    RD_START_WRITE:       1,
    RD_WRITE_PIXEL:       2,
    RD_WRITE_FILL_RECT:   3,
    RD_WRITE_FAST_VLINE:  4,
    RD_WRITE_FAST_HLINE:  5,
    RD_WRITE_LINE:        6,
    RD_END_WRITE:         7,
    RD_SET_ROTATION:      8,
    RD_INVERT_DISPLAY:    9,
    RD_DRAW_FAST_VLINE:   10,
    RD_DRAW_FAST_HLINE:   11,
    RD_FILL_RECT:         12,
    RD_FILL_SCREEN:       13,
    RD_DRAW_LINE:         14,
    RD_DRAW_RECT:         15,
    RD_DRAW_CIRCLE:       16,
    RD_DRAW_CIRCLE_HELPER: 17,
    RD_FILL_CIRCLE:        18,
    RD_FILL_CIRCLE_HELPER: 19,
    RD_FILL_TRIANGLE:      20,
    RD_DRAW_TRIANGLE:      21,
    RD_DRAW_ROUND_RECT:    22,
    RD_FILL_ROUND_RECT:    23,
    RD_DRAW_BITMAP1:       24,
    RD_DRAW_BITMAP2:       25,
    RD_DRAW_X_BITMAP:      26,
    RD_DRAW_GRAYSCALE_BITMAP1: 27,
    RD_DRAW_GRAYSCALE_BITMAP2: 28,
    RD_DRAW_RGB_BITMAP1:   29,
    RD_DRAW_RGB_BITMAP2:   30,
    RD_DRAW_CHAR:          31,
    RD_SET_CURSOR:         32,
    RD_SET_TEXT_COLOR1:    33,
    RD_SET_TEXT_COLOR2:    34,
    RD_SET_TEXT_SIZE:      35,
    RD_SET_TEXT_WRAP:      36,
    RD_CP437:              37,
    RD_SET_FONT:           38,
    RD_GET_TEXT_BOUNDS:    39,
    RD_WRITE:              40,
    RD_HEIGHT:             41,
    RD_WIDTH:              42,
    RD_GET_ROTATION:       43,
    RD_GET_CURSOR_X:       44,
    RD_GET_CURSOR_Y:       45,
    RD_RESET:              46,
    RD_BEGIN:              47,
    RD_VSCROLL:            48,
    RD_HSCROLL:            49,
    RD_SCROLL:             50,
    RD_DATA_MSG:           51,
    RD_DATA_RESET:         52,
    RD_SET_ADDR_WINDOW:    53,
    RD_PUSH_COLORS:        54,
    RD_CACHE_DATA:         55,
    RD_SCREEN_REFRESH:     56,
    RD_SCREEN_OFF:         57,
    RD_SCREEN_ON:          58,
    RD_JS_SCRIPT:          59
}

var statusButton;
var statusText;
var serverSocket;
var canvas;
var ctx;
var state = { wssURL:'ws://'+window.location.hostname+':81/display',
              x:0, y:0, 
              width:0, height:0,
              fg:"#ffffff", bg:"#000000", 
              fillTextBg:true, fontSize:1, fontHeight:8, 
              wrap:1, 
              rotation:0, 
              inverted:0
              };
              
var transformMatrix = { a:1, b:0, c:0, d:1, e:0, f:0 };


//
// The Image data is not translated,
// so we must account for the rotation.
//
function vscroll(message,sWidth,sHeight) {
  
   let x = 0;
   let y0,y1,y2,y3;
   let width = sWidth;
   let height = message.height;
   let delta = 0 - message.delta;

   if (delta > 0) {
     y0 = message.y;
     y1 = message.y + delta;
     y2 = message.y + height - delta;
     y3 = y0;
   } else {
     delta = 0 - delta;
     y0 = message.y + delta;
     y2 = message.y;
     y1 = message.y;
     y3 = message.y + height - delta;
   }
   height = message.height - delta;

   if (state.rotation != 0) {
        if (state.rotation == 2) {
            y0 = sHeight - y0 - height;
            y1 = sHeight - y1 - height;
            y2 = sHeight - y2 - delta;
            y3 = sHeight - y3 - delta;
        }
   }

   if ((height > 0) && (delta > 0)) {
     let imageData = ctx.getImageData(x,y0,width,height);
     let deltaImage = ctx.getImageData(x,y2,width,delta);

     ctx.putImageData(imageData, x, y1);
     ctx.putImageData(deltaImage, x, y3);
   }

}

function hscroll(message,sWidth,sHeight) {
  
   let x0,x1,x2,x3;
   let y = 0;
   let width = message.width;
   let height = sHeight;
   let delta = message.delta;
  
   if (delta > 0) {
     x0 = message.x;
     x1 = message.x + delta;
     x2 = message.x + width - delta;
     x3 = x0;
   } else {
     delta = 0 - delta;
     x0 = message.x + delta;
     x2 = message.x;
     x1 = message.x;
     x3 = message.x + width - delta;
   }
   width = message.width - delta;

   if ((width > 0) && (delta > 0)) {
     
     let imageData = ctx.getImageData(x0,y,width,height);
     let deltaImage = ctx.getImageData(x2,y,delta,height);

     ctx.putImageData(imageData, x1, y);
     ctx.putImageData(deltaImage, x3, y);
   }

}

function toRGB(color) {
  let RGB = [];

  RGB[0] = parseInt(color.substr(1,2),16);
  RGB[1] = parseInt(color.substr(3,2),16);
  RGB[2] = parseInt(color.substr(5,2),16);
  
  return (RGB);
}

function toRGBfrom16Bit(b1,b2) {
  let RGB = [];

  let rgb16 = (b1 << 8) + b2;
  
  RGB[0] = (rgb16 & 0xF800) >> 8; 
  RGB[1] = (rgb16 & 0x07E0) >> 3;
  RGB[2] = (rgb16 & 0x1F) << 3;
  
  return (RGB);
}

function transformX(x,y) {
  return ((transformMatrix.a * x) + 
          (transformMatrix.c * y) +
           transformMatrix.e);
}

function transformY(x,y) {
  return ((transformMatrix.d * y) + 
          (transformMatrix.b * x) +
           transformMatrix.f);
} 

function getAlpha(row,column,mask,width) {
  
  if (mask.length > 0) {
      let index = (row * Math.floor((width+8)/8)) + Math.floor(column/8);
      let m = 0x80 >> Math.floor(column % 8);
      
      if ((mask[index].codePointAt(0) & m) > 0) {
          return (255);
      } else {
          return (0);
      }
  }
  return (255);
}

function oneBitTranslate(RGB, bitmap, data, imgWidth, width, aMask) {

   let row = 0;
   let column = 0;

   for(let i = 0; i < bitmap.length; i++) {

    let b = bitmap[i];
    
    let mask = 0x80;
    for (let m = 0; m < 8; m++) {
      
      if ((b.codePointAt(0) & mask) != 0) {
          
          let r = transformY(column,row);
          let c = transformX(column,row);
          
          let dataIndex = (((r * imgWidth) + c) * 4);

          data[dataIndex++] = RGB[0];
          data[dataIndex++] = RGB[1];
          data[dataIndex++] = RGB[2];
          data[dataIndex] = 255;

      }
      
      column += 1;
      if (column > width) {
          column = 0;
          row += 1;
      }
      mask = mask >> 1;
    }
 }

}

function grayscaleTranslate(RGB, bitmap, data, imgWidth, width, mask) {

   let row = 0;
   let column = 0;
   
   for(let i = 0; i < bitmap.length; i++ ) {
      let b = bitmap[i].codePointAt(0);
      let r = transformY(column,row);
      let c = transformX(column,row);
      
      let dataIndex = (((r * imgWidth) + c) * 4);

      if (getAlpha(row,column,mask,width) != 0) {
          data[dataIndex++] = b;
          data[dataIndex++] = b;
          data[dataIndex++] = b;
          data[dataIndex] =  255;
      }
      
      column += 1;
      if (column > width) {
          column = 0;
          row += 1;
      }
   }
}

function rgb16Translate(RGBx, bitmap, data, imgWidth, width, mask) {

   let row = 0;
   let column = 0;

   for(let i = 0; i < bitmap.length; i+=2 ) {
      let b2 = bitmap[i].codePointAt(0);
      let b1 = bitmap[i+1].codePointAt(0);
      let RGB = toRGBfrom16Bit(b1,b2);
      let r = transformY(column,row);
      let c = transformX(column,row);
      
      let dataIndex = (((r * imgWidth) + c) * 4);
     
       if (getAlpha(row,column,mask,width) != 0) {
          data[dataIndex++] = RGB[0];
          data[dataIndex++] = RGB[1];
          data[dataIndex++] = RGB[2];
          data[dataIndex] = 255;
          
       }
      
      column += 1;
      if (column > width) {
          column = 0;
          row += 1;
      }
   }
}

function drawBitmap(x, y, width, height, RGB, tranlateFunction, bitmap, mask = []) {

   let orgX = x;
   let orgY = y;
   let imgWidth = width;
   let imgHeight = height;
   let e = 0;
   let f = 0;

   // get/putImageData is not translated, so we have to 
   // convert our coordinates manually and deal 
   // with rotation.

   x = transformX(orgX,orgY);
   y = transformY(orgX,orgY);
 
   switch (state.rotation) {
       case 0:
         imgWidth = width+1;
         break;
       case 1:
         y -= width;
         imgWidth = height;
         imgHeight = width;
         f = width;
         break;
       case 2:
         x -= width;
         y -= height;
         e = width;
         f = height;
         break;
       case 3:
         x -= height;
         imgWidth = height;
         imgHeight = width;
         e = height;
         break;
   }         


   let imageData = ctx.getImageData(x,y,imgWidth,imgHeight);
   let data = imageData.data;

   // adjust the matrix to the size of the image
   let saveE = transformMatrix.e;
   let saveF = transformMatrix.f;  
   transformMatrix.e = e;
   transformMatrix.f = f;       

   tranlateFunction(RGB,bitmap,data,imgWidth,width,mask);
   
   ctx.putImageData(imageData,x,y);

   transformMatrix.e = saveE;
   transformMatrix.f = saveF;
}
                
function socketReceiver(event) {
   let message = JSON.parse(event.data);

   switch (message.command) {
      case RDCommand.RD_DRAW_PIXEL:
         ctx.fillStyle=message.color;
         ctx.fillRect(message.x,message.y,1,1);
         break;
      case RDCommand.RD_START_WRITE:
         break;
      case RDCommand.RD_WRITE_PIXEL:
         ctx.fillStyle=message.color;
         ctx.fillRect(message.x,message.y,1,1);
         break;
      case RDCommand.RD_WRITE_FILL_RECT:
         ctx.fillStyle=message.color;
         ctx.fillRect(message.x,message.y,message.width,message.height);
         break;
      case RDCommand.RD_WRITE_FAST_VLINE:
         ctx.strokeStyle=message.color;
         ctx.beginPath();
         ctx.moveTo(message.x,message.y);
         ctx.lineTo(message.x,message.y+message.height);
         ctx.stroke();
         break;
      case RDCommand.RD_WRITE_FAST_HLINE:
         ctx.strokeStyle=message.color;
         ctx.beginPath();
         ctx.moveTo(message.x,message.y);
         ctx.lineTo(message.x+message.width,message.y);
         ctx.stroke();
         break;
      case RDCommand.RD_WRITE_LINE:
         ctx.strokeStyle=message.color;
         ctx.beginPath();
         ctx.moveTo(message.x0,message.y0);
         ctx.lineTo(message.x1,message.y1);
         ctx.stroke();
         break;
      case RDCommand.RD_END_WRITE:
         break;
      case RDCommand.RD_SET_ROTATION:
         if (message.rotation != state.rotation) {
 
           if ((message.rotation % 2) != (state.rotation % 2)) {
               let temp = state.width;
               state.width = state.height;
               state.height = temp;
           }
           switch (message.rotation) {
              case 0:
                transformMatrix.a = 1;
                transformMatrix.b = 0;
                transformMatrix.c = 0;
                transformMatrix.d = 1;
                transformMatrix.e = 0;
                transformMatrix.f = 0;
                break;
              case 1:
                transformMatrix.a = 0;
                transformMatrix.b = -1;
                transformMatrix.c = 1;
                transformMatrix.d = 0;
                transformMatrix.e = 0;
                transformMatrix.f = canvas.height;
                break;
              case 2:
                transformMatrix.a = -1;
                transformMatrix.b = 0;
                transformMatrix.c = 0;
                transformMatrix.d = -1;
                transformMatrix.e = canvas.width;
                transformMatrix.f = canvas.height;
                break;
              case 3:
                transformMatrix.a = 0;
                transformMatrix.b = 1;
                transformMatrix.c = -1;
                transformMatrix.d = 0;
                transformMatrix.e = canvas.width;
                transformMatrix.f = 0;
                break;
           }
           ctx.setTransform(transformMatrix.a,transformMatrix.b,transformMatrix.c,transformMatrix.d,transformMatrix.e,transformMatrix.f);
  
           state.rotation = message.rotation;
         }
         break;
      case RDCommand.RD_INVERT_DISPLAY:
         if (message.b != state.inverted) {
             state.inverted = message.mode;

             let width = state.width;
             let height = state.height;

             if ((state.rotation % 2) == 1) {
               width = state.height;
               height = state.width;
             }

             let imageData = ctx.getImageData(0, 0, width, height);
             let data = imageData.data;

             for(let i = 0; i < data.length; i++ ) {
                // RGBA values (skip the A)
                if ((i % 4) != 3)  {
                   data[i] = 255 - data[i];
                }
             }
             ctx.putImageData(imageData,0,0);
         }
         break;
      case RDCommand.RD_DRAW_FAST_VLINE:
         ctx.strokeStyle=message.color;
         ctx.beginPath();
         ctx.moveTo(message.x,message.y);
         ctx.lineTo(message.x,message.y+message.height);
         ctx.stroke();
         break;
      case RDCommand.RD_DRAW_FAST_HLINE:
         ctx.strokeStyle=message.color;
         ctx.beginPath();
         ctx.moveTo(message.x,message.y);
         ctx.lineTo(message.x+message.width,message.y);
         ctx.stroke();        

         break;
      case RDCommand.RD_FILL_RECT:
         ctx.fillStyle=message.color;
         ctx.fillRect(message.x,message.y,message.width,message.height);
         break;
      case RDCommand.RD_FILL_SCREEN:
         ctx.fillStyle=message.color;
         ctx.fillRect(0,0,state.width,state.height);
         break;
      case RDCommand.RD_DRAW_LINE:
         ctx.strokeStyle=message.color;
         ctx.beginPath();
         ctx.moveTo(message.x0,message.y0);
         ctx.lineTo(message.x1,message.y1);
         ctx.stroke();
         break;
      case RDCommand.RD_DRAW_RECT:
         ctx.strokeStyle=message.color;
         ctx.strokeRect(message.x,message.y,message.width,message.height);
         break;
      case RDCommand.RD_DRAW_CIRCLE:
         ctx.beginPath();
         ctx.strokeStyle=message.color;
         ctx.arc(message.x,message.y,message.r,0,2*Math.PI,false);
         ctx.stroke();
         break;
      case RDCommand.RD_DRAW_CIRCLE_HELPER:
         {
           ctx.beginPath();
           let startAngle = 0 * Math.PI / 180;
           let endAngle = 90 * Math.PI / 180;
           switch (message.cornername) {
               case 0:
                  startAngle = 0 * Math.PI / 180;
                  endAngle = 90 * Math.PI / 180;
                  break;
               case 1:
                  startAngle = 90 * Math.PI / 180;
                  endAngle = 10 * Math.PI / 180;
                  break;
               case 2:
                  startAngle = 180 * Math.PI / 180;
                  endAngle = 270 * Math.PI / 180;imension.height
                  break;
               case 3:
                  startAngle = 270 * Math.PI / 180;
                  endAngle = 0 * Math.PI / 180;
                  break;
           }
           ctx.strokeStyle=message.color;
           ctx.arc(message.x,message.y,message.r,startAngle,endAbgle,true);
           ctx.stroke();
         }
         break;
      case RDCommand.RD_FILL_CIRCLE:
         ctx.beginPath();
         ctx.fillStyle=message.color;
         ctx.arc(message.x,message.y,message.r,0,2*Math.PI,false);
         ctx.fill();
         break;
      case RDCommand.RD_FILL_CIRCLE_HELPER:
         {
           ctx.beginPath();
           let startAngle = 0 * Math.PI / 180;
           let endAngle = 90 * Math.PI / 180;
           switch (message.cornername) {
               case 0:
                  startAngle = 0 * Math.PI / 180;
                  endAngle = 90 * Math.PI / 180;
                  break;
               case 1:
                  startAngle = 90 * Math.PI / 180;
                  endAngle = 10 * Math.PI / 180;
                  break;
               case 2:maskData
                  startAngle = 180 * Math.PI / 180;
                  endAngle = 270 * Math.PI / 180;
                  break;
               case 3:
                  startAngle = 270 * Math.PI / 180;
                  endAngle = 0 * Math.PI / 180;
                  break;
           }
           ctx.strokeStyle=message.color;
           ctx.arc(message.x,message.y,message.r,startAngle,endAbgle,true);
           ctx.fill();
         }
         break;
      case RDCommand.RD_FILL_CIRCLE:
         ctx.beginPath();
         ctx.fillStyle=message.color;
         ctx.arc(message.x,message.y,message.r,2*Math.PI);
         ctx.fill();
         break;
      case RDCommand.RD_FILL_TRIANGLE:
         ctx.fillStyle=message.color;
         ctx.beginPath();
         ctx.moveTo(message.x0,message.y0); 
         ctx.lineTo(message.x1,message.y1);
         ctx.lineTo(message.x2,message.y2);
         ctx.fill();
         break;

      case RDCommand.RD_DRAW_TRIANGLE:
         ctx.strokeStyle=message.color;
         ctx.beginPath();
         ctx.moveTo(message.x0,message.y0);state.height
         ctx.lineTo(message.x1,message.y1);
         ctx.lineTo(message.x2,message.y2);
         ctx.stroke();
         break;

      case RDCommand.RD_DRAW_ROUND_RECT:

         ctx.strokeStyle=message.color;
         ctx.beginPath();
         ctx.moveTo(message.x, message.y + message.r);
         ctx.lineTo(message.x, message.y + message.height - message.r);
         ctx.arcTo(message.x, message.y + message.height, 
                   message.x + message.r, message.y + message.height, 
                   message.r);
         ctx.lineTo(message.x + message.width - message.r, message.y + message.height);
         ctx.arcTo(message.x + message.width, 
                   message.y + message.height, 
                   message.x + message.width, 
                   message.y + message.height-message.r, 
                   message.r);
         ctx.lineTo(message.x + message.width, message.y + message.r);
         ctx.arcTo(message.x + message.width, message.y, 
                   message.x + message.width - message.r, 
                   message.y, message.r);
         ctx.lineTo(message.x + message.r, message.y);
         ctx.arcTo(message.x, message.y, 
                   message.x, message.y + message.r, 
                   message.r);
         ctx.stroke();
         break;

      case RDCommand.RD_FILL_ROUND_RECT:

         ctx.fillStyle=message.color;
         ctx.beginPath();
         ctx.moveTo(message.x, message.y + message.r);
         ctx.lineTo(message.x, message.y + message.height - message.r);
         ctx.arcTo(message.x, message.y + message.height, 
                   message.x + message.r, message.y + message.height, 
                   message.r);
         ctx.lineTo(message.x + message.width - message.r, message.y + message.height);
         ctx.arcTo(message.x + message.width, 
                   message.y + message.height, 
                   message.x + message.width, 
                   message.y + message.height-message.r, 
                   message.r);
         ctx.lineTo(message.x + message.width, message.y + message.r);
         ctx.arcTo(message.x + message.width, message.y, 
                   message.x + message.width - message.r, 
                   message.y, message.r);
         ctx.lineTo(message.x + message.r, message.y);
         ctx.arcTo(message.x, message.y, 
                   message.x, message.y + message.r, 
                   message.r);
         ctx.fill();
         break;

      case RDCommand.RD_DRAW_BITMAP2:
         ctx.fillStyle=message.bg;
         ctx.fillRect(message.x,message.y,message.width,message.height);
         // fall through
      case RDCommand.RD_DRAW_BITMAP1:
         drawBitmap(message.x,message.y,message.width-1,message.height-1,toRGB(message.color), oneBitTranslate, atob(message.bitmap));
         break;

      case RDCommand.RD_DRAW_X_BITMAP:
         break;
      case RDCommand.RD_DRAW_GRAYSCALE_BITMAP2:
         drawBitmap(message.x,message.y,message.width-1,message.height-1,"#000000", grayscaleTranslate, atob(message.bitmap), atob(message.mask));
         break;
      case RDCommand.RD_DRAW_GRAYSCALE_BITMAP1:
         drawBitmap(message.x,message.y,message.width-1,message.height-1,"#000000", grayscaleTranslate, atob(message.bitmap));
  
         break;
      case RDCommand.RD_DRAW_RGB_BITMAP2:
         drawBitmap(message.x,message.y,message.width-1,message.height-1,"#000000", rgb16Translate, atob(message.bitmap), atob(message.mask));
         break;
      case RDCommand.RD_DRAW_RGB_BITMAP1:
         drawBitmap(message.x,message.y,message.width-1,message.height-1,"#000000", rgb16Translate, atob(message.bitmap));
         break;

      case RDCommand.RD_DRAW_CHAR: // Fall through
      case RDCommand.RD_WRITE:
         {
           let fontHeight = message.size*8;
           ctx.font = ""+fontHeight+"px monospace";
           let width = ctx.measureText(message.c).width;
           if ((state.x + width) > state.width) {
              state.y = state.y + state.fontHeight;
              state.x = 0;
           }
           if (state.fillTextBg) {
             ctx.fillStyle = state.bg;
             ctx.fillRect(state.x,state.y,width,state.fontHeight);
           }
           
           ctx.fillStyle = message.color;
           ctx.fillText(message.c,state.x,state.y);
           state.x = state.x + width;
         }
         break;
      case RDCommand.RD_SET_CURSOR:
         state.x = message.x;
         state.y = message.y;
         break;
      case RDCommand.RD_SET_TEXT_COLOR1:
         state.fg = message.color;
         state.fillTextBg = false;
         break;
      case RDCommand.RD_SET_TEXT_COLOR2:
         state.fg = message.color;
         state.bg = message.bg;
         state.fillTextBg = true;
         break;
      case RDCommand.RD_SET_TEXT_SIZE:
         state.fontSize = message.s;
         state.fontHeight = message.s*8;
         break;
      case RDCommand.RD_SET_TEXT_WRAP:
         state.wrap=message.b;
         break;
      case RDCommand.RD_CP437:
         break;
      case RDCommand.RD_SET_FONT:
         break;
      case RDCommand.RD_GET_TEXT_BOUNDS:
         break;
      case RDCommand.RD_HEIGHT:
         break;
      case RDCommand.RD_WIDTH:
         break;
      case RDCommand.RD_GET_ROTATION:
         break;
      case RDCommand.RD_GET_CURSOR_X:
         break;
      case RDCommand.RD_GET_CURSOR_Y:
         break;
      case RDCommand.RD_RESET:
         state.x = 0;
         state.y = 0;
         state.width = canvas.width;
         state.height = canvas.height;
         state.rotation = 0;
         transformMatrix.a = 1;
         transformMatrix.b = 0;
         transformMatrix.c = 0;
         transformMatrix.d = 1;
         transformMatrix.e = 0;
         transformMatrix.f = 0;
         ctx.setTransform(transformMatrix.a,transformMatrix.b,transformMatrix.c,transformMatrix.d,transformMatrix.e,transformMatrix.f);
           
         break;
      case RDCommand.RD_BEGIN:
         break;
      case RDCommand.RD_VSCROLL:

         if (state.rotation % 2 == 0) {
            vscroll(message,state.width,state.height);
         } else {
            vscroll(message,state.height,state.width);
         }
         
         break;

       case RDCommand.RD_HSCROLL:

         hscroll(message,state.width,state.height);

         break;
         
      case RDCommand.RD_SCROLL:
      
         break;

      case RDCommand.RD_DATA_MSG:
         {
           ctx.font = ""+state.fontHeight+"px monospace";
  
           let width = ctx.measureText(message.string).width;
  
           if (state.fillTextBg) {
             ctx.fillStyle = state.bg;
             ctx.fillRect(state.x,state.y,width,state.fontHeight);
           }
           
           ctx.fillStyle = state.fg;
           ctx.fillText(message.string,state.x,state.y+state.fontHeight);
           
           if ((state.x + width) > state.width) {
              state.y = state.y + state.fontHeight;
              state.x = 0;
           } else {
              state.x += width
           }
         }
         break;
      case RDCommand.RD_DATA_RESET:
         break;
      case RDCommand.RD_JS_SCRIPT:
         eval(message.script);
         break;
   }
}

function clearText() {
  statusText.innerHTML = "<a href='https://github.com/smischny?tab=repositories' target='_blank'>Smischny GitHub</a>\n";
}

var reconnect=false;

function socketClosed() {
   statusButton.style.backgroundColor = "red";
   if (reconnect) {
      reconnect = false;
      connectWebSocket(state.wssURL);
      statusText.innerHTML = "Connected";
   }
}

function socketError(event) {
   statusButton.style.backgroundColor = "red";
   statusText.innerHTML = "Socket Error";
   console.log('WebSocket error: ', event);
   
   reconnect = true;
}

function deac() {
   serverSocket.close();
}

function connectWebSocket(webSocketURL) {
  
   serverSocket = new WebSocket(webSocketURL);
   serverSocket.onmessage = socketReceiver;
   serverSocket.onclose = socketClosed;
   serverSocket.onerror = socketError;

   statusButton.style.backgroundColor = "green";
   
}

function init(webSocketURL) {

   statusText = document.getElementById("status");
   statusButton = document.getElementById("statusButton");

   canvas = document.getElementById("display");

   ctx = canvas.getContext("2d");

   state.wssURL = webSocketURL; 
   state.x = 0;
   state.y = 0;
   state.width = canvas.width;
   state.height = canvas.height;
   state.rotation = 0;
   transformMatrix.a = 1;
   transformMatrix.b = 0;
   transformMatrix.c = 0;
   transformMatrix.d = 1;
   transformMatrix.e = 0;
   transformMatrix.f = 0;
   ctx.setTransform(transformMatrix.a,transformMatrix.b,transformMatrix.c,transformMatrix.d,transformMatrix.e,transformMatrix.f);
   
   connectWebSocket(webSocketURL);
  
   window.onunload = deac;
   
   clearText();
}

)~";

#endif
