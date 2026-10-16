#include "global.h"
#include "engine.h"
/* Parameters at 0x02000328 (meaning unknown). */
struct ScreenParams328 {
    u8 *kBlock, *hBlock;
    s32 o, c, p, f;
};
extern struct ScreenParams328 gUnk_02000328;
/* Script: -h and -k blocks, -o, -c, -p and -f (default 5). */
void ScreenParams328_SetFromScript(void)
{
    if (Script_SeekToKeyword('h'))
        gUnk_02000328.hBlock = Script_GetPc();
    if (Script_SeekToKeyword('k'))
        gUnk_02000328.kBlock = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        gUnk_02000328.o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gUnk_02000328.c = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        gUnk_02000328.p = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gUnk_02000328.f = Script_GetValue();
    else
        gUnk_02000328.f = 5;
}
