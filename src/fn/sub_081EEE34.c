#include "global.h"
#define SceneMover_InterpolateScreenX sub_081EEE34
struct ScreenTween {
    u8 pad0[0xAF];
    u8 finished;
    u8 padB0;
    u8 initialize;
    u8 padB2[0x278-0xB2];
    s16 screenX;
    u8 pad27A[0x294-0x27A];
    s32 targetX;
    u8 pad298[0x5D2-0x298];
    s16 duration;
    u8 pad5D4[2];
    s16 startX;
};
s32 Div(s32, s32);
void sub_081EEE34(struct ScreenTween *p, s32 frame)
{
    struct ScreenTween *tween = p;
    s32 x;
    if (p->initialize) {
        p->initialize = 0;
        p->startX = p->screenX;
    }
    if (frame <= p->duration) {
        p->screenX = (u16)p->startX + Div((p->targetX - p->startX) * frame, p->duration);
    } else p->screenX = p->targetX;
    if (frame == tween->duration) p->finished = 1;
}
