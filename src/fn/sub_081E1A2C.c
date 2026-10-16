#include "global.h"
#include "engine.h"
/* Parameters at 0x020003A0 (meaning unknown). */
struct ScreenParams3A0Params {
    u8 *mBlock;
    u8 *aBlock;
    s32 l;
    s32 F;
    s32 f;
    s32 c;
    s32 b;
};
extern struct ScreenParams3A0Params gUnk_020003A0;
/* Script: sets the parameter block (see the option letters). */
void ScreenParams3A0_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gUnk_020003A0.mBlock = Script_GetPc();
    if (Script_SeekToKeyword('a'))
        gUnk_020003A0.aBlock = Script_GetPc();
    if (Script_SeekToKeyword('l'))
        gUnk_020003A0.l = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gUnk_020003A0.f = Script_GetValue();
    if (Script_SeekToKeyword('F'))
        gUnk_020003A0.F = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gUnk_020003A0.c = Script_GetValue();
    if (Script_SeekToKeyword('b'))
        gUnk_020003A0.b = Script_GetValue();
}
