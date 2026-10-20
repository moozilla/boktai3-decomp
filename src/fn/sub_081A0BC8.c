#include "global.h"

#ifdef NONMATCHING
// fuzzy: 93.9%
// Remaining differences: animation argument lifetimes and RNG arithmetic/register allocation.
// MATCHING_CONCESSION: register-pinning
#include "engine.h"
#define Hresvelgr_UpdateBoundaryBarrage sub_081A0BC8
// confidence=low: traverses a boundary, spawning falling projectiles over valid terrain.
struct Vec {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct Action {
    u8 state, pad[3];
    u32 frame;
};
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
    struct Action action;
    u8 pad2F8[0x33];
    u8 side;
    u8 pad32C[0xf8];
    u8 boundsLow[0x2e0];
    u8 boundsHigh[4];
};
void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
void ActionState_AdvanceState(void *);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void sub_0819E754(void *, u32);
void Hresvelgr_SpawnProjectile(void *, u32, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline void SetHeading(struct Hresvelgr *p, s32 heading)
{
    s32 shifted;
    register u32 mask asm("r1");
    p->heading = heading;
    shifted = (p->heading + 96) >> 6;
    mask = 3;
    mask &= shifted;
    asm("" : "+r"(mask));
    p->direction4 = mask;
    shifted = (p->heading + 112) >> 5;
    mask = 7;
    mask &= shifted;
    asm("" : "+r"(mask));
    p->direction8 = mask;
}
static inline s32 RandomValue(const u16 *table)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    return *(const u16 *)((u32)table + offset);
}
static inline void SetVector(struct Vec *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
static inline s32 CellIndex(struct Vec *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u8 *CellProperties(s32 cell)
{
    u8 *p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        return p + 4;
    return (cell * 4 + 12) + gUnk_030052F4->cells;
}
static inline s32 Height(struct Vec *p)
{
    u8 *cell = CellProperties(CellIndex(p));
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
void Hresvelgr_UpdateBoundaryBarrage(struct Hresvelgr *actor, u32 tick)
{
    struct Action *action = &actor->action;
    struct Vec *target = actor->target;
    u32 *flags;
    register s32 state asm("r5");
    if (actor->pending) {
        actor->pending = 0;
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
    }
    flags = EnableFlags(actor, 12);
    state = action->state;
    switch (state) {
        case 0: {
            u32 mask = 16;
            *flags |= mask;
        }
            if (action->frame > 128) {
                if (!actor->side)
                    SetVector(&actor->position, 1024, 512, target->z);
                else
                    SetVector(&actor->position, target->x, 512, 1024);
                ActionState_AdvanceState(action);
            } else
                action->frame++;
            break;
        case 1: {
            u32 mask = 0x1010000;
            *flags |= mask;
        }
            if (!action->frame) {
                action->frame = state;
                SetHeading(actor, actor->targetHeading);
                Hresvelgr_SetAnimation(actor, actor->animation, (u8 *)actor + 0x174, 2, 1, 0);
                SetCollisionMask(actor, 0xd000);
                actor->collisionBounds = actor->boundsHigh;
                Sound_PlaySong(0x4d0);
            } else {
                sub_0819E754(actor, 32);
                if (!(tick & state)) {
                    u32 mask = 16;
                    *flags |= mask;
                }
                if (!(tick & 7)) {
                    struct Vec pos;
                    register const u16 *table asm("r9") = gUnk_0203B400;
                    s32 rawX = RandomValue(table);
                    register s32 px asm("r8") = actor->position.x;
                    register s32 x asm("r2") = (rawX >> 3) % 512;
                    s32 rawZ = RandomValue(table);
                    s32 pz = actor->position.z;
                    s32 z = (rawZ >> 3) % 512;
                    u32 height;
                    SetVector(&pos, px + x - 256, 0x2000, pz + z - 256);
                    height = Height(&pos) - 256;
                    if (height <= 0x6ff)
                        Hresvelgr_SpawnProjectile(&pos, 0, 1);
                }
                {
                    u32 direction = actor->direction4 >> 1;
                    if (direction == 0 && actor->position.x > 0x1400)
                        ActionState_AdvanceState(action);
                    else if (direction == 1 && actor->position.z > 0x1400)
                        ActionState_AdvanceState(action);
                }
            }
            break;
        case 2:
            actor->collisionMask = 0x2f0;
            actor->collisionBounds = actor->boundsLow;
            {
                u32 mask = 16;
                *flags |= mask;
            }
            actor->finished = 1;
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081A0BC8);
#endif
