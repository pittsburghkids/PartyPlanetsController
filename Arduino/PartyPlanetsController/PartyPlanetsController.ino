
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

#include <FastLED.h>
#include <Bounce2.h>
#include "pio_encoder.h"

#define SLOT_COUNT 6

#define LEVER_COUNT 4

#define ENCODER_ONE_PIN_A 0
#define ENCODER_ONE_PIN_B 1
#define ENCODER_TWO_PIN_A 2
#define ENCODER_TWO_PIN_B 3
#define ENCODER_THREE_PIN_A 4
#define ENCODER_THREE_PIN_B 5
#define ENCODER_FOUR_PIN_A 6
#define ENCODER_FOUR_PIN_B 7

#define SWITCH_ONE_PIN 16
#define SWITCH_TWO_PIN 17
#define SWITCH_THREE_PIN 18
#define SWITCH_FOUR_PIN 19
#define SWITCH_FIVE_PIN 20
#define SWITCH_SIX_PIN 21

#define BUTTON_PIN 15

#define LED_DATA_PIN 28
#define LED_BRIGHTNESS 96
#define LED_COUNT 16 * SLOT_COUNT

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

CRGBArray<LED_COUNT> leds;

CRGBSet segments[SLOT_COUNT] = {
    leds(0, 15),
    leds(16, 31),
    leds(32, 47),
    leds(48, 63),
    leds(64, 79),
    leds(80, 95),
};

struct Slot
{
    Bounce2::Button *button;
    CRGBSet *leds;
    CRGB color;
};

Bounce2::Button buttons[SLOT_COUNT];

Slot slots[SLOT_COUNT] = {
    {&buttons[0], &segments[0]},
    {&buttons[1], &segments[1]},
    {&buttons[2], &segments[2]},
    {&buttons[3], &segments[3]},
    {&buttons[4], &segments[4]},
    {&buttons[5], &segments[5]},
};

Bounce2::Button mainButton;

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

PioEncoder encoderA(ENCODER_ONE_PIN_A);
PioEncoder encoderB(ENCODER_TWO_PIN_A);
PioEncoder encoderC(ENCODER_THREE_PIN_A);
PioEncoder encoderD(ENCODER_FOUR_PIN_A);

Lever levers[LEVER_COUNT] = {
    {&encoderA},
    {&encoderB},
    {&encoderC},
    {&encoderD},
};

void setup()
{
    Serial.begin(115200);

    // Initialize USB.
    {

        String productDescriptor = "Party Planets Controller";
        String serialDescriptor = "0000-0000-0000-0000-0000";

        TinyUSBDevice.setManufacturerDescriptor("Children's Museum of Pittsburgh");
        TinyUSBDevice.setProductDescriptor(productDescriptor.c_str());
        TinyUSBDevice.setSerialDescriptor(serialDescriptor.c_str());

        if (!TinyUSBDevice.isInitialized())
        {
            TinyUSBDevice.begin(0);
        }
    }

    // Initialize MIDI.
    {
        MIDI.begin(MIDI_CHANNEL_OMNI);

        if (TinyUSBDevice.mounted())
        {
            TinyUSBDevice.detach();
            delay(10);
            TinyUSBDevice.attach();
        }

        // MIDI.setHandleSystemExclusive(handleSystemExclusive);
    }

    // Initalize levers.
    {
        levers[0].encoder->begin();
        delay(10);

        levers[1].encoder->begin();
        delay(10);

        levers[2].encoder->begin();
        delay(10);

        levers[3].encoder->begin();
        delay(10);
    }

    mainButton.attach(BUTTON_PIN, INPUT_PULLUP);

    FastLED.addLeds<WS2811, LED_DATA_PIN, GRB>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LED_BRIGHTNESS);

    // Initialize slots.
    {
        slots[0].button->attach(SWITCH_ONE_PIN, INPUT_PULLUP);
        slots[0].leds->fill_solid(CRGB::Gray25);

        slots[1].button->attach(SWITCH_TWO_PIN, INPUT_PULLUP);
        slots[1].leds->fill_solid(CRGB::Gray25);

        slots[2].button->attach(SWITCH_THREE_PIN, INPUT_PULLUP);
        slots[2].leds->fill_solid(CRGB::Gray25);

        slots[3].button->attach(SWITCH_FOUR_PIN, INPUT_PULLUP);
        slots[3].leds->fill_solid(CRGB::Gray25);

        slots[4].button->attach(SWITCH_FIVE_PIN, INPUT_PULLUP);
        slots[4].leds->fill_solid(CRGB::Gray25);

        slots[5].button->attach(SWITCH_SIX_PIN, INPUT_PULLUP);
        slots[5].leds->fill_solid(CRGB::Gray25);
    }
}

void loop()
{
    if (!TinyUSBDevice.mounted())
    {
        return;
    }

    {
        mainButton.update();
        if (mainButton.fell())
        {
            Serial.println("Main button pressed");
        }
    }

    for (int i = 0; i < SLOT_COUNT; i++)
    {
        slots[i].button->update();
        if (slots[i].button->fell())
        {
            Serial.print("Slot ");
            Serial.print(i);
            Serial.println(" button pressed");
            slots[i].leds->fill_solid(CRGB::Purple);

            // Joystick.button(i + 1, true);
            MIDI.sendNoteOn(60 + i, 127, 1);
        }
        else if (slots[i].button->rose())
        {
            Serial.print("Slot ");
            Serial.print(i);
            Serial.println(" button released");
            slots[i].leds->fill_solid(CRGB::Gray25);

            // Joystick.button(i + 1, false);
            MIDI.sendNoteOff(60 + i, 0, 1);
        }
    }

    for (int i = 0; i < LEVER_COUNT; i++)
    {
        if (levers[i].update())
        {
            Serial.print("Lever ");
            Serial.print(i);
            Serial.print(" value: ");
            Serial.println(levers[i].getNoramlizedValue());

            MIDI.sendControlChange(i, levers[i].getNoramlizedValue() * 127, 1);
        }

        FastLED.show();
    }
}
