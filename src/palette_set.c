#include "palette_set.h"

void sub_0800E0CC(struct PaletteSet *a, struct PaletteSetEntry *b)
{
    sub_08221110(b->blend, b->source[a->from], b->source[a->to], a->blend, 6);
    CpuSet(b->blend, b->colors, 0x10);
}

void sub_0800E10C(struct PaletteSet *a, struct PaletteSetEntry *b)
{
    sub_08221110(b->colors, b->blend, b->overlay, a->overlay, 6);
}

int PaletteSet_Update(struct PaletteSet *p)
{
    int amount = sub_080036C0(), changed = 0, i;
    struct PaletteSetEntry *palette;
    if (p->current != gUnk_03005430[14]) {
        sub_08227950(&p->from, &p->to);
        goto refresh;
    }
    if (p->blend != amount)
        changed = 1;
    if (changed) {
    refresh:
        p->blend = amount;
        for (i = 0; i < p->count; i++)
            sub_0800E0CC(p, &p->palettes[i]);
    }
    amount = sub_080036DC();
    if (p->overlay != amount) {
        p->overlay = amount;
        for (i = 0; i < p->count; i++)
            sub_0800E10C(p, &p->palettes[i]);
    }
    gUnk_03004DA4 = gUnk_030051CC;
    gUnk_030051C0 = 0;
    for (i = 0; i < p->count; i++) {
        sub_08217344(p->palettes[i].slot, p->palettes[i].colors);
        gUnk_030051C0++;
    }
    p->previous = gUnk_03005430[15];
    p->current = gUnk_03005430[14];
    return 0;
}

u32 sub_0800E228(void)
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
            entry->slot = sub_08227938(entry->id) + offsets[i];
            entry->source[0] = gUnk_030042E4 + (entry->slot << 5);
            entry->source[1] = (u8 *)entry->source[0] + 32;
            entry->source[2] = (u8 *)entry->source[1] + 32;
            entry->slot = sub_08227938(entry->id);
            sub_08221208(overlay, entry->source[2], 0, 14, 6);
            p->count++;
        }
    }
    sub_08227950(&p->from, &p->to);
    p->blend = 64;
    p->previous = gUnk_03005430[15];
    p->current = gUnk_03005430[14];
    p->overlay = 0;
    for (i = 0; i < p->count; i++) {
        sub_0800E0CC(p, &p->palettes[i]);
        sub_0800E10C(p, &p->palettes[i]);
    }
    return 0;
}

void *sub_0800E390(u32 arg)
{
    struct PaletteSet *obj;
    obj = Task_Create(12, 0x2C8);
    if (obj != 0) {
        Task_SetCallbacks(obj, (void *)PaletteSet_Update, (void *)sub_0800E228);
        if (PaletteSet_Initialize(obj, arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
