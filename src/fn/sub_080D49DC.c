#include "global.h"

#include "scene_actor_reset.h"
#include "engine.h"
#include "ram.h"

s32 SceneStimulus_FindOverlap(s32, s32, u32, u32, u32);
u32 SceneActorContacts_ReleaseNode(void *);

void SceneActorAttachment_RetargetNearestPlayer(void *, void *);

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.

extern u16 gUnknown_02000580[];

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.
s32 Mod(s32, s32);
s32 Div(s32, s32);
void sub_0807E334(void *);
void SceneActorControl_OnIdle(void);
void SceneActorReaction_RestoreAfterDelay(void *, s32);
s32 Nav_IsPositionInMesh(void *, void *, void *);
s32 Terrain_ApplyBlocked(void *, void *, void *);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.
static inline s32 sub080D49DC_Cell(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline void sub080D49DC_Clip(struct ContactPosition *st, s32 cell)
{
    u8 *p;
    u8 b;
    s32 v;

    p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        p += 4;
    else
        p = gTerrainMap->cells + (cell * 4 + 12);
    b = *p;
    v = (b & 0xF) << 8;
    switch (b >> 4) {
        case 1:
            v -= *((u8 *)st + 4);
            break;
        case 2:
            v -= *((u8 *)st + 0);
            break;
    }
    ((u16 *)st)[1] = v;
}

static inline s32 sub080D49DC_Gate(struct SceneActorRegistryResetView *actor, void *route,
                                   void *pos, void *target)
{
    s32 res;

    if (!Nav_IsPositionInMesh(route, pos, target)) {
        actor->flags2D4 &= ~0x200;
        return 0;
    }
    res = Terrain_ApplyBlocked(route, pos, target);
    if (res == 0) {
        actor->flags2D4 &= ~0x200;
        return 0;
    }
    if (res != 2)
        Memory_Clear(&actor->body.motion, 8);
    return 1;
}

#ifdef NONMATCHING
// fuzzy: 26.1%
// scope shape 917 (differences once low registers are set aside)
// confidence=low: per-frame terrain-route follower; probes map cells, gates on route checks and fires the restore callbacks.
void sub_080D49DC(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRegistryControlGroup *group;
    struct SceneActorReactionMotion *motion;
    struct ContactPosition *state;
    struct ContactPosition tmp;
    u16 *src;
    s32 r, cell, cellA, cx, cy, idx, n, dz, mx;
    u32 val, t;
    u8 kind;
    void *p;
    void *route;
    u8 dir;
    u8 *g8c;

    group = &actor->group;
    motion = (struct SceneActorReactionMotion *)((u8 *)actor + 0xDC);
    if (actor->field2B1)
        actor->field2B1 = 0;
    if (Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    if (actor->counter2AC <= 60) {
        actor->counter2AC++;
        return;
    }
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
    actor->state2A8 = 3;
    actor->phase2A9 = 1;
    actor->behavior2AA = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = 14;
    actor->target = (void *)SceneActorReaction_RestoreAfterDelay;
    actor->field2C8 = 0;
    actor->flags114 &= -2;
    actor->word104 = 0;

    if (actor->flags2D4 & 0x80) {
        actor->flags2CC |= 0x80;
        state = (struct ContactPosition *)actor->state5D4;
        src = *(u16 **)((u8 *)actor + 0x448 + 8);
        ((u16 *)state)[0] = src[0];
        ((u16 *)state)[2] = src[1];
        cx = (s8)(src[0] >> 8);
        cy = (s8)(src[1] >> 8);
        cell = sub080D49DC_Cell(cx, cy);
        sub080D49DC_Clip(state, cell);

        if (gEngineFlags & 0x4800) {
            p = motion->fieldDC;
            if (!p)
                goto L734;
            kind = *((u8 *)p + 8);
            idx = -1;
            if (kind <= 3)
                idx = kind;
            if (idx < 0)
                goto L734;
            cellA =
                *(u16 *)(*(u8 **)((u8 *)(void *)*(u32 *)(gUnknown_02000580 + idx * 2) + 0x40) + 6);
        } else {
            p = motion->fieldDC;
            idx = 0;
            cellA = sub080D49DC_Cell((s8)(*(u16 *)((u8 *)p + 0xC) >> 8),
                                     (s8)(*(u16 *)((u8 *)p + 0x10) >> 8));
        }
        if (cellA != actor->terrainCell &&
            !(*(u32 *)((u8 *)(void *)*(u32 *)(gUnknown_02000580 + idx * 2) + 0x20) & 0x20000))
            goto L67C;
        if (actor->flags2D4 & 0x400) {
            r = 0;
            goto L74E;
        }

        group->flags414 &= ~5;
        if (actor->flags2D4 & 0x80) {
            actor->field2B2 = 1;
            actor->field2B0 = 0;
            actor->field2B5 = 26;
            actor->target = group->entryControl41C;
            actor->field2C8 = (u16)cellA;
            actor->flags114 &= -2;
            actor->word104 = cellA;
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = 12;
            actor->counter2AC = cellA;
            actor->field2B1 = 1;
        } else {
            actor->field2B2 = 1;
            actor->field2B0 = 0;
            actor->field2B5 = 1;
            actor->target = *(void **)((u8 *)actor + 0x228);
            actor->field2C8 = 0;
            actor->flags114 &= -2;
            actor->word104 = 0;
            actor->state2A8 = 5;
            actor->phase2A9 = 0;
            actor->behavior2AA = 5;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        }
        goto L4DA;
    } else {
        actor->flags2CC |= 0x10;
        g8c = (u8 *)group + 0x8C;
        *(u32 *)(g8c + 0xC) = 0x1E;
        *(void **)(g8c + 0x10) = *(void **)((u8 *)actor + 0x228);
        g8c[8] = 0;
        g8c[9] = 0;
        g8c[0xA] = 7;
        state = (struct ContactPosition *)actor->state5D4;
        src = *(u16 **)((u8 *)actor + 0x448 + 8);
        ((u16 *)state)[0] = src[0];
        ((u16 *)state)[2] = src[1];
        cx = (s8)(src[0] >> 8);
        cy = (s8)(src[1] >> 8);
        cell = sub080D49DC_Cell(cx, cy);
        sub080D49DC_Clip(state, cell);
        route = actor->routeObject;
        r = 0;
        if (Nav_IsPositionInMesh(route, &actor->body.position, state)) {
            {
                s32 res = Terrain_ApplyBlocked(route, &actor->body.position, state);
                if (res == 0) {
                    actor->flags2D4 &= ~0x200;
                } else {
                    if (res != 2)
                        Memory_Clear(&actor->body.motion, 8);
                    r = 1;
                }
            }
        } else {
            actor->flags2D4 &= ~0x200;
        }
        if (r) {
            ((u16 *)state)[3] = 1;
            return;
        }
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = 0;
        actor->target = (void *)SceneActorControl_OnIdle;
        actor->field2C8 = 0;
        actor->flags114 &= -2;
        actor->word104 = 0;
        actor->state2A8 = 3;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        return;
    }

L4DA:
    dir = actor->body.direction;
    if (actor->flags2D4 & 0x400)
        goto L640;
    if (group->flags414 & 4)
        goto L640;
    /* 0x526: pick a palette-table slot that differs from dir (up to 4 tries) */
    n = 0;
    do {
        t = (gRandomIndex + 1) & 0x3FF;
        gRandomIndex = t;
        val = (gRandomTable[t] & 3) << 6;
        if (val != dir)
            break;
        n++;
    } while (n <= 3);
    if (n > 3)
        val = dir;
    *((u8 *)actor + 0x5D8) = (u8)val;
    actor->field2B5 = 27;
    actor->state2A8 = 3;
    actor->phase2A9 = 1;
    actor->behavior2AA = 0;
    *((u8 *)actor + 0x5D9) = dir;
    *((u8 *)actor + 0x5DA) = 6;
    actor->field2B2 = 1;
    actor->target = *(void **)((u8 *)actor + 0x5F0);
    actor->field2C8 = 0;
    actor->flags114 &= -2;
    actor->word104 = 0;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
    g8c = (u8 *)group + 0x8C;
    *(void **)(g8c + 0x10) = actor->target;
    g8c[7] = actor->field2B5;
    g8c[4] = actor->state2A8;
    g8c[5] = actor->phase2A9;
    g8c[6] = actor->behavior2AA;

L640:
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
    r = 0;
    goto L74E;

L67C:
    mx = Mod(cellA, *(s16 *)(gTerrainMap->cells + 4));
    dz = Div(cellA, *(s16 *)(gTerrainMap->cells + 4));
    tmp.x = (mx << 8) + 0x80;
    p = TerrainModifiers_FindCellOverride(cellA, 1);
    if (p)
        tmp.y = (*((u8 *)p + 4) & 0xF) << 8;
    else
        tmp.y = (*(gTerrainMap->cells + (cellA * 4 + 12)) & 0xF) << 8;
    tmp.z = (dz << 8) + 0x80;
    route = actor->routeObject;
    r = 0;
    if (Nav_IsPositionInMesh(route, &actor->body.position, &tmp)) {
        s32 res = Terrain_ApplyBlocked(route, &actor->body.position, &tmp);
        if (res == 0) {
            actor->flags2D4 &= ~0x200;
        } else {
            if (res != 2)
                Memory_Clear(&actor->body.motion, 8);
            r = 1;
        }
    } else {
        actor->flags2D4 &= ~0x200;
    }
    if (r == 0) {
        Callback_InvokeActor(actor, *(void **)((u8 *)group + 0x224));
        r = 0;
    }
    goto L74E;

L734:
    Callback_InvokeActor(actor, *(void **)((u8 *)group + 0x224));
    r = 0;

L74E:
    if (r)
        goto L7D0;
    route = actor->routeObject;
    r = 0;
    if (Nav_IsPositionInMesh(route, &actor->body.position, (u8 *)motion->fieldDC + 0xC)) {
        s32 res = Terrain_ApplyBlocked(route, &actor->body.position, (u8 *)motion->fieldDC + 0xC);
        if (res == 0) {
            actor->flags2D4 &= ~0x200;
        } else {
            if (res != 2)
                Memory_Clear(&actor->body.motion, 8);
            r = 1;
        }
    } else {
        actor->flags2D4 &= ~0x200;
    }
    if (r == 0) {
        Callback_InvokeActor(actor, *(void **)((u8 *)group + 0x224));
        return;
    }

L7D0:
    group->flags414 |= 4;
    g8c = (u8 *)group + 0x8C;
    *(u32 *)(g8c + 0xC) = 0x1E;
    *(void **)(g8c + 0x10) = group->entryControl41C;
    g8c[8] = 0;
    g8c[9] = 0;
    g8c[0xA] = 12;
    return;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080D49DC);
#endif
