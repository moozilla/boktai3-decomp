#include "global.h"
// Provisional name: ActorPalette_UpdateFadeAndNotify
struct PaletteController {
    u8 beforeEffect[0xe08];
    u8 effect[12];
    const u16 *palette;
    u8 beforeColors[0xee8 - 0xe18];
    u16 alternate[16], base[16], output[16];
    const u16 *savedPalette;
    s16 step;
    u16 duration;
    void *handle, *completionHandle;
    u8 beforeMode[2];
    u16 mode;
    u8 beforeCallback[0xf8c - 0xf5c];
    void (*callback)(void *);
};
void AnimData_SelectPaletteBank(void *, s32);

s32 Div(s32, s32);
void Script_Call(void *, s32);
// Interpolate the actor palette over fifty updates, then restore its asset palette.
void ActorPalette_UpdateFadeAndNotify(struct PaletteController *p)
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
    if (++p->step > p->duration) {
        p->callback = 0;
        AnimData_SelectPaletteBank(p->effect, 344);
        p->palette = p->savedPalette;
        if (p->completionHandle) {
            void *handle = p->completionHandle;
            p->completionHandle = 0;
            Script_Call(handle, 0);
        }
    }
}
