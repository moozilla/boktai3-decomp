#define SpriteBurst_InitParticles sub_08237DD4
#define Effect_SetSpriteOffset Effect_SetOrigin
#define Effect_SetPaletteSlot Effect_SetPalette
#include "global.h"
struct SpriteBurstParticle { u8 effect[0x28]; u8 active; u8 unknown29[7]; };
struct SpriteBurst { u8 unknown0[0x77]; u8 state; void *resource; struct SpriteBurstParticle particles[4]; };
void *AnimData_FindEntry(s32);
void sub_08217DE0(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void sub_08217EEC(void *, void *, u32);
void Effect_SetPalette(void *, u32);
void sub_08237DD4(struct SpriteBurst *p)
{
    s32 i;
    struct SpriteBurstParticle *particle;
    u8 *active;
    s32 offset;
    p->state = 0;
    p->resource = AnimData_FindEntry(0x1c1e);
    offset = -4;
    active = &p->particles[0].active;
    particle = p->particles;
    for (i = 3; i >= 0; i--) {
        sub_08217DE0(particle, p->resource, 1);
        Effect_SetOrigin(particle, offset, offset);
        sub_08217EEC(particle, p->resource, 0x11);
        Effect_SetPalette(particle, 1);
        *active = 0;
        active += 0x30;
        particle++;
    }
}
