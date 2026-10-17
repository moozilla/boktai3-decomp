#include "global.h"

#define ScriptPrompt_SetFromScript sub_081E0CB4
#include "engine.h"
struct Settings {
    u8 *m;
    u8 *t;
    s32 o, c, p;
};
extern struct Settings gUnk_02000388;
/* Script: -m and -t (script positions) and -o, -c, -p (values) into the
   block at gUnk_02000388, next to ScriptMessage_SetFromScript's (tentative). */
void ScriptPrompt_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gUnk_02000388.m = Script_GetPc();
    if (Script_SeekToKeyword('t'))
        gUnk_02000388.t = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        gUnk_02000388.o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gUnk_02000388.c = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        gUnk_02000388.p = Script_GetValue();
}
