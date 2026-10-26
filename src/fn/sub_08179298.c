#include "global.h"
#include "engine.h"
#include "game_state.h"
void *Hud_CreateAlternateLayout(u32, u32);
void *Hud_CreateStandardLayout(u32, u32);
void *Hud_CreateEngineFlagVariant(u32, u32);
static inline u8 testFlag(u32 mask, u32 *flags)
{
    if (*flags & mask)
        return TRUE;
    return FALSE;
}
/* Registry routine CC28: creates the field HUD/menu manager (0x02000214),
   choosing one of three creators by a game-state flag (+0x868 bit 7) and an
   engine flag (gEngineFlags bit 11). */
void *Hud_Create(u32 a, u32 b)
{
    void *p;
    if (testFlag(0x80, (u32 *)((u8 *)gGameState + 0x868)))
        p = Hud_CreateAlternateLayout(a, b);
    else if (!(gEngineFlags & 0x800))
        p = Hud_CreateStandardLayout(a, b);
    else
        p = Hud_CreateEngineFlagVariant(a, b);
    return p;
}
