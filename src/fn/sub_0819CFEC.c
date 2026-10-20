#include "global.h"

// confidence=low: prioritizes defeat, recovery, and interrupted charge reactions.
#define Hresvelgr_TryStartReaction sub_0819CFEC
struct Sprite {
    u8 pad[9];
    u8 direction;
};
struct Hresvelgr {
    u8 pad0[0x48];
    struct Sprite sprite;
    u8 pad54[0x3c];
    u32 timer;
    s16 health;
    u8 pad96[4];
    u8 enabled;
    u8 pad9B;
    u32 flags;
    u8 padA0[2];
    u8 mode;
    u8 padA3;
    u32 modeTime;
    u8 padA8[3];
    u8 animation, state, nextState, changed, padAF, active, reset;
    u8 padB2[0x12];
    u32 ticks;
    u8 padC8[0x38];
    void (*callback)(void);
    u8 pad104[8];
    u8 *player;
    u8 pad110[0x274];
    void (*savedCallback)(void);
    u8 savedState, savedAnimation;
};
static inline void setState(struct Hresvelgr *p, u32 state, u32 next, u32 changed)
{
    p->state = state;
    p->nextState = next;
    p->changed = changed;
    p->ticks = 0;
    p->active = 1;
}
static inline void setCallback(struct Hresvelgr *p, void (*callback)(void), u32 animation)
{
    p->animation = animation;
    p->callback = callback;
    p->timer = 0;
    p->reset = 1;
    p->mode = 0;
    p->modeTime = 0;
}
void sub_081A14FC(void);
void sub_081A0FF0(void);
void Hresvelgr_UpdateTimedPose(void);
void Hresvelgr_UpdateMotionPhase(void);
static inline void Request(struct Hresvelgr *p, u32 kind)
{
    p->padA8[2] = kind;
}
static inline u32 TestFlags(struct Hresvelgr *p, u32 mask)
{
    return p->flags & mask;
}
s32 Hresvelgr_TryStartReaction(struct Hresvelgr *p)
{
    struct Hresvelgr *q = p;
    if (p->health <= 0 && p->enabled) {
        u32 mask = 2;
        p->flags |= mask;
        Request(p, 38);
    }
    p->enabled = 0;
    if (TestFlags(p, 2)) {
        setState(p, 1, 0, 5);
        setCallback(p, sub_081A14FC, 38);
        return 1;
    }
    if (p->flags & 1) {
        q->savedState = p->changed;
        setState(p, 1, 0, 0);
        setCallback(p, sub_081A0FF0, 34);
        return 1;
    }
    if (p->flags & 0x10000000) {
        q->savedState = p->changed;
        setState(p, 1, 0, 1);
        setCallback(p, Hresvelgr_UpdateTimedPose, 35);
        return 1;
    }
    if (p->flags & 0x100000) {
        q->savedState = p->changed;
        q->savedCallback = p->callback;
        q->savedAnimation = p->animation;
        setState(p, 1, 0, 2);
        setCallback(p, Hresvelgr_UpdateMotionPhase, 32);
        return 1;
    }
    return 0;
}
