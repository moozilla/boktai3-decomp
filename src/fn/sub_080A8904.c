#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#define FIELD(p, o, T)                  (*(T *)((u8 *)(p) + (o)))
#define TentacleActor_SelectWanderRoute sub_080A8904
#ifdef NONMATCHING
// fuzzy: 87.6%
// Remaining differences concern local lifetimes, register allocation and ordering.
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 SceneActorTerrain_GetNeighborCell(void *, s32);
u8 TentacleActor_HasVisitedRoute(void *, s32);
void TentacleActor_RecordVisitedRoute(void *, s32);
static inline s32 Abs(s32 x)
{
    return x < 0 ? -x : x;
}
static inline s32 Random(void)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    index *= 2;
    return *(const u16 *)((u32)gUnk_0203B400 + index);
}
void TentacleActor_SelectWanderRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    u8 *route = state + 0x8f8;
    s32 cells[4];
    s32 facing = ((actor->direction + 32) & 255) >> 6;
    s32 *p;
    s32 count, forward, dir, n, i, cell;
    u32 surface;
    p = &cells[3];
    do {
        *p = -1;
        p--;
    } while ((s32)p >= (s32)cells);
    count = 0;
    forward = -1;
    surface = *SceneActorTerrain_GetCell(actor->terrainCell) >> 4;
    if (surface - 1 > 1) {
        if (FIELD(state, 0x8e8, u8) == 2)
            dir = (facing + 1) & 3;
        else if (FIELD(state, 0x8e8, u8) == 3)
            dir = (facing + Random()) & 3;
        else
            dir = facing;
    } else
        dir = facing;
    if (FIELD(state, 0x8e8, u8) > 16) {
        FIELD(state, 0x8e9, u8) = ((FIELD(actor, 0xe9, u8) + 32) & 255) >> 6;
        FIELD(state, 0x8e8, u8) = 0;
    }
    for (n = 3; n >= 0; n--) {
        if (Abs(dir - FIELD(state, 0x8e9, u8)) != 2 && Abs(dir - facing) != 2) {
            cell = SceneActorTerrain_GetNeighborCell(actor, dir);
            if (cell >= 0) {
                if (dir == facing)
                    forward = cell;
                else
                    cells[count++] = cell;
            }
        }
        dir = (dir + 1) & 3;
    }
    if (count) {
        struct ContactPosition *dest = (void *)(route + 12);
        for (i = 0; i < count; i++) {
            if (!TentacleActor_HasVisitedRoute(actor, cells[i])) {
                SceneActorTerrain_GetCenter(cells[i], dest);
                TentacleActor_RecordVisitedRoute(actor, cells[i]);
                FIELD(state, 0x8e8, u8)++;
                FIELD(state, 0x8e4, u16) = 40;
                return;
            }
        }
        i = Mod(Random(), count);
        SceneActorTerrain_GetCenter(cells[i], dest);
        TentacleActor_RecordVisitedRoute(actor, cells[i]);
        FIELD(state, 0x8e8, u8)++;
        FIELD(state, 0x8e4, u16) = 40;
    } else if (forward != -1 && !TentacleActor_HasVisitedRoute(actor, forward)) {
        SceneActorTerrain_GetCenter(forward, (void *)(route + 12));
        TentacleActor_RecordVisitedRoute(actor, forward);
        FIELD(state, 0x8e8, u8) += 8;
        FIELD(state, 0x8e4, u16) = 40;
    } else {
        cell = SceneActorTerrain_GetNeighborCell(actor, (FIELD(state, 0x8e9, u8) + 2) & 3);
        if (cell >= 0) {
            SceneActorTerrain_GetCenter(cell, (void *)(route + 12));
            TentacleActor_RecordVisitedRoute(actor, cell);
            FIELD(state, 0x8e8, u8) += 2;
            FIELD(state, 0x8e4, u16) = 40;
        } else
            FIELD(state, 0x8e9, u8) = (FIELD(state, 0x8e9, u8) + 1) & 3;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080A8904);
#endif
