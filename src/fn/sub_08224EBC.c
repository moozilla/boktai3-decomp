#include "global.h"

extern u32 *gEventBytecodePtrs[];

u32 sub_08224EBC(u32 a, u32 b, u32 c) {
    u32 *t = gEventBytecodePtrs[a];
    if (t != 0) {
        u32 *u = (u32 *)t[b];
        if (u != 0)
            return u[c];
    }
    return 0;
}
