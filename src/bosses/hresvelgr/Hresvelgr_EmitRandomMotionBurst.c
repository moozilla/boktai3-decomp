#include "bosses/hresvelgr/hresvelgr_travel.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"

void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);
void *HresvelgrMotion_CreateWithDelta(u32, struct ContactPosition *, u32, u32);
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
        struct ContactPosition delta = {0};
        union RandomTableView table = {gRandomTable};
        delta.x = (nextRandom(table.samples) & 15) - 8;
        delta.y = nextRandom(table.samples) & 7;
        delta.z = (nextRandom(table.samples) & 15) - 8;
        HresvelgrMotion_CreateWithDelta(emitter, &delta, nextRandom(table.samples) % 4, 60);
    }
}
void Hresvelgr_EmitRandomMotionBurst(struct HresvelgrTravelActor *p)
{
    if (p->changed) {
        p->changed = 0;
        Hresvelgr_SetAnimation(p, &p->animation, &p->animationData, 15, 1, 4);
        p->reactionStage = 8;
        emitBurst((u32)&p->animation.position);
        Sound_PlaySong(0x4d1);
    }
    if (!(u8)Callback_InvokeActor(p, p->callback) && p->animationCompletions > 9) {
        u32 mask = 0x10000000;
        p->flags |= mask;
        p->finished = 1;
    }
}
