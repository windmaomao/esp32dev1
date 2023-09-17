#include <AiEsp32RotaryEncoder.h>
#include <Arduino.h>
#include <Button2.h>

#define PIN_LED LED_BUILTIN
#define PIN_KEY (4)
#define PIN_ROTARY_LEFT (18)
#define PIN_ROTARY_RIGHT (19)

Button2 key;
volatile int count = 0;
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(PIN_ROTARY_LEFT, PIN_ROTARY_RIGHT);

IRAM_ATTR void onPress(Button2 &btn)
{
  digitalWrite(PIN_LED, HIGH);
}

IRAM_ATTR void onRelease(Button2 &btn)
{
  digitalWrite(PIN_LED, LOW);
  count++;
  Serial.println(count);
}

void IRAM_ATTR readEncoderISR()
{
  rotary.readEncoder_ISR();
}

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  key.begin(PIN_KEY);
  key.setPressedHandler(onPress);
  key.setReleasedHandler(onRelease);
  pinMode(PIN_ROTARY_LEFT, INPUT_PULLUP);
  pinMode(PIN_ROTARY_RIGHT, INPUT_PULLUP);
  rotary.begin();
  rotary.setup(readEncoderISR);
  rotary.areEncoderPinsPulldownforEsp32 = false;
}

void loop()
{
  key.loop();
  if (rotary.encoderChanged())
  {
    Serial.println(rotary.readEncoder());
  }
}
