#pragma once

#include <Wire1.h>
#include "tda7339_to_tda7439.h"

#define TDA7439_address 0x44

#define TDA7439_INPUT_SEL 0x00
#define TDA7439_INPUT_GAIN 0x01
#define TDA7439_VOLUME 0x02
#define TDA7439_BASS 0x03
#define TDA7439_MIDDLE 0x04
#define TDA7439_TREBBLE 0x05

#define TDA7439_RATT 0x06
#define TDA7439_LATT 0x07

// выбор входного канала
enum TDA7439_input : uint8_t
{
	INPUT_4, // вход 4
	INPUT_3, // вход 3
	INPUT_2, // вход 2
	INPUT_1	 // вход 1
};

// диапазон регулировки тембра
enum TDA7439_bands : uint8_t
{
	BASS = 0x03,	 // низкие частоты
	MIDDLE = 0x04, // средние частоты
	TREBBLE = 0x05 // высокие частоты
};

#define TDA7439_MUTE 0x38 // Mute main volume

#include <Arduino.h>
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
	 * @param volume уровень громкости; 0..48 (0db..-47db, с шагом 1db)
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
	TwoWire *_wire = &Wire1;

	void writeWire(uint8_t reg, uint8_t data);
};

TDA7439::TDA7439() {}

void TDA7439::begin()
{
	_wire->begin();
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
	if (volume == 0 || volume > 48)
	{
		volume = TDA7439_MUTE;
	}

	writeWire(TDA7439_VOLUME, (48 - volume));
}

void TDA7439::setTimbre(int8_t val, TDA7439_bands range)
{
	val = (val < -7) ? -7
									 : ((val > 7) ? 7
																: val);
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
	_wire->beginTransmission(TDA7439_address);
	if (!_wire->endTransmission())
	{
		_wire->beginTransmission(TDA7439_address);
		_wire->write(reg);
		_wire->write(data);
		_wire->endTransmission();
	}
}