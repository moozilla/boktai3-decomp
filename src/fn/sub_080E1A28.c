#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
/* Dog_LeapTowardTarget: on entry turns the leap velocity toward the context's target position (snapping to it within 16, else turning 16 at a time) and plays sound 0x3A4; halves the velocity while the motion's bit 0 is set, and hands over to the landing reaction (kind 0x24) when flags2D0 & 0x10 (provisional name) */
void Dog_UpdateAnimationSixteenReaction(void);
s32 Math_Atan2(s32, s32);
extern const s16 gUnk_086149C4[];

static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline s32 Half(s32 n)
{
    if (n >= 0)
        return n >> 1;
    return -((-n) >> 1);
}
static inline void Polar(struct ContactPosition *out, u32 angle, s32 speed)
{
    out->x = Scale12(gUnk_086149C4[(angle + 0x40) & 0xFF] * speed);
    out->y = 0;
    out->z = Scale12(gUnk_086149C4[angle & 0xFF] * speed);
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *from,
                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

void Dog_LeapTowardTarget(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    struct ContactPosition velocity;
    if (TakeFlag(&actor->field2B2)) {
        s32 angle, facing, turn;
        context = actor->context;
        Delta(&velocity, &actor->body.position, (struct ContactPosition *)(context + 0x6D8));
        angle = Math_Atan2(velocity.x, velocity.z);
        facing = ((actor->body.direction + 0x20) & 0xFF) >> 6 << 6;
        turn = angle - facing;
        if (turn > 0x80)
            turn -= 0x100;
        else if (turn < -0x80)
            turn += 0x100;
        if ((turn < 0 ? -turn : turn) <= 0xF)
            Polar(&velocity, angle, 24);
        else if (turn > 0)
            Polar(&velocity, (facing + 0x10) & 0xFF, 24);
        else
            Polar(&velocity, (facing + 0xEF) & 0xFF, 24);
        actor->secondaryMotion.x = velocity.x;
        actor->secondaryMotion.y = 0x20;
        actor->secondaryMotion.z = velocity.z;
        Sound_PlaySong(0x3A4);
    }
    Collider_Refresh(context + 0x624);
    if (**(u8 **)((u8 *)actor + 0x6C) & 1) {
        actor->secondaryMotion.x = Half(actor->secondaryMotion.x);
        actor->secondaryMotion.z = Half(actor->secondaryMotion.z);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
    {
        u32 mask = 0x10;
        if (actor->flags2D0 & mask) {
            void *callback = (void *)Dog_UpdateAnimationSixteenReaction;
            u32 kind = 0x24;
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
    }
}
