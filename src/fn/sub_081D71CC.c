#include "global.h"
#include "render.h"

#define ScreenTransition_SetPhase TextSequence_SetState
#define gScreenBlendControl       gObjFadeColor
#define gScreenBlendAmount        gObjFadeLevel
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
void TextSequence_SetState(struct ScreenTransition *, u8);
s32 Div(s32, s32);
void ScreenTransition_FadeBlendOut(struct ScreenTransition *p, struct ScreenTransitionStep *step)
{
    s16 *timer = &p->timer;
    if (*timer == 0)
        gObjFadeColor = 0x1084;
    if (++*timer >= step->duration) {
        TextSequence_SetState(p, 0);
        do {
            ++p->state;
        } while (0);
        gObjFadeLevel = 0;
    } else {
        gObjFadeLevel = 64 - Div(*timer << 6, step->duration);
    }
}
