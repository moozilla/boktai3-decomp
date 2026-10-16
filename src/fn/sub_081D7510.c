#include "global.h"
#include "engine.h"
/* Parameters at 0x020002A0 (meaning unknown). */
struct ScreenParams2A0Params {
    s32 g;
    s32 o;
    s32 s;
    s32 c;
    s32 r;
    s32 l;
    s32 d;
    s32 f;
};
extern struct ScreenParams2A0Params gUnk_020002A0;
/* Script: sets the parameter block (see the option letters). */
void ScreenParams2A0_SetFromScript(void)
{
    if (Script_SeekToKeyword('g'))
        gUnk_020002A0.g = Script_GetValue();
    if (Script_SeekToKeyword('o'))
        gUnk_020002A0.o = Script_GetValue();
    if (Script_SeekToKeyword('s'))
        gUnk_020002A0.s = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gUnk_020002A0.c = Script_GetValue();
    if (Script_SeekToKeyword('r'))
        gUnk_020002A0.r = Script_GetValue();
    if (Script_SeekToKeyword('l'))
        gUnk_020002A0.l = Script_GetValue();
    if (Script_SeekToKeyword('d'))
        gUnk_020002A0.d = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gUnk_020002A0.f = Script_GetValue();
}
