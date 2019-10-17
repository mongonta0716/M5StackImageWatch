/*
 This Library is designed for M5Stack/M5StickC

 It has a function to save and redraw the background image data.

 Based Sprite.h https://github.com/Bodmer/TFT_eSPI
 
*/

#include "utility/In_eSPI.h"

class TFT_bgLayer : public TFT_eSPI {
  public:
    TFT_bgLayer(TFT_eSPI *tft);

    void      setColorDepth(int8_t bpp);
    int8_t    getColorDepth(void);

    void      setSwapBytes(bool swap);
    boolean   getSwapBytes(void);
    
    void      pushLayer(int32_t x, int32_t y);
    void      pushSubLayer();
    void      drawBufferPixel(int32_t x, int32_t y);
    void      drawBufferPixel(int32_t x, int32_t y, uint32_t color);
    uint16_t  readBufferPixel(int32_t x, int32_t y);

    void      drawBufferLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
    void      drawFastVLine(int32_t x, int32_t y, int32_t h);
    void      drawFastHLine(int32_t x, int32_t y, int32_t w);

    void      drawBufferRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
    void      drawBufferRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int8_t mergine);

    void      pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data);
    void      pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data);

  private:
    TFT_eSPI *_tft;

  protected:
    boolean _iswapBytes;
    uint8_t _bpp;               // 16bit or 8bit(bit per pixel)
    int32_t _iwidth, _iheight;  // BackGround Layer Size
    uint16_t *_img;             // pointer to 16 bit layer
    uint8_t  *_img8;            // pointer to 8 bit layer
};
