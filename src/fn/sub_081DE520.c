#include "global.h"
#include "engine.h"
#include "ram.h"
/* Parameters at 0x02000328 (meaning unknown). */
struct ScreenParams328 {
    u8 *kBlock, *hBlock;
    s32 o, c, p, f;
};
void ScreenParams328_SetFromScript(void)
{
    if (Script_SeekToKeyword('h'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).hBlock = Script_GetPc();
    if (Script_SeekToKeyword('k'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).kBlock = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).c = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).p = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).f = Script_GetValue();
    else
        (*(struct ScreenParams328 *)gNamePlaceholderScript).f = 5;
}
