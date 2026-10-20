#include "global.h"

s32 sub_08239F74(u8 *p, s32 r)
{
    s32 i;
    for (i = 0; i <= 1; i++) {
        switch (i) {
            case 0: {
                u16 *t = (u16 *)(p + 0x530);
                if (*t != 0) {
                    if (*(u32 *)(p + 0x1C) & 1) {
                        *t -= 1;
                    }
                    r = 2;
                    *(u16 *)(p + 0xACE) = 0x40;
                } else {
                    u16 *t2 = (u16 *)(p + 0xACE);
                    if (*t2 != 0) {
                        if ((*t2 >> 2) & 1) {
                            r = 2;
                        }
                        (*t2)--;
                    }
                }
                break;
            }
            case 1: {
                u16 *t = (u16 *)(p + 0x532);
                if (*t != 0) {
                    if (*(u32 *)(p + 0x1C) & 1) {
                        *t -= 1;
                        if (*t == 0) {
                            {
                                u8 *c = *(u8 **)(p + 0x344);
                                u32 x = *(u16 *)(c + 0x14);
                                p[0x54A] = x;
                            }
                        }
                    }
                }
                break;
            }
        }
    }
    return r;
}
