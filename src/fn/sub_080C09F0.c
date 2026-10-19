#include "global.h"
#ifdef NONMATCHING
// fuzzy: 55.5%
// Golem chase/turn behaviour. Layout follows the ROM: the chase and turn blocks are inlines
// whose parameter copies keep their hoisted addresses apart. Remaining: register pressure
// (mine spills the outer actor and the group pointer; ROM keeps r6/r7/sb/sl) and the turn
// loop's exhausted path, where the ROM copies facing into r0 explicitly.
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"

void Skeleton_UpdateScriptWaitState(void);
void sub_0807E334(struct SceneActorRegistryResetView *);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
s32 Math_Atan2(s32, s32);
extern u32 gUnk_030053F4;

struct GolemRoutePoint {
    s16 x, z;
};
struct GolemRouteLink {
    u8 unknown0[8];
    struct GolemRoutePoint *point;
};
struct GolemSavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};
struct GolemPlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0x1C];
    struct {
        u8 unknown0[6];
        u16 cell;
    } *body;
};
extern struct GolemPlayer *gUnk_02000580[4];

static inline u8 TakeFlag2B1(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *control)
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
static inline u32 CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u32 CellHeight(struct ContactPosition *position, u32 cell)
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
static inline u8 IsScenePaused(void)
{
    if (gUnk_030053F4 & 0x4800)
        return 1;
    return 0;
}
static inline s32 PlayerIndex(u8 *object)
{
    s32 index = -1;
    if (object[8] <= 3)
        index = object[8];
    return index;
}
static inline u8 HasFlag414(struct SceneActorRegistryResetView *actor)
{
    if (*(u16 *)((u8 *)actor + 0x414) & 4)
        return 1;
    return 0;
}
static inline s32 Scale16(s32 n)
{
    return n >= 0 ? n >> 16 : -((-n) >> 16);
}

static inline void Golem_StartTurn(struct SceneActorRegistryResetView *input, u8 *inputGroup)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *group = inputGroup;
    u8 direction = actor->direction;
    if (!(actor->flags2D4 & 0x400) && !HasFlag414(actor)) {
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
            struct GolemSavedControl *saved = (struct GolemSavedControl *)(group + 0x8C);
            saved->target = actor->target;
            saved->kind = actor->field2B5;
            saved->state = actor->state2A8;
            saved->phase = actor->phase2A9;
            saved->behavior = actor->behavior2AA;
        }
        *(u8 *)(group + 0x205) = direction;
        SetControl(actor, 0x1B, *(void **)(group + 0x21C));
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

static inline void Golem_Chase(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *group = (u8 *)&actor->group;
    struct ContactPosition *dest;
    u32 cell;
    s32 index;

    dest = *(struct ContactPosition **)(group + 0x200);
    {
        struct GolemRouteLink *link = (struct GolemRouteLink *)(group + 0x74);
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = CellHeight(dest, CellIndex(dest));
    }
    if (IsScenePaused()) {
        u8 *object = (u8 *)actor->fieldDC;
        if (!object || (index = PlayerIndex(object)) < 0) {
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return;
        }
        cell = gUnk_02000580[index]->body->cell;
    } else {
        cell = CellIndex((struct ContactPosition *)((u8 *)actor->fieldDC + 0xC));
        index = 0;
    }
    if (cell == actor->terrainCell || (gUnk_02000580[index]->flags & 0x20000)) {
        if (actor->flags2D4 & 0x400)
            return;
        *(u16 *)(group + 0x40) &= ~4;
        if (actor->flags2D4 & 0x80) {
            SetControl(actor, 0x1A, *(void **)(group + 0x48));
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = 0xC;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        } else {
            SetControl(actor, 1, actor->entryTarget228);
            actor->state2A8 = 0;
            actor->phase2A9 = 0;
            actor->behavior2AA = 5;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            dest = *(struct ContactPosition **)(group + 0x200);
            {
                struct GolemRouteLink *link = (struct GolemRouteLink *)(group + 0x74);
                dest->x = link->point->x;
                dest->z = link->point->z;
                dest->y = CellHeight(dest, CellIndex(dest));
            }
            (*(struct ContactPosition **)(group + 0x200))->y =
                CellHeight(*(struct ContactPosition **)(group + 0x200),
                           CellIndex(*(struct ContactPosition **)(group + 0x200)));
            (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        }
        Golem_StartTurn(actor, group);
    } else {
        struct ContactPosition center;
        void *mover;
        s32 result;
        SceneActorTerrain_GetCenter(cell, &center);
        mover = *(void **)(group + 0x1FC);
        if (!sub_0821F204(mover, &actor->position, &center)) {
            actor->flags2D4 &= ~0x200;
            result = 0;
        } else {
            result = sub_0821F7A8(mover, &actor->position, &center);
            if (result == 0) {
                actor->flags2D4 &= ~0x200;
                result = 0;
            } else {
                if (result != 2)
                    Memory_Clear(&actor->motion, 8);
                result = 1;
            }
        }
        if (!result)
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
    }
}

void sub_080C09F0(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    u8 *group = (u8 *)&actor->group;

    if (TakeFlag2B1(actor)) {
        void *target = Skeleton_UpdateScriptWaitState;
        SetControl(actor, 0x26, target);
    }
    if (**(u8 **)((u8 *)actor + 0x6C) & 1) {
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0xC;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        SetControl(actor, 0x1A, *(void **)(group + 0x48));
        Golem_Chase(actor);
        *(u32 *)((u8 *)actor->context + 0x6DC) |= 0x80;
    } else {
        struct ContactPosition delta;
        u32 angle;
        s32 speed;
        SceneActorTerrain_Subtract(&delta, (struct ContactPosition *)((u8 *)actor->fieldDC + 0xC),
                                   &actor->position);
        angle = Math_Atan2(delta.x, delta.z);
        if (actor->distanceF0 < *(u32 *)(body + 0x708)) {
            angle = (angle + 0x80) & 0xFF;
            actor->field2C2 = 0x20;
        } else if (actor->distanceF0 > *(u32 *)(body + 0x70C)) {
            actor->field2C2 = 0x20;
            actor->direction = angle;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
        if (actor->flags2D4 & 0xA02F)
            actor->field2C2 = 0x40;
        else if (actor->field2C2 == 0)
            actor->field2C2 = 0x40;
        if (actor->drawableKind == 0)
            Anim_SetDurationScale((u8 *)actor->drawable + 0x48, actor->field2C2);
        else
            Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, actor->field2C2);
        actor->motion.x += Scale16(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
        actor->motion.z += Scale16(gUnk_086149C4[angle & 0xFF] * speed);
        actor->flags2CC |= 0x2000;
        if (actor->distanceF0 > *(u32 *)(body + 0x708) &&
            actor->distanceF0 < *(u32 *)(body + 0x70C))
            actor->flags2D6 |= 0x80;
        else
            actor->counter2AC++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C09F0);
#endif
