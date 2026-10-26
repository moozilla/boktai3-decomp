#include "global.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"
#define gTileTriggerStateCallbacks gUnk_08606510
struct TileTrigger {
    u32 flags;
    u8 unknown4[0x70];
    u8 animation[0x10];
    struct SceneVec position;
    u32 key;
    u8 unknown90[0x28];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
};
struct TileTriggerManager {
    u8 unknown0[0x18];
    void *animationData;
    u8 unknown1C[0x1C];
    struct TileTrigger *tiles[12];
    u32 used;
};
extern u32 gUnk_02000560;
extern void *gUnk_08606510[];
s32 Callback_InvokeActor(void *, void *);
static inline u8 Used(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}
static inline u32 TestPair(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
s32 TileTrigger_TickManager(struct TileTriggerManager *manager)
{
    struct TileTrigger **tiles = manager->tiles;
    s32 i;
    if (TestPair(0x400))
        return 0;
    {
        u32 mask = 4;
        if (gUnk_02000560 & mask)
            return 0;
    }
    if (!manager->used)
        return 0;
    for (i = 0; i < 12; i++) {
        if (Used(&manager->used, i)) {
            struct TileTrigger *tile = tiles[i];
            if (!(tile->triggerFlags & 0x200))
                Callback_InvokeActor(tile, gUnk_08606510[tile->state]);
        }
    }
    return 0;
}
