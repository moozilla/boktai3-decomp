#include "global.h"
#include "ram.h"

static inline u8 Tst(u16 *m, s32 one)
{
    if (*m & one)
        return TRUE;
    return FALSE;
}

u32 NamedObject_IsMapCellOccupied(u32 x)
{
    u8 *b = (u8 *)gNamedActorManager;
    s32 i = 0;
    s32 one = 1;
    for (; i < 12; i++) {
        if (Tst((u16 *)(b + i * 0x19C + 0xD4), one)) {
            if (*(u16 *)(b + i * 0x19C + 0x166) == x)
                return 1;
        }
    }
    return 0;
}
