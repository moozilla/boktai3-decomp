#include "global.h"
#include "engine.h"
#include "ram.h"
struct StateSprite {
    u8 unk0[0x13A];
    s16 state;
    u8 unk13C[8];
};
struct StateSpritePool {
    u8 unk0[0x3C];
    struct StateSprite slots[4];
    u32 occupiedMask;
};
extern u32 gUnk_02000560;
extern void *gUnk_08610704[];
s32 Callback_InvokeActor(void *, void *);
static inline u8 Occupied(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}
static inline u32 Paused(void)
{
    u32 mask = 0x400;
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 Locked(void)
{
    u32 mask = 4;
    return gUnk_02000560 & mask;
}
s32 StateSpritePool_Tick(struct StateSpritePool *pool)
{
    struct StateSprite *start = pool->slots;
    struct StateSprite *slot;
    u32 state;
    s32 i;
    u32 mask;
    void **table;
    if (pool->occupiedMask) {
        if (!Paused()) {
            if (!Locked()) {
                i = 0;
                mask = (u32)start + 0x510;
                table = gUnk_08610704;
                state = (u32)&pool->slots[0].state;
                slot = start;
                for (; i < 4;) {
                    if (Occupied((u32 *)mask, i))
                        Callback_InvokeActor(slot, table[*(s16 *)state]);
                    {
                        u32 stride = 0x144;
                        state += stride;
                        slot = (void *)((u32)slot + stride);
                    }
                    i++;
                }
            }
        }
    }
    return 0;
}
