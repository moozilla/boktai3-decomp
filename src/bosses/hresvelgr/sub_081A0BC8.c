#include "engine/terrain/terrain_sampling.h"
#include "engine/shared/global.h"

#include "bosses/hresvelgr/boss_action_state.h"
#include "engine/shared/engine.h"
struct Vec {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct Hresvelgr {
    u8 pad0[0x51];
    u8 heading;
    u8 pad52[2];
    struct Vec position;
    u8 pad5C[0x40];
    u32 flags;
    u8 padA0[8];
    u8 direction4, direction8;
    u8 padAA[5];
    u8 finished, padB0, pending;
    u8 padB2[0x1a];
    u8 targetHeading;
    u8 padCD[0x43];
    struct Vec *target;
    u8 pad114[0x60];
    u8 animationData[0x20], animation[0x20];
    u8 pad1B4[0x1a];
    u16 collisionMask;
    u8 pad1D0[0xc];
    void *collisionBounds;
    u8 pad1E0[0x110];
    struct BossActionState action;
    u8 pad300[0x2b];
    u8 side;
    u8 pad32C[0xf8];
    u8 boundsLow[0x2e0];
    u8 boundsHigh[4];
};
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void Actor_SetHeadingVelocity(void *, u32);
void Hresvelgr_SpawnProjectile(void *, u32, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 FacingSector4(s16 angle)
{
    return (angle + 96) >> 6;
}
static inline s32 FacingSector8(s16 angle)
{
    return (angle + 112) >> 5;
}
static inline void SetHeading(struct Hresvelgr *p, s32 heading)
{
    p->heading = heading;
    {
        s32 sector = FacingSector4(p->heading);
        u32 mask = 3;
        mask &= sector;
        p->direction4 = mask;
    }
    {
        s32 sector = FacingSector8(p->heading);
        u32 mask = 7;
        mask &= sector;
        p->direction8 = mask;
    }
}
static inline s32 RandomValue(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)((u32)table + offset);
}
static inline void SetVector(struct Vec *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
static inline u8 *CellProperties(s32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return (cell * 4 + 12) + gTerrainMap->cells;
}
static inline s32 Height(struct Vec *p)
{
    u8 *cell = CellProperties(Terrain_GetCellIndex(p));
    u32 shape = *cell >> 4;
    s32 height = (*cell & 15) << 8;
    switch (shape) {
        case 1:
            height -= *(u8 *)&p->z;
            break;
        case 2:
            height -= *(u8 *)&p->x;
            break;
    }
    return height;
}
static inline u32 *EnableFlags(struct Hresvelgr *actor, u32 mask)
{
    u32 *p = &actor->flags;
    *p |= mask;
    return p;
}
static inline void SetCollisionMask(struct Hresvelgr *actor, u32 mask)
{
    actor->collisionMask = mask;
}
// confidence=low: traverses a boundary and spawns falling projectiles over valid terrain.
void Hresvelgr_UpdateBoundaryBarrage(struct Hresvelgr *actor, u32 tick)
{
    u32 normalCollisionMask = 0x2f0;
    struct BossActionState *action = &actor->action;
    struct Vec *target = actor->target;
    u32 *flags;
    s32 state;
    if (actor->pending) {
        actor->pending = 0;
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
    }
    flags = EnableFlags(actor, 12);
    state = action->state;
    switch (state) {
        case 0:
            *flags |= 16;
            if (action->frame > 128) {
                if (!actor->side)
                    SetVector(&actor->position, 1024, 512, target->z);
                else
                    SetVector(&actor->position, target->x, 512, 1024);
                ActionState_AdvanceState(action);
            } else
                action->frame++;
            break;
        case 1:
            *flags |= 0x1010000;
            if (!action->frame) {
                action->frame = state;
                SetHeading(actor, actor->targetHeading);
                Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData, 2, state, 0);
                SetCollisionMask(actor, 0xd000);
                actor->collisionBounds = actor->boundsHigh;
                Sound_PlaySong(0x4d0);
            } else {
                Actor_SetHeadingVelocity(actor, 32);
                state &= tick;
                if (!state) {
                    u32 mask = 16;
                    *flags |= mask;
                }
                if (!(tick & 7)) {
                    struct Vec pos;
                    const u16 *table = gRandomTable;
                    s32 rawX = RandomValue(table);
                    s32 px = actor->position.x;
                    // Not `% 512`: as the whole right-hand side, expand_divmod would
                    // compute the quotient in x itself. Here the division is a
                    // subexpression, so the quotient gets its own register, as in the ROM.
                    s32 x = (rawX >> 3) - ((rawX >> 3) / 512) * 512;
                    s32 rawZ = RandomValue(table);
                    s32 pz = actor->position.z;
                    u32 height;
                    SetVector(&pos, px + x - 256, 0x2000, pz + ((rawZ >> 3) % 512) - 256);
                    height = Height(&pos) - 256;
                    if (height <= 0x6ff)
                        Hresvelgr_SpawnProjectile(&pos, 0, 1);
                }
                {
                    u32 direction =
                        (actor->direction4 >> 1) | (normalCollisionMask & (actor->direction4 >> 8));
                    if (direction == 0 && actor->position.x > 0x1400)
                        ActionState_AdvanceState(action);
                    else if (direction == 1 && actor->position.z > 0x1400)
                        ActionState_AdvanceState(action);
                }
            }
            break;
        case 2:
            actor->collisionMask = normalCollisionMask;
            actor->collisionBounds = actor->boundsLow;
            {
                u32 mask = 16;
                *flags |= mask;
            }
            actor->finished = 1;
            break;
    }
}
