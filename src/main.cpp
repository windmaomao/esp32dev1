#include <AiEsp32RotaryEncoder.h>
#include <Arduino.h>
#include <Button2.h>
#include <BleMouse.h>

#define PIN_LED LED_BUILTIN
#define PIN_KEY (4)
#define PIN_ROTARY_LEFT (18)
#define PIN_ROTARY_RIGHT (19)
#define TIME_INACTIVITY (1000 * 60 * 5)

Button2 key;
volatile int count = 0;
AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(PIN_ROTARY_LEFT, PIN_ROTARY_RIGHT);
BleMouse bleMouse;
unsigned long lastActivityTime = 0;

void keepActive()
{
  lastActivityTime = millis();
  bleMouse.move(0, 0, 0, 1);
}

bool isIdle()
{
  return millis() - lastActivityTime >= TIME_INACTIVITY;
}

IRAM_ATTR void onPress(Button2 &btn)
{
  digitalWrite(PIN_LED, HIGH);
}

IRAM_ATTR void onRelease(Button2 &btn)
{
  digitalWrite(PIN_LED, LOW);
  count++;
  Serial.println(count);
  bleMouse.click(MOUSE_LEFT);
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
  rotary.setAcceleration(0);

  bleMouse.begin();
}

void loop()
{
  if (!bleMouse.isConnected())
  {
    Serial.print(".");
    // Note: no delay can be used here, otherwise
    // the device will not be able to connect.
    return;
  }

  if (isIdle())
  {
    Serial.println("Device is idle. Keeping it alive...");
    keepActive();
    return;
  }

  key.loop();

  int res = rotary.encoderChanged();
  if (res != 0)
  {
    bleMouse.move(0, 0, res);
  }
}
