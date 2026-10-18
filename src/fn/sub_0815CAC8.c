#include "global.h"
extern u8 *gUnk_02000580[];
s32 Script_GetActorIndex(void);
void PlayerCue_OverlayOff(u8 *);
void ScriptActor_EnableCompletionScript(u8 *);
void ScriptActor_DisableOverlayFromScript(void)
{
    u8 *p;
    s32 i = Script_GetActorIndex();
    p = gUnk_02000580[i];
    if (p) {
        PlayerCue_OverlayOff(p);
        ScriptActor_EnableCompletionScript(p);
    }
}
