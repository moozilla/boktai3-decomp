#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"

struct Player;
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Player_ApplyPositionRequest(struct Player *, struct SceneVec *, s32, u32, s32, s32, s32, u32);

struct IvyContext {
    u8 unknown0[0x62C];
    u32 flags;
};

struct IvyTarget {
    u8 unknown0[8];
    u8 kind;
    u8 unknown9[0x33];
    struct Player *player;
};

static inline void CellCenter(struct SceneVec *out, u32 cell)
{
    s32 x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
    s32 z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
    u8 *data;
    out->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    out->y = (*data & 15) << 8;
    out->z = (z << 8) + 0x80;
}

/* Ivy release start (tentative): on entry disables the main collider, plays directional
 * animation 0x15 and puts a held player back on the centre of the actor's terrain cell; then
 * clears context flag 2. */
void Ivy_BeginRelease(struct SceneActorRegistryResetView *actor)
{
    struct IvyContext *context = actor->context;
    if (SceneActorReaction_TakePrepared(actor)) {
        {
            u16 *destination = &actor->colliderA.flags;
            u32 flags = *destination;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *destination = mask;
        }
        {
            u32 mask = 0x800;
            mask |= actor->flags2C0;
            actor->flags2C0 = mask;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0x15);
        {
            struct IvyTarget *target = (void *)actor->tracking.target;
            struct SceneVec position;
            u8 angle;
            CellCenter(&position, actor->terrainCell);
            {
                s32 turn = (actor->body.direction + 0x20) & 0xFF;
                s32 quadrant = turn >= 0 ? turn >> 6 : -(-turn >> 6);
                angle = quadrant << 6;
            }
            if (target->kind <= 3)
                Player_ApplyPositionRequest(target->player, &position, 0x40, angle, 0x3C, 0, 0, 0);
        }
        ((u8 *)actor->drawable)[7] = 2;
        {
            u32 *flags = &context->flags;
            u32 mask = ~2;
            *flags &= mask;
        }
    }
    if (actor->field2B3)
        actor->field2B0 = 1;
}
