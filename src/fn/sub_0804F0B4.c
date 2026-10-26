#include "global.h"
struct P {
    u8 f[0x25c];
    u32 x;
};
extern u8 *gGameState;
void AnimData_SelectPaletteBank(u8 *, s32);
void ResourceStation_TransferStoredResource(struct P *p)
{
    u32 *q = &p->x;
    u8 *x = (u8 *)*q;
    if (x[0x457] == 8) {
        u32 m = 0x40;
        if (*(u32 *)(x + 0x20) & m) {
            s32 *c = (s32 *)(gGameState + 0x77c);
            s32 v = *c;
            if (v > 0) {
                if (*(u16 *)(x + 0x428) < *(u16 *)(x + 0x42a)) {
                    *c = v - 1;
                    (*(u16 *)((u8 *)*q + 0x428))++;
                }
            }
        }
    } else {
        AnimData_SelectPaletteBank((u8 *)p + 0x44, 0x1ae);
        *((u8 *)p + 0x262) = 0;
    }
    *((u8 *)p + 0x264) = 1;
}
