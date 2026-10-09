#include "global.h"
extern const u16 gUnk_08E61280[];
u32 sub_0816CEA0(u32 index) {
    return (gUnk_08E61280[index * 8] & 0xF0) >> 4;
}
