#include "global.h"
#include "projected_display.h"

extern u16 gUnk_03005418[];

/* SparkleEmitter_UpdateParticles: advances position by velocity, cycles a flicker
 * frame, projects to screen space and culls (deactivates + decrements the active
 * count) particles that leave the visible area; called by the emitter's two
 * per-frame update wrappers. */
#define SparkleEmitter_UpdateParticles sub_081C2838

struct SparkParticle {
    u32 flags;
    u8 pad4[0x10 - 0x04];
    u16 frame;
    u8 pad12[0x1c - 0x12];
    struct ContactPosition position;
    u8 pad24[0x2c - 0x24];
    struct ContactPosition velocity;
    s32 counter;
};

void sub_081C2838(u8 *p)
{
    struct SparkParticle *particle = (struct SparkParticle *)(p + 0x18);
    u16 *camera = gUnk_03005418;
    s32 i;
    s32 limit = 0x140;

    for (i = 0x1f; i >= 0; i--, particle++) {
        s16 *posBase;
        s32 x, z;
        s32 sum, vertical, y, depth;
        s16 screen[4];

        if (particle->flags & 1)
            continue;

        posBase = (s16 *)&particle->position;

        particle->position.x += particle->velocity.x;
        particle->position.y += particle->velocity.y;
        particle->position.z += particle->velocity.z;

        particle->frame = (s16)((particle->counter >> 3) % 4);
        particle->counter += 1;

        x = particle->position.x;
        z = posBase[2];
        screen[0] = projectScale(3 * (x - z));
        sum = projectScale(3 * (x + z));
        vertical = projectScale(3 * posBase[1]);
        y = sum - vertical;
        depth = sum + vertical;
        screen[0] = (u16)screen[0] - camera[0] + 0x78;
        screen[1] = y - camera[1] + 0x50;
        screen[2] = depth - camera[2];

        if ((u16)(screen[0] + 0x80) > 0x1C0 || screen[1] > limit || screen[1] < -0x80) {
            particle->flags |= 1;
            *(u32 *)(p + 0x778) -= 1;
        }
    }
}
