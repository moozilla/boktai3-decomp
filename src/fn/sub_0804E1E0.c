#include "global.h"
#include "engine.h"

extern u8 *gUnk_0200010C;

void sub_0804E1E0(void)
{
    u8 *g = gUnk_0200010C;
    if (g) {
        s32 found = Script_SeekToKeyword(0x74);
        if (found)
            g[0x27] = Script_GetValue();
        else
            g[0x27] = found;
    }
}
