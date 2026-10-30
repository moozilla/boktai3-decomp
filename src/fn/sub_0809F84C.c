// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_control_phase.h"
#include "player.h"

struct CrowDrawableView {
    u32 flags;
    u16 unknown4;
    u8 direction;
};
struct CrowPlayerView {
    u8 pad[0x30];
    struct ContactPosition position;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);

struct CrowSteeringState {
    u8 unknown0[0x3DA];
    u16 approachTime;
    u8 speed, turnDelay;
    u8 unknown3DE[2];
    u32 orbitRange;
    u8 approachTurns;
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
static inline s32 Minimum(s32 n, s32 min, s32 *out)
{
    *out = n;
    n = n > 0 ? n : min;
    return n;
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
// confidence=low: distance and angle gates suggest approach and orbit flight steering.
void Crow_SteerAroundPlayer(struct SceneActorRegistryResetView *actor)
{
    struct CrowSteeringState *state = actor->context;
    struct ContactPosition delta;
    struct ContactPosition velocity;
    s32 angle;
    s32 turn;
    s32 step;
    s16 t1;
    s16 t2;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        ((struct CrowDrawableView *)actor->drawable)->flags |= 2;
        SetControl(actor, actor->entryTarget224, 2);
    }
    if (state->approachTime < ((s32)actor->counter2AC)) {
        Delta(&delta, &actor->body.position, &(*(struct CrowPlayerView **)0x02000580)->position);
        t1 = delta.x;
        t2 = delta.z;
        angle = Math_Atan2(t1, t2);
        turn = Normalize(angle - actor->body.direction);
        if (turn < 0)
            turn = -turn;
        if (turn <= 11) {
            state->approachTurns = 0;
            ((struct CrowDrawableView *)actor->drawable)->flags &= ~2;
            SetState(actor, 0, 2, 0);
            return;
        }
        step = Minimum(Scale(turn, 2), 4, &step);
        if (step > 8)
            step = 8;
        if (state->approachTurns > 20) {
            state->approachTurns = 0;
            state->turnDelay = 0;
            SetState(actor, 0, 0, 4);
            return;
        } else {
            state->approachTurns++;
            state->turnDelay = 0;
        }
    } else if (actor->tracking.distance <= 0x7CFF) {
        SetState(actor, 0, 0, 4);
        return;
    } else if (state->orbitRange > actor->tracking.distance) {
        Delta(&delta, &(*(struct CrowPlayerView **)0x02000580)->position, &actor->body.position);
        angle = Math_Atan2(delta.x, delta.z);
        turn = Magnitude(Normalize(((angle + 64) & 255) - actor->body.direction));
        if (turn <= 63)
            angle += 64;
        else
            angle += 192;
        angle &= 255;
        step = Div(state->speed, 8);
        if (step <= 0)
            step = 2;
        goto clamp;
    } else {
        Delta(&delta, &actor->body.position, &(*(struct CrowPlayerView **)0x02000580)->position);
        angle = Math_Atan2(delta.x, delta.z);
        turn = Normalize(angle - actor->body.direction);
        if (turn < 0)
            turn = -turn;
        if (turn <= 31) {
            SetState(actor, 0, 0, 4);
            return;
        }
        step = Minimum(Scale(turn, 3), 2, &step);
    clamp:
        if (step > 6)
            step = 6;
    }
    Polar(&velocity, actor->body.direction, state->speed);
    actor->body.motion = velocity;
    if (!state->turnDelay) {
        s32 difference = Normalize(angle - actor->body.direction);
        asm("" : "+r"(difference) : "r"(angle));
        if (difference > 0)
            actor->body.direction += step;
        else
            actor->body.direction -= step;
        ((struct CrowDrawableView *)actor->drawable)->direction = 0x9F - actor->body.direction;
        if (actor->paletteOffset == 0)
            state->turnDelay = 4;
        else
            state->turnDelay = 2;
    }
    if (state->turnDelay)
        state->turnDelay--;
    actor->counter2AC++;
}
