#include "palette_controller.h"
u8 *sub_0821A520(u16, u16);
void sub_08227950(u16 *, u16 *);
void sub_08002E00(struct PaletteController *, struct PaletteTransition *);
void sub_08002E54(struct PaletteController *);
void sub_08003038(struct PaletteController *);
void sub_0800301C(struct PaletteController *);
void Palette_ReplaceResource(u32 bank, u32 index, u32 resource, u32 flags)
{
    struct PaletteController *p = gUnk_02000468;
    if (p) {
        u8 *colors;
        u16 id = resource;
        colors = sub_0821A520(0x92B3, id) + 20;
        p->colorSets[bank][index] = (u16 *)colors;
        if (flags & 1) {
            if (p->flags & 1) {
                u16 first, second;
                sub_08227950(&first, &second);
                if (first == 2)
                    sub_08002E00(p, &p->transition);
                sub_08002E54(p);
            }
            if (p->flags & 8)
                sub_08003038(p);
            else
                sub_0800301C(p);
            p->previousBlend = -1;
            p->previousState = -1;
        }
    }
}
