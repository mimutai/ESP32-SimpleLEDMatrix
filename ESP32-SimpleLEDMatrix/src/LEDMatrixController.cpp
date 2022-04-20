#include <Arduino.h>

#include <FreeRTOS.h>
#include <FastLED.h>

#include "LEDMatrixController.hpp"
#include "CONFIG.hpp"

CRGB *LEDMatrixController::_leddata;
bool LEDMatrixController::_do_cancel = false;

///
/// LEDの点灯準備
///
void LEDMatrixController::setup()
{
    _leddata = (CRGB *)malloc(sizeof(CRGB) * NUM_OF_LEDS);

    FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(_leddata, NUM_OF_LEDS);

    clearLED();
}

///
/// すべてのLEDを消灯
///
void LEDMatrixController::clearLED()
{
    for (int idx_led = 0; idx_led < NUM_OF_LEDS; idx_led++)
    {
        LEDMatrixController::_leddata[idx_led].r = 0;
        LEDMatrixController::_leddata[idx_led].g = 0;
        LEDMatrixController::_leddata[idx_led].b = 0;
    }
    FastLED.show();
    vTaskDelay(10);
}

void LEDMatrixController::process(void *pvParameters)
{
    ///
    /// LEDデータ文字列からLEDデータのオブジェクトに変換する
    ///
    CRGB *leddata_img0 = dataStr2Object(data_img0);
    CRGB *leddata_hue_gradation = dataStr2Object(HUE_GRADATION);

    const int UPDATE_INTERVAL = 50; // 更新間隔[ms]
    int hue_base = 0;
    int update_counter = 0;

    _do_cancel = false;

    while (!_do_cancel)
    {
        for (int idx_led = 0; idx_led < NUM_OF_LEDS; idx_led++)
        {
            // 黒以外
            if (leddata_img0[idx_led].r > 0 && leddata_img0[idx_led].g > 0 && leddata_img0[idx_led].b > 0)
            {
                ///
                /// テキスト表示処理
                ///
                if (update_counter < 32)
                {
                    // 文字用のテクスチャのピクセルを設定
                    _leddata[idx_led] = leddata_img0[idx_led];
                }
                else
                {
                    // 注意を引くために一定時間黒くする
                    _leddata[idx_led] = CRGB::Black;
                }
                continue;
            }

            ///
            /// グラデーション処理
            ///

            // LEDのマトリクス上での座標を計算
            int led_x = idx_led / LED_MATRIX_HEIGHT;
            int led_y = idx_led % LED_MATRIX_HEIGHT;
            if (led_x % 2 == 1)
                led_y = LED_MATRIX_HEIGHT - 1 - led_y;

            // LEDのHueのindexを計算
            int target_hue = (hue_base + led_x * 3 + led_y * 4) % HUE_GRADATION_RES;

            // このピクセルに色をセット
            _leddata[idx_led] = leddata_hue_gradation[target_hue];
        }

        // Matrixに表示するHUEのベース値を更新する
        if (++hue_base >= HUE_GRADATION_RES)
            hue_base = 0;

        // カウンターを更新
        if (++update_counter >= 40)
            update_counter = 0;

        // LEDを更新して光らせる
        FastLED.show();
        vTaskDelay(UPDATE_INTERVAL);
    }

    LEDMatrixController::clearLED();
    vTaskDelay(10);

    vTaskDelete(NULL);
}

CRGB *LEDMatrixController::dataStr2Object(const char *data_str)
{
    CRGB *data = (CRGB *)malloc(sizeof(CRGB) * NUM_OF_LEDS);

    for (int led_idx = 0; led_idx < NUM_OF_LEDS; led_idx++)
    {
        int base_idx = led_idx * 6;
        data[led_idx].r = (hex2dec(data_str[base_idx + 0]) << 4) + hex2dec(data_str[base_idx + 1]);
        data[led_idx].g = (hex2dec(data_str[base_idx + 2]) << 4) + hex2dec(data_str[base_idx + 3]);
        data[led_idx].b = (hex2dec(data_str[base_idx + 4]) << 4) + hex2dec(data_str[base_idx + 5]);
    }

    return data;
}