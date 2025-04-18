#include "header_file.h"
#include "tda7339.h"
#include <shTDA7439.h>
#include "Arduino.h"

// ===================================================

#if USE_EXTERNAL_SOUND_SOURCE
void changeInput4State()
{
  EEPROM.update(EEPROM_INDEX_FOR_INPUT_STATE, int_inputs_state);

  if (!int_inputs_state)
  {
    tda7439.setVolume(0);
    // выставляем средний уровень по всем полосам эквалайзера, пускай этим рулит внешний источник
    tda7439.setTimbre(0, 0, 0);
    // активируем четвертый вход
    setNewInput(INPUT_4);
    TDA_PRINTLN(F("External sound source activated"));
  }
  else
  {
    // активируем работу от внутренних источников звука
    tda7439.setVolume(0);
    tda7439.setTimbre(tda7439_bass, tda7439_middle, tda7439_trebble);

    setNewInput(tda7439_input);

    tda7439_output = NO_SET;
    TDA_PRINTLN(F("External sound source inactive"));
  }
}

void checkButton()
{
  switch (mp3Btn.getButtonState())
  {
  case BTN_ONECLICK:
    int_inputs_state = !int_inputs_state;
    changeInput4State();
    break;
  case BTN_LONGCLICK:
    if (service_mode)
    {
      if (int_inputs_state)
      {
        changeSoundSettings(tda7439_input);
      }
      else
      {
        changeSoundSettings(INPUT_4);
      }
    }
    break;
  case BTN_UP:
    changeSoundSettings(tda7439_input, 1);
    break;
  }
}

void ledsGuard()
{
  static uint32_t led_timer = 0;
  if (millis() - led_timer >= 50)
  {
    led_timer = millis();
    uint8_t rled_state = int_inputs_state;
    uint8_t gled_state = !rled_state;

    if (service_mode)
    {
      // в сервисном режиме светодиоды мигают с частотой 1 Гц
      static uint8_t num = 0;

      if (num >= 10)
      {
        rled_state = LOW;
        gled_state = LOW;
      }

      if (++num >= 20)
      {
        num = 0;
      }
    }

    digitalWrite(RLED_PIN, rled_state);
    digitalWrite(GLED_PIN, gled_state);
  }
}

uint8_t _get_sound_settings_from_eeprom(TDA7439_input _input)
{
  int8_t _set = EEPROM.read(EEPROM_INDEX_FOR_SOUND_SETTINGS + (uint8_t)_input);

  if (_set < MAX_ATT_DATA || _set > MAX_GAIN_DATA)
  {
    _set = sound_data[(uint8_t)_input];
    EEPROM.update(EEPROM_INDEX_FOR_SOUND_SETTINGS + (uint8_t)_input, _set);
  }

  return (_set);
}

void changeSoundSettings(TDA7439_input _input, uint8_t _reset)
{
  static ChangeSoundSettings _data = {._dir = true,
                                      ._cont = false};

  switch (_reset)
  {
  case 1:
    if (_data._cont)
    {
      _data._dir = !_data._dir;
    }
    return;
  case 2:
    _data._dir = true;
    _data._cont = false;
    return;
  }

  _data._cont = true;

  int8_t _set = _get_sound_settings_from_eeprom(_input);

  int8_t _step = (_set >= 0) ? 1 : 2;
  if (!_data._dir)
  {
    _step = -1 * _step;
  }

  _set += _step;

  if (_set > MAX_GAIN_DATA)
  {
    _set = MAX_GAIN_DATA;
  }
  else if (_set < MAX_ATT_DATA)
  {
    _set = MAX_ATT_DATA;
  }

  EEPROM.update(EEPROM_INDEX_FOR_SOUND_SETTINGS + (uint8_t)_input, _set);

  cur_input_gain = (_set > 0) ? _set : 0;
  cur_input_att = (_set < 0) ? (-1 * _set) : 0;

  tda7439.setInputGain(cur_input_gain);
  tda7439.setSpeakerAtt(cur_input_att);

  TDA_PRINT(F("New Input Gain: "));
  TDA_PRINTLN(cur_input_gain);
  TDA_PRINT(F("New Input Att: "));
  TDA_PRINTLN(cur_input_att);
}

#endif

void getSoundSettings(TDA7439_input _input)
{
#if USE_EXTERNAL_SOUND_SOURCE
  int8_t _set = _get_sound_settings_from_eeprom(_input);
#else
  int8_t _set = sound_data[(uint8_t)_input];
#endif

  cur_input_gain = (_set > 0) ? _set : 0;
  cur_input_att = (_set < 0) ? (-1 * _set) : 0;

  TDA_PRINT(F("Input Gain: "));
  TDA_PRINTLN(cur_input_gain);
  TDA_PRINT(F("Input Att: "));
  TDA_PRINTLN(cur_input_att);
}

void setup()
{
#if USE_DEBUG_OUT
  Serial.begin(DEBUG_BAUD_COUNT);
#endif

#if USE_EXTERNAL_SOUND_SOURCE
  if (EEPROM.read(EEPROM_INDEX_FOR_VALIDATE_FLAG) != VALIDATE_FLAG)
  {
    // валидация данных в EEPROM
    EEPROM.write(EEPROM_INDEX_FOR_VALIDATE_FLAG, VALIDATE_FLAG);
    for (uint8_t i = 0; i < 4; i++)
    {
      EEPROM.write(EEPROM_INDEX_FOR_SOUND_SETTINGS + i, sound_data[i]);
    }
  }

  pinMode(RLED_PIN, OUTPUT);
  pinMode(GLED_PIN, OUTPUT);

  int_inputs_state = EEPROM.read(EEPROM_INDEX_FOR_INPUT_STATE);

  service_mode = !digitalRead(BUTTON_PIN);

  mp3Btn.setVirtualClickOn();
  mp3Btn.setTimeoutOfDblClick(100);
  if (service_mode)
  {
    mp3Btn.setLongClickMode(LCM_CLICKSERIES);
    mp3Btn.setIntervalOfSerial(500);
    while (!digitalRead(BUTTON_PIN))
    {
      // если запустили сервисный режим, то ждем отпускания кнопки, чтобы не было ложных срабатываний
      // не держите кнопку зажатой долго ))
      ledsGuard();
    }
  }

#endif

  tda7339_init(TDA7339_I2C_ADDRESS);

  TDA_PRINTLN(F("Start device"));
  TDA_PRINTLN();
}

void loop()
{
  tda7339_tick();

#if USE_EXTERNAL_SOUND_SOURCE
  ledsGuard();
  checkButton();
#endif
}
