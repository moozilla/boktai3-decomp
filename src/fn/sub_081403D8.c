#include "global.h"
extern u8 *gUnk_02000710;
static inline u8 chk(u32 m)
{
    if (*(u32 *)(gUnk_02000710 + 0x868) & m)
        return 1;
    return 0;
}
u32 Player_IsStatusBit3ActionActive(u32 x)
{
    if (x & 0x80) {
        if (chk(8))
            return 1;
    }
    return 0;
}
