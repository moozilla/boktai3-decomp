/* Descriptive aliases and object roles are tentative (confidence: low). */
#include "global.h"
#include "player.h"
struct PackedItemPosition {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct ItemLaunchPlayer {
    u8 unknown0[0x30];
    struct PackedItemPosition position;
};
struct SceneActorItemLaunch {
    u8 unknown0[0x5c];
    struct PackedItemPosition position;
    u8 unknown64[0x67c - 0x64];
    s16 slot, kind, parameter, field682;
    u8 unknown684[0x6d4 - 0x684];
    u16 flags;
};
u8 SceneInventory_FindSlotByValue(s16 *, s16 *, s16 *, s32);
s32 FloatingItemEffect_Spawn(s32, s32, s32, s32, void *, void *, s32, void *);
void Inventory_ClearItemSlot(u32);
void Sound_PlaySong(u32);
static inline u8 busy(struct SceneActorItemLaunch *p)
{
    u32 mask = 1;
    if (p->flags & mask)
        return 1;
    return 0;
}
static inline u8 occupied(struct SceneActorItemLaunch *p)
{
    if (busy(p) || p->slot >= 0)
        return 1;
    return 0;
}
s32 SceneActor_TryLaunchItem13(struct SceneActorItemLaunch *p)
{
    struct SceneActorItemLaunch *q = p;
    struct PackedItemPosition pos = ((struct ItemLaunchPlayer **)gPlayers)[0]->position;
    pos.y += 200;
    if (occupied(p))
        return -1;
    if (!SceneInventory_FindSlotByValue(&p->slot, &p->kind, &p->parameter, 13))
        return -1;
    if (FloatingItemEffect_Spawn(0, p->kind, 3, 0, &pos, &q->position, 24, p) < 0) {
        p->slot = 0xffff;
        return -1;
    }
    Inventory_ClearItemSlot(p->slot);
    switch (p->kind) {
        case 2:
        case 5:
        case 14:
            return 0;
    }
    Sound_PlaySong(0x1ce);
    return 1;
}
