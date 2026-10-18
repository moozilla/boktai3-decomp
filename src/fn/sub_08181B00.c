#define ActorPalette_FadeWhite sub_08181B00
#define Palette_BlendBank sub_08221208
#include "global.h"
struct ActorPaletteFade {
    u32 active;
    void *callback;
    u8 pad8[0x488-8];
    s16 frame;
    u8 bank, secondaryBank;
};
void sub_08221208(void *, void *, int, int, int);
s32 Callback_InvokeActor(void *, void *);
void sub_08181B00(struct ActorPaletteFade *p)
{
    if (p->frame <= 16) {
        void *source;
        source = (u8 *)p + ((p->bank << 5) + 8);
        sub_08221208((u8 *)p + ((p->bank << 5) + 0x208), source, 0x7FFF, p->frame, 4);
        source = (u8 *)p + ((p->bank << 5) + 0x28);
        sub_08221208((u8 *)p + ((p->bank << 5) + 0x228), source, 0x7FFF, p->frame, 4);
        source = (u8 *)p + ((p->bank << 5) + 0x48);
        sub_08221208((u8 *)p + ((p->bank << 5) + 0x248), source, 0x7FFF, p->frame, 4);
        source = (u8 *)p + ((p->secondaryBank << 5) + 8);
        sub_08221208((u8 *)p + ((p->secondaryBank << 5) + 0x208), source, 0x7FFF, p->frame, 4);
    } else {
        p->active = 0;
        if (p->callback) {
            Callback_InvokeActor(p, p->callback);
            p->callback = 0;
        }
    }
    p->frame++;
}
