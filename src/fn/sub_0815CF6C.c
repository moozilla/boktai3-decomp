#include "global.h"
#include "engine.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
s32 sub_0815C9EC(void);
s32 sub_0815CA08(void);
void sub_0815B49C(u8 *actor, s32 direction, s32 h, s32 e);
void sub_0814F828(u8 *);
/* Script: sub_0815B49C on the scripted actor chosen by -i with -d, -h
   (default 1500) and -e (tentative). */
void ScriptActor_StartActionFromScript(void)
{
    u8 *actor;
    s32 i = sub_0815A400();
    actor = gUnk_02000580[i];
    if (actor) {
        s32 direction = sub_0815C9EC();
        s32 e = sub_0815CA08();
        s32 h;
        if (Script_SeekToKeyword('h'))
            h = Script_GetValue();
        else
            h = 1500;
        sub_0815B49C(actor, direction, h, e);
        sub_0814F828(actor);
    }
}
