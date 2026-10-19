#include "global.h"
/* Dog_ChaseAfterKnockback: decays the knockback push (a quarter of the remaining offset, clamped to 200) into the motion; once settled, either chases player 0 (flags2D4 & 0x80, kind 0x1A) or routes back toward it with the entry target (kind 1), arriving or rerouting through the route point (provisional name) */
#define Dog_ChaseAfterKnockback sub_080E0750
#include "scene_actor_chase.h"

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

static inline s32 Quarter(s32 n)
{
    s32 r;
    if (n >= 0)
        r = n >> 2;
    else
        r = -((-n) >> 2);
    return r;
}

static inline u16 LoadMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 value = actor->flags2D4;
    return value & mask;
}
static inline u8 TestField(u16 *field, u32 mask)
{
    if (*field & mask)
        return 1;
    return 0;
}

static inline void Arrive_BeginTurn(void *obj, u8 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&((struct SceneActorRegistryResetView *)obj)->group;
    if (!LoadMotionFlags((struct SceneActorRegistryResetView *)obj, 0x400) &&
        !TestField((u16 *)((u8 *)obj + 0x414), 4)) {
        s32 prior = actor->direction;
        s32 tries;
        u8 choice;
        for (tries = 0; tries <= 3; tries++) {
            choice = Chase_RandomBits(3) << 6;
            if (choice != prior)
                goto chosen;
        }
        choice = prior;
    chosen:
        group[0x204] = choice;
        Chase_SavePacket(group + 0x8C, actor);
        group[0x205] = saved;
        Chase_SetCtl(actor, *(void **)(group + 0x21C), 0x1B);
        Chase_SetState(actor, 3, 1, 0);
        group[0x206] = 6;
    }
}

static inline u8 Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    s32 player;
    Chase_SnapToRoute(*(struct ContactPosition **)((u8 *)actor + 0x5D4), (u8 *)actor + 0x448);
    if (Chase_Test(gUnk_030053F4, 0x4800)) {
        struct ChaseTarget *t = *slot;
        if (!t)
            goto invoke;
        {
            u32 index = t->index;
            player = -1;
            if (index <= 3)
                player = index;
        }
        if (player < 0)
            goto invoke;
        cell = gUnk_02000580[player]->cell[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell && !Chase_Bits32(&gUnk_02000580[player]->flags, 0x20000))
        goto reroute;
    if (Chase_MotionFlags(actor, 0x400))
        goto done;
    ((struct ChaseFlags40 *)(group + 0x40))->bits &= ~4;
    if (Chase_MotionFlags(actor, 0x80)) {
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
    } else {
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 5);
        Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
        (*(struct ContactPosition **)(group + 0x200))->y =
            Chase_HeightAt(Chase_CellAt(*(struct ContactPosition **)(group + 0x200)),
                           *(struct ContactPosition **)(group + 0x200));
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    }
    {
        u8 saved = actor->direction;
        Arrive_BeginTurn(obj, saved);
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
    if (Chase_TryRoute(obj, *(void **)(group + 0x1FC), &actor->position, targetPointer))
        return 1;
}
invoke:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
done:
    return 0;
}

static inline u8 Settled(s32 dx, s32 dz)
{
    if (dx == 0 && dz == 0)
        return 1;
    return 0;
}

void sub_080E0750(struct SceneActorRegistryResetView *actor)
{
    u8 *group;
    s16 *pushX, *pushZ;
    s32 dx, dz;
    u16 *flags;
    u16 value;
    if (TakeFlag(&actor->field2B2)) {
        actor->secondaryMotion.x = Quarter(actor->secondaryMotion.x);
        actor->secondaryMotion.z = Quarter(actor->secondaryMotion.z);
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        {
            u32 bits = 0x20;
            bits |= actor->flags2D4;
            bits &= ~0x100;
            actor->flags2D4 = bits;
        }
    }
    {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
    }
    {
        s32 diffX, diffZ;
        diffX = *(s16 *)((u8 *)actor + 0x310) - *(s16 *)((u8 *)actor + 0x318);
        pushX = (s16 *)((u8 *)actor + 0x318);
        dx = Quarter(diffX);
        diffZ = *(s16 *)((u8 *)actor + 0x314) - *(s16 *)((u8 *)actor + 0x31C);
        pushZ = (s16 *)((u8 *)actor + 0x31C);
        dz = Quarter(diffZ);
    }
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->motion.x += dx;
    actor->motion.z += dz;
    *pushX += dx;
    *pushZ += dz;
    if (!Settled(dx, dz))
        goto done;
    if (!actor->field2B3)
        goto done;
    group = (u8 *)&actor->group;
    actor->field2B0 = 1;
    flags = &actor->flags2D4;
    {
        s32 mask = ~0x20;
        value = mask & *flags;
        *flags = value;
    }
    if (*(u32 *)((u8 *)actor + 0x2DC) & 0x80000) {
        actor->flags2D6 |= 2;
        return;
    }
    if ((u16)(value & 0x80)) {
        {
            u32 mask = 0x80;
            actor->flags2CC |= mask;
        }
        actor->fieldDC = (u32)((u8 *)gUnk_02000580[0] + 0x24);
        if (!Arrive(actor) && !Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                                              &((struct ChaseTarget *)actor->fieldDC)->position))
            goto fail;
        ((struct ChaseFlags40 *)(group + 0x40))->bits |= 4;
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
        return;
    }
    actor->fieldDC = (u32)((u8 *)gUnk_02000580[0] + 0x24);
    {
        u32 mask = 0x10;
        actor->flags2CC |= mask;
    }
    (*(struct ContactPosition **)((u8 *)actor + 0x5D4))->pad = 0;
    if (!Arrive(actor) && !Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                                          &((struct ChaseTarget *)actor->fieldDC)->position)) {
    fail:
        Callback_InvokeActor(actor, *(void **)(group + 0x224));
        return;
    }
    Chase_SetCtl(actor, actor->entryTarget228, 1);
    Chase_SetBehavior(actor, 7);
    actor->field2C2 = 0x10;
    return;
done: {
    u32 mask = 0x2000;
    actor->flags2CC |= mask;
}
}
