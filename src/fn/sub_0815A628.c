#include "global.h"
#include "player.h"
extern u8 *gGameState;
s32 Script_GetActorIndex(void);
u32 Script_GetKeywordValue(u32, u32);
void ScriptActor_SetEnergyMaxFromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gPlayers[i];
    if (p) {
        u32 r = Script_GetKeywordValue(0x65, *(u16 *)(p + 0x42a));
        *(u16 *)(p + 0x428) = r;
        *(u16 *)(gGameState + 0x2c) = r;
    }
}
