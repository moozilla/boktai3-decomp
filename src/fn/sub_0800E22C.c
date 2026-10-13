#include "global.h"
typedef struct {
    u16 id, slot;
    u16 colors[16], overlay[16], blend[16];
    void *source[3];
} Palette;
typedef struct {
    u8 task[24];
    u16 id, count, from, to;
    u8 blend, overlay;
    u16 previous, current, pad;
    Palette palettes[6];
} Manager;
extern u8 gUnk_03005430[];
extern u32 gUnk_03004DA4, gUnk_030051CC, gUnk_030051C0;
int sub_080036C0(void);
int sub_080036DC(void);
void sub_08227950(u16 *, u16 *);
void sub_0800E0CC(Manager *, Palette *);
void sub_0800E10C(Manager *, Palette *);
void sub_08217344(u32, void *);

int Script_SeekToKeyword(int);
int Script_GetValue(void);
void *Script_GetPc(void);
int sub_08227938(u32);
void sub_08221208(void *, void *, int, int, int);
extern u8 *gUnk_030042E4;
#define PaletteSet_Initialize sub_0800E22C
int PaletteSet_Initialize(Manager *p, u16 id)
{
    u16 offsets[6];
    int i;
    Palette *entry;
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
