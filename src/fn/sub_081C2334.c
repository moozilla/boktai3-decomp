#include "global.h"
#include "contact_actor.h"
#include "render.h"
struct ResourceHeader32 {
    u32 words[8];
};
struct PanelWidget {
    u8 unknown0[0x3A];
    u16 paletteBank;
    u8 unknown3C[0xC];
    u8 *palette; /* 0x48 */
    u8 unknown4C[0x14];
};
struct S081C2334 {
    u8 unknown0[0x378];
    struct ResourceHeader32 resource; /* 0x378 */
    struct PanelWidget widgets[5];    /* 0x398 */
};
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
void sub_081C2334(struct S081C2334 *p)
{
    struct ContactPosition position;
    s32 i;
    s32 zero;
    struct ResourceHeader32 *resource = ScriptProc_Start(0xCB05, 0x530D);
    p->resource = *resource;
    ResourceHeader_Relocate(&p->resource, resource);
    position.x = 144;
    position.y = 64;
    zero = position.z = 0;
    Widget_InitLinked(&p->widgets[0], &p->resource, 0x28, 0x30, 1, zero, 0x3C, &position);
    p->widgets[0].paletteBank = 0x3AC;
    p->widgets[0].palette = gPaletteBankData + p->widgets[0].paletteBank * 32;
    position.x += 8;
    for (i = 0; i < 4; i++) {
        Widget_InitLinked(&p->widgets[i + 1], &p->resource, 0x1E, 0x30, 1, 0, 0x3C, &position);
        p->widgets[i + 1].paletteBank = 0x3AC;
        p->widgets[i + 1].palette = gPaletteBankData + p->widgets[i + 1].paletteBank * 32;
        position.x += 8;
    }
}
