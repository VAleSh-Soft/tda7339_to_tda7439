/*
 * Файл с методами для расшифровки команд процессора музыкального центра,
 * отправляемых на темброблок TDA7339 (как считает процессор), и вызов
 * соответствующих методов для работы с темброблоком TDA7439
 */

#pragma once

#include <Wire.h>
#include "tda7439.h"

TDA7439 tda7439;

// ===================================================

void tda7339_init(uint8_t _addr = 0x42);
void receiveEvent(int howMany);
void receiveInput();
void receiveVolume();
void receiveEq();

// ===================================================

void tda7339_init(uint8_t _addr)
{
  Wire.begin(_addr);
  Wire.onReceive(receiveEvent);
  tda7439.begin();
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
    Serial.print("input: ");
    switch (x)
    {
    case 0xEE:
      Serial.println(1);
      tda7439.setInput(INPUT_1);
      break;
    case 0xEA:
      Serial.println(2);
      tda7439.setInput(INPUT_2);
      break;
    case 0xE6:
      Serial.println(3);
      tda7439.setInput(INPUT_3);
      break;
    }
  }
  else
  {
    Serial.println("unknown input data??");
  }
}

void receiveVolume()
{
  uint8_t x = Wire.read();
  uint8_t y = Wire.read();

  uint8_t vol = 0;

  if (!((x << 7) & 0x00) && !((y << 7) & 0x01))
  {
    Serial.print("volume: ");
    x = x >> 1;
    y = y >> 1;
    if (x == 0)
    {
      if (y <= 4)
      {
        vol = 32 - y;
      }
      else if (y <= 32)
      {
        vol = 28 - (y - 4) / 2;
      }
      else
      {
        vol = 14 - (y - 32) / 3;
      }
    }
    else
    {
      if (x <= 18)
      {
        vol = 9 - x / 3;
      }
      else
      {
        switch (x)
        {
        case 24:
          vol = 2;
          break;
        case 34:
          vol = 1;
          break;
        case 63:
          vol = 0;
          break;
        default:
          Serial.println("unknown volume data??");
          return;
        }
      }
    }

    Serial.println(vol);
    tda7439.setVolume(vol * 3 / 2);
  }
  else
  {
    Serial.println("unknown volume data??");
  }
}

void _setEq(uint8_t e)
{
  TDA7439_bands band;
  switch (e >> 5)
  {
  case 0x04:
    Serial.print("  treble: ");
    band = TREBBLE;
    break;
  case 0x05:
    Serial.print("  middle: ");
    band = MIDDLE;
    break;
  case 0x06:
    Serial.print("  bass:   ");
    band = BASS;
    break;
  default:
    Serial.print("unknown eq data?? - ");
    Serial.println(e);
    return;
  }
  e = e << 3;
  e = e >> 3;
  Serial.print(" - ");
  int8_t x = (e <= 15) ? e/2 : ((e - 16) * -1) / 2;
  tda7439.setSnd(x, band);
  Serial.println(x);
}

void receiveEq()
{
  Serial.println("equaliser");
  uint8_t x = Wire.read();
  uint8_t y = Wire.read();
  uint8_t z = Wire.read();
  _setEq(x);
  _setEq(y);
  _setEq(z);
}
