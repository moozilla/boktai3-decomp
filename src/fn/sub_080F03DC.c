#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"

struct Player;
s32 Div(s32, s32);
s32 Mod(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Player_ApplyPositionRequest(struct Player *, struct SceneVec *, s32, u32, s32, s32, s32, u32);
void SceneActorControl_OnIdle(void);

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

static inline u8 TestFlags(struct IvyContext *context, u32 maskInput)
{
    u32 mask = maskInput;
    if (context->flags & mask)
        return 1;
    return 0;
}

static inline void SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}

static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    u32 state = s;
    actor->state2A8 = state;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

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

/* Ivy release (tentative): once per capture (context flag 1), and when context flag 2 says the
 * player is held, puts the player back on the centre of the actor's terrain cell facing the
 * actor's quadrant (cue 0xE6); then returns the actor to idle with both colliders disabled. */
void Ivy_ReleaseCapturedPlayer(struct SceneActorRegistryResetView *actor)
{
    if (TestFlags(actor->context, 1))
        return;
    if (TestFlags(actor->context, 2)) {
        struct SceneVec position;
        s32 angle;
        CellCenter(&position, actor->terrainCell);
        {
            s32 turn = (actor->direction + 0x20) & 0xFF;
            angle = (turn >= 0 ? turn >> 6 : -(-turn >> 6)) << 6;
        }
        {
            struct IvyTarget *target = (void *)actor->fieldDC;
            if (target->kind <= 3) {
                Player_ApplyPositionRequest(target->player, &position, 0x40, angle, 0x3C, 0, 0, 0);
                Sound_PlaySong(0xE6);
            }
        }
        ((u8 *)actor->drawable)[7] = 2;
    }
    SetCtl(actor, (void *)SceneActorControl_OnIdle, 16);
    SetState(actor, 0, 0, 3);
    {
        struct ContactCollider *collider = &actor->colliderA;
        collider->flags |= 4;
    }
    {
        struct ContactCollider *collider = &actor->colliderB;
        collider->flags |= 4;
    }
    {
        u32 m = 0x800;
        actor->flags2C0 = m | actor->flags2C0;
    }
    ((struct IvyContext *)actor->context)->flags |= 1;
    actor->health = 0;
}
