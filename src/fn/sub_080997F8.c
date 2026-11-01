#include "global.h"
#include "scene_actor_control_phase.h"
#include "input.h"
#include "gba/io_reg.h"
#include "player.h"

extern u32 gUnk_02000560;
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
static inline u8 Pressed(u16 mask, u32 keys)
{
    return mask & keys;
}
#define TEST_PRESSED_BUTTON(result, buttons, bit)                                                  \
    do {                                                                                           \
        u32 mask = (bit);                                                                          \
        (result) = Pressed(mask, (buttons));                                                       \
    } while (0)

void Ghost_UpdateFacingTrigger(struct SceneActorRegistryResetView *actor)
{
    struct GhostTriggerContext *context = actor->context;
    u32 one;
    if (SceneActorControl_TakeFlag(&actor->field2B1))
        SetControl(actor, actor->savedTarget, 0);
    {
        u32 mask = 0x800;
        actor->flags2C0 = mask | actor->flags2C0;
    }
    actor->body.direction = actor->tracking.heading;
    {
        u32 buttons = gPadState[1];
        u32 pressed;
        one = 1;
        TEST_PRESSED_BUTTON(pressed, buttons, A_BUTTON);
        if (!pressed)
            goto wait;
    }
    if (((u8 **)gPlayers)[0]) {
        u32 zero;
        u16 phaseReset[1] = {0};
        s32 own = ((actor->body.direction + 0x20) & 0xFF) >> 5;
        s32 player = ((((u8 **)gPlayers)[0][0x2D] + 0x20) & 0xFF) >> 5;
        s32 diff = ((own + 4) & 7) - player;
        if (diff < 0)
            diff = -diff;
        if (diff <= 1 && actor->tracking.distance < context->range && !(gEngineFlags & 0x200) &&
            !(zero = Bits(4, &gUnk_02000560))) {
            u32 two = 2;
            actor->state2A8 = two;
            actor->phase2A9 = phaseReset[0];
            actor->behavior2AA = phaseReset[0];
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
