#include "global.h"
struct ScreenTransitionStep {
    s16 delay;
    u16 hold, duration;
    u8 unknown[6];
};
struct ScreenTransition {
    u8 task[0x18];
    struct ScreenTransitionStep steps[64];
    void *effect;
    s32 lastStep;
    s16 state, timer;
    u8 phase;
};
struct ScreenTransitionCallbacks {
    void *phase[6];
};
extern const struct ScreenTransitionCallbacks gUnk_0824FCA4;
s32 sub_08249240(void *, void *, void *);
s32 ScreenTransition_DispatchPhase(struct ScreenTransition *p)
{
    struct ScreenTransitionCallbacks callbacks = gUnk_0824FCA4;
    if (p->state <= p->lastStep) {
        struct ScreenTransitionStep *step = &p->steps[p->state];
        void *callback = callbacks.phase[p->phase];
        if (callback)
            sub_08249240(p, step, callback);
    }
    return 0;
}
