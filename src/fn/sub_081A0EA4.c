#include "global.h"
#include "engine_globals.h"
#include "ram.h"

#define gMotionRandomIndex gRandomIndex
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
void Hresvelgr_SetAnimation(HresvelgrActor *, void *, void *, u32, u32, u32);
void *MotionEntry_CreateWithDelta(u32, MotionDelta *, u32, u32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
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
        union RandomTableView table = {gRandomTable};
        delta.x = (nextRandom(table.samples) & 15) - 8;
        delta.y = nextRandom(table.samples) & 7;
        delta.z = (nextRandom(table.samples) & 15) - 8;
        MotionEntry_CreateWithDelta(emitter, &delta, nextRandom(table.samples) % 4, 60);
    }
}
void Hresvelgr_EmitRandomMotionBurst(HresvelgrActor *p)
{
    if (p->changed) {
        p->changed = 0;
        Hresvelgr_SetAnimation(p, p->animation, p->animationData, 15, 1, 4);
        p->state = 8;
        emitBurst((u32)p->emitter);
        Sound_PlaySong(0x4d1);
    }
    if (!(u8)Callback_InvokeActor(p, p->callback) && p->phase > 9) {
        u32 mask = 0x10000000;
        p->flags |= mask;
        p->finished = 1;
    }
}
