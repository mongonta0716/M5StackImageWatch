/*
 This Library is designed for M5Stack/M5StickC

 It has a function to save and redraw the background image data.

 Based Sprite.cpp https://github.com/Bodmer/TFT_eSPI
 
*/
#include "BackgroundLayer.h"

TFT_bgLayer::TFT_bgLayer(TFT_eSPI *tft)
{
    _tft = tft;
    _iwidth     = 0;
    _iheight    = 0;
    _bpp        = 16;
    _iswapBytes = true;
    _tft->setSwapBytes(_iswapBytes);

}

void TFT_bgLayer::setColorDepth(int8_t bpp) { _bpp = bpp; }

int8_t TFT_bgLayer::getColorDepth(void) { return _bpp; }

void TFT_bgLayer::setSwapBytes(boolean iswapBytes) { _iswapBytes = iswapBytes; }

boolean TFT_bgLayer::getSwapBytes(void) { return _iswapBytes; }

void TFT_bgLayer::pushLayer(int32_t x, int32_t y)
{
    _tft->pushImage(x, y, _iwidth, _iheight, _img);
}

void TFT_bgLayer::drawBufferPixel(int32_t x, int32_t y)
{
    if ((x < 0) || (y < 0) || (x >= _iwidth) || (y >= _iheight)) return;
    uint16_t color = readBufferPixel(x, y);
    drawBufferPixel(x, y, color);
}

void TFT_bgLayer::drawBufferPixel(int32_t x, int32_t y, uint32_t color)
{
    if ((x < 0) || (y < 0) || (x >= _iwidth) || (y >= _iheight)) return;
    _tft->drawPixel(x, y, color);
}

uint16_t TFT_bgLayer::readBufferPixel(int32_t x, int32_t y)
{
    if ((x < 0) || (y < 0) || (x >= _iwidth) || (y >= _iheight)) return 0;

    uint16_t color = 0;
    
    color = _img[x + y * _iwidth];

    if (_bpp == 16)
    {
        return color;
    }

    if (_bpp == 8)
    {
        if (color != 0)
        {
            uint8_t blue[] = {0, 11, 21, 31};
            color = (color & 0xE0)<<8 | (color & 0xC0)<<5
                  | (color & 0x1C)<<6 | (color & 0x1C)<<3
                  | blue[color & 0x03];
        }
        return color;
    }
}

void TFT_bgLayer::drawBufferLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
    boolean steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap_coord(x0, y0);
        swap_coord(x1, y1);
    }
    
    if (x0 > x1) {
        swap_coord(x0, x1);
        swap_coord(y0, y1);
    }
    
    int32_t dx = x1 - x0, dy = abs(y1 - y0);
    
    int32_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;
    
    if (y0 < y1) ystep = 1;
    
    // Split into steep and not steep for FastH/V separation
    if (steep) {
        for(; x0 <= x1; x0++) {
            dlen++;
            err -= dy;
            if (err < 0) {
                err += dx;
                if (dlen == 1) drawBufferPixel(y0, xs);
                else drawFastVLine(y0, xs, dlen);
                dlen = 0; y0 += ystep; xs = x0 + 1;
            }
        }
        if (dlen) drawFastVLine(y0, xs, dlen);
    }
    else
    {
        for (; x0 <= x1; x0++) {
            dlen++;
            err -= dy;
            if (err < 0) {
                err += dx;
                if (dlen == 1) drawBufferPixel(xs, y0);
                else drawFastHLine(xs, y0, dlen);
                dlen = 0; y0 += ystep; xs = x0 + 1;
            }
        }
        if (dlen) drawFastHLine(xs, y0, dlen);
    }
}

void TFT_bgLayer::drawFastVLine(int32_t x, int32_t y, int32_t h)
{
    if ((x < 0) || (x >= _iwidth) || (y >= _iheight)) return;
    
    if (y < 0) { h += y; y = 0; }
    
    if ((y + h) > _iheight) h = _iheight - y;
    
    if (h < 1) return;
    
    while (h--)
    {
        drawBufferPixel(x, y);
        y++;
    }
}

void TFT_bgLayer::drawFastHLine(int32_t x, int32_t y, int32_t w)
{
    if ((y < 0) || (x >= _iwidth) || (y >= _iheight)) return;
    
    if (x < 0) { w += x; x = 0; }
    
    if ((x + w) > _iwidth) w = _iwidth -x;
    
    if (w < 1) return;
    
    while (w--)
    {
        drawBufferPixel(x, y);
        x++;
    }
}

void TFT_bgLayer::drawBufferRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
  if ((x0 == 0) && (y0 == 0)) { return; }

  drawBufferRect(x0, y0, x1, y1, 0);
}

void TFT_bgLayer::drawBufferRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int8_t mergin)
{
  if (x0 == 0) return;
  if (x0 > x1) {
    swap_coord(x0, x1);
  }
  if (y0 > y1) {
    swap_coord(y0, y1);  
  }
  x0 = x0 - mergin;
  y0 = y0 - mergin;
 
  int16_t dx = x1 - x0 + mergin, dy = y1 - y0 + mergin;
  Serial.printf("destSP:x0:%d,y0:%d,x1:%d,y1:%d\n", x0, y0, x1, y1);
  Serial.printf("dx:%d,dy:%d\n", dx, dy);
  uint32_t imageindex = 0;
  uint16_t destBuffer[dx*dy];
  for (uint16_t j = y0; j < y0 + dy; j++) {
    for (uint16_t i = x0; i < x0 + dx; i++) {
      destBuffer[imageindex] = _img[i + j * _iwidth];
      imageindex++;
    }
  }
  _tft->pushImage(x0, y0, dx, dy, destBuffer);
}

void TFT_bgLayer::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data) {
  _img = data;
  _iwidth  = w;
  _iheight = h;
  _tft->pushImage(x, y, w, h, _img);
}
void TFT_bgLayer::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data) {
  pushImage(x, y, w, h, (uint16_t*)data);
}
