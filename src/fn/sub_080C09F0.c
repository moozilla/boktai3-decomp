#include "global.h"
#include "scene_actor_terrain.h"
#include "scene_actor_reaction.h"

#ifdef NONMATCHING
// fuzzy: 81.2%
/* Skeleton chase step (tentative): steers toward or away from the target at +0xDC by the
 * context's near/far distances; when the route flag is set it snaps to the route, re-targets the
 * player's cell and may start a random turn. Still differs (40 instructions, ignoring
 * registers): which registers the RNG loop's hoisted constants get (the ROM also hoists the
 * table address), the exhausted-retry `choice = prior` copy, the mask-before-load order of
 * the group+0x40 bitfield clear, and the scale-pointer copies that keep the two 0x40 stores apart. */
void Skeleton_UpdateScriptWaitState(void);
void sub_0807E334(void *);
s32 Mod(s32, s32);
s32 Div(s32, s32);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
extern u32 gUnk_03005308;

struct ChasePlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0x1C];
    u16 *cell;
};
extern struct ChasePlayer *gUnk_02000580[];

struct ChaseTarget {
    u8 unknown0[8];
    u8 index;
    u8 unknown9[3];
    struct ContactPosition position;
};

struct ChaseContext {
    u8 unknown0[0x6DC];
    u32 flags;
    u8 unknown6E0[0x28];
    u32 nearDistance;
    u32 farDistance;
};

static inline u8 TakeArrived(struct SceneActorRegistryResetView *actor)
{
    if (actor->field2B1) {
        actor->field2B1 = 0;
        return 1;
    }
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
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
    u32 behavior = b;
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetBehavior(struct SceneActorRegistryResetView *actor, u32 b)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u16 MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
static inline u32 Bits32(u32 *field, u32 mask)
{
    return *field & mask;
}
static inline s32 CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline s32 CellAt(struct ContactPosition *position)
{
    return CellIndex(position->x >> 8, position->z >> 8);
}
static inline u32 HeightAt(s32 cell, struct ContactPosition *position)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    u32 kind, height;
    if (data)
        data += 4;
    else
        data = gUnk_030052F4->cells + (cell * 4 + 12);
    kind = *data >> 4;
    height = (*data & 15) << 8;
    if (kind == 1)
        goto slopeZ;
    if (kind == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *(u8 *)&position->z;
    goto done;
slopeX:
    height -= *(u8 *)&position->x;
done:
    return height;
}
static inline u32 Height(struct ContactPosition *position)
{
    return HeightAt(CellAt(position), position);
}
static inline void SnapToRoute(struct ContactPosition *p, u8 *route)
{
    p->x = (*(u16 **)(route + 8))[0];
    p->z = (*(u16 **)(route + 8))[1];
    p->y = Height(p);
}
static inline void ClearMotionFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u32 mask = ~bits;
    actor->flags2D4 &= mask;
}
static inline void ClearBits(u16 *field, u32 bits)
{
    *field &= ~bits;
}
static inline u8 TryRoute(struct SceneActorRegistryResetView *actor, void *route,
                          struct ContactPosition *position, struct ContactPosition *target)
{
    s32 result = sub_0821F204(route, position, target);
    if (!result) {
        ClearBits(&actor->flags2D4, 0x200);
        return 0;
    }
    result = sub_0821F7A8(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->motion, 8);
        return 1;
    }
    ClearBits(&actor->flags2D4, 0x200);
    return 0;
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline s32 Scale16(s32 n)
{
    return n >= 0 ? n >> 16 : -((-n) >> 16);
}

struct ChaseFlags40 {
    s16 bits : 16;
};

struct ChasePacket {
    void *control;
    u8 state, phase, behavior, kind;
};
static inline void SavePacket(void *p, struct SceneActorRegistryResetView *actor)
{
    struct ChasePacket *packet = p;
    packet->control = actor->target;
    packet->kind = actor->field2B5;
    packet->state = actor->state2A8;
    packet->phase = actor->phase2A9;
    packet->behavior = actor->behavior2AA;
}
static inline u32 RandomBits(u32 mask)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return gUnk_0203B400[index] & mask;
}
static inline void BeginTurn(void *obj, u8 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    if (!MotionFlags(actor, 0x400) && !Test(*(u16 *)((u8 *)actor + 0x414), 4)) {
        s32 prior = actor->direction;
        s32 tries = 0;
        s32 choice;
        do {
            choice = RandomBits(3) << 6;
            if (choice != prior)
                goto chosen;
        } while (++tries <= 3);
        choice = prior;
    chosen:
        group[0x204] = choice;
        SavePacket(group + 0x8C, actor);
        group[0x205] = saved;
        SetCtl(actor, *(void **)(group + 0x21C), 0x1B);
        SetState(actor, 3, 1, 0);
        group[0x206] = 6;
    }
}

static inline void Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    s32 player;
    SnapToRoute(*(struct ContactPosition **)((u8 *)actor + 0x5D4), (u8 *)actor + 0x448);
    if (Test(gUnk_030053F4, 0x4800)) {
        struct ChaseTarget *t = *slot;
        if (!t)
            goto invoke;
        {
            u32 index = t->index;
            player = -1;
            if (index <= 3)
                player = index;
        }
        if (player < 0) {
        invoke:
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            goto done;
        }
        cell = gUnk_02000580[player]->cell[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell && !Bits32(&gUnk_02000580[player]->flags, 0x20000))
        goto reroute;
    if (MotionFlags(actor, 0x400))
        goto done;
    ((struct ChaseFlags40 *)(group + 0x40))->bits &= ~4;
    if (MotionFlags(actor, 0x80)) {
        SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        SetBehavior(actor, 12);
    } else {
        SetCtl(actor, actor->entryTarget228, 1);
        SetBehavior(actor, 5);
        SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
        (*(struct ContactPosition **)(group + 0x200))->y =
            HeightAt(CellAt(*(struct ContactPosition **)(group + 0x200)),
                     *(struct ContactPosition **)(group + 0x200));
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    }
    {
        u8 saved = actor->direction;
        BeginTurn(obj, saved);
    }
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
    goto done;
reroute: {
    struct ContactPosition *targetPointer = &target;
    s32 x = Mod(cell, *(s16 *)(gUnk_030052F4->cells + 4));
    s32 z = Div(cell, *(s16 *)(gUnk_030052F4->cells + 4));
    u8 *data;
    targetPointer->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gUnk_030052F4->cells + (cell * 4 + 12);
    targetPointer->y = (*data & 15) << 8;
    targetPointer->z = (z << 8) + 0x80;
    if (!TryRoute(actor, *(void **)(group + 0x1FC), &actor->position, targetPointer))
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
done:;
}

void sub_080C09F0(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition vec;
    struct ChaseContext *context = actor->context;
    u8 *group = (u8 *)&actor->group;
    if (TakeArrived(actor))
        SetCtl(actor, (void *)Skeleton_UpdateScriptWaitState, 0x26);
    if (**(u8 **)((u8 *)actor + 0x6C) & 1) {
        SetBehavior(actor, 12);
        SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Arrive(actor);
        {
            struct ChaseContext *flagged = actor->context;
            u32 mask = 0x80;
            flagged->flags |= mask;
        }
        return;
    }
    {
        u32 angle;
        s32 speed;
        Delta(&vec, &actor->position, &((struct ChaseTarget *)actor->fieldDC)->position);
        angle = Math_Atan2(vec.x, vec.z);
        if (actor->distanceF0 < context->nearDistance) {
            angle = (angle + 0x80) & 0xFF;
            actor->field2C2 = 0x20;
        } else if (actor->distanceF0 > context->farDistance) {
            actor->field2C2 = 0x20;
            actor->direction = angle;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
        speed = Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x260));
        {
            u32 mask = 0xA02F;
            u16 *scale;
            if (actor->flags2D4 & mask) {
                actor->field2C2 = 0x40;
                scale = &actor->field2C2;
            } else {
                scale = &actor->field2C2;
                if (*scale == 0)
                    *scale = 0x40;
            }
            if (actor->drawableKind == 0)
                Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
            else
                Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
        }
        actor->motion.x += Scale16(gUnk_086149C4[(angle + 64) & 255] * speed);
        actor->motion.z += Scale16(gUnk_086149C4[angle & 255] * speed);
        {
            u32 mask = 0x2000;
            actor->flags2CC |= mask;
        }
        if (actor->distanceF0 > context->nearDistance && actor->distanceF0 < context->farDistance) {
            u16 mask = 0x80;
            actor->flags2D6 |= mask;
        } else
            actor->counter2AC++;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C09F0);
#endif
