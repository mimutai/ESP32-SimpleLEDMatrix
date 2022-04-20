#pragma once

#include <FreeRTOS.h>
#include <array>

#include <FastLED.h>

#include "CONFIG.hpp"

class LEDMatrixController
{
public:
    static void setup();

    ///
    /// 明るさをセットする
    ///
    inline static void setBrightness(uint8_t scale)
    {
        FastLED.setBrightness(scale);
    }

    static void clearLED();

    ///
    /// プロセスを開始する
    ///
    inline static void start()
    {
        xTaskCreatePinnedToCore(process, "process", 32768, NULL, 1, NULL, 1);
    }

    static bool _do_cancel;

    ///
    /// プロセスを終了させる
    ///
    inline static void stop()
    {
        _do_cancel = true;
        vTaskDelay(200); // プロセス終了まで待機
    }

    //
    static CRGB *_leddata;

    ///
    /// LEDの色データを格納した文字列(16進数2桁)をFastLEDのCRGBオブジェクトに変換する
    ///
    static CRGB *dataStr2Object(const char *data_str);

    ///
    /// 16進文字を10進数値に変換する
    ///
    inline static uint8_t hex2dec(const char c)
    {
        // 0-9の数値
        if (c >= 48 && c <= 57)
            return c - 48;

        // a-f
        else if (c >= 97 && c <= 102)
            return (c - 97) + 10;

        // 無効
        else
            return 0;
    }

private:
    static void process(void *pvParameters);
};