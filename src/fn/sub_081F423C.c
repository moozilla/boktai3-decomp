#include "global.h"

// MATCHING_CONCESSION: register-pinning
// One empty frame-use barrier preserves the ROM register allocation.
struct DirectionalActor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[4];
    u8 active, finished, padB0, changed, padB2, locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x217];
    u8 animation[0x20];
    u8 animationData[0x160];
    u8 paletteValue, pad4AD, paletteMode, paletteRate;
    u32 paletteTimer;
    u8 pad4B4[0x99e];
    u16 motionE52, motionE54, padE56, motionE58;
    u8 padE5A[0xaa];
    s32 completionFrame;
};
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
static inline void facing(struct DirectionalActor *p, int angle)
{
    u32 v;
    int shifted;
    p->angle = angle;
    shifted = (p->angle + 0x60) >> 6;
    v = 3;
    v &= shifted;
    p->facing = v;
    shifted = (p->angle + 0x70) >> 5;
    v = 7;
    v &= shifted;
    p->facing8 = v;
}
void PaletteEffect_RefreshPalette(void *);
void Player_ClearFlag740(void);
static inline void palette(struct DirectionalActor *p, u32 duration, u8 rate, u8 mode, u32 zero)
{
    p->paletteTimer = duration;
    p->paletteMode = mode;
    p->paletteRate = rate;
    if (p->paletteMode == 2) {
        u8 value = 32;
        p->paletteValue = value;
    } else if (p->paletteMode == 1)
        p->paletteValue = zero;
}
void ActorMotion_UpdateTimedPaletteAnimation(struct DirectionalActor *p, s32 frame)
{
    struct DirectionalActor *q = p;
    u32 zero;
    facing(p, p->requestedAngle);
    zero = 0;
    if (p->changed) {
        p->changed = zero;
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 8, 1, 3, 4);
        palette(p, 500, 5, 4, zero);
        q->motionE52 = 0;
        q->motionE54 = 0;
        q->motionE58 = 0;
    }
    // Keep the frame value in its own register through the palette setup.
    asm("" : : "r"(frame));
    if (frame > 299) {
        p->finished = 1;
        PaletteEffect_RefreshPalette(q);
    }
    if (p->active == 4) {
        u32 v;
        Player_ClearFlag740();
        {
            u32 mask = 8;
            v = mask | p->flags;
        }
        v |= 16;
        p->flags = v;
    }
    {
        u32 mask = 0x100000;
        p->flags = mask | p->flags;
        if (p->active != 4)
            p->flags |= 64;
    }
    {
        u32 mask = 0x1000000;
        p->flags = mask | p->flags;
    }
    Callback_InvokeActor(p, p->callback);
}
