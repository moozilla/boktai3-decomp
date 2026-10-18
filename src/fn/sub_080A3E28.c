#include "scene_actor_reset.h"
struct PlacementTetherState {
    u8 unknown0[0x62C];
    u16 flags;
    u16 timer;
};
s32 TetherActor_GetParameter(void);
u32 TetherActor_IsStateOne(void);
void SceneActorPlacement_TriggerTetherAction(struct SceneActorRegistryResetView *);
static inline u8 HasPendingAction(struct PlacementTetherState *p)
{
    u32 mask = 4;
    if (mask & p->flags)
        return 1;
    return 0;
}
void SceneActorPlacement_UpdateTetherTimer(struct SceneActorRegistryResetView *p)
{
    struct PlacementTetherState *s;
    if (p->paletteOffset == 1)
        return;
    s = p->context;
    if (TetherActor_GetParameter() != -1 || !TetherActor_IsStateOne()) {
        s->timer = 300;
        {
            u16 *flags = &s->flags;
            s32 mask = -5;
            mask &= *flags;
            *flags = mask;
        }
    } else if (s->timer) {
        s->timer--;
    } else {
        s->timer = 300;
        if (!HasPendingAction(s))
            SceneActorPlacement_TriggerTetherAction(p);
    }
}
