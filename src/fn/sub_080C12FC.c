#include "global.h"
#ifdef NONMATCHING
// fuzzy: 80.2%
// Golem/skeleton reaction dispatcher (returns 1 when it changed control): hit (0x2000) turns
// round and chases, 0xC0000 hands over to reactionTarget230, otherwise roar-and-chase through
// three copies of Chase_Arrive (include/scene_actor_chase.h); 080C23FC is its far tail.
#include "scene_actor_chase.h"

void Sound_PlaySong(u32);
void SceneActorScript_TriggerCallOnCount(struct SceneActorRegistryResetView *);

static inline u8 StartRoar(struct SceneActorRegistryResetView *actor)
{
    u8 *roar = (u8 *)actor + 0x5CA;
    if (*roar == 0 && (*(u16 *)((u8 *)actor + 0x2E6) & 8)) {
        actor->field2B8 = 0x2D;
        *roar = 0x41;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        return 1;
    }
    return 0;
}
static inline void ChasePlayer(struct SceneActorRegistryResetView *actor)
{
    actor->fieldDC = (u32)((u8 *)gUnk_02000580[0] + 0x24);
}

#define Golem_ReactToContact sub_080C12FC
// confidence=low: contact flags select chase, roar, or a reaction control.
u8 Golem_ReactToContact(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    u8 *group = (u8 *)&actor->group;
    u16 flags;

    if (!(actor->field2D8 & 0x20000)) {
        if (*(u16 *)((u8 *)actor + 0x2E4) == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0x13E);
    }
    SceneActorScript_TriggerCallOnCount(actor);
    flags = actor->flags2D4;
    if (flags & 0x2000) {
        actor->direction = *((u8 *)actor + 0x2B6) + 0x80;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        actor->flags2D4 &= ~0x2020;
        actor->flags2CC |= 0x10;
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
        ChasePlayer(actor);
        if (!Chase_Arrive(actor)) {
            *(u16 *)(body + 0x6F0) = 0;
            if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                                &((struct ChaseTarget *)actor->fieldDC)->position)) {
                Callback_InvokeActor(actor, *(void **)(group + 0x224));
                return 0;
            }
        }
        Chase_SetCtl(actor, actor->entryTarget228, 1);
        Chase_SetBehavior(actor, 7);
        actor->field2C2 = Div(0x40, 3);
        return 1;
    }
    if (flags & 0x8002) {
        actor->flags2D4 = flags & ~0x20;
        return 0;
    }
    if (*(u32 *)((u8 *)actor + 0x2DC) & 0xC0000) {
        Chase_SetCtl(actor, actor->reactionTarget230, 0x12);
        Chase_SetState(actor, 3, 0, 0);
        actor->flags2D0 &= ~0x1000000;
        return 1;
    }
    flags &= ~0x20;
    actor->flags2D4 = flags;
    if (flags & 0x100)
        return 0;
    if (actor->field2B8)
        return 0;
    if (actor->flagsEB & 0x10)
        return 0;
    if (actor->flags2D4 & 0x80) {
        if (StartRoar(actor))
            Sound_PlaySong(0x3A7);
        if (!actor->fieldDC)
            return 0;
        actor->flags2CC |= 0x80;
        *(u16 *)(group + 0x40) |= 4;
        ChasePlayer(actor);
        if (!Chase_Arrive(actor)) {
            *(u16 *)(body + 0x6F0) = 0;
            if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                                &((struct ChaseTarget *)actor->fieldDC)->position)) {
                Callback_InvokeActor(actor, *(void **)(group + 0x224));
                return 0;
            }
        }
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
        actor->field2C2 = 0x20;
        return 1;
    }
    if (StartRoar(actor))
        Sound_PlaySong(0x3A7);
    if (!actor->fieldDC)
        return 0;
    ChasePlayer(actor);
    actor->flags2CC |= 0x10;
    (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    if (!Chase_Arrive(actor)) {
        *(u16 *)(body + 0x6F0) = 0;
        if (!Chase_TryRoute(actor, *(void **)(group + 0x1FC), &actor->position,
                            &((struct ChaseTarget *)actor->fieldDC)->position)) {
            Callback_InvokeActor(actor, *(void **)(group + 0x224));
            return 0;
        }
    }
    Chase_SetCtl(actor, actor->entryTarget228, 1);
    Chase_SetBehavior(actor, 7);
    actor->field2C2 = 0x20;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C12FC);
#endif
