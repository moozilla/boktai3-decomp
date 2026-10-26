#include "global.h"
#include "scene_actor_control_phase.h"

// MATCHING_CONCESSION: register-pinning

extern u8 *gUnk_02000580[];
extern u32 gUnk_02000560;
extern u16 gUnk_03005260[];
void SceneActorControl_OnIdle(void);

struct GhostTriggerContext {
    u8 unknown0[0x5C0];
    u16 script;
    u8 unknown5C2[2];
    u32 range;
};
static inline u32 Bits(u32 mask, u32 *p)
{
    return *p & mask;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    u32 zero = 0;
    actor->field2B2 = 1;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = zero;
}
// confidence=low: while the player faces the Ghost within range and presses the button, runs its script and idles; otherwise counts up.
void Ghost_UpdateFacingTrigger(struct SceneActorRegistryResetView *actor)
{
    struct GhostTriggerContext *context = actor->context;
    u8 zero8;
    u32 one;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        SetControl(actor, actor->savedTarget, 0);
    {
        u32 mask = 0x800;
        actor->flags2C0 = mask | actor->flags2C0;
    }
    zero8 = 0;
    actor->direction = actor->fieldE9;
    if (!({
            u32 buttons = gUnk_03005260[1];
            register u32 m asm("r0");
            one = 1;
            m = 1;
            m &= buttons;
            m;
        }))
        goto wait;
    if (gUnk_02000580[0]) {
        u32 zero;
        s32 own = ((actor->direction + 0x20) & 0xFF) >> 5;
        s32 player = ((gUnk_02000580[0][0x2D] + 0x20) & 0xFF) >> 5;
        s32 diff = ((own + 4) & 7) - player;
        if (diff < 0)
            diff = -diff;
        if (diff <= 1 && actor->distanceF0 < context->range && !(gEngineFlags & 0x200) &&
            !(zero = Bits(4, &gUnk_02000560))) {
            u32 two = 2;
            actor->state2A8 = two;
            actor->phase2A9 = zero8;
            actor->behavior2AA = zero8;
            actor->counter2AC = zero;
            actor->field2B1 = one;
            SetControl(actor, SceneActorControl_OnIdle, 0x10);
            Script_Call(context->script, 0);
            return;
        }
    }
wait:
    actor->counter2AC++;
}
