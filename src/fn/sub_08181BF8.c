#define ActorPalette_BlendSavedBanks sub_08181BF8
#define Palette_InterpolateBanks sub_08221110
#include "global.h"
struct ActorPaletteFade {
    u32 active;
    void *callback;
    u8 pad8[0x488-8];
    s16 frame;
    u8 bank, secondaryBank;
};
void sub_08221110(void *, void *, void *, int, int);
s32 Callback_InvokeActor(void *, void *);
void sub_08181BF8(struct ActorPaletteFade *p)
{
    if (p->frame <= 16) {
        void *source;
        void *saved;
        saved = (u8 *)p + 0x408;
        source = (u8 *)p + ((p->bank << 5) + 8);
        sub_08221110((u8 *)p + ((p->bank << 5) + 0x208), saved, source, p->frame, 4);
        saved = (u8 *)p + 0x428;
        source = (u8 *)p + ((p->bank << 5) + 0x28);
        sub_08221110((u8 *)p + ((p->bank << 5) + 0x228), saved, source, p->frame, 4);
        saved = (u8 *)p + 0x448;
        source = (u8 *)p + ((p->bank << 5) + 0x48);
        sub_08221110((u8 *)p + ((p->bank << 5) + 0x248), saved, source, p->frame, 4);
        saved = (u8 *)p + 0x468;
        source = (u8 *)p + ((p->secondaryBank << 5) + 8);
        sub_08221110((u8 *)p + ((p->secondaryBank << 5) + 0x208), saved, source, p->frame, 4);
    } else {
        p->active = 0;
        if (p->callback) {
            Callback_InvokeActor(p, p->callback);
            p->callback = 0;
        }
    }
    p->frame++;
}
