#include "global.h"
#include "player.h"

struct ControlPanelGauge {
    u8 unknown0[0x30];
    s16 frame;
    u8 unknown32[0x68 - 0x32];
    u16 phase;
    u8 unknown6A[0x57C - 0x6A];
    u16 soundTicks;
};
s32 Div(s32, s32);
void Sound_PlaySong(u32);

void ControlPanel_UpdateGaugeIndicator(struct ControlPanelGauge *gauge)
{
    s32 increment;
    if (gPlayers[0]->indicatorUpdateRate) {
        increment = Div(128, gPlayers[0]->indicatorUpdateRate);
        if (increment <= 0)
            increment = 1;
    } else {
        increment = 1;
    }
    gauge->phase += increment;
    if (gauge->phase > 11)
        gauge->phase -= 12;
    gauge->frame = gauge->phase >> 2;
    if (!gauge->soundTicks) {
        u16 taps = gPlayers[0]->displaySegmentCount;
        if (taps <= 1)
            Sound_PlaySong(0x23A);
        else if (taps <= 3)
            Sound_PlaySong(0x23B);
        else if (taps <= 5)
            Sound_PlaySong(0x23C);
        else
            Sound_PlaySong(0x23D);
    }
    gauge->soundTicks++;
    if (gauge->soundTicks > 9)
        gauge->soundTicks = 0;
}
