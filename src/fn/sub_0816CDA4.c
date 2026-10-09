#include "global.h"
extern u16 gUnk_020005A0;
u32 sub_0816CDA4(u32 bit) {
    return gUnk_020005A0 & (1 << bit);
}
