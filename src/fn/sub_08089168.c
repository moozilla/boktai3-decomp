#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
// confidence=low: animation-three state eases stored displacement into motion until animation completion.

void SparkParticle_Spawn(void *, u32, void *, u32, u32, u32, u32, u32);
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
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
    register u32 offset asm("r2") = (u32) & ((struct SceneActorRegistryResetView *)0)->flags2D4;
    asm("" : : "r"(offset));
    {
        register u16 *flags asm("r0") = (void *)((u8 *)a + offset);
        return *flags & mask;
    }
}
static inline u32 RandomSample(void)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    u32 v;
    gUnk_03005308 = index;
    index *= 2;
    {
        register const u16 *table asm("r3") = gUnk_0203B400;
        {
            const u16 *entry = (const u16 *)(index + (u32)table);
            v = *entry;
        }
    }
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
static inline s32 AxisDelta(struct SceneActorRegistryResetView *actor, u32 offset, s16 **saved)
{
    s32 source;
    s16 *current;
    register s32 value asm("r0");
    if (offset == 0x310) {
        register u32 sourceOffset asm("r5") = offset;
        asm("" : "+r"(sourceOffset));
        {
            register s16 *address asm("r0") = (void *)((u8 *)actor + sourceOffset);
            source = *address;
        }
    } else {
        u32 sourceOffset = offset;
        {
            s16 *address = (void *)((u8 *)actor + sourceOffset);
            source = *address;
        }
    }
    current = (void *)((u8 *)actor + (offset + 8));
    value = *current;
    value = source - value;
    asm("" : "+r"(value));
    *saved = current;
    return Quarter(value);
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
        SparkParticle_Spawn(&actor->position, RandomSample(), &range, actor->direction + 128, 10, 6,
                            2, 2);
    }
    dx = AxisDelta(actor, 0x310, &x);
    dz = AxisDelta(actor, 0x314, &z);
    if (dx > 200)
        dx = 200;
    else if (dx < -200)
        dx = -200;
    if (dz > 200)
        dz = 200;
    else if (dz < -200)
        dz = -200;
    actor->motion.x += dx;
    actor->motion.z += dz;
    *x += dx;
    *z += dz;
    if (Stopped(dx, dz) && actor->field2B3) {
        register u32 offset asm("r2") = (u32) & ((struct SceneActorRegistryResetView *)0)->field2B0;
        asm("" : : "r"(offset));
        {
            register u8 *state asm("r1") = (void *)((u8 *)actor + offset);
            *state = 1;
        }
    }
}
