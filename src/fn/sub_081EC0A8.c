#include "global.h"
#include "engine.h"
/* Parameters at 0x020003F8 (meaning unknown). */
struct ScreenParams3F8Params {
    u8 *mBlock;
    u8 *eBlock;
    s32 b;
    s32 d;
    s32 R;
    s32 t;
    s32 c;
    s32 B;
    s32 f;
    s32 M;
};
extern struct ScreenParams3F8Params gUnk_020003F8;
/* Script: sets the parameter block (see the option letters). */
void ScreenParams3F8_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gUnk_020003F8.mBlock = Script_GetPc();
    if (Script_SeekToKeyword('e'))
        gUnk_020003F8.eBlock = Script_GetPc();
    if (Script_SeekToKeyword('b'))
        gUnk_020003F8.b = Script_GetValue();
    if (Script_SeekToKeyword('d'))
        gUnk_020003F8.d = Script_GetValue();
    if (Script_SeekToKeyword('R'))
        gUnk_020003F8.R = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gUnk_020003F8.c = Script_GetValue();
    if (Script_SeekToKeyword('t'))
        gUnk_020003F8.t = Script_GetValue();
    if (Script_SeekToKeyword('B'))
        gUnk_020003F8.B = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gUnk_020003F8.f = Script_GetValue();
    if (Script_SeekToKeyword('M'))
        gUnk_020003F8.M = Script_GetValue();
}
