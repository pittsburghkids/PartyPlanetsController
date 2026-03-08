#include "Globals.h"

#define CENTER_MIDI_CHANNEL 2

int lastEncoderAValue = 0;
int lastEncoderBValue = 0;

void setupCenter()
{
}

void loopCenter()
{
  // Encoders
  {
    auto encoderOne = encoders[0];
    auto encoderTwo = encoders[1];

    int encoderAValue = encoderOne.getCount();
    if (encoderAValue != lastEncoderAValue)
    {
      int value = abs(encoderAValue) % 128;
      lastEncoderAValue = encoderAValue;

      MIDI.sendControlChange(1, value, CENTER_MIDI_CHANNEL);
    }

    int encoderBValue = encoderTwo.getCount();
    if (encoderBValue != lastEncoderBValue)
    {
      int value = abs(encoderBValue) % 128;
      lastEncoderBValue = encoderBValue;

      MIDI.sendControlChange(2, value, CENTER_MIDI_CHANNEL);
    }
  }
}