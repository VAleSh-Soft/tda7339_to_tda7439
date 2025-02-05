#include "tda7339_to_tda7439.h"
#include "tda7339.h"
#include "tda7439.h"

// ===================================================

void setup()
{
#if USE_DEBUG_OUT
  Serial.begin(DEBUG_BAUD_COUNT);
#endif
  TDA_PRINTLN(F("Start device"));
  TDA_PRINTLN();
  tda7339_init();
}

void loop()
{}
