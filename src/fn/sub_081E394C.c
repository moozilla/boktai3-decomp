#include "global.h"
#define ActorPalette_UpdateReverseFade sub_081E394C
// Provisional name: ActorPalette_UpdateReverseFade
// MATCHING_CONCESSION: register-pinning
struct PaletteController {
    u8 beforeEffect[0xe08];
    u8 effect[12];
    const u16 *palette;
    u8 beforeAnim[0xe2c - 0xe18];
    u16 frame;
    u8 gapFrame[4];
    u16 timer;
    u8 beforeColors[0xee8 - 0xe34];
    u16 alternate[16], base[16], output[16];
    const u16 *savedPalette;
    s16 step;
    u16 duration;
    void *handle, *completionHandle;
    u8 beforeMode[2];
    u16 mode : 4;
    u16 phase : 12;
    u16 oldMode;
    u8 beforeFlags[6];
    u8 flags;
    u8 beforeSaved[0xf86 - 0xf65];
    u8 savedFrame, savedTimer;
    u8 beforeCallback[4];
    void (*callback)(void *);
};
void sub_08215284(void *, s32);

s32 Div(s32, s32);
static inline u32 mode_bits(u8 *p, u32 mask)
{
    register u32 v asm("r1") = *p;
    u32 result = mask & v;
    return result;
}
// Interpolate the actor palette over fifty updates, then restore its asset palette.
void ActorPalette_UpdateReverseFade(struct PaletteController *p)
{
    s32 i;
    struct RGB {
        s32 r : 8, g : 8, b : 8;
    } a, b, c;
    for (i = 0; i < 16; i++) {
        a.r = p->base[i] & 31;
        a.g = (p->base[i] >> 5) & 31;
        a.b = (p->base[i] >> 10) & 31;
        b.r = p->alternate[i] & 31;
        b.g = (p->alternate[i] >> 5) & 31;
        b.b = (p->alternate[i] >> 10) & 31;
        c.r = Div((b.r - a.r) * p->step, p->duration) + a.r;
        c.g = a.g + Div((b.g - a.g) * p->step, p->duration);
        c.b = Div((b.b - a.b) * p->step, p->duration) + a.b;
        p->output[i] = (c.b << 10) | (c.g << 5) | c.r;
    }
    if (--p->step < 0) {
        p->callback = 0;
        sub_08215284(p->effect, 343);
        p->palette = p->savedPalette;
        p->oldMode = p->mode;
        {
            u32 mode = 2;
            p->mode = mode;
        }
        {
            register u32 raw asm("r1") = *(u16 *)((u8 *)p + 0xf5a);
            register u32 mask asm("r2") = 15;
            *(u16 *)((u8 *)p + 0xf5a) = (mask & raw) | 0xd0;
            if (p->oldMode == 3) {
                register u32 bits asm("r0") = mode_bits((u8 *)p + 0xf5a, mask);
                if (bits == 3) {
                    p->savedFrame = p->frame;
                    p->savedTimer = p->timer;
                }
            }
        }
        {
            u8 one = 1;
            p->flags |= one;
        }
    }
}
