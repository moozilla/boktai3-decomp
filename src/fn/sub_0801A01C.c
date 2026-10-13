#include "global.h"
typedef struct {
    u32 pad[2], flags;
} Graphic;
extern u8 gUnk_08E60F80[][16];
int sub_0822E104(u32);
void sub_08019FD8(void *, u32);
void sub_0821980C(u8 *, u8 *, u32, u32);
void sub_08030BF8(void);
void sub_08033468(void);
void sub_0803343C(u32);
void Presentation_UpdateItemGraphic(u8 *p, u32 fallback)
{
    int index = sub_0822E104(p[0x1d]);
    if (index >= 0) {
        sub_08019FD8(gUnk_08E60F80[index], *(u32 *)(p + 0xdac));
        ((Graphic *)(p + 0x5b4))->flags &= ~1;
        sub_0821980C(p + 0x5b4, p + 0x6c, 0x18a, 0);
    } else {
        sub_08030BF8();
        if (!fallback)
            sub_08033468();
        else
            sub_0803343C(fallback);
        ((Graphic *)(p + 0x5b4))->flags |= 1;
    }
}
