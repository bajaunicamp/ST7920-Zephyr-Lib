#include "zephyr/kernel.h"
#include <ST7920.h>
#include <bitmap.h>

int main() {

  ST7920_Init();
  k_msleep(1000);

  ST7920_GraphicMode(1);
  ST7920_DrawBitmap(jupiter_logo);

  k_msleep(1000);

  ST7920_Clear();

  ST7920_GraphicMode(0);

  ST7920_SendString(0, 0, "HELLO WORLD");
  ST7920_SendString(1, 0, "FROM");
  ST7920_SendString(2, 0, "CONTROLLERSTECH");
  ST7920_SendString(3, 0, "1234567890!@#$%^");

  k_msleep(2000);

  ST7920_Clear();

  ST7920_GraphicMode(1);

  DrawCircle(110, 31, 12);

  DrawCircle(110, 31, 16);

  DrawLine(3, 60, 127, 33);

  ST7920_Update();

  DrawRectangle(100, 12, 20, 14);

  ST7920_Update();

  DrawFilledRectangle(30, 20, 30, 10);

  ST7920_Update();

  DrawFilledCircle(15, 30, 6);

  ST7920_Update();

  DrawFilledTriangle(1, 5, 10, 5, 6, 15);

  ST7920_Update();

  while (true) {
    k_msleep(10);
  }
}
