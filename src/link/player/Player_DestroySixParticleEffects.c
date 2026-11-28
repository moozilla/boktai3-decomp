#include "global.h"

struct SixParticle {
    u8 unknown0[0x34];
};
void Effect_Remove(void *);
void Player_DestroySixParticleEffects(u8 *player)
{
    struct SixParticle *particle = (struct SixParticle *)(player + 0x870);
    s32 i;

    for (i = 0; i < 6; i++) {
        Effect_Remove(particle);
        particle++;
    }
}
