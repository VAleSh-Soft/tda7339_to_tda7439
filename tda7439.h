#pragma once

#include <Wire1.h>
#include <Arduino.h>
#include "header_file.h"

#define I2C_PORT Wire1 // используемый I2C-интерфейс для управления TDA7439

// 7-битный адрес микросхемы на шине I2C
#define TDA7439_address 0x44

// Sub addresses
#define TDA7439_INPUT_SEL 0x00  // 0b00000000
#define TDA7439_INPUT_GAIN 0x01 // 0b00000001
#define TDA7439_VOLUME 0x02     // 0b00000010
#define TDA7439_BASS 0x03       // 0b00000011
#define TDA7439_MIDDLE 0x04     // 0b00000100
#define TDA7439_TREBBLE 0x05    // 0b00000101

#define TDA7439_RATT 0x06       // 0b00000110
#define TDA7439_LATT 0x07       // 0b00000111

// выбор входного канала
enum TDA7439_input : uint8_t
{
  INPUT_4, // вход 4
  INPUT_3, // вход 3
  INPUT_2, // вход 2
  INPUT_1  // вход 1
};

// диапазон регулировки тембра
enum TDA7439_bands : uint8_t
{
  BASS = 0x03,   // низкие частоты
  MIDDLE = 0x04, // средние частоты
  TREBBLE = 0x05 // высокие частоты
};

// TDA7439 диапазон предусиления на входе, db (с шагом 2db)
// 0x00 .. 0x0F

// TDA7439 диапазон громкости (-db)
// 0x00 .. 0x2F
#define TDA7439_MUTE 0x38 // отключение звука (mute_flag)

class TDA7439
{
public:
  /**
   * @brief Construct a new TDA7439 object
   *
   */
  TDA7439();

  /**
   * @brief инициализация TDA7439
   *
   */
  void begin();

  /**
   * @brief установка входного канала
   *
   * @param input выбранный канал - INPUT_4, INPUT_3, INPUT_2, INPUT_1
   */
  void setInput(TDA7439_input input);

  /**
   * @brief установка усиления на входе
   *
   * @param gain уровень усиления; 0..15 (0db..30db, с шагом 2db)
   */
  void setInputGain(uint8_t gain);

  /**
   * @brief установка громкости
   *
   * @param volume уровень громкости; 0..47 (0db..-47db, с шагом 1db)
   */
  void setVolume(uint8_t volume);

  /**
   * @brief установка тембра
   *
   * @param val устанавливаемое значение; -7..7 (-14db..14db, с шагом 2db)
   * @param range диапазон - BASS, MIDDLE, TREBBLE
   */
  void setTimbre(int8_t val, TDA7439_bands range);

  /**
   * @brief отключение звука
   *
   */
  void mute();

  /**
   * @brief установка баланса
   *
   * @param att_r правый канал; 0..79 (0db..-79db, с шагом 1db)
   * @param att_l левый канал; 0..79 (0db..-79db, с шагом 1db)
   */
  void spkAtt(uint8_t att_r, uint8_t att_l);

private:
  void writeWire(uint8_t reg, uint8_t data);
};

// ===================================================

TDA7439::TDA7439() {}

void TDA7439::begin()
{
	I2C_PORT.begin();
}

void TDA7439::setInput(TDA7439_input input)
{
  writeWire(TDA7439_INPUT_SEL, (uint8_t)input);
}

void TDA7439::setInputGain(uint8_t gain)
{
  if (gain > 15)
  {
    gain = 15;
  }
  writeWire(TDA7439_INPUT_GAIN, gain);
}

void TDA7439::setVolume(uint8_t volume)
{
	volume = (volume) ? ((volume <= 47) ? 47 - volume : 0) : TDA7439_MUTE;
	writeWire(TDA7439_VOLUME, volume);
}

void TDA7439::setTimbre(int8_t val, TDA7439_bands range)
{
	val = (val < -7) ? -7 : ((val > 7) ? 7 : val);
  val = (val > 0) ? 15 - val : val + 7;
  writeWire((uint8_t)range, val);
}

void TDA7439::mute()
{
  writeWire(TDA7439_VOLUME, TDA7439_MUTE);
}

void TDA7439::spkAtt(uint8_t att_r, uint8_t att_l)
{
  // Mainly used to override the default attenuation of mute at power up
  // can be used for balance with some simple code changes here.
  if (att_l > 79)
  {
    att_l = 79;
  }
  if (att_r > 79)
  {
    att_r = 79;
  }

  writeWire(TDA7439_RATT, att_r);
  writeWire(TDA7439_LATT, att_l);
}

void TDA7439::writeWire(uint8_t reg, uint8_t data)
{
	I2C_PORT.beginTransmission(TDA7439_address);
	I2C_PORT.write(reg);
	I2C_PORT.write(data);
	I2C_PORT.endTransmission();
  }

// ===================================================

TDA7439 tda;

