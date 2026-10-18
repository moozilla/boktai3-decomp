#include "global.h"
struct MenuParticle {
    u32 flags;
    u8 unknown4[0x14];
    u16 x, y;
    u8 unknown1C[0x14];
    u16 velocityX, velocityY;
    u8 unknown34[4];
    u8 active, age;
};
u32 MenuParticle_UpdateMotion(struct MenuParticle *particle)
{
    if (++particle->age > 19) {
        particle->flags |= 1;
        particle->active = 0;
    } else {
        particle->x += particle->velocityX;
        particle->y += particle->velocityY;
        if (!(particle->age & 3))
            particle->velocityY++;
    }
    return particle->active;
}
