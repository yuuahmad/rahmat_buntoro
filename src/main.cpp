#include <Arduino.h>
#include <Wire.h>
#include <AiEsp32RotaryEncoder.h>
#include <BleKeyboard.h>

#if defined(ESP8266)
#define ROTARY_ENCODER_A_PIN D6
#define ROTARY_ENCODER_B_PIN D5
#define ROTARY_ENCODER_BUTTON_PIN D7
#else
#define ROTARY_ENCODER_A_PIN 32      //ini pin clk
#define ROTARY_ENCODER_B_PIN 21      //ini cs
#define ROTARY_ENCODER_BUTTON_PIN 25 //ini button
#endif
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define ROTARY_ENCODER_STEPS 2

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);
BleKeyboard bleKeyboard("yuuahmad_keys", "Ytech", 80);
int pos = 0;

void rotary_onButtonClick()
{
  static unsigned long lastTimePressed = 0;
  if (millis() - lastTimePressed < 500) //ignore multiple press in that time milliseconds
  {
    return;
  }
  lastTimePressed = millis();
  Serial.print("button pressed ");
  Serial.print(millis());
  Serial.println(" milliseconds after restart");
}

void rotary_loop()
{
  if (rotaryEncoder.encoderChanged())
  {
    Serial.print("Value: ");
    Serial.println(rotaryEncoder.readEncoder());
  }
  if (rotaryEncoder.isEncoderButtonClicked())
  {
    bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
    rotary_onButtonClick();
  }
}

void IRAM_ATTR readEncoderISR()
{
  rotaryEncoder.readEncoder_ISR();
}

void setup()
{
  Serial.begin(115200);
  bleKeyboard.begin();
  rotaryEncoder.begin();
  rotaryEncoder.setup(readEncoderISR);
  bool circleValues = true;
  rotaryEncoder.setBoundaries(0, 100, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotaryEncoder.setAcceleration(0);                  //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
}

void loop()
{
  rotary_loop();
  int newPos = rotaryEncoder.readEncoder();
  if (newPos > pos)
  {
    bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
    pos = newPos;
  }
  else if (newPos < pos)
  {
    bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
    pos = newPos;
  }
  delay(50); //or do whatever you need to do...
}