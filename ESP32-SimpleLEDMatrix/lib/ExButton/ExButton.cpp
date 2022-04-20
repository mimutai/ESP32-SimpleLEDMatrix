#include <Arduino.h>
#include "ExButton.h"

//ボタンの初期設定
ExButton::ExButton(int pin)
{
    _button_pin = pin;
    pinMode(_button_pin, INPUT_PULLUP);

    _prev_state = false;
    _was_returned = false;
    _press_state = 0;
}

void ExButton::update()
{
    //ボタンのピンが設定されていないときは終了
    if (_button_pin == 255)
        return;

    bool cur_state = getButton();

    if (_was_returned)
    {
        // 前回の処理で判定が確定した時
        _press_state = 0; // 待機
    }

    if (cur_state) // ボタンが押されている
    {
        if (!_prev_state) // ボタンが押されたその瞬間
        {
            _hold_stime = millis(); // ボタン押し始めの時間を記録
            _was_returned = false;  // 返していない判定とする
        }
        else if (!_was_returned) // 判定が確定していない時
        {
            unsigned long hold_time = abs(millis() - _hold_stime); // 継続押下時間を計算

            if (hold_time >= THRESHOLD_HOLDTIME) // 一定時間経過したか
            {
                _press_state = 3;     // 長押しが確定した
                _was_returned = true; // 押した判定を返した
            }
            else
            {
                _press_state = 2; // 長押しのために継続押下中
            }
        }
    }
    else if (_prev_state && !_was_returned) // この瞬間ボタンがリリースされた
    {
        unsigned long hold_time = abs(millis() - _hold_stime); // 継続押下時間を計算

        if (hold_time >= THRESHOLD_PRESSTIME && hold_time < THRESHOLD_HOLDTIME) // 一定時間経過したかつ長押しではない
        {
            _press_state = 1;     // 押下
            _was_returned = true; // 押した判定を返した
        }
    }

    _prev_state = cur_state; // 前の状態として保存
    return;
}

//現在ボタンが押されているかどうか
bool ExButton::getButton()
{
    return digitalRead(_button_pin) == LOW;
}
