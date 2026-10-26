#include "global.h"
#include "engine.h"
/* Parameters at 0x02000328 (meaning unknown). */
struct ScreenParams328 {
    u8 *kBlock, *hBlock;
    s32 o, c, p, f;
};
extern struct ScreenParams328 gNamePlaceholderScript;
void ScreenParams328_SetFromScript(void)
{
    if (Script_SeekToKeyword('h'))
        gNamePlaceholderScript.hBlock = Script_GetPc();
    if (Script_SeekToKeyword('k'))
        gNamePlaceholderScript.kBlock = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        gNamePlaceholderScript.o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gNamePlaceholderScript.c = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        gNamePlaceholderScript.p = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gNamePlaceholderScript.f = Script_GetValue();
    else
        gNamePlaceholderScript.f = 5;
}
