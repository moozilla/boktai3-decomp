// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 86.8%
// fix.py best version (reprinted C)
// Remaining differences: turn-loop allocation, PRE spills, and contact flag addressing.
// confidence=low: selects and pursues a contact or restores the delayed response.
#define Mummy_ProcessContactPursuit sub_080D7568
#include "scene_actor_chase.h"
#include "scene_actor_contacts.h"
#include "ram.h"
s32 Mummy_CanPassMovementThreshold(void *);
u8 Mummy_TryDirectPursuit(void *);
void SceneActorTarget_ResetContactResponse(void *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
struct MummyContactState {
    struct ChaseTarget *target;
    u8 unknown[11];
    u8 flags;
};
static inline u8 TestContext(u8 *context, u32 mask)
{
    if (*(u16 *)(context + 0x6F2) & mask)
        return 1;
    return 0;
}
static inline u8 Test16(u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline u8 Test8(u8 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline void Mummy_BeginTurn(void *obj, u8 saved)
{
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
    if (!Chase_MotionFlags(actor, 0x400) && !Chase_Test(*(u16 *)((u8 *)actor + 0x414), 4)) {
        register s32 prior asm("r7") = actor->body.direction;
        s32 tries = 0;
        s32 choice;
        for (;;) {
            const u16 *table = gRandomTable;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            choice = (table[gRandomIndex] & 3) << 6;
            if (choice != prior)
                break;
            if (++tries > 3) {
                choice = prior;
                break;
            }
        }
        group[0x204] = choice;
        Chase_SavePacket(group + 0x8C, actor);
        group[0x205] = saved;
        Chase_SetCtl(actor, *(void **)(group + 0x21C), 0x1B);
        Chase_SetState(actor, 3, 1, 0);
        group[0x206] = 6;
    }
}

static inline u8 Mummy_Arrive(void *obj)
{
    struct ContactPosition target;
    struct SceneActorRegistryResetView *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    s32 player;
    Chase_SnapToRoute(*(struct ContactPosition **)((u8 *)actor + 0x5D4), (u8 *)actor + 0x448);
    if (Chase_Test(gEngineFlags, 0x4800)) {
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
        cell = gPlayers[player]->cell[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->terrainCell && !Chase_Bits32(&gPlayers[player]->flags, 0x20000))
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
        u8 saved = actor->body.direction;
        Mummy_BeginTurn(obj, saved);
    }
    sub_0807E334(actor);
    actor->field2C2 = 0x20;
    goto done;
reroute: {
    struct ContactPosition *targetPointer = &target;
    s32 x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
    s32 z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
    u8 *data;
    targetPointer->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    targetPointer->y = (*data & 15) << 8;
    targetPointer->z = (z << 8) + 0x80;
    if (Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->body.position, targetPointer))
        return 1;
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
}
done:
    return 0;
}

static inline u32 MotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}
u8 Mummy_ProcessContactPursuit(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    u8 *group = (u8 *)(&actor->group.common);
    struct ChaseTarget **target;
    struct SceneActorContactNode *contact;
    s16 cell;
    if (MotionFlags(actor, 0x10))
        return 0;
    if ((u8)Callback_InvokeActor(actor, actor->update26C))
        return 1;
    if (MotionFlags(actor, 0x100))
        return 0;
    if (!MotionFlags(actor, 0x200))
        return 0;
    if (!Chase_Bits32(&actor->flags2D0, 0x1000000))
        return 0;
    if ((*((u32 *)(group + 0x64))) != gTrackedPlayerId[0])
        return 0;
    target = (void *)(&actor->tracking.target);
    ((struct MummyContactState *)target)->flags |= 2;
    if (!actor->tracking.limit)
        goto restore;
    contact = SceneActorContacts_SelectForActor((void *)actor, 2);
    if (!contact)
        goto restore;
    *target = contact->object;
    cell = *((u16 *)((*((u8 **)(((u8 *)(*target)) + 0x1C))) + 6));
    if (*((u32 *)(((u8 *)actor) + 0x100))) {
        if (Test16(&actor->flags2C0, 2))
            *target = (void *)(((u8 *)gPlayers[0]) + 0x24);
        else if (!(*target))
            goto threshold;
        {
            s32 x = (*target)->position.x - actor->body.position.x;
            s32 z = (*target)->position.z - actor->body.position.z;
            actor->tracking.heading = Math_Atan2(x, z);
            actor->tracking.distance = (x * x) + (z * z);
        }
    threshold:
        if (!((u8)Mummy_CanPassMovementThreshold(actor))) {
            if (TestContext(context, 0x1400))
                return 0;
            Mummy_Arrive(actor);
            return 0;
        }

        if (!Mummy_TryDirectPursuit(actor))
            return 0;
        if (Test8(&((struct MummyContactState *)target)->flags, 8)) {
            if ((*((u16 *)(((u8 *)actor) + 0x2F8))) != cell) {
                Chase_TryRoute(actor, *((void **)(group + 0x1FC)), &actor->body.position,
                               &(*target)->position);
                *((u16 *)(((u8 *)actor) + 0x2F8)) = cell;
            }
        } else
            SceneActorTarget_ResetContactResponse(actor);
        actor->flags2D6 |= 0x80;
        ((struct MummyContactState *)target)->flags |= 0x10;
        return 1;
    }
    if ((actor->flags2CC & 0x80) && (!MotionFlags(actor, 0x400)))
        goto restore;
    if (TestContext(context, 0x1400))
        goto restore;
    if (actor->field2C2 != 64)
        return 0;
    if ((*((u32 *)(group + 0x64))) != gTrackedPlayerId[0])
        return 0;
    actor->flags2D6 |= 0x100;
restore:
    if (TestContext(context, 0x4000)) {
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 1, actor->category);
        *((u16 *)(context + 0x6F2)) &= ~0x4000;
    }

    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080D7568);
#endif
