#pragma once
#include "tda7439.h"

// ==== общие настройки ==============================

#define USE_DEBUG_OUT 1             // включить вывод отладочной информации через UART
#define DEBUG_BAUD_COUNT 9600       // скорость интерфейса UART
#define USE_EXTERNAL_SOUND_SOURCE 1 // использовать дополнительный источник звука, например mp3-модуль, на четвертом входе TDA7439; 0 - не использовать, 1 - использовать
#define NO_MUTE_FOR_INPUT4 1        // не пропускать сигнал MUTE от центрального процессора, если активен четвертый вход

#define TDA7339_I2C_PORT Wire  // I2C-интефейс для получения команд от центрального процессора
#define TDA7439_I2C_PORT Wire1 // I2C-интефейс для работы с tda7439

#if USE_EXTERNAL_SOUND_SOURCE
// ==== пины для подключения обвязки mp3-модуля ======

constexpr uint8_t BUTTON_PIN = PIN_PD5; // пин для подключения кнопки
constexpr uint8_t RLED_PIN = PIN_PD6;   // пин для подключения красного светодиода
constexpr uint8_t GLED_PIN = PIN_PD7;   // пин для подключения зеленого светодиода

constexpr uint16_t EEPROM_INDEX_FOR_INPUT_STATE = 10; // индекс в EEPROM для хранения текущего режима работы муз.центра - внутренний/внешний источник звука; uint8_t

#endif

constexpr uint16_t EEPROM_INDEX_FOR_SOUND_SETTINGS = 11; // индекс в EEPROM для хранения данных коррекции громкости звука для входов; uint8_t x 4
constexpr uint16_t EEPROM_INDEX_FOR_VALIDATE_FLAG = 9;   // индекс в EEPROM для хранения флага о том, что параметры звука сохранены и записаны в EEPROM; uint8_t
constexpr uint8_t VALIDATE_FLAG = 0X2F;                  // значение флага

// данные настройки звука по входам по умолчанию; значения в пределах -60..15;
// нумерация входов обратная - 4, 3, 2, 1;
const int8_t sound_data[4] = {-30, -15, -15, -15};

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

uint8_t cur_input_gain = 0;
uint8_t cur_input_att = 15;

#if USE_EXTERNAL_SOUND_SOURCE
uint8_t tda7439_volume_in4 = tda7439_volume;
#endif

bool service_mode = false; // флаг включения сервисного режима регулировки громкости

enum TDA7439_output : uint8_t
{
  NO_SET,
  INPUT_SET,
  VOLUME_SET,
  EQ_SET
};

TDA7439_output tda7439_output = NO_SET;

struct ChangeSoundSettings
{
  bool _dir;
  bool _cont;
};

#if USE_EXTERNAL_SOUND_SOURCE

#include <shButton.h>
#include <EEPROM.h>

shButton mp3Btn(BUTTON_PIN);

bool int_inputs_state = true; // true - внутренние источники звука, false - внешний источник звука

// изменение состояния четвертого входа tda7439 - активен/неактивен
void changeInput4State();
// работа с кнопкой
void checkButton();
// управление светодиодами
void ledsGuard();

#endif

// получение настроек звука для входа из EEPROM
void getSoundSettings(TDA7439_input _input);
// настройка звука текущего входа - предусиление и аттеньюатор
void changeSoundSettings(TDA7439_input _input, uint8_t _reset = 0);
