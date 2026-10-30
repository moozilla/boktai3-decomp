#include "scene_actor_control_motion.h"
extern const s16 gUnk_086149C4[];
struct BeeFlightState {
    u8 unknown0[0x3D4];
    u32 flags3D4;
    u8 unknown3D8[2];
    u16 previousGround;
    u8 unknown3DC[0x432 - 0x3DC];
    u8 phase;
};
struct BeeFlightView {
    u8 unknown0[0x2BE];
    u16 ground;
};
static inline u32 ActorFlag(struct SceneActorRegistryResetView *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}
static inline u8 IsUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline s32 TruncateSine(s32 value, u32 shift)
{
    return value >= 0 ? value >> shift : -((-value) >> shift);
}
void Bee_UpdateFlightHeight(struct SceneActorRegistryResetView *actor)
{
    s32 delta, absolute;
    u16 *g;
    struct BeeFlightState *state;
    {
        struct BeeFlightState *raw = (void *)SceneActorControl_GetState(actor);
        state = raw;
    }

    if (IsUnavailable(actor))
        return;
    g = &((struct BeeFlightView *)actor)->ground;
    {
        u32 diff = *g - state->previousGround;
        actor->field2EA -= diff;
        state->previousGround = *g;
    }
    if (!ActorFlag(actor, 0x100000, 0x2D0)) {
        actor->body.position.y = *g + actor->field2EA;
        return;
    }
    if ((s16)actor->field2EA <= 259) {
        {
            u32 mask = 1;
            if (Test(*(volatile u32 *)&state->flags3D4, mask)) {
                actor->body.position.y = ((struct BeeFlightView *)actor)->ground;
                return;
            }
        }
        actor->field2EA += TruncateSine(gUnk_086149C4[state->phase], 9);
        if (state->phase <= 63)
            state->phase++;
        actor->body.position.y = ((struct BeeFlightView *)actor)->ground + actor->field2EA;
    }
    {
        s32 currentGround = ((struct BeeFlightView *)actor)->ground;
        s32 raisedY = actor->body.position.y - 260;
        delta = currentGround - raisedY;
    }
    absolute = delta >= 0 ? delta : -delta;
    if (absolute > 16) {
        if (delta < 0)
            actor->field2EA -= 8;
        else
            actor->field2EA += 8;
    }
    actor->body.position.y = ((struct BeeFlightView *)actor)->ground + actor->field2EA;
    {
        struct SceneActorRegistrySequenceDrawable *drawable = actor->drawable;
        drawable->position.y -= TruncateSine(gUnk_086149C4[state->phase], 5);
    }
    if (state->phase > 64)
        state->phase = 0;
    else
        state->phase += 8;
}
