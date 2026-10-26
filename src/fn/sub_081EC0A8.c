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
extern struct ScreenParams3F8Params gScreenTextParameters;
void ScreenParams3F8_SetFromScript(void)
{
    if (Script_SeekToKeyword('m'))
        gScreenTextParameters.mBlock = Script_GetPc();
    if (Script_SeekToKeyword('e'))
        gScreenTextParameters.eBlock = Script_GetPc();
    if (Script_SeekToKeyword('b'))
        gScreenTextParameters.b = Script_GetValue();
    if (Script_SeekToKeyword('d'))
        gScreenTextParameters.d = Script_GetValue();
    if (Script_SeekToKeyword('R'))
        gScreenTextParameters.R = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        gScreenTextParameters.c = Script_GetValue();
    if (Script_SeekToKeyword('t'))
        gScreenTextParameters.t = Script_GetValue();
    if (Script_SeekToKeyword('B'))
        gScreenTextParameters.B = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        gScreenTextParameters.f = Script_GetValue();
    if (Script_SeekToKeyword('M'))
        gScreenTextParameters.M = Script_GetValue();
}
