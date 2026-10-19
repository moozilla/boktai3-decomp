#include "global.h"

/* tentative: blends bank palettes toward a colour then back using saved copies */
#define ActorPalette_PulseBlendSaved sub_08181E78

struct ActorPaletteFade {
    u32 active;
    void *callback;
    u8 pad8[0x488 - 8];
    s16 frame;
    u8 bank, secondaryBank;
};
void Palette_BlendBank(void *, void *, int, int, int);
void Palette_CopyBank(void *, void *);
void Palette_InterpolateBanks(void *, void *, void *, int, int);
void ActorPalette_PulseBlendSaved(struct ActorPaletteFade *p)
{
    void *dest;
    void *source;
    void *saved;
    if (p->frame <= 8) {
        source = (u8 *)p + ((p->bank << 5) + 8);
        dest = (u8 *)p + ((p->bank << 5) + 0x208);
        Palette_BlendBank(dest, source, 0x10, p->frame, 3);
        source = (u8 *)p + ((p->bank << 5) + 0x28);
        dest = (u8 *)p + ((p->bank << 5) + 0x228);
        Palette_BlendBank(dest, source, 0x10, p->frame, 3);
        source = (u8 *)p + ((p->bank << 5) + 0x48);
        dest = (u8 *)p + ((p->bank << 5) + 0x248);
        Palette_BlendBank(dest, source, 0x10, p->frame, 3);
        source = (u8 *)p + ((p->secondaryBank << 5) + 8);
        dest = (u8 *)p + ((p->secondaryBank << 5) + 0x208);
        Palette_BlendBank(dest, source, 0x10, p->frame, 3);
        if (p->frame == 8) {
            u32 b;
            b = p->bank << 5;
            saved = (u8 *)p + 0x408;
            dest = (u8 *)p + (b + 0x208);
            Palette_CopyBank(saved, dest);
            b = p->bank << 5;
            saved = (u8 *)p + 0x428;
            dest = (u8 *)p + (b + 0x228);
            Palette_CopyBank(saved, dest);
            b = p->bank << 5;
            saved = (u8 *)p + 0x448;
            dest = (u8 *)p + (b + 0x248);
            Palette_CopyBank(saved, dest);
            b = p->secondaryBank << 5;
            saved = (u8 *)p + 0x468;
            dest = (u8 *)p + (b + 0x208);
            Palette_CopyBank(saved, dest);
        }
    } else {
        s32 t = 16 - p->frame;
        source = (u8 *)p + ((p->bank << 5) + 8);
        saved = (u8 *)p + 0x408;
        dest = (u8 *)p + ((p->bank << 5) + 0x208);
        Palette_InterpolateBanks(dest, saved, source, t, 3);
        source = (u8 *)p + ((p->bank << 5) + 0x28);
        saved = (u8 *)p + 0x428;
        dest = (u8 *)p + ((p->bank << 5) + 0x228);
        Palette_InterpolateBanks(dest, saved, source, t, 3);
        source = (u8 *)p + ((p->bank << 5) + 0x48);
        saved = (u8 *)p + 0x448;
        dest = (u8 *)p + ((p->bank << 5) + 0x248);
        Palette_InterpolateBanks(dest, saved, source, t, 3);
        source = (u8 *)p + ((p->secondaryBank << 5) + 8);
        saved = (u8 *)p + 0x468;
        dest = (u8 *)p + ((p->secondaryBank << 5) + 0x208);
        Palette_InterpolateBanks(dest, saved, source, t, 3);
    }
    p->frame++;
    if (p->frame > 15)
        p->frame = 0;
}
