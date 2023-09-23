#include <AiEsp32RotaryEncoder.h>
#include <Arduino.h>
#include <Button2.h>
#include <BleMouse.h>
#include <RGBLed.h>
#include <arduino-timer.h>

#define PIN_LED LED_BUILTIN
#define PIN_KEY (4)
#define PIN_ROTARY_LEFT (18)
#define PIN_ROTARY_RIGHT (19)
#define PIN_RED (26)
#define PIN_GREEN (27)
#define PIN_BLUE (25)
#define PIN_KEY2 (23)

AiEsp32RotaryEncoder rotary = AiEsp32RotaryEncoder(PIN_ROTARY_LEFT, PIN_ROTARY_RIGHT);
BleMouse bleMouse;
RGBLed rgbLed(PIN_RED, PIN_GREEN, PIN_BLUE, RGBLed::COMMON_CATHODE);
hw_timer_t *timer = NULL;
Button2 scrollToggler;
volatile bool scrollOn = false;
volatile bool scrollForward = true;
Button2 focusButton;
Timer<10> timer2;

void timerISR()
{
  // keep from idle
  bleMouse.move(1, 0);

  // enable scroll
  if (scrollOn)
  {
    if (scrollForward)
      bleMouse.move(0, 0, 1);
    else
      bleMouse.move(0, 0, -1);
  }
}

void toggleScroll(bool forward)
{
  scrollOn = !scrollOn;
  scrollForward = forward;
}

bool onTimer(void *argument)
{
  Serial.println("timer");
  return true;
}

void setup()
{
  Serial.begin(9600);

  pinMode(PIN_LED, OUTPUT);

  pinMode(PIN_ROTARY_LEFT, INPUT_PULLUP);
  pinMode(PIN_ROTARY_RIGHT, INPUT_PULLUP);
  rotary.begin();
  rotary.setup([]()
               { rotary.readEncoder_ISR(); });
  rotary.areEncoderPinsPulldownforEsp32 = false;
  rotary.setAcceleration(0);

  bleMouse.begin();

  rgbLed.off();

  timer = timerBegin(2, 80, true);
  timerAttachInterrupt(timer, &timerISR, true);
  timerAlarmWrite(timer, 300000, true);
  timerAlarmEnable(timer);

  scrollToggler.begin(PIN_KEY);
  scrollToggler.setClickHandler([](Button2 &btn)
                                { toggleScroll(true); });
  scrollToggler.setDoubleClickHandler([](Button2 &btn)
                                      { toggleScroll(false); });

  focusButton.begin(PIN_KEY2);
  focusButton.setClickHandler([](Button2 &btn)
                              { bleMouse.click(MOUSE_LEFT); });

  timer2.every(1000, &onTimer);
}

void loop()
{
  timer2.tick();

  if (!bleMouse.isConnected())
  {
    rgbLed.fadeOut(RGBLed::RED, 5, 100);
    return;
  }

  scrollToggler.loop();
  focusButton.loop();

  int res = rotary.encoderChanged();
  if (res != 0)
  {
    bleMouse.move(0, 0, res);
  }
}
