#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_restoration.h"
#include "scene_actor_control_helpers.h"
#include "engine.h"
#include "ram.h"

extern u8 *gPlayers[];
s32 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorAttachment_ReplaceResponseSlot(struct SceneActorRegistryResetView *, u32, u32, u8);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Callback_InvokeActor(void *, void *);
void sub_0807E334(void *);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void Memory_Clear(void *dst, s32 bytes);

static inline u32 Dog080E3874_Rand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return gRandomTable[index];
}

// Cell index for a position (its high byte is the cell coordinate), 0 outside the grid.
static inline s32 Dog080E3874_CellIndex(u16 x, u16 z)
{
    s32 cx = (s16)x >> 8;
    s32 cz = (s16)z >> 8;
    struct EngineMap *map = (struct EngineMap *)gTerrainMap;
    if (cx < 0 || cz < 0 || cx >= gTerrainWidth || cz >= gTerrainHeight)
        return 0;
    return *(u16 *)((u8 *)map + 0x24 + (cz << 1)) + cx;
}

// Terrain word for a cell: the override if present, else the base table.
static inline u8 *Dog080E3874_CellData(s32 cell)
{
    u8 *override = TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return override + 4;
    return *(u8 **)((u8 *)gTerrainMap + 4) + (cell << 2) + 0xC;
}

// Fills out from pos: x and z copied, y from the terrain word of the cell.
static inline void Dog080E3874_Sample(struct ContactPosition *out, const u16 *pos)
{
    u8 b;
    s32 y;
    out->x = pos[0];
    out->z = pos[2];
    b = *Dog080E3874_CellData(Dog080E3874_CellIndex(pos[0], pos[2]));
    y = (b & 0xF) << 8;
    if ((b >> 4) == 1)
        y -= (u8)out->z;
    else if ((b >> 4) == 2)
        y -= (u8)out->x;
    out->y = y;
}

// Re-arms the reaction control with a new target and kind.
static inline void Dog080E3874_Arm(u8 *a, void *target, u32 kind)
{
    *(u8 *)(a + 0x2B2) = 1;
    *(u8 *)(a + 0x2B0) = 0;
    *(u8 *)(a + 0x2B5) = kind;
    *(void **)(a + 0x21C) = target;
    *(u16 *)(a + 0x2C8) = 0;
    *(u32 *)(a + 0x114) &= ~1;
    *(u32 *)(a + 0x104) = 0;
}

// Writes the pending step block at ctl: 0x1E frames, the target, and the state.
static inline void Dog080E3874_Step(u8 *ctl, void *target, u8 state)
{
    *(u32 *)(ctl + 0xC) = 0x1E;
    *(void **)(ctl + 0x10) = target;
    ctl[8] = 0;
    ctl[9] = 0;
    ctl[0xA] = state;
}

// Picks a new facing that differs from the current one (up to four tries).
static inline u32 Dog080E3874_NewFacing(u32 current)
{
    u32 tries;
    for (tries = 0; tries < 4; tries++) {
        u32 candidate = (Dog080E3874_Rand() & 3) << 6;
        if (candidate != current)
            return candidate;
    }
    return current;
}

// Stores the chosen facing: the old facing goes to 0x205, the new one to 0x204 and 0x59,
// then the reaction control is re-armed (ROM block 3DBA..3ED4).
static inline void Dog080E3874_Turn(u8 *a)
{
    u32 current = *(u8 *)(a + 0x59);
    u32 dir = Dog080E3874_NewFacing(current);
    *(u8 *)(a + 0x205) = current;
    *(u8 *)(a + 0x204) = dir;
    *(u8 *)(a + 0x59) = dir;
    *(u32 *)(a + 0x2AC) = 0;
    *(u8 *)(a + 0x2B1) = 1;
    *(u8 *)(a + 0x2A8) = 0;
    *(u8 *)(a + 0x2A9) = 0;
    *(u8 *)(a + 0x2AA) = 3;
    *(u8 *)(a + 0x2B2) = 1;
    *(void **)(a + 0x21C) = (void *)SceneActorReaction_RestoreAfterDelay;
}

#ifdef NONMATCHING
// fuzzy: 19.6%
// scope shape 1158 (differences once low registers are set aside)
// confidence=low: dog state update; re-arms the reaction control, then steers toward a terrain cell or idles.
void sub_080E3874(struct SceneActorRegistryResetView *actor)
{
    u8 *a = (u8 *)actor;
    u8 *q;
    u16 *pos;
    struct ContactPosition *out;
    s32 state;
    u32 cell;
    s32 res;
    s32 cx, cz;
    struct ContactPosition target;

    if (*(u8 *)(a + 0x2B1))
        *(u8 *)(a + 0x2B1) = 0;
    if ((u8)Callback_InvokeActor(actor, *(void **)(a + 0x270)))
        return;
    if ((u8)sub_0812F3CC(actor))
        return;
    if (*(u32 *)(a + 0x2AC) <= 0x3C) {
        *(u32 *)(a + 0x2AC) += 1;
        return;
    }
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, *(u8 *)(a + 0x98));
    *(u8 *)(a + 0x2A8) = 3;
    *(u8 *)(a + 0x2A9) = 1;
    *(u8 *)(a + 0x2AA) = 0;
    *(u32 *)(a + 0x2AC) = 0;
    *(u8 *)(a + 0x2B1) = 1;
    Dog080E3874_Arm(a, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);

    if (*(u16 *)(a + 0x2D4) & 0x80) {
        Dog080E3874_Step(a + 0x460, *(void **)(a + 0x41C), 0xC);
        out = *(struct ContactPosition **)(a + 0x5D4);
        Dog080E3874_Sample(out, *(u16 **)(a + 0x450));
        if (gEngineFlags & 0x4800) {
            q = *(u8 **)(a + 0xDC);
            if (q == 0)
                goto fallback;
            state = q[8];
            if (state > 3)
                state = -1;
            if (state < 0)
                goto fallback;
            cell = *(u16 *)(*(u8 **)(gPlayers[state] + 0x40) + 6);
        } else {
            u16 *p = *(u16 **)(a + 0xDC);
            state = 0;
            cell = Dog080E3874_CellIndex(p[6], p[8]);
        }
        if (cell != *(u16 *)(a + 0x2F4)) {
            if (!(*(u32 *)(gPlayers[state] + 0x20) & 0x20000))
                goto navigate;
        }
        if (*(u16 *)(a + 0x2D4) & 0x400)
            return;
        *(u16 *)(a + 0x414) &= ~4;
        if (*(u16 *)(a + 0x2D4) & 0x80) {
            Dog080E3874_Arm(a, *(void **)(a + 0x41C), 0x1A);
            *(u8 *)(a + 0x2B0) = 0;
            *(u8 *)(a + 0x2B5) = 0x1A;
            *(u8 *)(a + 0x2AA) = 0xC;
            *(u32 *)(a + 0x2AC) = 0;
            *(u8 *)(a + 0x2B1) = 1;
            goto tail;
        }
        Dog080E3874_Arm(a, *(void **)(a + 0x228), 1);
        *(u8 *)(a + 0x2A8) = 0;
        *(u8 *)(a + 0x2A9) = 0;
        *(u8 *)(a + 0x2AA) = 5;
        *(u32 *)(a + 0x2AC) = 0;
        *(u8 *)(a + 0x2B1) = 1;
        out = *(struct ContactPosition **)(a + 0x5D4);
        Dog080E3874_Sample(out, *(u16 **)(a + 0x450));
        out = *(struct ContactPosition **)(a + 0x5D4);
        out->pad = 0;
        goto tail;
    }
    /* Block B: an entry step with a target, or a short idle step. */
    if (*(u32 *)(a + 0x2CC) & 0x10) {
        Dog080E3874_Step(a + 0x460, *(void **)(a + 0x228), 7);
        out = *(struct ContactPosition **)(a + 0x5D4);
        Dog080E3874_Sample(out, *(u16 **)(a + 0x450));
        res = (s32)Nav_IsPositionInMesh(*(void **)(a + 0x5D0), (struct ContactPosition *)(a + 0x5C),
                                        out);
        if (!res) {
            *(u16 *)(a + 0x2D4) &= ~0x200;
        } else {
            res = Terrain_ApplyBlocked(*(void **)(a + 0x5D0), (struct ContactPosition *)(a + 0x5C),
                                       out);
            if (res == 0)
                *(u16 *)(a + 0x2D4) &= ~0x200;
        }
        if (res) {
            (*(struct ContactPosition **)(a + 0x5D4))->pad = 1;
            return;
        }
        *(u8 *)(a + 0x2A8) = 3;
        *(u8 *)(a + 0x2A9) = 0;
        *(u8 *)(a + 0x2AA) = 0;
        *(u32 *)(a + 0x2AC) = 0;
        *(u8 *)(a + 0x2B1) = 1;
        Dog080E3874_Arm(a, (void *)SceneActorControl_OnIdle, 3);
        return;
    }
    Dog080E3874_Step(a + 0x460, *(void **)(a + 0x228), 2);
    *(u16 *)(a + 0x2C2) = 0x40;
    return;

navigate:
    cx = Mod(cell, *(s16 *)(*(u8 **)(gTerrainMap + 4) + 4));
    cz = Div(cell, *(s16 *)(*(u8 **)(gTerrainMap + 4) + 4));
    target.x = (cx << 8) + 0x80;
    target.z = (cz << 8) + 0x80;
    target.y = (Dog080E3874_CellData(cell)[0] & 0xF) << 8;
    if (!Nav_IsPositionInMesh(*(void **)(a + 0x5D0), (struct ContactPosition *)(a + 0x5C),
                              &target)) {
        *(u16 *)(a + 0x2D4) &= ~0x200;
        res = 0;
    } else {
        res = Terrain_ApplyBlocked(*(void **)(a + 0x5D0), (struct ContactPosition *)(a + 0x5C),
                                   &target);
        if (res == 0) {
            *(u16 *)(a + 0x2D4) &= ~0x200;
        } else if (res == 2) {
            res = 1;
        } else {
            Memory_Clear(a + 0x64, 8);
            res = 1;
        }
    }
    if (res)
        return;
    goto fallback;

tail:
    if (*(u16 *)(a + 0x2D4) & 0x400 || *(u16 *)(a + 0x414) & 4) {
        sub_0807E334(actor);
        *(u16 *)(a + 0x2C2) = 0x20;
        return;
    }
    Dog080E3874_Turn(a);
    sub_0807E334(actor);
    *(u16 *)(a + 0x2C2) = 0x20;
    return;

fallback:
    Callback_InvokeActor(actor, *(void **)(a + 0x224));
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080E3874);
#endif
