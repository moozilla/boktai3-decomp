#ifndef GUARD_SCENE_ACTOR_DAMAGE_VISIBILITY_H
#define GUARD_SCENE_ACTOR_DAMAGE_VISIBILITY_H

#include "scene_actor_reset.h"
#include "area_effect_link.h"

/* Link-session ownership controls whether this actor reports damage locally. */
static inline u8 SceneActorDamage_TestFlags(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline s32 SceneActorDamage_LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}

/* The query returns halfword flags; participant comparisons use the full word. */
static inline u16 SceneActorDamage_OwnershipBits(u32 value, u32 mask)
{
    return value & mask;
}

static inline u8 SceneActorDamage_ShouldShow(struct SceneActorRegistryResetView *actor)
{
    if (SceneActorDamage_TestFlags(gEngineFlags, 0x4800)) {
        u32 *owner = &actor->field2E0;
        u32 mask = *owner;
        if (!SceneActorDamage_OwnershipBits(mask, 0x20) && mask) {
            switch (SceneActorDamage_LocalPlayer()) {
                case 0:
                    if (*owner != 1)
                        return 0;
                    break;
                case 1:
                    if (*owner != 2)
                        return 0;
                    break;
                case 2:
                    if (*owner != 4)
                        return 0;
                    break;
                case 3:
                    if (*owner != 8)
                        return 0;
                    break;
            }
        }
    }
    return 1;
}

#endif
