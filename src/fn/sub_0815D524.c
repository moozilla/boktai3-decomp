#include "global.h"
#include "engine.h"
#include "player.h"
s32 Script_GetActorIndex(void);
void sub_0815C73C(u8 *actor, s32 mode, s32 c, s32 a);
/* Script: sub_0815C73C on the scripted actor chosen by -i with mode -m; modes
   4 and 5 also take -c and -A (default 120 each), others pass 0. */
void ScriptActor_SetModeFromScript(void)
{
    u8 *actor;
    s32 i = Script_GetActorIndex();
    actor = gPlayers[i];
    if (actor && Script_SeekToKeyword('m')) {
        s32 mode = Script_GetValue();
        s32 c, a;
        if ((u32)(mode - 4) <= 1) {
            if (Script_SeekToKeyword('c'))
                c = Script_GetValue();
            else
                c = 120;
            if (Script_SeekToKeyword('A'))
                a = Script_GetValue();
            else
                a = 120;
        } else {
            a = 0;
            c = 0;
        }
        sub_0815C73C(actor, mode, c, a);
    }
}
