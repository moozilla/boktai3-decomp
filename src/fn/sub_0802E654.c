#include "global.h"
#include "engine.h"
#include "ram.h"
/* Actor_IsInReactionStateFromScript: Tentatively tests whether the script-selected actor has a reaction code from thirteen through fifteen */
#define Actor_IsInReactionStateFromScript sub_0802E654

u8 *Actor_FindById(u8 *, s32);

u32 sub_0802E654(void)
{
    u8 *g = gSceneActorListManager;
    if (g) {
        u8 *e = Actor_FindById(g, Script_GetKeywordValue(0x6e, 0));
        if (e && (u8)(e[0x227] - 13) <= 2)
            goto yes;
    }
    return 0;
yes:
    return 1;
}
