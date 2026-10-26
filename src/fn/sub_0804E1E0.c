#include "global.h"
/* AreaEffect_SetModeFromScript: Sets the manager mode from script keyword 0x74 or defaults it to zero. */
#define AreaEffect_SetModeFromScript sub_0804E1E0
#define AreaEffect_SetModeFromScript sub_0804E1E0
#include "engine.h"
#include "area_effect.h"

void AreaEffect_SetModeFromScript(void)
{
    u8 *g = gAreaEffectManager;
    if (g) {
        s32 found = Script_SeekToKeyword(0x74);
        if (found)
            g[0x27] = Script_GetValue();
        else
            g[0x27] = found;
    }
}
