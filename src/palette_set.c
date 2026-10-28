#include "palette_set.h"

void PaletteSet_BlendSourceColors(struct PaletteSet *a, struct PaletteSetEntry *b)
{
    Palette_InterpolateBanks(b->blend, b->source[a->from], b->source[a->to], a->blend, 6);
    CpuSet(b->blend, b->colors, 0x10);
}

void PaletteSet_BlendOverlayColors(struct PaletteSet *a, struct PaletteSetEntry *b)
{
    Palette_InterpolateBanks(b->colors, b->blend, b->overlay, a->overlay, 6);
}

int PaletteSet_Update(struct PaletteSet *p)
{
    int amount = Palette_GetTransitionLevel(), changed = 0, i;
    struct PaletteSetEntry *palette;
    if (p->current != gSunClock[14]) {
        Palette_GetStateBlendIndices(&p->from, &p->to);
        goto refresh;
    }
    if (p->blend != amount)
        changed = 1;
    if (changed) {
    refresh:
        p->blend = amount;
        for (i = 0; i < p->count; i++)
            PaletteSet_BlendSourceColors(p, &p->palettes[i]);
    }
    amount = Palette_GetBrightness();
    if (p->overlay != amount) {
        p->overlay = amount;
        for (i = 0; i < p->count; i++)
            PaletteSet_BlendOverlayColors(p, &p->palettes[i]);
    }
    gObjPaletteBankCount = gPaletteBanksInUse;
    gObjPaletteReserved = 0;
    for (i = 0; i < p->count; i++) {
        Palette_CacheObjectBank(p->palettes[i].slot, p->palettes[i].colors);
        gObjPaletteReserved++;
    }
    p->previous = gSunClock[15];
    p->current = gSunClock[14];
    return 0;
}

u32 PaletteSet_Destroy(void)
{
    return 0;
}

int PaletteSet_Initialize(struct PaletteSet *p, u16 id)
{
    u16 offsets[6];
    int i;
    struct PaletteSetEntry *entry;
    u16 *overlay;
    p->id = id;
    p->count = 0;
    if (Script_SeekToKeyword(111)) {
        for (i = 0; i < 6 && Script_GetPc(); i++)
            offsets[i] = Script_GetValue();
        for (; i < 6; i++)
            offsets[i] = 0;
    } else {
        for (i = 0; i < 6; i++)
            offsets[i] = 0;
    }
    if (Script_SeekToKeyword(112)) {
        for (i = 0, overlay = p->palettes[0].overlay, entry = p->palettes; i < 6 && Script_GetPc();
             overlay += 56, entry++, i++) {
            entry->id = Script_GetValue();
            entry->slot = PaletteSet_GetSlotForId(entry->id) + offsets[i];
            entry->source[0] = gPaletteBankData + (entry->slot << 5);
            entry->source[1] = (u8 *)entry->source[0] + 32;
            entry->source[2] = (u8 *)entry->source[1] + 32;
            entry->slot = PaletteSet_GetSlotForId(entry->id);
            Palette_BlendBank(overlay, entry->source[2], 0, 14, 6);
            p->count++;
        }
    }
    Palette_GetStateBlendIndices(&p->from, &p->to);
    p->blend = 64;
    p->previous = gSunClock[15];
    p->current = gSunClock[14];
    p->overlay = 0;
    for (i = 0; i < p->count; i++) {
        PaletteSet_BlendSourceColors(p, &p->palettes[i]);
        PaletteSet_BlendOverlayColors(p, &p->palettes[i]);
    }
    return 0;
}

void *PaletteSet_Create(u32 arg)
{
    struct PaletteSet *obj;
    obj = Task_Create(12, 0x2C8);
    if (obj != 0) {
        Task_SetCallbacks(obj, (void *)PaletteSet_Update, (void *)PaletteSet_Destroy);
        if (PaletteSet_Initialize(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
