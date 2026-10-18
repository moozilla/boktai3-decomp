#include "global.h"
#define BouncingParticle_Update     sub_081269E4
#define BouncingParticle_ShowSprite Sprite_LinkToList
#define BouncingParticle_HideSprite Sprite_UnlinkFromList
struct BouncingParticle {
    u32 flags;
    u8 queued, unknown5[11];
    u16 frame;
    u8 unknown12[10];
    u16 x, y, z;
    u8 unknown22[10];
    u16 velocityX, velocityY, velocityZ, unknown32;
    u8 unknown34;
    u8 delay;
    u16 stepInterval, age;
    u8 unknown3A[2], state, frameShift;
};
s32 Mod(s32, s32);
void Sprite_LinkToList(struct BouncingParticle *, s32);
void Sprite_UnlinkFromList(struct BouncingParticle *, u32);
void sub_081269E4(void *unused, struct BouncingParticle *particle)
{
    struct BouncingParticle *sprite = particle;
    s32 frame;
    if ((s8)particle->delay > 0) {
        particle->delay--;
    } else {
        if ((s8)particle->delay == 0)
            particle->delay |= 0xFF;
        particle->age++;
        frame = particle->age >> particle->frameShift;
        if (frame == 6) {
            particle->state = 2;
        } else if (Mod(particle->age, particle->stepInterval)) {
            if (particle->queued)
                Sprite_UnlinkFromList(particle, 0);
        } else {
            if (!particle->queued)
                Sprite_LinkToList(particle, 0);
            sprite->x += sprite->velocityX;
            sprite->y += sprite->velocityY;
            sprite->z += sprite->velocityZ;
            sprite->velocityX = (s16)sprite->velocityX >> 1;
            sprite->velocityZ = (s16)sprite->velocityZ >> 1;
            sprite->frame = frame;
        }
    }
}
