#include "scene_actor_reset.h"
#include "scene_actor_terrain.h"
#include "engine.h"
#include "ram.h"
struct TentacleWanderTarget {
    u8 unknown0[12];
    struct ContactPosition position;
};
struct TentacleWanderState {
    u8 unknown0[0x8E4];
    u16 time;
    u8 unknown8E6[2];
    u8 attempts, preferred;
    u8 unknown8EA[0xE];
    struct TentacleWanderTarget target;
};
s32 SceneActorTerrain_GetNeighborCell(void *, s32);
u8 TentacleActor_HasVisitedRoute(void *, s32);
void TentacleActor_RecordVisitedRoute(void *, s32);
static inline s32 Tentacle_Abs(s32 x)
{
    return x < 0 ? -x : x;
}
static inline u32 Tentacle_Random(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)(RANDOM_TABLE_ADDR + index);
}
static inline s32 Tentacle_Cardinal(u8 direction)
{
    return ((direction + 32) & 255) >> 6;
}

static inline s16 Tentacle_MapWidth(void)
{
    return SceneActorTerrain_Width();
}

static inline void Tentacle_CellCenter(struct ContactPosition *position, s32 cell)
{
    s32 x = Mod(cell, Tentacle_MapWidth());
    s32 z = Div(cell, Tentacle_MapWidth());
    position->x = ((u32)x << 8) + 128;
    position->y = (*SceneActorTerrain_GetCell(cell) & 15) << 8;
    position->z = ((u32)z << 8) + 128;
}

void TentacleActor_SelectWanderRoute(struct SceneActorRegistryResetView *actor)
{
    struct TentacleWanderState *state = actor->base.context;
    struct TentacleWanderTarget *route = &state->target;
    s32 cells[4];
    s32 facing = Tentacle_Cardinal(actor->base.body.direction);
    s32 count;
    s32 forward;
    s32 dir;
    s32 *p;
    s32 i;
    s32 cell;
    u32 surface;
    {
        s32 empty = -1;
        p = &cells[3];
        do {
            *p = empty;
            p--;
        } while ((s32)p >= (s32)cells);
    }
    count = 0;
    forward = -1;
    surface = (*SceneActorTerrain_GetCell(actor->base.terrainCell)) >> 4;
    if ((surface - 1) > 1) {
        if (state->attempts == 2)
            dir = (facing + 1) & 3;
        else if (state->attempts == 3)
            dir = (facing + Tentacle_Random()) & 3;
        else
            dir = facing;
    } else
        dir = facing;
    if (state->attempts > 16) {
        state->preferred = ((actor->base.tracking.heading + 32) & 255) >> 6;
        state->attempts = 0;
    }
    for (i = 0; i < 4; i++) {
        if (Tentacle_Abs(dir - state->preferred) == 2) {
            dir = (dir + 1) & 3;
            continue;
        }
        if (Tentacle_Abs(dir - facing) == 2) {
            dir = (dir + 1) & 3;
            continue;
        }
        cell = SceneActorTerrain_GetNeighborCell(actor, dir);
        if (cell < 0) {
            dir = (dir + 1) & 3;
            continue;
        }
        if (dir == facing)
            forward = cell;
        else
            cells[count++] = cell;
        dir = (dir + 1) & 3;
    }

    if (count) {
        for (i = 0; i < count; i++) {
            if (!TentacleActor_HasVisitedRoute(actor, cells[i])) {
                Tentacle_CellCenter(&route->position, cells[i]);
                TentacleActor_RecordVisitedRoute(actor, cells[i]);
                state->attempts++;
                state->time = 40;
                return;
            }
        }

        i = Mod(Tentacle_Random(), count);
        Tentacle_CellCenter(&route->position, cells[i]);
        TentacleActor_RecordVisitedRoute(actor, cells[i]);
        state->attempts++;
        state->time = 40;
    } else if ((forward != (-1)) && (!TentacleActor_HasVisitedRoute(actor, forward))) {
        Tentacle_CellCenter(&route->position, forward);
        TentacleActor_RecordVisitedRoute(actor, forward);
        state->attempts += 8;
        state->time = 40;
    } else {
        cell = SceneActorTerrain_GetNeighborCell(actor, (state->preferred + 2) & 3);
        if (cell >= 0) {
            Tentacle_CellCenter(&route->position, cell);
            TentacleActor_RecordVisitedRoute(actor, cell);
            state->attempts += 2;
            state->time = 40;
        } else
            state->preferred = (state->preferred + 1) & 3;
    }
}
