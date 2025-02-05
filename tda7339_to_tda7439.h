#pragma once

// ===================================================

#define USE_DEBUG_OUT 1         // включить вывод отладочной информации в сериал
#define DEBUG_BAUD_COUNT 9600 // скорость передачи данных в сериал

// ===================================================

#if USE_DEBUG_OUT
#define TDA_PRINTLN(x) Serial.println(x)
#define TDA_PRINT(x) Serial.print(x)
#else
#define TDA_PRINTLN(x)
#define TDA_PRINT(x)
#endif
