#define RouteActor_SetDestination Lun_BeginMoveToPosition
#define gPlayerSlots              gUnk_02000580
#include "global.h"
#include "scene_actor_reset.h"
void sub_0809B6AC(void);
void Lun_BeginMoveToPosition(void *, void *);
extern u8 *gUnk_02000580[4];
struct RouteActorDestinationState {
    u8 unknown0[0x442];
    u16 delayDuration, delay;
};
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u16 SetFlag(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 *flags = &actor->flags2D4;
    return *flags = mask | *flags;
}
void RouteActor_UpdatePlayerDestinationPhase(struct SceneActorRegistryResetView *actor)
{
    if (TakeFlag(&actor->field2B1)) {
        struct RouteActorDestinationState *state = actor->context;
        void *callback = sub_0809B6AC;
        u32 kind = 10;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
        SetFlag(actor, 0x100);
        state->delay = state->delayDuration;
    }
    if (actor->field2B0)
        Lun_BeginMoveToPosition(actor, gUnk_02000580[0] + 0x30);
    else
        actor->counter2AC++;
}
