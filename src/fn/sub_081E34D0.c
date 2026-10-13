#include "global.h"
// Provisional name: ActorPalette_BeginFade
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
// Begin an actor palette transition; mode 2 also completes a waiting request.
void ActorPalette_BeginFade(struct PaletteController *p)
{
    p->step = 0;
    p->duration = 50;
    sub_08215284(p->effect, 344);
    p->savedPalette = p->palette;
    p->palette = p->output;
    if ((*(u8 *)&p->mode & 15) == 2)
        p->callback = ActorPalette_UpdateFadeAndNotify;
    else
        p->callback = ActorPalette_UpdateFade;
}
