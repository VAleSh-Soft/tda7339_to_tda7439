/*
 * Файл с методами для расшифровки команд процессора музыкального центра,
 * отправляемых на темброблок TDA7339 (как считает процессор), и вызов
 * соответствующих методов для работы с темброблоком TDA7439
 */

#pragma once

#include <Wire.h>
#include "header_file.h"
#include "tda7439.h"

// ===================================================

void tda7339_init(uint8_t _addr = 0x42);
void tda7339_tick();
void receiveEvent(int howMany);
void receiveInput();
void receiveVolume();
void receiveEq();

// ===================================================

void tda7339_init(uint8_t _addr)
{
  tda7439.begin();
  tda7439.spkAtt(15, 15);
  tda7439.setInputGain(0);
  // при старте установка эквалайзера - rock
  tda7439.setTimbre(4, BASS);
  tda7439.setTimbre(-1, MIDDLE);
  tda7439.setTimbre(3, TREBBLE);

  Wire.begin(_addr);
  Wire.onReceive(receiveEvent);
}

void tda7339_tick()
{
  switch (tda7439_output)
  {
  case INPUT_SET:
    tda7439.setInput(tda7439_input);
    tda7439_output = NO_SET;
    break;
  case VOLUME_SET:
    tda7439.setVolume(tda7439_volume);
    tda7439_output = NO_SET;
    break;
  case EQ_SET:
    tda7439.setTimbre(tda7439_trebble, TREBBLE);
    tda7439.setTimbre(tda7439_middle, MIDDLE);
    tda7439.setTimbre(tda7439_bass, BASS);
    tda7439_output = NO_SET;
    break;
  case NO_SET:
    break;
  }
}

void receiveEvent(int howMany)
{
  (void)howMany;

  uint8_t num = Wire.available();

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
  while (1 < Wire.available())
  {
    char c = Wire.read();
  }
  int x = Wire.read();
}

void receiveInput()
{
  uint8_t x = Wire.read();
  if (x >> 5 == 0x07)
  {
    tda7439_output = INPUT_SET;
    TDA_PRINT(F("New input: "));
    switch (x)
    {
    case 0xEE:
      TDA_PRINTLN(1);
      tda7439_input = INPUT_1;
      break;
    case 0xEA:
      TDA_PRINTLN(2);
      tda7439_input = INPUT_2;
      break;
    case 0xE6:
      TDA_PRINTLN(3);
      tda7439_input = INPUT_3;
      break;
    }
  }
  else
  {
    TDA_PRINTLN(F("unknown input data??"));
  }
}

void receiveVolume()
{
  uint8_t x = Wire.read();
  uint8_t y = Wire.read();

  if (!((x << 7) & 0x00) && !((y << 7) & 0x01))
  {
    TDA_PRINT(F("New volume set: "));
    tda7439_volume = ((x >> 1) + (y >> 1)) / 2;
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
  uint8_t x = Wire.read();
  uint8_t y = Wire.read();
  uint8_t z = Wire.read();
  _setEq(x);
  _setEq(y);
  _setEq(z);
  tda7439_output = EQ_SET;
}
