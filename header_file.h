#pragma once
#include "tda7439.h"

// ===================================================

#define USE_DEBUG_OUT 1             // включить вывод отладочной информации в сериал
#define DEBUG_BAUD_COUNT 9600       // скорость передачи данных в сериал
#define USE_EXTERNAL_SOUND_SOURCE 1 // использовать дополнительный источник звука, например mp3-модуль, на четвертом входе TDA7439

// ===================================================
#if USE_EXTERNAL_SOUND_SOURCE

constexpr uint8_t BUTTON_PIN = 5; // пин для подключения кнопки
constexpr uint8_t RLED_PIN = 6;   // пин для подключения красного светодиода
constexpr uint8_t GLED_PIN = 7;   // пин для подключения зеленого светодиода

constexpr uint16_t EEPROM_INDEX_FOR_INPUT_STATE = 10; // индекс в EEPROM для хранения текущего режима работы муз.центра - внутренний/внешний источник звука

#endif

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

#if USE_EXTERNAL_SOUND_SOURCE

#include <shButton.h>
#include <EEPROM.h>

shButton mp3Btn(BUTTON_PIN);

bool int_inputs_state = true; // true - внутренние источники звука, false - внешний источник звука

// работа с кнопкой
void btnCheck();
// изменение состояния четвертого входа tda7439 - активен/неактивен
void changeInput4State();

#endif
