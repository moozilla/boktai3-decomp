#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_motion.h"
void CpuSet(const void *, void *, u32);
#include "gba/macro.h"
#include "player_cue_sequence.h"
void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorAnimationView *, s32);
static inline u32 StepDrawable(struct SceneActorControlMotionDrawable *owner)
{
    return PlayerCue_StepSequenceEnded(owner, &owner->sequence);
}
static inline u32 CellIndex(const struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 GroundHeight(u32 cell, const struct ContactPosition *position)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    register u32 flags asm("r1");
    u32 shape, height;
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    flags = *data;
    shape = flags >> 4;
    height = (flags & 15) << 8;
    switch (shape) {
        case 1:
            height -= *(u8 *)&position->z;
            break;
        case 2: {
            register const struct ContactPosition *raw asm("r4") = position;
            asm("" : : "r"(raw));
            height -= *(u8 *)&raw->x;
            break;
        }
    }
    return height;
}
static inline void ClearVector(void *destination, u32 *zero)
{
    void *target = destination;
    u32 *source;
    *zero = 0;
    source = zero;
    CpuSet(source, target, 0x05000002);
}
void SceneActorControl_RestoreMotion(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
    struct SceneActorControlMotionDrawable *drawable = actor->drawable;

    u32 height;
    state->field3DB = 0;
    {
        register u32 offset asm("r4") = 0x3E0;
        u32 zero = 0;
        u8 byteZero = 0;
        register struct ContactPosition *stored asm("r2");
        *(u16 *)((u8 *)state + offset) = zero;
        state->phase = byteZero;
        offset += 3;
        *(s8 *)((u8 *)state + offset) = 1;
        state->sway = zero;
        offset += 3;
        *((u8 *)state + offset) = byteZero;
        state->startTick = byteZero;
        state->flags3D4 = zero;
        offset += 0x4E;
        stored = (void *)((u8 *)state + offset);
        actor->body.position = *stored;
        {
            u32 cell = CellIndex(stored);
            {
                u16 *field = &actor->terrainCell;
                *field = cell;
            }
        }
    }
    height = GroundHeight(actor->terrainCell, &state->storedPosition);
    {
        u16 *ground = &((struct SceneActorControlGroundView *)actor)->ground;
        s16 *actorY;
        u32 previousY;
        u32 difference;
        register u32 offset asm("r4");
        *ground = height;
        actorY = &actor->body.position.y;
        previousY = (u16)*actorY;
        difference = previousY - height;
        offset = 0x2EA;
        asm("" : "+r"(offset));
        {
            register u16 *field asm("r1") = (void *)((u8 *)actor + offset);
            *field = difference;
        }
        *actorY = *ground;
    }
    if (actor->drawableKind == 0)
        ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position =
            actor->body.position;
    else
        ((struct SceneActorRegistryTileDrawable *)actor->drawable)->position = actor->body.position;
    {
        u32 zero;
        {
            register u32 offset asm("r0") = 0x41c;
            asm("" : "+r"(offset));
            ClearVector((u8 *)state + offset, &zero);
        }
        {
            u32 offset = 0x424;
            ClearVector((u8 *)state + offset, &zero);
        }
        {
            u32 offset = 0x42c;
            ClearVector((u8 *)state + offset, &zero);
        }
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    StepDrawable(drawable);
}
