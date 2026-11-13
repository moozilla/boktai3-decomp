#ifndef GUARD_BLEND_TRANSITION_H
#define GUARD_BLEND_TRANSITION_H
#include "engine.h"

/* Alpha blend uses both coefficients; brightness modes use the first. */
struct BlendTransition {
    struct Task task;
    u8 mode;
    s8 initial[2], delta[2];
    u8 unknown1D[3];
    u16 source, destination;
    u32 time, duration;
};
s32 BlendTransition_Update(struct BlendTransition *);
u32 BlendTransition_Destroy(void);
s32 BlendTransition_InitializeFromScript(struct BlendTransition *);
struct BlendTransition *BlendTransition_Create(void);
void CircleWindow_InitManager(void);
s32 Div(s32, s32);
void Video_SetBlendControl(u32, u32, u32);
void Video_SetBlendAlpha(s32, s32);
void Video_SetBlendBrightness(s32);
#endif
