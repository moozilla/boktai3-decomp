#include "global.h"
#include "engine.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
void sub_0815AEAC(u8 *actor, s32 animation);
void sub_0814F828(u8 *);
/* Script: the scripted actor chosen by -i plays animation -p (passed to
   sub_0815AEAC as -p + 366; tentative). */
void ScriptActor_SetAnimationFromScript(void)
{
    u8 *actor;
    s32 i = sub_0815A400();
    actor = gUnk_02000580[i];
    if (actor && Script_SeekToKeyword('p')) {
        s32 animation = Script_GetValue();
        sub_0815AEAC(actor, animation + 366);
        sub_0814F828(actor);
    }
}
