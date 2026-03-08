#pragma once

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <FastLED.h>
#include <Bounce2.h>
#include "pio_encoder.h"

// MIDI

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// Buttons.

#define BUTTON_COUNT 6

#define SWITCH_ONE_PIN 5
#define SWITCH_TWO_PIN 6
#define SWITCH_THREE_PIN 7
#define SWITCH_FOUR_PIN 26
#define SWITCH_FIVE_PIN 27
#define SWITCH_SIX_PIN 28

Bounce2::Button buttons[BUTTON_COUNT];

// Encoders.

#define ENCODER_COUNT 3

#define ENCODER_ONE_PIN_A 12
#define ENCODER_ONE_PIN_B 13

#define ENCODER_TWO_PIN_A 19
#define ENCODER_TWO_PIN_B 20

#define ENCODER_THREE_PIN_A 17
#define ENCODER_THREE_PIN_B 18

PioEncoder encoders[ENCODER_COUNT] = {
    PioEncoder(ENCODER_ONE_PIN_A),
    PioEncoder(ENCODER_TWO_PIN_A),
    PioEncoder(ENCODER_THREE_PIN_A)};

// Board functions.

extern void setupLeft();
extern void loopLeft();

extern void setupCenter();
extern void loopCenter();

extern void setupRight();
extern void loopRight();
