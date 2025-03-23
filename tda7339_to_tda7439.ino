#include "header_file.h"
#include "tda7339.h"
#include "tda7439.h"

// ===================================================

#if USE_EXTERNAL_SOUND_SOURCE
void changeInput4State()
{
  EEPROM.update(EEPROM_INDEX_FOR_INPUT_STATE, int_inputs_state);
  digitalWrite(RLED_PIN, int_inputs_state);
  digitalWrite(GLED_PIN, !int_inputs_state);

  if (!int_inputs_state)
  {
    tda7439.setVolume(TDA7439_MUTE);
    // выставляем средний уровень по всем полосам эквалайзера, пускай этим рулит внешний источник
    tda7439.setTimbre(0, BASS);
    tda7439.setTimbre(0, MIDDLE);
    tda7439.setTimbre(0, TREBBLE);
    // активируем четвертый вход
    setNewInput(INPUT_4);
    TDA_PRINTLN(F("External sound source activated"));
  }
  else
  {
    // активируем работу от внутренних источников звука
    tda7439.setVolume(TDA7439_MUTE);
    tda7439.setTimbre(tda7439_bass, BASS);
    tda7439.setTimbre(tda7439_middle, MIDDLE);
    tda7439.setTimbre(tda7439_trebble, TREBBLE);

    setNewInput(tda7439_input);

    tda7439_output = NO_SET;
    TDA_PRINTLN(F("External sound source inactive"));
  }
}
#endif

void setup()
{
#if USE_DEBUG_OUT
  Serial.begin(DEBUG_BAUD_COUNT);
#endif

#if USE_EXTERNAL_SOUND_SOURCE
  pinMode(RLED_PIN, OUTPUT);
  pinMode(GLED_PIN, OUTPUT);

  int_inputs_state = EEPROM.read(EEPROM_INDEX_FOR_INPUT_STATE);
#endif
  tda7339_init();

  TDA_PRINTLN(F("Start device"));
  TDA_PRINTLN();
}

void loop()
{
  tda7339_tick();

#if USE_EXTERNAL_SOUND_SOURCE
  // работаем с кнопкой
  if (mp3Btn.getButtonState() == BTN_DOWN)
  {
    int_inputs_state = !int_inputs_state;
    changeInput4State();
  }
#endif
}
