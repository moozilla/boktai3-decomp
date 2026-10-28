#include "global.h"
#include "engine.h"
#include "player.h"
s32 Script_GetActorIndex(void);
/* Script: -l (default: actor +0x426) goes to +0x424 of the scripted actor
   chosen by -i and to game state +0x28 (tentative: level). */
void ScriptActor_SetLevelFromScript(void)
{
    u8 *actor;
    s32 i = Script_GetActorIndex();
    actor = gPlayers[i];
    if (actor) {
        s32 level = Script_GetKeywordValue('l', *(u16 *)(actor + 0x426));
        *(u16 *)(actor + 0x424) = level;
        *(u16 *)((u8 *)gGameState + 0x28) = level;
    }
}
