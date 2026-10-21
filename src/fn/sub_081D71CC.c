// MATCHING_CONCESSION: register-pinning
#include "global.h"

#define ScreenTransition_SetPhase TextSequence_SetState
#define gScreenBlendControl       gUnk_030051DC
#define gScreenBlendAmount        gUnk_030051D4
struct ScreenTransition {
    u8 unknown[0x320];
    u16 state;
    s16 timer;
    u8 phase;
};
struct ScreenTransitionStep {
    u16 unknown[2];
    u16 duration;
};
extern u16 gUnk_030051DC;
extern volatile u32 gUnk_030051D4;
void TextSequence_SetState(struct ScreenTransition *, u8);
s32 Div(s32, s32);
void ScreenTransition_FadeBlendOut(struct ScreenTransition *p, struct ScreenTransitionStep *step)
{
    s16 *timer = &p->timer;
    if (*timer == 0)
        gUnk_030051DC = 0x1084;
    if (++*timer >= step->duration) {
        TextSequence_SetState(p, 0);
        ++p->state;
        gUnk_030051D4 = ({
            u32 z = 0;
            asm("" : "+r"(z));
            z;
        });
    } else {
        gUnk_030051D4 = 64 - Div(*timer << 6, step->duration);
    }
}
