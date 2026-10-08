#include "global.h"
extern const u8 gUnk_08E6092C[];

u8 *sub_08227F5C(u32 a) {
    return (u8 *)gUnk_08E6092C + (a << 5);
}
