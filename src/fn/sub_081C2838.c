#include "global.h"
#include "render.h"

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

static inline s32 Sparkle_ProjectScale(s32 value)
{
    if (value >= 0)
        value >>= 5;
    else
        value = -((-value) >> 5);
    return value;
}

static inline void Sparkle_ProjectPosition(struct SparklePosition *screen, s16 *position)
{
    s32 x, z, sum, height, y, depth;
    x = position[0];
    z = position[2];
    screen->x = Sparkle_ProjectScale(3 * (x - z));
    sum = Sparkle_ProjectScale(3 * (x + z));
    height = Sparkle_ProjectScale(3 * *++position);
    y = sum - height;
    depth = sum + height;
    screen->x = (u16)screen->x - gCameraOrigin[0] + 120;
    screen->y = y - gCameraOrigin[1] + 80;
    screen->z = depth - gCameraOrigin[2];
}

static inline void SparkleEmitter_RemoveParticle(struct SparkleEmitter *emitter)
{
    emitter->count--;
}

void SparkleEmitter_UpdateParticles(u8 *arg0)
{
    struct SparkleEmitter *emitter = (struct SparkleEmitter *)arg0;
    struct SparkleParticle *particle = emitter->particles;
    s32 i;
    i = 0;
    do {
        if (!(particle->flags & 1)) {
            struct SparklePosition screen;
            struct SparklePosition *position = &particle->position;
            position->x += particle->velocity.x;
            position->y += particle->velocity.y;
            position->z += particle->velocity.z;
            particle->frame = (particle->age >> 3) % 4;
            particle->age++;
            Sparkle_ProjectPosition(&screen, &position->x);
            if ((u16)(screen.x + 128) > 448 || screen.y > 320 || screen.y < -128) {
                particle->flags |= 1;
                SparkleEmitter_RemoveParticle(emitter);
            }
        }
        particle++;
        i++;
    } while (i < 32);
}
