#include "global.h"
#include "player.h"
s32 Script_GetActorIndex(void);
void PlayerCue_OverlayOff(u8 *);
void ScriptActor_EnableCompletionScript(u8 *);
void ScriptActor_DisableOverlayFromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gPlayers[i];
    if (p) {
        PlayerCue_OverlayOff(p);
        ScriptActor_EnableCompletionScript(p);
    }
}
