#ifndef PALETTE_SET_H
#define PALETTE_SET_H
#include "engine.h"

// Tentative: a task holding up to six 16 color palettes that are blended between two source
// variants and an overlay, then uploaded to palette slots every frame.

#define PALETTE_SET_MAX 6

struct PaletteSetEntry {
    u16 id, slot;
    u16 colors[16];  // current output colors
    u16 overlay[16]; // overlay target colors
    u16 blend[16];   // source blend result before the overlay
    void *source[3]; // the three variants of this palette in the graphics bank
};

struct PaletteSet {
    u8 task[24];
    u16 id, count, from, to; // from/to: variant indices blended by `blend`
    u8 blend, overlay;       // blend amounts (64 is full)
    u16 previous, current, pad;
    struct PaletteSetEntry palettes[PALETTE_SET_MAX];
};

extern u8 gUnk_03005430[]; // game state bytes (14: current, 15: previous; guess)
extern u32 gUnk_03004DA4, gUnk_030051CC, gUnk_030051C0;

int Palette_GetTransitionLevel(void);
int Palette_GetBrightness(void);
void sub_08227950(u16 *from, u16 *to);
void Palette_CacheObjectBank(u32 slot, void *colors);
void sub_08221110(void *dest, void *a, void *b, u32 amount, u32 count);
int PaletteSet_GetSlotForId(u32 id);
void sub_08221208(void *dest, void *source, int a, int b, int c);

void PaletteSet_BlendSourceColors(struct PaletteSet *set, struct PaletteSetEntry *entry);
void PaletteSet_BlendOverlayColors(struct PaletteSet *set, struct PaletteSetEntry *entry);
int PaletteSet_Update(struct PaletteSet *p);
u32 PaletteSet_Destroy(void);
int PaletteSet_Initialize(struct PaletteSet *p, u16 id);
void *PaletteSet_Create(u32 arg);

#endif
