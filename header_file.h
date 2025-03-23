#pragma once
#include "tda7439.h"

// ==== общие настройки ==============================

#define USE_DEBUG_OUT 0             // включить вывод отладочной информации через UART
#define DEBUG_BAUD_COUNT 9600       // скорость интерфейса UART
#define USE_EXTERNAL_SOUND_SOURCE 1 // использовать дополнительный источник звука, например mp3-модуль, на четвертом входе TDA7439; 0 - не использовать, 1 - использовать
#define NO_MUTE_FOR_INPUT4 0        // не пропускать сигнал MUTE от центрального процессора, если активен четвертый вход

#define TDA7339_I2C_PORT Wire  // I2C-интефейс для получения команд от центрального процессора
#define TDA7439_I2C_PORT Wire1 // I2C-интефейс для работы с tda7439

// ==== индивидуальные настройки входов ==============

constexpr uint8_t INPUT1_GAIN = 0; // уровень усиления входного сигнала первого входа (0..15)
constexpr uint8_t INPUT2_GAIN = 0; // уровень усиления входного сигнала второго входа (0..15)
constexpr uint8_t INPUT3_GAIN = 0; // уровень усиления входного сигнала третьего входа (0..15)
#if USE_EXTERNAL_SOUND_SOURCE
constexpr uint8_t INPUT4_GAIN = 0; // уровень усиления входного сигнала четвертого входа (0..15)
#endif

constexpr uint8_t INPUT1_ATT = 15; // уровень приглушения звука для первого входа (0..79, чем больше тем сильнее приглушение)
constexpr uint8_t INPUT2_ATT = 15; // уровень приглушения звука для второго входа (0..79, чем больше тем сильнее приглушение)
constexpr uint8_t INPUT3_ATT = 15; // уровень приглушения звука для третьего входа (0..79, чем больше тем сильнее приглушение)
#if USE_EXTERNAL_SOUND_SOURCE
constexpr uint8_t INPUT4_ATT = 30; // уровень приглушения звука для четвертого входа (0..79, чем больше тем сильнее приглушение)
#endif

#if USE_EXTERNAL_SOUND_SOURCE
// ==== пины для подключения обвязки mp3-модуля ======

constexpr uint8_t BUTTON_PIN = PIN_PD5; // пин для подключения кнопки
constexpr uint8_t RLED_PIN = PIN_PD6;   // пин для подключения красного светодиода
constexpr uint8_t GLED_PIN = PIN_PD7;   // пин для подключения зеленого светодиода

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
uint8_t tda7439_volume = 15;
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

// изменение состояния четвертого входа tda7439 - активен/неактивен
void changeInput4State();

#endif
