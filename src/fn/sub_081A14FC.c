// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
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
    struct Vec position, velocity;
    u8 pad64[0x38];
    u32 flags;
    u8 padA0[8];
    u8 direction4, direction8;
    u8 padAA[5];
    u8 finished, padB0, pending, pulse;
    u8 padB3[0x19];
    u8 targetHeading;
    u8 padCD[0xa7];
    u8 animationData[0x20], animation[0x20];
    struct Vec renderPosition;
    u8 pad1BC[0x134];
    struct Action action;
    u8 pad2F8[0x24];
    s16 motionX, motionY, motionZ, motionW;
    u8 pad324[7];
    u8 side;
    u8 pad32C[0x70];
    struct Vec target, delta;
};
void ActionState_Reset(void *);
void ActionState_AdvanceState(void *);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void SceneActors_RunFirstEventScript(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, u32, u32, u32);
void SceneActors_RunSecondEventScript(void *, void *);
void Anim_SetFrameDurationScale(void *, u16);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void GameState_SetEventFlag(u32);
void AnimatedParticles_EmitBurst(void *, u32, u32, u32, u32, u32, u32, u32);
void MotionEntry_CreateWithDelta(void *, void *, s32, u32);
s32 sub_08249420(u32, u32);
s32 sub_08249498(u32, u32);
s32 ScalarApproach_StepQuarter(s32, s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Sound_PlaySong(u32);

static inline void AddVector(struct Vec *out, struct Vec *a, struct Vec *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
static inline void SetMotion(struct Hresvelgr *actor, s16 x, s16 y, s16 z, s16 w)
{
    actor->motionX = x;
    actor->motionY = y;
    actor->motionZ = z;
    actor->motionW = w;
}
static inline void EnableFlags(struct Hresvelgr *actor, u32 mask)
{
    actor->flags |= mask;
}
static inline s32 RandomValue(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
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
void sub_081A14FC(struct Hresvelgr *saved, u32 tick)
{
    s32 height;
    s32 i;
    struct Hresvelgr *actor = saved;
    struct Action *action = &actor->action;
    if (actor->pending) {
        actor->pending = 0;
        ActionState_Reset(action);
    }
    switch (action->state) {
        case 0:
            if (!action->frame) {
                Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData, 15, 1, 0);
                action->frame++;
                Sound_PlaySong(0x4cf);
            } else if (saved->padA0[3] > 8) {
                if (!Player_IsSlotInActionTwelveOrThirtyEight(0)) {
                    SceneActors_RunFirstEventScript(saved, &actor->renderPosition);
                    Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData, 20, 1, 0);
                    Sound_PlaySong(0x3e2);
                    ActionState_AdvanceState(action);
                }
            }
            break;
        case 1: {
            u32 frame = action->frame;
            s32 step = sub_08249420(frame, 30);
            if (step == 8) {
                ActionState_AdvanceState(action);
                Anim_SetFrameDurationScale(actor->animation, 64);
            } else {
                s32 remainder = sub_08249498(frame, 30);
                if (!remainder) {
                    s32 x = ScalarApproach_StepQuarter(saved->position.x, 0xb54);
                    s32 z = ScalarApproach_StepQuarter(saved->position.z, 0xb54);
                    s32 dx, dy, dz;
                    SetVector(&actor->target, x, remainder, z);
                    dx = (actor->target.x - saved->position.x) / 30;
                    dy = 60 / (step + 1);
                    dz = (actor->target.z - saved->position.z) / 30;
                    SetVector(&actor->delta, dx, dy, dz);
                    Anim_SetFrameDurationScale(actor->animation, (step + 1) * 24 + 64);
                    if (step == 7)
                        Anim_SetKeyframes(actor->animation, actor->animationData, 20, 3, remainder);
                } else {
                    saved->velocity = actor->delta;
                    if (actor->delta.y > 0)
                        actor->delta.y--;
                    if (step == 7)
                        EnableFlags(saved, 0x800000);
                }
                {
                    s32 period = (step + 1) * 4;
                    if (!sub_08249498(action->frame, period)) {
                        struct Vec delta, position;
                        register const u16 *table asm("r6") = (const u16 *)RANDOM_TABLE_ADDR;
                        s32 ox, px, oy, py, oz, pz;
                        s32 sampleX, sampleZ;
                        register s32 sampleY asm("r3");
                        s32 valueY;
                        register s32 rawY asm("r0");
                        SetVector(&delta, (RandomValue(table) >> 3) % 32 - 16, actor->delta.y,
                                  (RandomValue(table) >> 3) % 32 - 16);
                        sampleX = RandomValue(table) >> 3;
                        ox = sampleX - (sampleX / 256) * 256;
                        px = actor->renderPosition.x;
                        rawY = RandomValue(table);
                        valueY = rawY >> 3;
                        asm("" : : "r"(valueY));
                        sampleY = valueY;
                        oy = sampleY - (sampleY / 128) * 128;
                        py = actor->renderPosition.y;
                        sampleZ = RandomValue(table) >> 3;
                        oz = sampleZ - (sampleZ / 256) * 256;
                        pz = actor->renderPosition.z;
                        SetVector(&position, px + ox - 128, py + oy, pz + oz - 128);
                        MotionEntry_CreateWithDelta(&position, &delta,
                                                    (RandomValue(table) >> 3) % 4, 64);
                    }
                }
                if (saved->pulse)
                    Sound_PlaySong(0x3d9);
                action->frame++;
            }
            break;
        }
        case 2: {
            if (!action->frame) {
                SceneActors_StartUnmirroredAnimation(saved, actor->animation, actor->animationData,
                                                     21, 1, 0);
                action->frame++;
                Sound_PlaySong(0x3dd);
            }
            height = Height(&saved->position);
            if (saved->position.y < height) {
                Sound_PlaySong(0x3df);
                for (i = 0; i < 2; i++) {
                    struct Vec position;
                    register const u16 *table asm("r3") = (const u16 *)RANDOM_TABLE_ADDR;
                    SetVector(&position, (RandomValue(table) >> 3) % 1024 - 512, 0,
                              -((RandomValue(table) >> 3) % 256));
                    AddVector(&position, &position, &saved->position);
                    AnimatedParticles_EmitBurst(&position, 7, 0, 64, 4, 4, 4, 4);
                }
                if (action->frame == 1)
                    SceneActors_StartUnmirroredAnimation(saved, actor->animation,
                                                         actor->animationData, 22, 2, 0);
                saved->position.y = height;
                if (action->frame > 3) {
                    saved->position.y = height;
                    ActionState_AdvanceState(action);
                    break;
                }
                action->frame++;
                actor->delta.y = sub_08249420(32, action->frame);
            }
            saved->velocity = actor->delta;
            actor->delta.y--;
            if (!(tick & 7) && action->frame == 1)
                AnimatedParticles_EmitBurst(&saved->position, 2, 0, 64, 8, 8, 4, 4);
            break;
        }
        case 3:
            action->frame++;
            if (action->frame > 8) {
                ActionState_AdvanceState(action);
                GameState_SetEventFlag(0);
                GameState_SetEventFlag(3);
                GameState_SetEventFlag(78);
                SceneActors_RunSecondEventScript(saved, &saved->renderPosition);
            }
            break;
        case 4:
            saved->finished = 1;
            break;
    }
    SetMotion(actor, 0, 0, 0, 0);
    EnableFlags(saved, 14);
}
