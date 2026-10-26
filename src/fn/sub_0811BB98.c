#include "global.h"
struct S {
    u8 pad[0x892];
    s16 value;
};
extern struct S *volatile gGameState;

s16 sub_0811BB98(void)
{
    s32 v = gGameState->value;
    if (v > 999)
        v = 999;
    else if (v < 0)
        v = 0;
    gGameState->value = v;
    return gGameState->value;
}
