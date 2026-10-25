#include "global.h"
// confidence=low: ActorPalette_FadeWhite in 8 steps (blend shift 3) instead of 16.
struct ActorPaletteFade {
    u32 active;
    void *callback;
    u8 pad8[0x488 - 8];
    s16 frame;
    u8 bank, secondaryBank;
};
void Palette_BlendBank(void *, void *, int, int, int);
s32 Callback_InvokeActor(void *, void *);
void ActorPalette_FadeWhiteFast(struct ActorPaletteFade *p)
{
    if (p->frame <= 8) {
        void *source;
        source = (u8 *)p + ((p->bank << 5) + 8);
        Palette_BlendBank((u8 *)p + ((p->bank << 5) + 0x208), source, 0x7FFF, p->frame, 3);
        source = (u8 *)p + ((p->bank << 5) + 0x28);
        Palette_BlendBank((u8 *)p + ((p->bank << 5) + 0x228), source, 0x7FFF, p->frame, 3);
        source = (u8 *)p + ((p->bank << 5) + 0x48);
        Palette_BlendBank((u8 *)p + ((p->bank << 5) + 0x248), source, 0x7FFF, p->frame, 3);
        source = (u8 *)p + ((p->secondaryBank << 5) + 8);
        Palette_BlendBank((u8 *)p + ((p->secondaryBank << 5) + 0x208), source, 0x7FFF, p->frame, 3);
    } else {
        p->active = 0;
        if (p->callback) {
            Callback_InvokeActor(p, p->callback);
            p->callback = 0;
        }
    }
    p->frame++;
}
