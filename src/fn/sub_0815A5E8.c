#include "global.h"
#include "engine.h"
#include "player.h"
s32 Script_GetActorIndex(void);
/* Script: -l (default: actor +0x426) goes to +0x424 of the scripted actor
   chosen by -i and to game state +0x28 (tentative: level). */
void ScriptActor_SetLevelFromScript(void)
{
    struct Player *actor;
    s32 i = Script_GetActorIndex();
    actor = gPlayers[i];
    if (actor) {
        s32 level = Script_GetKeywordValue('l', actor->maximumStatA);
        actor->currentStatA = level;
        gGameState->life = level;
    }
}
