#include "global.h"
static inline u16 get(u8 *p, s32 i)
{
    if (i < 0)
        return 0xFFFF;
    return ((u16 *)*(u32 *)(p + 0x40))[i];
}
u32 LinkSession_HasParticipantCount(u8 *p, s32 n)
{
    s32 cnt = 0;
    s32 i = 0;
    do {
        if (get(p, i) != 0xFFFF)
            cnt++;
        i++;
    } while (i <= 3);
    if (cnt == n)
        return 1;
    return 0;
}
