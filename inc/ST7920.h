#ifndef ST7920_H
#define ST7920_H

#include "stdint.h"

/**
 * @file
 * @brief Biblioteca para utilização do display ST7920 com bitbanging.
 *
 * Esta biblioteca é uma tradução do código da ControllersTech para Zephyr. 
 * (https://controllerstech.com/glcd-128x64-st7920-interfacing-with-stm32/)
 *
 * Essa biblioteca faz um bitbanging de SPI para enviar os dados para o display.
 *
 * @author Giancarlo Bonvenuto
 */

/**
 * Inicializa o Display
 */
int ST7920_Init (void);

/** 
 * @brief Envia uma string para o LCD em uma determinada linha e coluna
 *
 * @param row Define a linha em que será escrito o texto, pode ser de 0 a 3
 * @param col Define a coluna em que será escrito o text, pode ser de 0 a 7
 */
void ST7920_SendString(int row, int col, char* string);


/**
 * @brief É necessário habilitar ou desabilitar o modo gráfico para escrever textos
 * ou desenhar imagens
 *
 * @param enable:
 *  - `0` desativa o modo gráfico e permite que textos possam ser escritos na tela
 *  - `1` ativa o modo gráfico e permite que bitmaps e formas geométricas possam ser desenhadas na tela
 */
void ST7920_GraphicMode (int enable);


/**
 * @brief Limpa a tela em qualquer modo
 */
void ST7920_Clear();


/**
 * @brief Desenha um bitmap na tela
 *
 * @param graphic é uma array de bits. Você pode gerar um bitmap a partir de
 * um png neste website: https://javl.github.io/image2cpp/
 */
void ST7920_DrawBitmap(const unsigned char* graphic);



/**
 * @brief desenha na tela as formas geométricas que foram definidas
 */
void ST7920_Update(void);

/**
 * @brief Define um pixel a ser desenhado
 *
 * @param x coordenada x do pixel
 * @param y coordenada y do pixel
 */
void SetPixel(uint8_t x, uint8_t y);



/**
 * @brief Define uma linha a ser desenhada
 *
 * @param x0 coordenada x inicial da linha
 * @param y0 coordenada y inicial da linha
 * @param x1 coordenada x final da linha
 * @param y1 coordenada y final da linha
 */
void DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);


/**
 * @brief Define retângulo a ser desenhado
 *
 * @param x coordenada x do retângulo
 * @param y coordenada y do retângulo
 * @param w a largura (width) do retângulo
 * @param h a altura (height) do retângulo
 */
void DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h);



/**
 * @brief Define retângulo preenchido a ser desenhado
 *
 * @param x coordenada x do retângulo
 * @param y coordenada y do retângulo
 * @param w a largura (width) do retângulo
 * @param h a altura (height) do retângulo
 */
void DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h);



/**
 * @brief Define um círculo a ser desenhado
 *
 * @param x0 coordenada x do círculo
 * @param y0 coordenada y do círculo
 * @param radius raio do círculo
 */
void DrawCircle(uint8_t x0, uint8_t y0, uint8_t radius);



/**
 * @brief Define um círculo preenchido a ser desenhado
 *
 * @param x0 coordenada x do círculo
 * @param y0 coordenada y do círculo
 * @param r raio do círculo
 */
void DrawFilledCircle(int16_t x0, int16_t y0, int16_t r);



/**
 * @brief Define um triângulo a ser desenhado
 *
 * @param x1 coordenada x do primeiro vértice
 * @param y1 coordenada y do primeiro vértice
 * @param x2 coordenada x do segundo vértice
 * @param y2 coordenada y do segundo vértice
 * @param x3 coordenada x do terceiro vértice
 * @param y3 coordenada y do terceiro vértice
 */
void DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);



/**
 * @brief Define um triângulo preenchido a ser desenhado
 *
 * @param x1 coordenada x do primeiro vértice
 * @param y1 coordenada y do primeiro vértice
 * @param x2 coordenada x do segundo vértice
 * @param y2 coordenada y do segundo vértice
 * @param x3 coordenada x do terceiro vértice
 * @param y3 coordenada y do terceiro vértice
 */
void DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);

#endif // !ST7920_H
