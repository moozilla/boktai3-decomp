#include "global.h"
#define ActorPalette_UpdateFade          sub_081E3550
#define ActorPalette_UpdateFadeAndNotify sub_081E36FC
#define ActorPalette_BeginReverseFade    sub_081E38C4
#define ActorPalette_UpdateReverseFade   sub_081E394C
// Provisional name: ActorPalette_BeginReverseFade
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
void sub_08215284(void *, s32);
void ActorPalette_UpdateFade(void *);
void ActorPalette_UpdateFadeAndNotify(void *);
void ActorPalette_UpdateReverseFade(void *);
// Begin the reverse palette transition and retain the normal asset palette.
void ActorPalette_BeginReverseFade(struct PaletteController *p)
{
    const u16 *src;
    u16 *dst;
    s32 i;
    void *effect;
    p->step = 50;
    p->duration = 50;
    effect = p->effect;
    sub_08215284(effect, 343);
    p->savedPalette = p->palette;
    sub_08215284(effect, 344);
    src = p->palette;
    dst = p->output;
    for (i = 15; i >= 0; i--) {
        *dst++ = *src++;
    }
    p->palette = p->output;
    p->callback = ActorPalette_UpdateReverseFade;
}
