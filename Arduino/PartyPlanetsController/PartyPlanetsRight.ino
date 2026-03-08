#include "Globals.h"

#define RIGHT_MIDI_CHANNEL 3
#define LEVER_COUNT 3

struct Lever
{
  PioEncoder *encoder;
  int max = INT_MIN;
  int min = INT_MAX;
  int value = 0;
  int lastValue = 0;

  bool update()
  {
    value = encoder->getCount();
    if (value != lastValue)
    {
      lastValue = value;
      return true;
    }
    return false;
  }

  float getNoramlizedValue()
  {
    if (value > max)
      max = value;
    else if (value < min)
      min = value;

    if (max == min)
      return 0.0f;

    return (float)(value - min) / (float)(max - min);
  }
};

Lever levers[LEVER_COUNT];

void setupRight()
{
  // Initalize levers.
  {

    levers[0].encoder = &encoders[0];
    levers[0].encoder->begin();
    levers[0].encoder->flip(true);
    delay(10);

    levers[1].encoder = &encoders[1];
    levers[1].encoder->begin();
    levers[1].encoder->flip(true);
    delay(10);

    levers[1].encoder = &encoders[2];
    levers[2].encoder->begin();
    levers[2].encoder->flip(true);
    delay(10);
  }
}

void loopRight()
{
  for (int i = 0; i < LEVER_COUNT; i++)
  {
    if (levers[i].update())
    {
      float value = levers[i].getNoramlizedValue();
      MIDI.sendControlChange(20 + i, value * 127, RIGHT_MIDI_CHANNEL);
    }
  }
}