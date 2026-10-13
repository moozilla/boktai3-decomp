#include "palette_set.h"
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
