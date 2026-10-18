#define ScreenTransition_SetPhase TextSequence_SetState
#include "global.h"
struct ScreenTransition {
    u8 unknown[0x318];
    void *effect;
    u8 unknown31C[4];
    u16 state;
    s16 timer;
    u8 phase;
};
struct ScreenTransitionStep {
    s16 delay;
    u16 hold, duration;
};
void BestiaryDisplay_Hide(void *);
void TextSequence_SetState(struct ScreenTransition *, u8);
void ScreenTransition_WaitForCompletion(struct ScreenTransition *p,
                                        struct ScreenTransitionStep *step)
{
    BestiaryDisplay_Hide(p->effect);
    if (++p->timer > step->duration + step->hold + step->delay) {
        TextSequence_SetState(p, 0);
        ++p->state;
    }
}
