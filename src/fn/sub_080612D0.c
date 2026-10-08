#include "global.h"

s32 sub_08061120(u8 *);
s32 sub_080611AC(u8 *);
s32 sub_0806122C(u8 *);

s32 sub_080612D0(u8 *p)
{
    switch (*(u8 *)(p + 0x1AA8)) {
    case 0:
        return sub_08061120(p);
    case 1:
        return sub_080611AC(p);
    case 2:
        return sub_0806122C(p);
    }
}
