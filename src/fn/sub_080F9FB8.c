#include "scene_actor_control_phase.h"
#include "player.h"
s32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void Axe_EmitMotionTrail(void *);
extern const s16 gUnk_086149C4[];
static inline s32 Normalize(s32 a)
{
    s32 r = a;
    if (a > 128)
        r -= 256;
    else if (a < -128)
        r += 256;
    return r;
}
static inline s32 Scale(s32 x)
{
    s32 r;
    if (x >= 0)
        r = x >> 12;
    else
        r = -(-x >> 12);
    return r;
}
static inline void Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
static inline s32 Abs(s32 x)
{
    s32 result = x;
    if (result < 0)
        result = -result;
    return result;
}
static inline s32 Eighth(s32 x)
{
    s32 result;
    if (x >= 0)
        result = x >> 3;
    else
        result = -(-x >> 3);
    return result;
}
static inline void FromPlayer(u8 **player, struct ContactPosition *out,
                              struct SceneActorRegistryResetView *owner)
{
    const struct ContactPosition *target = (void *)(*player + 0x30);
    const struct ContactPosition *actor = &owner->position;
    out->x = actor->x - target->x;
    out->y = actor->y - target->y;
    out->z = actor->z - target->z;
}
static inline void ToPlayer(u8 **player, struct ContactPosition *out,
                            const struct ContactPosition *actor)
{
    const struct ContactPosition *target = (void *)(*player + 0x30);
    out->x = target->x - actor->x;
    out->y = target->y - actor->y;
    out->z = target->z - actor->z;
}
struct PursuitContext {
    u8 unknown0[0xE56];
    u16 distance;
};
void Axe_UpdatePursuit(struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition delta;
    struct ContactPosition velocity;
    struct PursuitContext *context = actor->context;
    s32 angle, step;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = actor->entryTarget228;
        u32 kind = 1;
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
        *(u32 *)actor->drawable |= 2;
        context->distance = 512;
    }
    Axe_EmitMotionTrail(actor);
    if (context->distance > actor->distanceF0) {
        s32 difference;
        FromPlayer(&gPlayers, &delta, actor);
        angle = Math_Atan2(delta.x, delta.z);
        difference = Normalize(((angle + 64) & 255) - actor->direction);
        if (difference < 0)
            difference = -difference;
        if (difference <= 63)
            angle += 64;
        else
            angle += 192;
        angle &= 255;
        step = Div(32, 8);
        if (step <= 0)
            step = 2;
    } else {
        s32 difference, magnitude;
        ToPlayer(&gPlayers, &delta, &actor->position);
        angle = Math_Atan2(delta.x, delta.z);
        difference = Normalize(angle - actor->direction);
        magnitude = Abs(difference);
        {
            s32 result = Eighth(magnitude);
            step = result;
            if (result <= 0)
                result = 2;
            step = result;
        }
    }
    if (Normalize(angle - actor->direction) > 0)
        actor->direction += step;
    else
        actor->direction -= step;
    Polar(&velocity, actor->direction, 32);
    *((u8 *)actor->drawable + 6) = 159 - actor->direction;
    actor->motion = velocity;
    actor->counter2AC++;
}
