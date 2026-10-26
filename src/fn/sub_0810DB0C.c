#include "global.h"
#include "render.h"

struct PanelWidget {
    u8 f0[8];
    u32 flags;
    u8 fC[0x1A - 0xC];
    u8 f1A;
    u8 f1B[0x3A - 0x1B];
    u16 f3A;
    u8 f3C[0x48 - 0x3C];
    void *f48;
    u8 f4C[0x60 - 0x4C];
};
struct SelectionPanel {
    u8 f0[0x20];
    struct PanelWidget widgets[10];
    u8 res2[0x20];
    u8 res1[0x20];
    u16 entryIds[10];
    u16 fixed[10];
    s32 proc0;
    s32 proc1;
    u8 buf[1];
};
extern u32 gUnk_030053F4;
extern u32 gUnk_030053F0;
s32 ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, u32);
void CpuSet(const void *, void *, u32);

void sub_0810DB0C(struct SelectionPanel *p)
{
    s32 i;
    s32 t;
    t = ScriptProc_Start(0xCB05, 0x1289);
    p->proc0 = t;
    ResourceHeader_Relocate(p->res1);
    for (i = 0; i <= 6; i++) {
        Widget_InitLinked(&p->widgets[i], p->res1, p->entryIds[i], 0x10, 1, 0, 0, 0);
        p->fixed[i] = 0xFFFF;
    }
    p->widgets[3].f3A = 0xA008;
    p->widgets[3].f48 = p->buf;
    CpuSet(gPaletteBankData + 0x64E0, p->buf, 0x04000008);
    t = ScriptProc_Start(0xCB05, 0x530D);
    p->proc1 = t;
    ResourceHeader_Relocate(p->res2);
    for (i = 7; i <= 9; i++) {
        Widget_InitLinked(&p->widgets[i], p->res2, p->entryIds[i], 0x10, 1, 0, 0, 0);
        p->fixed[i] = 0xFFFF;
        {
            u32 mask = 0x800;
            if ((gUnk_030053F4 | gUnk_030053F0) & mask)
                p->widgets[i].flags |= 1;
        }
    }
    p->widgets[6].f1A = 0;
    p->widgets[3].f1A = 0;
}
