#include <Arduino.h>
#include <FreeRTOS.h>

#include "IndicatorLed.h"

// staticメンバ変数の初期化
bool IndicatorLED::_do_cancel = false;
uint8_t IndicatorLED::_num_of_colors = 1;
std::array<uint8_t, MAX_INDICATOR_LEDS> IndicatorLED::_led_pins = {255, 255};
std::array<uint8_t, MAX_INDICATOR_LEDS> IndicatorLED::_emit_color = {HIGH, HIGH};
unsigned long IndicatorLED::_led_blink_interval = DEFAULT_BLINK_INTERVAL;
int IndicatorLED::_led_blink_count = DEFAULT_BLINK_COUNT;

void IndicatorLED::setLEDPin(uint8_t ledPin)
{
    _led_pins[0] = ledPin;
    _led_pins[1] = 255;
    pinMode(_led_pins[0], OUTPUT);

    _emit_color[0] = HIGH; // 点灯色の変更

    _num_of_colors = 1; // 単色として設定
}

void IndicatorLED::setLEDPin2C(uint8_t ledPin0, uint8_t ledPin1)
{
    _led_pins[0] = ledPin0;
    _led_pins[1] = ledPin1;
    pinMode(_led_pins[0], OUTPUT);
    pinMode(_led_pins[1], OUTPUT);

    _num_of_colors = 2; // 2色として設定
}

void IndicatorLED::setEmitColor2C(uint8_t led0_val, uint8_t led1_val)
{
    _emit_color[0] = led0_val;
    _emit_color[1] = led1_val;
}

//タスクを開始する
void IndicatorLED::runBlinkTask(int blink_interval)
{
    _led_blink_interval = blink_interval;
    xTaskCreatePinnedToCore(IndicatorLED::blinkLEDTask, "blinkLEDTask", 1024, NULL, 1, NULL, 1);
}

//タスクを開始する
void IndicatorLED::runBlinkAnimTask(int blink_count)
{
    _led_blink_count = blink_count;
    xTaskCreatePinnedToCore(IndicatorLED::blinkAnimLEDTask, "blinkAnimLEDTask", 1024, NULL, 1, NULL, 1);
}

// LEDのタスクをキャンセルする
void IndicatorLED::cancelTask()
{
    _do_cancel = true;
}

// LEDを点灯させる
void IndicatorLED::turnOnLED()
{
    for (uint8_t idx = 0; idx < _num_of_colors; idx++)
    {
        if (_led_pins[idx] != 255)
        {
            digitalWrite(_led_pins[idx], _emit_color[idx]);
        }
    }
}

// LEDを点灯させる
void IndicatorLED::turnOffLED()
{
    // LED_PINが設定されていない時は終了
    for (uint8_t idx = 0; idx < _num_of_colors; idx++)
    {
        if (_led_pins[idx] != 255)
        {
            digitalWrite(_led_pins[idx], LOW);
        }
    }
}

//設定モードであることを示す LEDを点滅させるタスク
void IndicatorLED::blinkLEDTask(void *pvParameters)
{
    // LED_PINが設定されていない時は終了
    if (!isValidSettings())
    {
        vTaskDelete(NULL); //タスクの消去
        return;
    }

    _do_cancel = false;
    unsigned long stime = millis();

    // 点灯と消灯のループ
    while (!_do_cancel)
    {
        turnOnLED(); // LED点灯

        stime = millis();
        //指定時間待機
        while (abs(millis() - stime) < _led_blink_interval)
        {
            vTaskDelay(1);
            if (_do_cancel) //終了フラグが出ていたら終了
                break;
        }

        turnOffLED(); // LED消灯

        stime = millis();
        while (abs(millis() - stime) < _led_blink_interval)
        {
            vTaskDelay(1);
            if (_do_cancel) //終了フラグが出ていたら終了
                break;
        }
    }

    turnOffLED();      //終了時のLEDは消灯
    vTaskDelete(NULL); //タスクの消去
}

// LEDを点滅させるタスク
void IndicatorLED::blinkAnimLEDTask(void *pvParameters)
{
    // LED_PINが設定されていない時は終了
    if (!isValidSettings())
    {
        vTaskDelete(NULL); //タスクの消去
        return;
    }

    const unsigned long blink_interval_local = 100; //点滅間隔
    const unsigned long blink_wait_time = 1000;     //再点滅までの時間

    _do_cancel = false;
    unsigned long stime = millis();

    // 点灯と消灯のループ
    while (!_do_cancel)
    {
        for (int bc = 0; bc < _led_blink_count; bc++)
        {
            turnOnLED(); // LED点灯

            //指定時間待機
            stime = millis(); //このLED状態に設定した時間
            while (abs(millis() - stime) < blink_interval_local)
            {
                vTaskDelay(1);
                if (_do_cancel) //終了フラグが出ていたら終了
                    break;
            }

            turnOffLED(); // LED消灯

            stime = millis(); //このLED状態に設定した時間
            while (abs(millis() - stime) < blink_interval_local)
            {
                vTaskDelay(1);
                if (_do_cancel) //終了フラグが出ていたら終了
                    break;
            }
        }

        //次の点滅までの待機時間
        stime = millis(); //このLED状態に設定した時間
        while (abs(millis() - stime) < blink_wait_time)
        {
            vTaskDelay(1);
            if (_do_cancel) //終了フラグが出ていたら終了
                break;
        }
    }

    turnOffLED();      //終了時のLEDは消灯
    vTaskDelete(NULL); //タスクの消去
}

bool IndicatorLED::isValidSettings()
{
    // LED_PINが設定されていない時は正常ではない
    for (uint8_t idx = 0; idx < _num_of_colors; idx++)
    {
        if (_led_pins[idx] == 255)
            return false;
    }

    return true; // 全ての確認事項をクリアした
}