/*
 * Файл с методами для расшифровки команд процессора музыкального центра,
 * отправляемых на темброблок TDA7339 (как считает процессор), и вызов
 * соответствующих методов для работы с темброблоком TDA7439
 */

#pragma once

#include "header_file.h"
#include <shTDA7439.h>

#ifndef TDA7339_I2C_PORT
#define TDA7339_I2C_PORT Wire
#endif

#include <Wire.h>
#include <Wire1.h>

#define TDA7339_MUTE 0x3F

// ===================================================

void tda7339_init(uint8_t _addr = 0x42);
void tda7339_tick();
void setNewInput(TDA7439_input input);
void receiveEvent(int howMany);
void receiveInput();
void receiveVolume();
void receiveEq();

// ===================================================

void tda7339_init(uint8_t _addr)
{
  tda7439.begin(&TDA7439_I2C_PORT);

#if USE_EXTERNAL_SOUND_SOURCE
  // активировать активный в момент отключения муз.центра источник звука - внешний (вход 4)/внутренний
  changeSoundSettings(INPUT_4, 2);
  changeInput4State();
#else
  // при старте установка эквалайзера - rock
  tda7439.setTimbre(4, -1, 3);

  setNewInput(INPUT_1);
#endif

  TDA7339_I2C_PORT.begin(_addr);
  TDA7339_I2C_PORT.onReceive(receiveEvent);
}

void tda7339_tick()
{
  switch (tda7439_output)
  {
  case INPUT_SET:
#if USE_EXTERNAL_SOUND_SOURCE
    if (int_inputs_state)
#endif
    {
      setNewInput(tda7439_input);
    }
    tda7439_output = NO_SET;
    return;
  case VOLUME_SET:
#if USE_EXTERNAL_SOUND_SOURCE && NO_MUTE_FOR_INPUT4
    if (!int_inputs_state && (tda7439_volume == TDA7439_MUTE || tda7439_volume == TDA7339_MUTE))
    {
      tda7439_output = NO_SET;
      return;
    }
    else
#endif
    {
      tda7439.setVolume(tda7439_volume);
      tda7439_output = NO_SET;
      return;
    }
  case EQ_SET:
#if USE_EXTERNAL_SOUND_SOURCE
    if (int_inputs_state)
#endif
    {
      tda7439.setTimbre(tda7439_bass, tda7439_middle, tda7439_trebble);
    }
    tda7439_output = NO_SET;
    return;
  case NO_SET:
    return;
  }
}

void setNewInput(TDA7439_input _input)
{
  uint8_t _vol = tda7439_volume;

#if USE_EXTERNAL_SOUND_SOURCE
  changeSoundSettings(_input, 2);

  if (_input == INPUT_4)
  {
    _vol = tda7439_volume_in4;
  }
#endif

  getSoundSettings(_input);

  tda7439.setVolume(0);
  tda7439.setInput(_input);
  tda7439.setInputGain(cur_input_gain);
  tda7439.setSpeakerAtt(cur_input_att);
  tda7439.setVolume(_vol);
}

void receiveEvent(int howMany)
{
  (void)howMany;

  uint8_t num = TDA7339_I2C_PORT.available();

  switch (num)
  {
  case 1: // получен 1 байт - команда выбора входа
    receiveInput();
    return;
  case 2: // получено 2 байта - установить громкость
    receiveVolume();
    return;
  case 3: // получено 3 байта - установить тембр (басы, средние, высокие частоты)
    receiveEq();
    return;
  }

  // если получено непонятно что, очистить буфер шины
  while (1 < TDA7339_I2C_PORT.available())
  {
    TDA7339_I2C_PORT.read();
  }
  TDA7339_I2C_PORT.read();
}

void receiveInput()
{
  uint8_t x = TDA7339_I2C_PORT.read();
  if (x >> 5 == 0x07) // байт номера входа имеет три старших бита 111
  {
    x = x << 4;
    x = x >> 6;
    tda7439_input = (TDA7439_input)x;
    tda7439_output = INPUT_SET;
    TDA_PRINT(F("New input: "));
    TDA_PRINTLN(4 - x);
  }
  else
  {
    TDA_PRINTLN(F("unknown input data??"));
  }
}

void receiveVolume()
{
  uint8_t x = TDA7339_I2C_PORT.read();
  uint8_t y = TDA7339_I2C_PORT.read();

  // процессор муз.центра регулирует громкость двумя байтами - '1st Vol' и '2nd Vol' - одновременно, каждое может принимать значения 0..47, поэтому берем среднее значение от их суммы. В итоге получаем число от 0 до 47, как нам и нужно
  if (!((x >> 7) & 0x01) && !((y >> 7) & 0x01)) // байты громкости имеют нулевые старшие биты
  {
    TDA_PRINT(F("New volume set: "));
    // нулевые биты означают: 0 - 1st, 1 - 2nd, поэтому их отбрасываем
    tda7439_volume = ((x >> 1) + (y >> 1)) / 2;
    if ((tda7439_volume == TDA7339_MUTE) || (tda7439_volume == 47))
    {
      tda7439_volume = 0;
    }
    else
    {
      tda7439_volume = 47 - tda7439_volume;
    }

#if USE_EXTERNAL_SOUND_SOURCE
    if (tda7439_volume != 0)
    {
      tda7439_volume_in4 = tda7439_volume;
    }
#endif

    TDA_PRINTLN(tda7439_volume);
    tda7439_output = VOLUME_SET;
  }
  else
  {
    TDA_PRINTLN(F("unknown volume data??"));
  }
}

static void _setEq(uint8_t e)
{
  // байты тембра имеют старшие биты: 100 - trebble, 101 - middle, 110 - bass
  uint8_t band = e >> 5;
  e = e << 3;
  e = e >> 3;
  int8_t x = (e <= 15) ? e / 2 : ((e - 16) * -1) / 2;

  switch (band)
  {
  case 0x04:
    TDA_PRINT(F("  treble: - "));
    tda7439_trebble = x;
    break;
  case 0x05:
    TDA_PRINT(F("  middle: - "));
    tda7439_middle = x;
    break;
  case 0x06:
    TDA_PRINT(F("  bass:   - "));
    tda7439_bass = x;
    break;
  default:
    TDA_PRINT(F("  unknown eq data?? - "));
    TDA_PRINTLN(e);
    return;
  }
  TDA_PRINTLN(x);
}

void receiveEq()
{
  TDA_PRINTLN(F("New equaliser set"));
  uint8_t x = TDA7339_I2C_PORT.read();
  uint8_t y = TDA7339_I2C_PORT.read();
  uint8_t z = TDA7339_I2C_PORT.read();
  _setEq(x);
  _setEq(y);
  _setEq(z);
  tda7439_output = EQ_SET;
}
