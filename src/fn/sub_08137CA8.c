#define gPlayerSlots     gUnk_02000580
#define gPlayerSlotFlags gUnk_02000560
#include "global.h"
#include "area_effect_link.h"
#include "scene_actor_terrain.h"
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
s32 SceneActorTarget_FindNearestEligiblePlayer(const struct ContactPosition *position)
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
        struct AreaEffectPlayer *player = gPlayerSlots[i];
        if (player && !TestSlot(i) && !TestField(&player->flags, 0x20000)) {
            s32 cell = SceneActorTerrain_GetCellIndex((void *)&player->position);
            u8 *data = SceneActorTerrain_GetCell(cell);
            if (!Test(*(u16 *)(data + 2), 0x43)) {
                s32 distance;
                SceneActorTerrain_Subtract(&delta, (void *)&gPlayerSlots[i]->position, position);
                distance = delta.x * delta.x + delta.z * delta.z;
                if (distance < bestDistance) {
                    bestDistance = distance;
                    best = i;
                }
            }
        }
    }
    if (best < 0)
        return -1;
    return best;
}
