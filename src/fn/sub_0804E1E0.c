#include "global.h"
#define AreaEffect_SetModeFromScript sub_0804E1E0
#include "engine.h"

extern u8 *gUnk_0200010C;

void AreaEffect_SetModeFromScript(void)
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
