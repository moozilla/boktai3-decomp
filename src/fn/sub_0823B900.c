#include "global.h"
extern u8 *gGameState;
static inline u8 chk(u32 m)
{
    if (*(u32 *)(gGameState + 0x868) & m)
        return 1;
    return 0;
}
u32 Player_IsFlaggedWithGlobalModifier08(u32 x)
{
    if (x & 0x80) {
        if (chk(8))
            return 1;
    }
    return 0;
}
