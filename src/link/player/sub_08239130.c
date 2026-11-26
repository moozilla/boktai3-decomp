#include "global.h"
void Effect_Remove(void *);
void Player_DestroySixParticleEffects(u8 *s)
{
    u8 *p = s + 0x870;
    s32 i = 5;
    do {
        Effect_Remove(p);
        p += 0x34;
        i--;
    } while (i >= 0);
}
