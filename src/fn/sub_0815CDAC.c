#include "global.h"
#include "engine.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
s32 sub_0815CA08(void);
void PlayerCue_Motion7Front(u8 *actor, s32 f, s32 e);
void PlayerCue_Stand(u8 *actor, s32 state);
void sub_0814F828(u8 *);
void sub_0814F838(u8 *);
/* Script: puts the scripted actor chosen by -i into state 4 with -e (default 0).
   From states 0, 1 and 6 it goes through PlayerCue_Motion7Front, which also takes -f. */
void ScriptActor_EnterStateFourFromScript(void)
{
    u8 *actor;
    s32 i = sub_0815A400();
    actor = gUnk_02000580[i];
    if (actor) {
        s32 e = sub_0815CA08();
        u8 state = actor[0x418];
        if (state <= 1 || state == 6) {
            s32 f; /* meaning unknown */
            if (Script_SeekToKeyword('f'))
                f = Script_GetValue();
            else
                f = 0;
            PlayerCue_Motion7Front(actor, f, e);
            sub_0814F828(actor);
        } else {
            *(s32 *)(actor + 0x5A4) = e;
            PlayerCue_Stand(actor, 4);
            sub_0814F828(actor);
            sub_0814F838(actor);
        }
    }
}
