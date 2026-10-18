#include "global.h"
struct ArmorPaletteState {
    u8 pad0[0x650];
    void (*update)(void *);
    void (*finish)(void *);
    u8 pad658[0x200];
    u16 banks[3][2][16];
    u8 pad918[0x140];
    u16 saved[4][16];
    u16 timer;
    u8 bank;
    u8 otherBank;
};
void sub_08181778(void *);
void ActorPalette_BlendSavedBanksSlow(void *);
void ActorPalette_RestoreSelectedBanks(void *);
void Palette_CopyBank(void *, u32);
void Armor_InitPaletteBanks(struct ArmorPaletteState *s)
{
    u32 controller = (u32)&s->update;
    u32 off;
    u32 source;
    void (*update)(void *);
    void (*finish)(void *);
    sub_08181778(s);
    update = ActorPalette_BlendSavedBanksSlow;
    finish = ActorPalette_RestoreSelectedBanks;
    *(u32 *)controller = (u32)update;
    *(u32 *)(controller + 4) = (u32)finish;
    off = s->bank * 32 + 0x208;
    source = controller + off;
    Palette_CopyBank(s->saved[0], source);
    off = s->bank * 32 + 0x228;
    source = controller + off;
    Palette_CopyBank(s->saved[1], source);
    off = s->bank * 32 + 0x248;
    source = controller + off;
    Palette_CopyBank(s->saved[2], source);
    off = s->otherBank * 32 + 0x208;
    source = controller + off;
    Palette_CopyBank(s->saved[3], source);
    s->timer = 0;
}
