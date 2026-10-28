#include "global.h"

// MATCHING_CONCESSION: register-pinning
#define gMotionRandomIndex gRandomIndex
#define gMotionRandomTable gUnk_0203B400
typedef struct MotionDelta {
    s32 x : 16, y : 16, z : 16, pad : 16;
} MotionDelta;
typedef struct HresvelgrActor {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[3];
    u8 phase;
    u8 padA4[11];
    u8 finished;
    u8 padB0;
    u8 changed;
    u8 padB2[0x52];
    void *callback;
    u8 pad108[0x6c];
    u8 animationData[0x20];
    u8 animation[0x20];
    u8 emitter[0x15c];
    u16 state;
} HresvelgrActor;
extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];
void Hresvelgr_SetAnimation(HresvelgrActor *, void *, void *, u32, u32, u32);
void *MotionEntry_CreateWithDelta(u32, MotionDelta *, u32, u32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void sub_0819C8F8(void *, s32, s32);
static inline s32 nextRandom(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)table);
}
static inline void emitBurst(u32 emitter)
{
    int i;

    for (i = 0; i < 4; ++i) {
        MotionDelta delta = {0};
        register const u16 *table asm("r4") = gUnk_0203B400;
        delta.x = (nextRandom(table) & 15) - 8;
        delta.y = nextRandom(table) & 7;
        delta.z = (nextRandom(table) & 15) - 8;
        MotionEntry_CreateWithDelta(emitter, &delta, nextRandom(table) % 4, 60);
    }
}
// confidence=low: oscillates side-relative velocity during a four-entry burst animation.
#define Hresvelgr_UpdateFlaggedMotionBurst sub_081A1D18
void Hresvelgr_UpdateFlaggedMotionBurst(HresvelgrActor *p, s32 tick)
{
    HresvelgrActor *saved = p;
    {
        u32 mask = 12;
        p->flags |= mask;
    }
    if (p->changed) {
        p->changed = 0;
        Hresvelgr_SetAnimation(p, p->animation, p->animationData, 15, 1, 4);
        p->state = 8;
        emitBurst((u32)p->emitter);
        Sound_PlaySong(0x4d1);
    }
    sub_0819C8F8(saved, 24, tick << 2);
    if (p->phase > 3)
        p->finished = 1;
}
