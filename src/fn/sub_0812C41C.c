// MATCHING_CONCESSION: register-pinning
#include "scene_steering_actor.h"
#include "engine.h"
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void SceneMovingParticle_Create(struct ContactPosition *, struct ContactPosition *, s32, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void FragmentEffect_SpawnRandomParticle(struct ContactPosition *input)
{
    struct ContactPosition *position = input;
    /* The original materializes this dead literal across its inlined RNG.
     * Keep the defined value live only for register-allocation matching. */
    register u32 opaque asm("r12") = 0x0403b714;
    const u16 *table = gUnk_0203B400;
    struct ContactPosition velocity;
    struct ContactPosition *v;
    s32 angle;
    s32 magnitude;
    register u32 *cursor asm("r3");
    u32 mask;
    asm volatile("" : "+r"(opaque));
    {
        register u32 index asm("r0") = ((cursor = &gRandomIndex), *cursor + 1) & 1023;
        const u8 *sample = (void *)(index * 2 + (u32)table);
        mask = 255;
        angle = *sample;
        index = (index + 1) & 1023;
        *cursor = index;
        {
            register s32 raw asm("r0") = *(const u16 *)(index * 2 + (u32)table);
            s32 bitmask = 31;
            raw &= bitmask;
            asm("" : "+r"(raw));
            magnitude = raw;
        }
    }
    v = &velocity;
    magnitude -= 16;
    {
        register const s16 *sine asm("r1") = gUnk_086149C4;
        s32 product = sine[(angle + 64) & mask] * magnitude;
        register u32 scratch asm("r5") = opaque;
        asm volatile("" : : "r"(scratch));
        v->x = Truncate(product);
        v->y = 0;
        {
            s32 product = sine[angle] * magnitude;
            asm("" : "+r"(product) : "r"(angle));
            v->z = Truncate(product);
        }
    }
    {
        u32 offset;
        register const u16 *source asm("r2");
        *cursor = (*cursor + 1) & 1023;
        offset = *cursor * 2;
        source = gUnk_0203B400;
        {
            u32 value = *(const u16 *)(offset + (u32)source);
            {
                u32 mask = 63;
                value &= mask;
            }
            value -= 48;
            velocity.y = value;
        }
    }
    SceneMovingParticle_Create(position, &velocity, 60, 3);
}
