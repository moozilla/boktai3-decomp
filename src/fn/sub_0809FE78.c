#include "global.h"
#include "scene_actor_control_phase.h"
#include "player.h"

extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Callback_InvokeActor(void *, void *);

struct CrowApproachState {
    u8 unknown0[0x3DC];
    u8 speed;
    u8 unknown3DD[3];
    u32 range;
};
static inline s32 Normalize(s32 n)
{
    if (n > 128)
        n -= 256;
    else if (n < -128)
        n += 256;
    return n;
}
static inline s32 Magnitude(s32 n)
{
    if (n < 0)
        n = -n;
    return n;
}
static inline s32 Scale(s32 n, u32 bits)
{
    if (n >= 0)
        return n >> bits;
    return -((-n) >> bits);
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    out->y = 0;
    out->z = Scale(gUnk_086149C4[angle & 255] * speed, 12);
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
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
}
void Crow_ApproachPlayer(struct SceneActorRegistryResetView *actor)
{
    struct CrowApproachState *state = actor->context;
    struct ContactPosition delta, velocity;
    u8 **pp;
    s32 limit;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        *(u32 *)actor->drawable &= ~2;
        SetControl(actor, actor->entryTarget228, 1);
    }
    limit = actor->paletteOffset == 1 ? 0x18 : 0x28;
    if ((s32)actor->counter2AC > limit) {
        if (state->range > actor->distanceF0) {
            SetState(actor, 0, 0, 1);
            return;
        }
        pp = &gPlayers;
        Delta(&delta, &actor->position, (struct ContactPosition *)(*pp + 0x30));
        if (Magnitude(Normalize(Math_Atan2(delta.x, delta.z) - actor->direction)) > 0x40) {
            SetState(actor, 0, 0, 1);
            return;
        }
    }
    Polar(&velocity, actor->direction, state->speed);
    actor->motion = velocity;
    actor->counter2AC++;
}
