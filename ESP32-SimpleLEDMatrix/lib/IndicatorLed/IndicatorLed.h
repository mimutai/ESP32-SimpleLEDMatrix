#pragma once

#include <array>

constexpr int DEFAULT_BLINK_INTERVAL = 500;
constexpr int DEFAULT_BLINK_COUNT = 2;
constexpr int MAX_INDICATOR_LEDS = 2;

class IndicatorLED
{
public:
    static void setLEDPin(uint8_t ledPin); // 単色のLEDの設定
    static void setLEDPin2C(uint8_t ledPin0, uint8_t ledPin1); // 2色のLEDの設定
    static void setEmitColor2C(uint8_t led0_val, uint8_t led1_val); // 点灯時のLED発行状態(色)を指定する

    static void runBlinkTask(int blink_interval = DEFAULT_BLINK_INTERVAL);
    static void runBlinkAnimTask(int blink_count = DEFAULT_BLINK_COUNT);
    static void cancelTask(); //インジケータの点滅処理を終了する
    static void turnOnLED(); // LEDを点灯させる
    static void turnOffLED(); // LEDを消灯させる

protected:
    static void blinkLEDTask(void *pvParameters); //インジケータのタスク
    static void blinkAnimLEDTask(void *pvParameters);
    static bool isValidSettings(); // LED設定が正しいかどうか

protected:
    static bool _do_cancel;
    static uint8_t _num_of_colors;                            // インジケータLEDの色の数
    static std::array<uint8_t, MAX_INDICATOR_LEDS> _led_pins; // インジケータLEDの個別のLEDのピン番号
    static std::array<uint8_t, MAX_INDICATOR_LEDS> _emit_color; // ON時の色(各色はDigital)
    static unsigned long _led_blink_interval;
    static int _led_blink_count;
};
