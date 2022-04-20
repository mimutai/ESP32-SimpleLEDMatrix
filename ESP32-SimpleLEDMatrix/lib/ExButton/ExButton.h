#pragma once

//この時間以上継続してボタンが押されていたら押下判定を出す [ms]
constexpr unsigned long THRESHOLD_PRESSTIME = 50;
//この時間以上継続してボタンが押されていたら長押し判定を出す [ms]
constexpr unsigned long THRESHOLD_HOLDTIME = 2000;

class ExButton
{
public:
    ExButton(int pin);
    bool getButton();
    void update(); // ボタンの押下状態を更新する

    // ボタンが押されたか（瞬時的な押下は判定されない, 押下したフレームのみTrueが返る）
    inline bool wasPressed()
    {
        return (_press_state == 1);
    }

    // ボタンが長押しされたか（長押しが確定したフレームのみTrueが返る）
    inline bool wasLongPressed()
    {
        return (_press_state == 3);
    }

protected:
    uint8_t _button_pin = 255;
    unsigned long _hold_stime; // ボタンが押され始めた時間
    bool _prev_state;          // 前に処理した時にボタンが押されていたか
    bool _was_returned;        // ボタンの押下判定を返したかどうか（長押しでも1回のみ判定を返すため）
    uint8_t _press_state;      // 現在の押下状態(0: 待機, 1: 押下したフレーム, 2: 長押し継続中, 3: 長押し確定)
};
