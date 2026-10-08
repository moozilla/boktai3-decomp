#include "global.h"
extern u32 gUnk_02000580;
s32 sub_08050188(u8 *);
s32 sub_08050128(u8 *);
s32 sub_080501E8(u8 *);
void sub_0821D6D0(u8 *);
void sub_08158CD8(u32);
void sub_0821AD08(u32, u32);
void sub_0821D698(u8 *, u32, u32, u32, u32, u32);
void sub_08050280(u8 *p)
{
    s32 r = sub_08050188(p);
    if (r != 0) {
        if (sub_08050128(p) != 0 && (r = sub_080501E8(p)) == 0) {
            if (p[0x1d] != 0) {
                sub_0821D6D0(p + 0x24);
                p[0x1d] = r;
                sub_08158CD8(gUnk_02000580);
                if (*(u32 *)(p + 0x34) != 0)
                    sub_0821AD08(*(u32 *)(p + 0x34), 0);
            }
        } else if (p[0x1d] == 0) {
            sub_0821D698(p + 0x24, *(u16 *)(p + 0x1e), 0, *(u16 *)(p + 0x20), 0xff, 0x82a);
            p[0x1d] = 1;
        }
    } else if (p[0x1d] != 0) {
        sub_0821D6D0(p + 0x24);
        p[0x1d] = r;
    }
}
