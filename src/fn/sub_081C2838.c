#include "global.h"
#include "render.h"
// MATCHING_CONCESSION: register-pinning

struct SparklePosition {
    s16 x, y, z, unk_06;
};

struct SparkleParticle {
    u32 flags;
    u8 unk_04[0x0C];
    u16 frame;
    u8 unk_12[0x0A];
    struct SparklePosition position;
    u8 unk_24[8];
    struct SparklePosition velocity;
    s32 age;
};

struct SparkleEmitter {
    u8 unk_00[0x18];
    struct SparkleParticle particles[32];
    u8 unk_718[0x60];
    u32 count;
};

static inline s32 Sparkle_ProjectScale(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        value >>= 5;
    else
        value = -((-value) >> 5);
    return value;
}

static inline void Sparkle_ProjectPosition(struct SparklePosition *screen, s16 *position,
                                           const u16 *camera)
{
    s32 x, z, sum, height, y, depth;
    x = position[0];
    z = position[2];
    screen->x = Sparkle_ProjectScale(x - z);
    sum = Sparkle_ProjectScale(x + z);
    height = Sparkle_ProjectScale(*++position);
    y = sum - height;
    depth = sum + height;
    screen->x = (u16)screen->x - camera[0] + 120;
    screen->y = y - camera[1] + 80;
    screen->z = depth - camera[2];
}

static inline void SparkleEmitter_RemoveParticle(struct SparkleEmitter *emitter)
{
    emitter->count--;
}

#ifdef NONMATCHING
// fuzzy: 99.3%
// scope shape 2 (differences once low registers are set aside)
void SparkleEmitter_UpdateParticles(u8 *arg0)
{
    struct SparkleEmitter *emitter = (struct SparkleEmitter *)arg0;
    struct SparkleParticle *particle = emitter->particles;
    const u16 *camera = gCameraOrigin;
    register s32 i asm("r8") = 31;
    do {
        if (!(particle->flags & 1)) {
            struct SparklePosition screen;
            struct SparklePosition *position = &particle->position;
            position->x += particle->velocity.x;
            position->y += particle->velocity.y;
            position->z += particle->velocity.z;
            particle->frame = (particle->age >> 3) % 4;
            particle->age++;
            Sparkle_ProjectPosition(&screen, &position->x, camera);
            if ((u16)(screen.x + 128) > 448 || screen.y > 320 || screen.y < -128) {
                particle->flags |= 1;
                SparkleEmitter_RemoveParticle(emitter);
            }
        }
        particle++;
        i--;
    } while (i >= 0);
}
#else
INCLUDE_ASM("asm/nonmatching", SparkleEmitter_UpdateParticles);
#endif
