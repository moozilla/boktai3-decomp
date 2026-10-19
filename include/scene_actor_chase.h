#ifndef GUARD_SCENE_ACTOR_CHASE_H
#define GUARD_SCENE_ACTOR_CHASE_H
/* Chase-the-player step shared by the 080BBDD0..080C12FC behaviour states (inlined about
 * nine times): route the destination back to the route point, follow the chased object or
 * player to its terrain cell, and when it is reached pick a new facing and save the
 * current control. Draft; see the NONMATCHING units that include it. */
#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"

void Skeleton_UpdateScriptWaitState(void);
void sub_0807E334(struct SceneActorRegistryResetView *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
s32 Math_Atan2(s32, s32);
extern u32 gUnk_030053F4;

struct SceneActorChaseRoutePoint {
    s16 x, z;
};
struct SceneActorChaseRouteLink {
    u8 unknown0[8];
    struct SceneActorChaseRoutePoint *point;
};
struct SceneActorChaseSavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};
struct SceneActorChasePlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0x1C];
    struct {
        u8 unknown0[6];
        u16 cell;
    } *body;
};
extern struct SceneActorChasePlayer *gUnk_02000580[4];

static inline u8 SceneActorChase_TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SceneActorChase_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                              void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline u32 SceneActorChase_CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 SceneActorChase_CellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}
static inline u8 SceneActorChase_IsScenePaused(void)
{
    if (gUnk_030053F4 & 0x4800)
        return 1;
    return 0;
}
static inline s32 SceneActorChase_PlayerIndex(u8 *object)
{
    s32 index = -1;
    if (object[8] <= 3)
        index = object[8];
    return index;
}
static inline u16 SceneActorChase_TestFlags2D4(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u8 SceneActorChase_HasFlag414(struct SceneActorRegistryResetView *actor)
{
    if (*(u16 *)((u8 *)actor + 0x414) & 4)
        return 1;
    return 0;
}
static inline s32 SceneActorChase_Scale16(s32 n)
{
    return n >= 0 ? n >> 16 : -((-n) >> 16);
}

static inline void SceneActorChase_StartTurn(struct SceneActorRegistryResetView *input,
                                             u8 *inputGroup)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *group = inputGroup;
    u8 direction = actor->direction;
    if (!(actor->flags2D4 & 0x400) && !SceneActorChase_HasFlag414(actor)) {
        u8 facing = actor->direction;
        s32 i;
        u32 next = facing;
        for (i = 0; i <= 3; i++) {
            u32 r;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3FF;
            r = (gUnk_0203B400[gUnk_03005308] & 3) << 6;
            if (r != facing) {
                next = r;
                break;
            }
        }
        *(u8 *)(group + 0x204) = next;
        {
            struct SceneActorChaseSavedControl *saved =
                (struct SceneActorChaseSavedControl *)(group + 0x8C);
            saved->target = actor->target;
            saved->kind = actor->field2B5;
            saved->state = actor->state2A8;
            saved->phase = actor->phase2A9;
            saved->behavior = actor->behavior2AA;
        }
        *(u8 *)(group + 0x205) = direction;
        SceneActorChase_SetControl(actor, 0x1B, *(void **)(group + 0x21C));
        actor->state2A8 = 3;
        actor->phase2A9 = 1;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        *(u8 *)(group + 0x206) = 6;
    }
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
}

/* Step the mover toward a point; clears flags2D4 bit 0x200 when it cannot. */
static inline u8 SceneActorChase_MoveToward(struct SceneActorRegistryResetView *actor, void *mover,
                                            struct ContactPosition *to)
{
    s32 result;
    if (!sub_0821F204(mover, &actor->position, to)) {
        actor->flags2D4 &= ~0x200;
        result = 0;
    } else {
        result = sub_0821F7A8(mover, &actor->position, to);
        if (result == 0) {
            actor->flags2D4 &= ~0x200;
            result = 0;
        } else {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            result = 1;
        }
    }
    return result;
}

static inline u8 SceneActorChase_Pursue(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *group = (u8 *)&actor->group;
    u8 **chased = (u8 **)&actor->fieldDC;
    struct ContactPosition *dest;
    u32 cell;
    s32 index;

    dest = *(struct ContactPosition **)(group + 0x200);
    {
        struct SceneActorChaseRouteLink *link = (struct SceneActorChaseRouteLink *)(group + 0x74);
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = SceneActorChase_CellHeight(dest, SceneActorChase_CellIndex(dest));
    }
    if (SceneActorChase_IsScenePaused()) {
        u8 *object = *chased;
        if (!object || (index = SceneActorChase_PlayerIndex(object)) < 0) {
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return 0;
        }
        cell = gUnk_02000580[index]->body->cell;
    } else {
        cell = SceneActorChase_CellIndex((struct ContactPosition *)(*chased + 0xC));
        index = 0;
    }
    if (cell == actor->terrainCell || (gUnk_02000580[index]->flags & 0x20000)) {
        if (SceneActorChase_TestFlags2D4(actor, 0x400))
            return 0;
        {
            u16 *flags = (u16 *)(group + 0x40);
            s32 mask = ~4;
            *flags &= mask;
        }
        if (SceneActorChase_TestFlags2D4(actor, 0x80)) {
            SceneActorChase_SetControl(actor, 0x1A, *(void **)(group + 0x48));
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = 0xC;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        } else {
            SceneActorChase_SetControl(actor, 1, actor->entryTarget228);
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = 5;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            dest = *(struct ContactPosition **)(group + 0x200);
            {
                struct SceneActorChaseRouteLink *link =
                    (struct SceneActorChaseRouteLink *)(group + 0x74);
                dest->x = link->point->x;
                dest->z = link->point->z;
                dest->y = SceneActorChase_CellHeight(dest, SceneActorChase_CellIndex(dest));
            }
            (*(struct ContactPosition **)(group + 0x200))->y = SceneActorChase_CellHeight(
                *(struct ContactPosition **)(group + 0x200),
                SceneActorChase_CellIndex(*(struct ContactPosition **)(group + 0x200)));
            (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        }
        SceneActorChase_StartTurn(actor, (u8 *)&actor->group);
    } else {
        struct ContactPosition center;
        SceneActorTerrain_GetCenter(cell, &center);
        if (SceneActorChase_MoveToward(actor, *(void **)(group + 0x1FC), &center))
            return 1;
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
    }
    return 0;
}

#endif
