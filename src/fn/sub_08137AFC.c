#define gPlayerSlotFlags gUnk_02000560
#include "global.h"
#include "area_effect_link.h"
#include "scene_actor_terrain.h"
#include "scene_actor_reset.h"

u32 Nav_IsPositionInMesh(void *, struct ContactPosition *, void *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, void *);

static inline u32 TestSlot(s32 i)
{
    u32 mask = 8;
    return mask & (&gPlayerSlotFlags)[i];
}
static inline u32 TestField(u32 *field, u32 mask)
{
    return mask & *field;
}
static inline u32 Test(u32 value, u32 mask)
{
    return value & mask;
}
static inline s32 FindNearestEligiblePlayer(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition delta;
    s32 bestDistance = 0x7FFFFFFF;
    s32 best = -1;
    struct AreaEffectLinkState *session = gLinkSession;
    s32 count = -1;
    s32 i;
    if (session)
        count = session->participantCount;
    for (i = 0; i < count; i++) {
        struct AreaEffectPlayer *player = gPlayers[i];
        if (player && !TestSlot(i) && !TestField(&player->flags, 0x20000)) {
            s32 cell = SceneActorTerrain_GetCellIndex((void *)&player->position);
            u8 *data = SceneActorTerrain_GetCell(cell);
            if (!Test(*(u16 *)(data + 2), 0x43)) {
                s32 distance;
                SceneActorTerrain_Subtract(&delta, (void *)&gPlayers[i]->position,
                                           &actor->body.position);
                distance = delta.x * delta.x + delta.z * delta.z;
                if (distance < bestDistance) {
                    bestDistance = distance;
                    best = i;
                }
            }
        }
    }
    return best;
}

static inline void ClearHalfFlags(u16 *flags, u32 mask)
{
    *flags &= mask;
}
static inline u8 MoveTo(struct SceneActorRegistryResetView *actor, u8 *group,
                        struct AreaEffectPlayer **slot)
{
    void *shape = *(void **)(group + 0x1fc);
    struct ContactPosition *position = &actor->body.position;
    void *destination = &(*slot)->position;
    if (Nav_IsPositionInMesh(shape, position, destination)) {
        s32 result = Terrain_ApplyBlocked(shape, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->body.motion, 8);
            return 1;
        }
    }
    ClearHalfFlags(&actor->flags2D4, ~0x200);
    return 0;
}

// confidence=low: heads for the nearest eligible link player; FALSE when there is none.
u32 sub_08137AFC(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    s32 best = FindNearestEligiblePlayer(actor);

    if (best < 0)
        return 0;
    {
        struct AreaEffectPlayer **slots = gPlayers;
        MoveTo(actor, group, &slots[best]);
    }
    return 1;
}
