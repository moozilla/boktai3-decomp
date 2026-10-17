#include "global.h"

#include "engine.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
s32 sub_0815C9EC(void);
s32 sub_0815CA08(void);
void sub_0815B404(u8 *, s32, s32, s32);
/* Script: on the scripted actor picked by -i, sub_0815B404 with -d
   (direction, -1 to keep), -f and -e (tentative). */
void ScriptActor_FaceFromScript(void)
{
    s32 i = sub_0815A400();
    u8 *actor = gUnk_02000580[i];
    if (actor) {
        s32 a = sub_0815C9EC();
        s32 b = sub_0815CA08();
        s32 f;
        if (Script_SeekToKeyword('f'))
            f = Script_GetValue();
        else
            f = 0;
        sub_0815B404(actor, a, f, b);
    }
}
