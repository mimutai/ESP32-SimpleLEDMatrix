#include <Arduino.h>

#include <FreeRTOS.h>

#include <ExButton.h>
#include "LEDMatrixController.hpp"
#include "CONFIG.hpp"

ExButton button(BUTTON_PIN);

void setup()
{
  Serial.begin(SERIAL_BAUD);
  vTaskDelay(10);

  LEDMatrixController::setup();
  LEDMatrixController::setBrightness(LED_BRIGHTNESS);

  LEDMatrixController::start();
}

void loop()
{
  button.update(); // ボタンの状態を更新

  // 長押ししてLEDを消灯する
  if (button.wasLongPressed())
  {
    LEDMatrixController::stop();
  }
}