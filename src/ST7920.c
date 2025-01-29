
#include "zephyr/kernel.h"
#include <ST7920.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(st7920);

const struct gpio_dt_spec *rst =
    &(const struct gpio_dt_spec)GPIO_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user),
                                                        st7920_gpios, 0);
const struct gpio_dt_spec *sid =
    &(const struct gpio_dt_spec)GPIO_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user),
                                                        st7920_gpios, 1);
const struct gpio_dt_spec *cs =
    &(const struct gpio_dt_spec)GPIO_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user),
                                                        st7920_gpios, 2);
const struct gpio_dt_spec *sclk =
    &(const struct gpio_dt_spec)GPIO_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user),
                                                        st7920_gpios, 3);

uint8_t image[(128 * 64) / 8];

uint8_t startRow, startCol, endRow,
    endCol; // coordinates of the dirty rectangle
uint8_t numRows = 64;
uint8_t numCols = 128;
uint8_t Graphic_Check = 0;

void SendByteSPI(uint8_t byte) {
  for (int i = 0; i < 8; i++) {
    if ((byte << i) & 0x80) {
      gpio_pin_set_dt(sid, 1);
    }

    else
      gpio_pin_set_dt(sid, 0);

    gpio_pin_set_dt(sclk, 0);
    gpio_pin_set_dt(sclk, 1);
  }
}

void ST7920_SendCmd(uint8_t cmd) {
  gpio_pin_set_dt(cs, 1);

  SendByteSPI(0xf8 + (0 << 1));   // send the SYNC + RS(0)
  SendByteSPI(cmd & 0xf0);        // send the higher nibble first
  SendByteSPI((cmd << 4) & 0xf0); // send the lower nibble
  k_sleep(K_USEC(50));

  gpio_pin_set_dt(cs, 0);
}

void ST7920_SendData(uint8_t data) {
  gpio_pin_set_dt(cs, 1);

  SendByteSPI(0xf8 + (1 << 1));    // send the SYNC + RS(1)
  SendByteSPI(data & 0xf0);        // send the higher nibble first
  SendByteSPI((data << 4) & 0xf0); // send the lower nibble
  k_sleep(K_USEC(50));
  gpio_pin_set_dt(cs, 0);
}

void ST7920_SendString(int row, int col, char *string) {
  LOG_INF("Enviando String %s", string);
  switch (row) {
  case 0:
    col |= 0x80;
    break;
  case 1:
    col |= 0x90;
    break;
  case 2:
    col |= 0x88;
    break;
  case 3:
    col |= 0x98;
    break;
  default:
    col |= 0x80;
    break;
  }

  ST7920_SendCmd(col);

  while (*string) {
    ST7920_SendData(*string++);
  }
}

void ST7920_GraphicMode(int enable) // 1-enable, 0-disable
{
  if (enable == 1) {
    LOG_INF("Entrando no GraphicMode");
    ST7920_SendCmd(0x30); // 8 bit mode
    k_msleep(1);
    ST7920_SendCmd(0x34); // switch to Extended instructions
    k_msleep(1);
    ST7920_SendCmd(0x36); // enable graphics
    k_msleep(1);
    Graphic_Check = 1; // update the variable
  }

  else if (enable == 0) {
    LOG_INF("Saindo do GraphicMode");
    ST7920_SendCmd(0x30); // 8 bit mode
    k_msleep(1);
    Graphic_Check = 0; // update the variable
  }
}

void ST7920_DrawBitmap(const unsigned char *graphic) {
  LOG_INF("Desenhando Bitmap");
  uint8_t x, y;
  for (y = 0; y < 64; y++) {
    if (y < 32) {
      for (x = 0; x < 8; x++) // Draws top half of the screen.
      { // In extended instruction mode, vertical and horizontal coordinates
        // must be specified before sending data in.
        ST7920_SendCmd(
            0x80 |
            y); // Vertical coordinate of the screen is specified first. (0-31)
        ST7920_SendCmd(
            0x80 |
            x); // Then horizontal coordinate of the screen is specified. (0-8)
        ST7920_SendData(graphic[2 * x + 16 * y]); // Data to the upper byte is
                                                  // sent to the coordinate.
        ST7920_SendData(
            graphic[2 * x + 1 + 16 * y]); // Data to the lower byte is sent to
                                          // the coordinate.
      }
    } else {
      for (x = 0; x < 8; x++) // Draws bottom half of the screen.
      { // Actions performed as same as the upper half screen.
        ST7920_SendCmd(
            0x80 |
            (y - 32)); // Vertical coordinate must be scaled back to 0-31 as it
                       // is dealing with another half of the screen.
        ST7920_SendCmd(0x88 | x);
        ST7920_SendData(graphic[2 * x + 16 * y]);
        ST7920_SendData(graphic[2 * x + 1 + 16 * y]);
      }
    }
  }
}

void ST7920_Update(void) {
  LOG_INF("Atualizando...");
  ST7920_DrawBitmap(image);
}

void ST7920_Clear() {
  LOG_INF("Clearing...");
  if (Graphic_Check == 1) // if the graphic mode is set
  {
    uint8_t x, y;
    for (y = 0; y < 64; y++) {
      if (y < 32) {
        ST7920_SendCmd(0x80 | y);
        ST7920_SendCmd(0x80);
      } else {
        ST7920_SendCmd(0x80 | (y - 32));
        ST7920_SendCmd(0x88);
      }
      for (x = 0; x < 8; x++) {
        ST7920_SendData(0);
        ST7920_SendData(0);
      }
    }
  }

  else {
    ST7920_SendCmd(0x01); // clear the display using command
    k_msleep(2);          // delay >1.6 ms
  }
}

int ST7920_Init(void) {
  LOG_INF("Inicializando...");

  // Inicializando GPIO
  struct gpio_dt_spec *pinos[] = {rst, sid, cs, sclk};

  int err;

  for (int i = 0; i < 4; i++) {

    err = device_is_ready(pinos[i]->port);
    if (err < 0) {
      LOG_ERR("A porta %s não está pronta (%d)", pinos[i]->port->name, err);
      return err;
    }
  }

  for (int i = 0; i < 4; i++) {
    err = gpio_pin_configure_dt(pinos[i], GPIO_OUTPUT_HIGH);
    if (err < 0) {
      LOG_ERR("Não foi possível configurar o pino {%s %d} como output (%d)",
              pinos[i]->port->name, pinos[i]->pin, err);
      return err;
    }
  }

  // Inicializando painel
  gpio_pin_set_dt(rst, 0);
  k_msleep(10);
  gpio_pin_set_dt(rst, 1);

  k_msleep(50);

  ST7920_SendCmd(0x30); // 8bit mode
  k_sleep(K_USEC(110));

  ST7920_SendCmd(0x30); // 8bit mode
  k_sleep(K_USEC(40));

  ST7920_SendCmd(0x08); // D=0, C=0, B=0
  k_sleep(K_USEC(110));

  ST7920_SendCmd(0x01); // clear screen
  k_sleep(K_USEC(12));

  ST7920_SendCmd(0x06); // cursor increment right no shift
  k_msleep(1);

  ST7920_SendCmd(0x0C); // D=1, C=0, B=0
  k_msleep(1);

  ST7920_SendCmd(0x02); // return to home
  k_msleep(1);
  return 0;
}

void SetPixel(uint8_t x, uint8_t y) {
  if (y < numRows && x < numCols) {
    uint8_t *p = image + ((y * (numCols / 8)) + (x / 8));
    *p |= 0x80u >> (x % 8);

    *image = *p;

    // Change the dirty rectangle to account for a pixel being dirty (we assume
    // it was changed)
    if (startRow > y) {
      startRow = y;
    }
    if (endRow <= y) {
      endRow = y + 1;
    }
    if (startCol > x) {
      startCol = x;
    }
    if (endCol <= x) {
      endCol = x + 1;
    }
  }
}

void DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
  int dx = (x1 >= x0) ? x1 - x0 : x0 - x1;
  int dy = (y1 >= y0) ? y1 - y0 : y0 - y1;
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;

  for (;;) {
    SetPixel(x0, y0);
    if (x0 == x1 && y0 == y1)
      break;
    int e2 = err + err;
    if (e2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  /* Check input parameters */
  if (x >= numCols || y >= numRows) {
    /* Return error */
    return;
  }

  /* Check width and height */
  if ((x + w) >= numCols) {
    w = numCols - x;
  }
  if ((y + h) >= numRows) {
    h = numRows - y;
  }

  /* Draw 4 lines */
  DrawLine(x, y, x + w, y);         /* Top line */
  DrawLine(x, y + h, x + w, y + h); /* Bottom line */
  DrawLine(x, y, x, y + h);         /* Left line */
  DrawLine(x + w, y, x + w, y + h); /* Right line */
}

void DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  uint8_t i;

  /* Check input parameters */
  if (x >= numCols || y >= numRows) {
    /* Return error */
    return;
  }

  /* Check width and height */
  if ((x + w) >= numCols) {
    w = numCols - x;
  }
  if ((y + h) >= numRows) {
    h = numRows - y;
  }

  /* Draw lines */
  for (i = 0; i <= h; i++) {
    /* Draw lines */
    DrawLine(x, y + i, x + w, y + i);
  }
}

void DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius) {
  int f = 1 - (int)radius;
  int ddF_x = 1;

  int ddF_y = -2 * (int)radius;
  int x = 0;

  SetPixel(x0, y0 + radius);
  SetPixel(x0, y0 - radius);
  SetPixel(x0 + radius, y0);
  SetPixel(x0 - radius, y0);

  int y = radius;
  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    SetPixel(x0 + x, y0 + y);
    SetPixel(x0 - x, y0 + y);
    SetPixel(x0 + x, y0 - y);
    SetPixel(x0 - x, y0 - y);
    SetPixel(x0 + y, y0 + x);
    SetPixel(x0 - y, y0 + x);
    SetPixel(x0 + y, y0 - x);
    SetPixel(x0 - y, y0 - x);
  }
}

// Draw Filled Circle

void DrawFilledCircle(int16_t x0, int16_t y0, int16_t r) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  SetPixel(x0, y0 + r);
  SetPixel(x0, y0 - r);
  SetPixel(x0 + r, y0);
  SetPixel(x0 - r, y0);
  DrawLine(x0 - r, y0, x0 + r, y0);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    DrawLine(x0 - x, y0 + y, x0 + x, y0 + y);
    DrawLine(x0 + x, y0 - y, x0 - x, y0 - y);

    DrawLine(x0 + y, y0 + x, x0 - y, y0 + x);
    DrawLine(x0 + y, y0 - x, x0 - y, y0 - x);
  }
}

// Draw Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                  uint16_t x3, uint16_t y3) {
  /* Draw lines */
  DrawLine(x1, y1, x2, y2);
  DrawLine(x2, y2, x3, y3);
  DrawLine(x3, y3, x1, y1);
}

// Draw Filled Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                        uint16_t x3, uint16_t y3) {
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 = 0,
          yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, curpixel = 0;

#define ABS(x) ((x) > 0 ? (x) : -(x))

  deltax = ABS(x2 - x1);
  deltay = ABS(y2 - y1);
  x = x1;
  y = y1;

  if (x2 >= x1) {
    xinc1 = 1;
    xinc2 = 1;
  } else {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1) {
    yinc1 = 1;
    yinc2 = 1;
  } else {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay) {
    xinc1 = 0;
    yinc2 = 0;
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;
  } else {
    xinc2 = 0;
    yinc1 = 0;
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++) {
    DrawLine(x, y, x3, y3);

    num += numadd;
    if (num >= den) {
      num -= den;
      x += xinc1;
      y += yinc1;
    }
    x += xinc2;
    y += yinc2;
  }
}
