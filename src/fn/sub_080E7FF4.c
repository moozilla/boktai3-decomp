#include "global.h"
#include "scene_actor_reaction.h"

extern u32 gUnk_02000560[];

static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline s32 PlayerSlot(u8 *actor)
{
    if (actor[8] <= 3)
        return actor[8];
    return -1;
}

static inline void ClearFlags16(u16 *p, u32 mask)
{
    *p &= mask;
}

static inline u8 TryMove(struct SceneActorRegistryResetView *actor, u8 *group, u8 **slot)
{
    void *route = *(void **)(group + 0x1FC);
    struct ContactPosition *position = (void *)((u8 *)actor + 0x5C);
    struct ContactPosition *destination = (void *)(*slot + 0xC);
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        ClearFlags16(&actor->flags2D4, ~0x200);
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear((u8 *)actor + 0x64, 8);
            return 1;
        } else {
            ClearFlags16(&actor->flags2D4, ~0x200);
            return 0;
        }
    }
}

u8 sub_080E7FF4(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)actor + 0x3D4;
    u8 **slot;
    if (Test(gEngineFlags, 0x4800)) {
        u8 *partner;
        s32 index;
        u8 **p = (u8 **)((u8 *)actor + 0xDC);
        partner = *p;
        slot = p;
        if (!partner)
            goto fail;
        index = PlayerSlot(partner);
        if (index < 0)
            goto fail;
        {
            u32 mask = 8;
            if (!(gUnk_02000560[index] & mask))
                goto join;
            goto fail;
        }
    } else {
        u8 *partner;
        u8 **p = (u8 **)((u8 *)actor + 0xDC);
        partner = *p;
        slot = p;
        if (!partner)
            goto fail;
    }
join:
    if (TryMove(actor, group, slot))
        return 1;
fail:
    return 0;
}
