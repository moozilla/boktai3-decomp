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

#define TENTACLE_READ_ROUTE_CELL(value_, cell_)                                                    \
    do {                                                                                           \
        (value_) = *(cell_);                                                                       \
    } while (0)
#define TENTACLE_BEGIN_CARDINAL_SCAN(mask_, remaining_)                                            \
    do {                                                                                           \
        (mask_) = 3;                                                                               \
        (remaining_) = 3;                                                                          \
    } while (0)
#define TENTACLE_BEGIN_ROUTE_SCAN(cursor_, cells_)                                                 \
    do {                                                                                           \
        (cursor_) = (cells_);                                                                      \
    } while (0)

static inline void Tentacle_AdvanceDirection(s32 *direction, const u32 *mask)
{
    *direction = (*direction + 1) & *mask;
}

#define TENTACLE_ADVANCE_CARDINAL(direction_, mask_)                                               \
    do {                                                                                           \
        Tentacle_AdvanceDirection(&(direction_), mask_);                                           \
    } while (0)

#define TENTACLE_COUNT_NEIGHBOR(remaining_)                                                        \
    do {                                                                                           \
        --(remaining_);                                                                            \
    } while (0)

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

#define TENTACLE_NEXT_ROUTE_CELL(cell_)                                                            \
    do {                                                                                           \
        ++(cell_);                                                                                 \
    } while (0)

#define TENTACLE_NEXT_ROUTE_CANDIDATE(index_, cursor_)                                             \
    do {                                                                                           \
        TENTACLE_NEXT_ROUTE_CELL(cursor_);                                                         \
        ++(index_);                                                                                \
    } while (0)

void TentacleActor_SelectWanderRoute(struct SceneActorRegistryResetView *actor)
{
    struct TentacleWanderState *state = actor->context;
    struct TentacleWanderTarget *route = &state->target;
    s32 cells[4];
    s32 facing = Tentacle_Cardinal(actor->body.direction);
    s32 *p;
    s32 count;
    s32 forward;
    s32 dir;
    s32 n;
    s32 i;
    s32 cell;
    s32 currentCell;
    u32 surface;
    union {
        u32 mask;
        s32 *cell;
    } scan;
    {
        s32 empty = -1;
        p = &cells[3];
        do {
            *p = empty;
            p--;
        } while (((s32)p) >= ((s32)cells));
    }
    count = 0;
    forward = -1;
    surface = (*SceneActorTerrain_GetCell(actor->terrainCell)) >> 4;
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
        state->preferred = ((actor->tracking.heading + 32) & 255) >> 6;
        state->attempts = 0;
    }
    TENTACLE_BEGIN_CARDINAL_SCAN(scan.mask, n);
    for (; n >= 0;) {
        if ((Tentacle_Abs(dir - state->preferred) != 2) && (Tentacle_Abs(dir - facing) != 2)) {
            cell = SceneActorTerrain_GetNeighborCell(actor, dir);
            if (cell >= 0) {
                if (dir == facing)
                    forward = cell;
                else
                    cells[count++] = cell;
            }
        }
        TENTACLE_ADVANCE_CARDINAL(dir, &scan.mask);
        TENTACLE_COUNT_NEIGHBOR(n);
    }

    if (count) {
        i = 0;
        if (i < count) {
            TENTACLE_BEGIN_ROUTE_SCAN(scan.cell, cells);
            do {
                TENTACLE_READ_ROUTE_CELL(currentCell, scan.cell);
                if (!TentacleActor_HasVisitedRoute(actor, currentCell)) {
                    Tentacle_CellCenter(&route->position, *scan.cell);
                    TENTACLE_READ_ROUTE_CELL(currentCell, scan.cell);
                    TentacleActor_RecordVisitedRoute(actor, currentCell);
                    state->attempts++;
                    state->time = 40;
                    return;
                }
                TENTACLE_NEXT_ROUTE_CANDIDATE(i, scan.cell);
            } while (i < count);
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
