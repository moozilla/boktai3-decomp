#include "global.h"

// MATCHING_CONCESSION: register-pinning
#include "engine.h"

// confidence=low: moves a requested player position and eventually restores player control.
struct Vec {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct Action {
    u8 state, pad[3];
    u32 frame;
};
struct Player {
    u8 pad[0x20];
    u32 flags;
};
struct Hresvelgr {
    u8 pad0[0x51];
    u8 heading;
    u8 pad52[2];
    struct Vec position, velocity;
    u8 pad64[0x38];
    u32 flags;
    u8 padA0[15];
    u8 finished, padB0, pending, pulse;
    u8 padB3[0x59];
    struct Player *player;
    struct Vec *requestedPosition;
    u8 pad114[0x60];
    u8 animationData[0x20], animation[0x20];
    struct Vec renderPosition;
    u8 pad1BC[0x134];
    struct Action action;
    u8 pad2F8[0x78];
    s16 releaseHeight;
    u8 pad372[0x2a];
    struct Vec target, step, savedPosition;
};
extern void *gUnk_02000580;
void ActionState_Reset(void *);
void ActionState_AdvanceState(void *);
void ActionState_SetState(void *, u32);
void Player_BeginPositionRequest(void *, void *, u32, u32, u32, u32, u32, u32, u32);
void Player_ApplyPositionRequest(void *, void *, u32, u32, u32, u32, s32, u32);
void Hresvelgr_SelectPlayerAnimation(void *, u32);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void *MotionEntry_CreateWithDelta(void *, void *, u32, u32);
s32 Math_ApproachEighth(s32, s32);
void Camera_RequestShake(u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline u8 PlayerCanMove(struct Player *p, u32 mask)
{
    u32 flags = p->flags & mask;
    u8 result = 0;
    if (!flags)
        result = 1;
    return result;
}
static inline u32 PlayerHasFlag(struct Player *p, u32 mask)
{
    return p->flags & mask;
}
static inline s32 RandomValue(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)table);
}
static inline void SetVector(struct Vec *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
static inline void AddVector(struct Vec *out, struct Vec *a, struct Vec *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
static inline s32 CellIndex(struct Vec *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
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
static inline s32 SignedShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -((-value) >> shift);
}
static inline void PolarUnitVector(struct Vec *out, u32 angle)
{
    s32 value;
    s32 magnitude;
    angle &= 255;
    magnitude = 256;
    value = gUnk_086149C4[(angle + 64) & 255] * magnitude;
    out->x = SignedShift(value, 12);
    out->y = 0;
    value = gUnk_086149C4[angle & 255] * magnitude;
    out->z = SignedShift(value, 12);
}
static inline void FindReleaseTarget(void *object, struct Vec *out)
{
    struct Hresvelgr *actor = object;
    u32 blockedMask = 0x81;
    if (out) {
        s32 cell = CellIndex(&actor->position);
        if (!((*((u16 *)(CellProperties(cell) + 2))) & blockedMask)) {
            *out = actor->position;
            out->x = (out->x & (~255)) + 128;
            out->z = (out->z & (~255)) + 128;
        } else {
            s32 i;
            for (i = 0; i <= 3; i++) {
                s32 neighbor = cell + gTerrainMap->neighborOffsets[i & 3];
                if (!((*((u16 *)(CellProperties(neighbor) + 2))) & blockedMask)) {
                    PolarUnitVector(out, (i << 6) + 192);
                    AddVector(out, &actor->position, out);
                    out->x = (out->x & (~255)) + 128;
                    out->z = (out->z & (~255)) + 128;
                    return;
                }
            }

            SetVector(out, 0xb54, 0, 0xb54);
        }
    }
}

void Hresvelgr_UpdatePlayerRelease(struct Hresvelgr *actor, u32 tick)
{
    struct Hresvelgr *saved = actor;
    struct Action *action = &actor->action;
    if (actor->pending) {
        actor->pending = 0;
        if (PlayerCanMove(actor->player, 0x40000)) {
            Player_BeginPositionRequest(actor->player, actor->requestedPosition, 0, 0, 0, 0, 1, 0,
                                        64);
            ActionState_Reset(action);
            Sound_PlaySong(0x3dc);
        } else
            ActionState_SetState(action, 6);
    }
    {
        u32 mask = 0x200008;
        saved->flags |= mask;
    }
    switch (action->state) {
        case 0:
            if ((++action->frame) > 1) {
                if (PlayerHasFlag(actor->player, 0x1000))
                    ActionState_AdvanceState(action);
                else
                    ActionState_SetState(action, 6);
            }
            break;

        case 1:
            if (!action->frame) {
                Hresvelgr_SelectPlayerAnimation(actor, 0);
                {
                    struct Vec *requested = actor->requestedPosition;
                    saved->savedPosition = *requested;
                }
                action->frame = 1;
            }
            if (actor->pulse)
                ActionState_AdvanceState(action);
            break;

        case 2:
            if (!action->frame) {
                Hresvelgr_SelectPlayerAnimation(actor, 1);
                SetVector(&saved->step, 0, 72, 0);
                FindReleaseTarget(saved, &saved->target);
                action->frame = 1;
            }
            if (saved->step.y < 0)
                ActionState_AdvanceState(action);
            else {
                struct Vec delta;
                const u16 *table = gUnk_0203B400;
                s32 rawX = RandomValue(table) >> 3;
                u32 smallMask = 31;
                u8 x = rawX & smallMask;
                s32 z = (RandomValue(table) >> 3) & smallMask;
                // Preserve the shared random table across particle setup.
                asm("" : : "r"(table));
                SetVector(&delta, x - 16, 0, z - 16);
                actor->velocity = saved->step;
                saved->step.y--;
                if (tick & 1)
                    MotionEntry_CreateWithDelta(&saved->renderPosition, &delta,
                                                (RandomValue(table) >> 5) % 4, 64);
                actor->velocity.x =
                    Math_ApproachEighth(actor->position.x, saved->target.x) - actor->position.x;
                actor->velocity.z =
                    Math_ApproachEighth(actor->position.z, saved->target.z) - actor->position.z;
            }
            break;

        case 3: {
            u32 y;
            if (!action->frame) {
                Hresvelgr_SelectPlayerAnimation(actor, 2);
                action->frame = 1;
                Sound_PlaySong(0x3dd);
            }
            y = actor->position.y;
            if (y <= ((u32)Height(&actor->position))) {
                actor->position.y = Height(&actor->position);
                ActionState_AdvanceState(action);
            } else {
                struct Vec delta;
                const u16 *table = gUnk_0203B400;
                s32 x = (RandomValue(table) >> 3) & 31;
                s32 z = (RandomValue(table) >> 3) & 31;
                // Preserve the shared random table across particle setup.
                asm("" : : "r"(table));
                SetVector(&delta, x - 16, 0, z - 16);
                if (tick & 1)
                    MotionEntry_CreateWithDelta(&saved->renderPosition, &delta,
                                                (RandomValue(table) >> 5) % 4, 64);
                actor->velocity = saved->step;
                saved->step.y--;
            }
            break;
        }

        case 4: {
            u32 amount;
            if (!action->frame)
                Sound_PlaySong(0x3df);
            amount = 30 - action->frame;
            Camera_RequestShake((amount * amount) >> 2);
            if ((++action->frame) > 29) {
                Sound_PlaySong(0x4f4);
                ActionState_AdvanceState(action);
            }
            break;
        }

        case 5:
            if (!action->frame) {
                Hresvelgr_SetAnimation(saved, saved->animation, saved->animationData, 12, 2, 0);
                Player_ApplyPositionRequest(gUnk_02000580, &actor->position, 1, actor->heading, 50,
                                            0, saved->releaseHeight, 0);
                action->frame = 1;
            }
            if (actor->pulse)
                ActionState_AdvanceState(action);
            break;

        case 6:
            actor->finished = 1;
            break;
    }
}
