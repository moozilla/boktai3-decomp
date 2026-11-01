#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "ram.h"
// confidence=low: animation-three state eases stored displacement into motion until animation completion.

void SparkParticle_Spawn(void *, u32, void *, u32, u32, u32, u32, u32);
struct GhoulAnimation {
    u8 pad[7];
    u8 duration;
    u16 index;
    u8 padA[4];
    u16 elapsed;
};
struct GhoulDraw {
    u8 pad[0x48];
    struct GhoulAnimation anim;
};
static inline u32 MotionFlag(struct SceneActorRegistryResetView *a, u32 mask)
{
    return a->flags2D4 & mask;
}
static inline u32 RandomSample(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)(index + (u32)RANDOM_TABLE_ADDR);
    v &= 3;
    return v + 1;
}
static inline s32 Quarter(s32 value)
{
    s32 result;
    if (value >= 0)
        result = value >> 2;
    else
        result = -((-value) >> 2);
    return result;
}
static inline u8 Stopped(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
void Ghoul_UpdateDisplacementRecovery(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct GhoulAnimation *anim = &((struct GhoulDraw *)actor->drawable)->anim;
    struct ContactPackedVector range;
    s16 *x, *z;
    s32 dx, dz;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        Sound_PlaySong(0xD4);
        if (MotionFlag(actor, 8))
            Sound_PlaySong(0x12A);
        actor->field2B3 = 0;
    }
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
    if (frame <= 17 && anim->index == 0 && anim->elapsed == (anim->duration != 0) &&
        anim->duration != 1)
        anim->elapsed = 0;
    if (Mod(frame, 12) == 0 && MotionFlag(actor, 8)) {
        range.x = 60;
        range.y = 60;
        range.z = 40;
        SparkParticle_Spawn(&actor->body.position, RandomSample(), &range,
                            actor->body.direction + 128, 10, 6, 2, 2);
    }
    {
        s32 current = *(s16 *)((u8 *)actor + 0x310);
        s16 *field = (void *)((u8 *)actor + 0x318);
        s32 difference = current - *field;
        x = field;
        dx = Quarter(difference);
    }
    {
        s32 current = *(s16 *)((u8 *)actor + 0x314);
        s16 *field = (void *)((u8 *)actor + 0x31C);
        s32 difference = current - *field;
        z = field;
        dz = Quarter(difference);
    }
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->body.motion.x += dx;
    actor->body.motion.z += dz;
    *x += dx;
    *z += dz;
    if (Stopped(dx, dz) && actor->field2B3)
        actor->field2B0 = 1;
}
