#include "global.h"

#include "engine.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
void sub_0815AE94(u8 *, s32);
void sub_0814F828(u8 *);
/* Script: on the scripted actor picked by -i, posts request 1 with id
   -p + 0x16E (+0xAE4/+0xAD6) and runs sub_0814F828 (tentative). */
void ScriptActor_RequestFromScript(void)
{
    s32 i = sub_0815A400();
    u8 *actor = gUnk_02000580[i];
    if (actor && Script_SeekToKeyword('p')) {
        sub_0815AE94(actor, Script_GetValue() + 0x16E);
        sub_0814F828(actor);
    }
}
