#pragma once
#include "tda7439.h"

// ===================================================

#define USE_DEBUG_OUT 1       // включить вывод отладочной информации в сериал
#define DEBUG_BAUD_COUNT 9600 // скорость передачи данных в сериал

// ===================================================

#if USE_DEBUG_OUT
#define TDA_PRINTLN(x) Serial.println(x)
#define TDA_PRINT(x) Serial.print(x)
#else
#define TDA_PRINTLN(x)
#define TDA_PRINT(x)
#endif

// ===================================================

TDA7439 tda7439;

TDA7439_input tda7439_input = INPUT_1;
uint8_t tda7439_volume = 0;
// uint8_t tda7439_att = 0;
int8_t tda7439_trebble = 0;
int8_t tda7439_middle = 0;
int8_t tda7439_bass = 0;

enum TDA7439_output
{
  NO_SET,
  INPUT_SET,
  VOLUME_SET,
  EQ_SET
};

TDA7439_output tda7439_output = NO_SET;
